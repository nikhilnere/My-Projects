#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "record_mgr.h"
#include "tables.h"

typedef struct VarString {
  char *buf;
  int size;
  int bufsize;
} VarString;

#define MAKE_VARSTRING(var)				\
  do {							\
  var = (VarString *) malloc(sizeof(VarString));	\
  bzero(var,sizeof(VarString));		\
  var->size = 0;					\
  var->bufsize = 100;					\
  var->buf = malloc(100);				\
  bzero(var->buf,100);				\
  } while (0)

#define FREE_VARSTRING(var)			\
  do {						\
  free(var->buf);				\
  free(var);					\
  } while (0)

#define GET_STRING(result, var)			\
  do {						\
    result = malloc((var->size) + 1);		\
    bzero(result,(var->size) + 1);			\
    memcpy(result, var->buf, var->size);	\
    result[var->size] = '\0';			\
  } while (0)

#define RETURN_STRING(var)			\
  do {						\
    char *resultStr;				\
    GET_STRING(resultStr, var);			\
    FREE_VARSTRING(var);			\
    return resultStr;				\
  } while (0)

#define ENSURE_SIZE(var,newsize)				\
  do {								\
    if (var->bufsize < newsize)					\
    {								\
      int newbufsize = var->bufsize;				\
      while((newbufsize *= 2) < newsize);			\
      var->buf = realloc(var->buf, newbufsize);			\
    }								\
  } while (0)

#define APPEND_STRING(var,string)					\
  do {									\
    ENSURE_SIZE(var, var->size + strlen(string));			\
    memcpy(var->buf + var->size, string, strlen(string));		\
    var->size += strlen(string);					\
  } while(0)

#define APPEND(var, ...)			\
  do {						\
    char *tmp = malloc(10000);			\
    bzero(tmp,10000);				\
    sprintf(tmp, __VA_ARGS__);			\
    APPEND_STRING(var,tmp);			\
    free(tmp);					\
  } while(0)

/*tombstones Node */
typedef struct tombstoneNode {
    RID id;
    struct tombstoneNode *next;
}tombstoneNode;

typedef struct TableMetadata{
    int lastPage;	//Last page in the page file
    int lastSlot;	//Last slot in the last page
    int numOfTuples;
    int slotSize;
    int noOfSlots;
    int tNodeLen;
    tombstoneNode *tstone_root;
    BM_BufferPool *bm;
}TableMetadata;


typedef struct ScanData {
    Expr *cond;
    int curPage;
    int curSlot;
    int lastPage;
    int lastSlot;
    int noOfSlots;
}ScanData;

// table and manager


extern RC initRecordManager (void *mgmtData){
	return RC_OK;
}

extern RC shutdownRecordManager (){
	return RC_OK;
}

char *serializeMetadata(int lastPage, int lastSlot, tombstoneNode *tombroot){
	VarString *result;
	MAKE_VARSTRING(result);
	APPEND(result, "LastPage <%i> \n", lastPage);
	APPEND(result, "LastSlot <%i> \n", lastSlot);
	//RETURN_STRING(result);

	int tombLen = 0;
	tombstoneNode *temproot = tombroot;

	while(temproot != NULL){
		temproot = temproot->next;
		tombLen++;
	}
	APPEND(result, "tombstoneLen <%i> <", tombLen);

	temproot = tombroot;
	while(temproot != NULL){
		APPEND(result,"%i:%i%s",temproot->id.page, temproot->id.slot, (temproot->next != NULL) ? ",": "");
		temproot = temproot->next;
	}
	APPEND_STRING(result, ">\n");

	RETURN_STRING(result);
}

int getSlotSize(Schema *schema){
    int slotSize=0, i=0, attrSize;

    //Calculating the slot size according to the slot structure given in the serialization function
    slotSize = slotSize + 15;
    for(i=0; i<schema->numAttr; i++){
        switch (schema->dataTypes[i]){
            case DT_STRING:
            	attrSize = schema->typeLength[i];
                break;
            case DT_INT:
            	attrSize = 5;
                break;
            case DT_FLOAT:
            	attrSize = 10;
                break;
            case DT_BOOL:
            	attrSize = 5;
                break;
            default:
                break;
        }
        slotSize += (attrSize + strlen(schema->attrNames[i]) + 1 + 1);
    }
    return slotSize;
}

extern RC createTable (char *name, Schema *schema){

	RC returnCode;
	int i;

	//Check if the table already exists (Check if the file exists)
    if( access(name, F_OK) != -1 ) {
        return TABLE_ALREADY_EXISTS;
    }

    //Create a pageFile for the given table
	returnCode = createPageFile(name);
	if (returnCode != RC_OK)
		return returnCode;

	//Open pageFile
	SM_FileHandle *fh = (SM_FileHandle*) malloc (sizeof(SM_FileHandle));
	bzero(fh, sizeof(SM_FileHandle));
	returnCode = openPageFile(name, fh);
	if (returnCode != RC_OK)
			return returnCode;

	ensureCapacity(2,fh);

	//Store Table information
	RM_TableData *rm = (RM_TableData *) malloc (sizeof (RM_TableData));
	bzero(rm, sizeof (RM_TableData));
	rm->name = name;
	rm->schema = schema;

	char *tableData_str = serializeTableInfo (rm);

	returnCode = writeBlock(0, fh, tableData_str);
	if (returnCode != RC_OK)
				return returnCode;

	tombstoneNode *temp_root = NULL;

	char *metadata_Str = (char *)serializeMetadata(2,0,temp_root); //last page and last slot in the last page

	returnCode = writeBlock(1, fh, metadata_Str);
	if (returnCode != RC_OK)
		return returnCode;


	returnCode = closePageFile(fh);
	free(rm);
	return returnCode;
}


void *deserializeTableInfo (RM_TableData *rm, char *str, int *numTuples){

	Schema *schema = (Schema *) malloc(sizeof(Schema));
	bzero(schema, sizeof(Schema));
	int i, j;

	char *tempStr, *tempPtr;
	tempStr = strtok (str,"<");
	tempStr = strtok (NULL,">");
	rm->name = tempStr;

	tempStr = strtok (NULL,"<");
	tempStr = strtok (NULL,">");

	int numOfTuples = strtol(tempStr, &tempPtr, 10);
	*numTuples = numOfTuples;

	tempStr = strtok (NULL,"<");
	tempStr = strtok (NULL,">");
	int numOfAttr = strtol(tempStr, &tempPtr, 10);
	schema->numAttr = numOfAttr;

	schema->attrNames=(char **)malloc(sizeof(char*)*numOfAttr);
	bzero(schema->attrNames, sizeof(char*)*numOfAttr);
	schema->dataTypes=(DataType *)malloc(sizeof(DataType)*numOfAttr);
	bzero(schema->dataTypes, sizeof(DataType)*numOfAttr);
	schema->typeLength=(int *)malloc(sizeof(int)*numOfAttr);
	bzero(schema->typeLength,sizeof(int)*numOfAttr);

	tempStr = strtok (NULL,"(");
	for(i=0; i<numOfAttr; i++){
		tempStr = strtok (NULL,": ");
		schema->attrNames[i]=(char *)calloc(strlen(tempStr), sizeof(char));
		bzero(schema->attrNames[i], sizeof(char));

		strcpy(schema->attrNames[i], tempStr);

		if(i == numOfAttr-1){
			tempStr = strtok (NULL,") ");
		}
		else{
			tempStr = strtok (NULL,", ");
		}

		if (strcmp(tempStr, "INT") == 0){
			schema->dataTypes[i] = DT_INT;
			schema->typeLength[i] = 0;
		}
		else if (strcmp(tempStr, "FLOAT") == 0){
			schema->dataTypes[i] = DT_FLOAT;
			schema->typeLength[i] = 0;
		}
		else if (strcmp(tempStr, "BOOL") == 0){
			schema->dataTypes[i] = DT_BOOL;
			schema->typeLength[i] = 0;
		}else{
			char charArray [(strlen(tempStr)-7)];
			strncpy(charArray, &tempStr[7], (strlen(tempStr)-7));
			charArray [(strlen(tempStr)-8)] = '\0';
			schema->dataTypes[i] = DT_STRING;
			schema->typeLength[i] = strtol(charArray, &tempPtr, 10);
		}
	}

	int keyFlag = 0, keySize = 0;
	char* keyAttrs[numOfAttr];

	if((tempStr = strtok (NULL,"(")) != NULL){
		tempStr = strtok (NULL,")");
		char *key = strtok (tempStr,", ");
		while(key != NULL){
			keyAttrs[keySize] = (char *)malloc(strlen(key)*sizeof(char));
			bzero(keyAttrs[keySize],strlen(key)*sizeof(char));
			strcpy(keyAttrs[keySize], key);
			keySize++;
			key = strtok (NULL,", ");
		}
		keyFlag = 1;
	}
	if(keyFlag == 1){
		schema->keyAttrs=(int *)malloc(sizeof(int)*keySize);
		bzero(schema->keyAttrs,sizeof(int)*keySize);
		schema->keySize = keySize;
		for(i=0; i<keySize; i++){
			for(j=0; j<numOfAttr; j++){
				if(strcmp(keyAttrs[i], schema->attrNames[j]) == 0){
					schema->keyAttrs[i] = j;
					free(keyAttrs[i]);
				}
			}
		}
	}

	rm->schema = schema;
}

void *deserializeMetadata (TableMetadata *metadata, char *str){
	char *tempStr, *tempPtr;
	int i, page, slot;

	tempStr = strtok (str,"<");
	tempStr = strtok (NULL,">");
	metadata->lastPage = strtol(tempStr, &tempPtr, 10);

	tempStr = strtok (NULL,"<");
	tempStr = strtok (NULL,">");
	metadata->lastSlot = strtol(tempStr, &tempPtr, 10);

	tempStr = strtok (NULL,"<");
	tempStr = strtok (NULL,">");
	metadata->tNodeLen = strtol(tempStr, &tempPtr, 10);

	tempStr = strtok (NULL,"<");
	tombstoneNode *tempnode;
	tombstoneNode *node;

	for (i = 0; i < metadata->tNodeLen; i++){

		tempStr = strtok (NULL,":");
		page = strtol(tempStr, &tempPtr, 10);

		if (i == metadata->tNodeLen-1){
			tempStr = strtok (NULL,">");
		}else{
			tempStr = strtok (NULL,",");
		}

		slot = strtol(tempStr, &tempPtr, 10);

		if (metadata->tstone_root == NULL){
			metadata->tstone_root = (tombstoneNode *)malloc(sizeof(tombstoneNode));
			bzero(metadata->tstone_root, sizeof(tombstoneNode));
			metadata->tstone_root->id.page = page;
			metadata->tstone_root->id.slot = slot;
			metadata->tstone_root->next = NULL;

		}
		else{
			node = metadata->tstone_root;
			while(node->next != NULL){
				node = node->next;
			}

			node->next = (tombstoneNode *)malloc(sizeof(tombstoneNode));
			bzero(node->next, sizeof(tombstoneNode));
			node->next->id.page = page;
			node->next->id.slot = slot;
			node->next->next = NULL;
		}

	}
}

void deserializeRecord(RM_TableData *rel, char *record_str, Record *record){
	Schema *schema = rel->schema;
	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	//Record *record = (Record *) malloc (sizeof(Record));
	record->data = (char *)malloc(sizeof(char) * metadata->slotSize);
	bzero(record->data, sizeof(char) * metadata->slotSize);
	int i, val1;
	float val2;
	bool val3;
	Value *value;
	char *tempStr, *tempPtr;

	tempStr = strtok(record_str,"-");
	tempStr = strtok(NULL,"]");
	tempStr = strtok(NULL,"(");

	for(i=0; i<schema->numAttr; i++){
		tempStr = strtok (NULL,":");
		if(i == (schema->numAttr -1)){
			tempStr = strtok (NULL,")");
		}else{
			tempStr = strtok (NULL,",");
		}

		switch(schema->dataTypes[i]){

			case DT_INT:
				val1 = strtol(tempStr, &tempPtr, 10);
				MAKE_VALUE(value, DT_INT, val1);
				setAttr (record, schema, i, value);
				freeVal(value);
				break;

			case DT_STRING:
				MAKE_STRING_VALUE(value, tempStr);
				setAttr (record, schema, i, value);
				freeVal(value);
				break;

			case DT_FLOAT:
				val2 = strtof(tempStr, NULL);
				MAKE_VALUE(value, DT_FLOAT, val2);
				setAttr (record, schema, i, value);
				freeVal(value);
				break;

			case DT_BOOL:
				val3 = (tempStr[0] == 't') ? TRUE : FALSE;
				MAKE_VALUE(value, DT_BOOL, val3);
				setAttr (record, schema, i, value);
				freeVal(value);
				break;
		}
	}
	free(record_str);
}

updateTableInfoAndMetadataOnFile(RM_TableData *rel){

	SM_FileHandle *fh = (SM_FileHandle*) malloc (sizeof(SM_FileHandle));
	bzero(fh,sizeof(SM_FileHandle));
	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	openPageFile(rel->name, fh);

	char *tableData_str = serializeTableInfo (rel);
	writeBlock(0, fh, tableData_str);

	char *metadata_Str = (char *)serializeMetadata(metadata->lastPage,metadata->lastSlot,metadata->tstone_root); //last page and last slot in the last page
	writeBlock(1, fh, metadata_Str);

	free(tableData_str);
	free(metadata_Str);
	closePageFile(fh);

}

extern RC openTable (RM_TableData *rel, char *name){

	/*RC returnCode;
	int i;*/

	TableMetadata *metadata = (TableMetadata *) malloc(sizeof(TableMetadata));
	bzero(metadata,sizeof(TableMetadata));

	int *numOfTuples = (int *) malloc(sizeof(int));
	bzero(numOfTuples,sizeof(int));

	if(access(name, F_OK) == -1) {
		return TABLE_NOT_FOUND;
	}

	BM_BufferPool *bm = (BM_BufferPool *)malloc(sizeof(BM_BufferPool));
	bzero(bm, sizeof(BM_BufferPool));
	BM_PageHandle *page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
	bzero(page,sizeof(BM_PageHandle));

	initBufferPool(bm, name, 3, RS_LRU, NULL);
	pinPage(bm, page, 0);
	deserializeTableInfo (rel, page->data, numOfTuples);
	unpinPage(bm,page);

	pinPage (bm, page, 1);
	deserializeMetadata(metadata, page->data);
	unpinPage(bm,page);

	free(page);

	//Calculate slot size
	int slotSize = getSlotSize(rel->schema);

	//Maximum number of slots per page
	int noOfSlots = (int) floor ((float)PAGE_SIZE / (float)slotSize);

	metadata->numOfTuples = *numOfTuples;
	metadata->slotSize = slotSize;
	metadata->noOfSlots = noOfSlots;
	metadata->bm = bm;

	rel->mgmtData = metadata;
	free(numOfTuples);
	return RC_OK;
}


extern RC closeTable (RM_TableData *rel){

	shutdownBufferPool(((TableMetadata *)rel->mgmtData)->bm);
	free(rel->mgmtData);
	free(rel->schema->dataTypes);
	free(rel->schema->attrNames);
	free(rel->schema->keyAttrs);
	free(rel->schema->typeLength);
	free(rel->schema);

	return RC_OK;
}


extern RC deleteTable (char *name){

	if(access(name, F_OK) == -1) {
		return TABLE_NOT_FOUND;
	}
	remove(name);
	return RC_OK;
}

extern int getNumTuples (RM_TableData *rel){
	if (rel->mgmtData == NULL){
		return 0;
	}else{
		return ((TableMetadata *)rel->mgmtData)->numOfTuples ;
	}
}


// handling records in a table
extern RC insertRecord (RM_TableData *rel, Record *record){

	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	BM_PageHandle *page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
	bzero(page, sizeof(BM_PageHandle));
	int pageNum, slot;

	if (metadata->tstone_root != NULL){
		pageNum = metadata->tstone_root->id.page;
		slot = metadata->tstone_root->id.slot;
		metadata->tstone_root = metadata->tstone_root->next;
	}
	else{

		pageNum = metadata->lastPage;
		slot = metadata->lastSlot;

		if (slot > (metadata->noOfSlots - 1)){
			slot = 0;
			pageNum++;
		}
		metadata->lastPage = pageNum;
		metadata->lastSlot = slot+1;
	}

	record->id.page = pageNum;
	record->id.slot = slot;

	char * record_str = serializeRecord(record,rel->schema);

	pinPage(metadata->bm, page, pageNum);
	memcpy(page->data + (slot*metadata->slotSize), record_str, strlen(record_str));
	free(record_str);
	//printf("\npage content%s", &page->data[slot*metadata->slotSize]);

	markDirty(metadata->bm, page);
	unpinPage(metadata->bm, page);
	forcePage(metadata->bm, page);
	free(page);

	metadata->numOfTuples++;
	updateTableInfoAndMetadataOnFile(rel);

	return RC_OK;

}
extern RC deleteRecord (RM_TableData *rel, RID id){

	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	tombstoneNode *tnode, *temmpNode;

	if (metadata->tstone_root == NULL){
		metadata->tstone_root = (tombstoneNode *) malloc (sizeof(tombstoneNode));
		bzero(metadata->tstone_root, sizeof(tombstoneNode));
		metadata->tstone_root->id.page = id.page;
		metadata->tstone_root->id.slot = id.slot;
		metadata->tstone_root->next = NULL;
	}else{
		temmpNode = metadata->tstone_root;
		while (temmpNode->next != NULL){
			temmpNode = temmpNode->next;
		}
		tnode = (tombstoneNode *) malloc (sizeof(tombstoneNode));
		bzero(tnode, sizeof(tombstoneNode));
		tnode->id.page = id.page;
		tnode->id.slot = id.slot;
		tnode->next = NULL;
		temmpNode->next = tnode;
	}
	metadata->numOfTuples--;
	updateTableInfoAndMetadataOnFile(rel);

	return RC_OK;
}
extern RC updateRecord (RM_TableData *rel, Record *record){

	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	BM_PageHandle *page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
	bzero(page,sizeof(BM_PageHandle));

	int pageNum = record->id.page;
	int slot = record->id.slot;

	char *record_str = serializeRecord(record,rel->schema);
	pinPage(metadata->bm,page,pageNum);
	memcpy(page->data + (slot*metadata->slotSize), record_str, strlen(record_str));

	markDirty(metadata->bm,page);
	unpinPage(metadata->bm,page);
	forcePage(metadata->bm,page);

	updateTableInfoAndMetadataOnFile(rel);

	free(page);

	return RC_OK;
}


extern RC getRecord (RM_TableData *rel, RID id, Record *record){
	TableMetadata *metadata = (TableMetadata *)rel->mgmtData;
	BM_PageHandle *page;
	tombstoneNode *tempnode;
	int deletedRecord = 0;

	record->id.page = id.page;
	record->id.slot = id.slot;

	tempnode = metadata->tstone_root;
	if (tempnode != NULL){
		while (tempnode->next != NULL){
			if (tempnode->id.page == id.page && tempnode->id.slot == id.slot){
				deletedRecord = 1;		//Record found in deleted recordlist
				break;
			}
			tempnode = tempnode->next;
		}
	}

	if(deletedRecord == 0){
		//check ids with last ids
		//TODO checking condition

		if(metadata->lastPage <= id.page && metadata->lastSlot <= id.slot){
			return RC_RM_NO_MORE_TUPLES;
		}
		page = (BM_PageHandle *)malloc(sizeof(BM_PageHandle));
		bzero(page,sizeof(BM_PageHandle));

		pinPage(metadata->bm, page, id.page);
		char *record_str = (char *) malloc(sizeof(char) * metadata->slotSize);
		bzero(record_str,(sizeof(char) * metadata->slotSize));
		memcpy(record_str, page->data + ((id.slot)*metadata->slotSize), sizeof(char)*metadata->slotSize);
		unpinPage(metadata->bm,page);
		deserializeRecord(rel, record_str, record);
	}
	free(page);
	return RC_OK;
}


// scans
extern RC startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond){
	scan->rel = rel;

	ScanData *scanData = (ScanData *) malloc (sizeof(ScanData));
	scanData->curPage = 2;
	scanData->curSlot = 0;
	scanData->lastPage = ((TableMetadata *)rel->mgmtData)->lastPage;
	scanData->lastSlot = ((TableMetadata *)rel->mgmtData)->lastSlot;
	scanData->noOfSlots = ((TableMetadata *)rel->mgmtData)->noOfSlots;
	scanData->cond = cond;

	scan->mgmtData = scanData;

	return RC_OK;

}
extern RC next (RM_ScanHandle *scan, Record *record){

	ScanData *scanData = (ScanData *) scan->mgmtData;
    Value *value;
	record->id.page = scanData->curPage;
	record->id.slot	= scanData->curSlot;

	if (getRecord(scan->rel, record->id, record) == RC_RM_NO_MORE_TUPLES){
		return RC_RM_NO_MORE_TUPLES;
	}
	evalExpr(record, scan->rel->schema, scanData->cond, &value);
	if (scanData->curSlot == scanData->noOfSlots - 1){
		scanData->curSlot = 0;
		(scanData->curPage)++;
	}
	else{
		(scanData->curSlot)++;
	}
	scan->mgmtData = scanData;
	if(value->v.boolV != 1){
		return next(scan, record);
	}
	else{
		return RC_OK;
	}

}
extern RC closeScan (RM_ScanHandle *scan){
	free(scan->mgmtData);
	return RC_OK;
}

// dealing with schemas
extern int getRecordSize (Schema *schema){
    int size = 0, i;
    for(i=0; i<schema->numAttr; i++){
        switch (schema->dataTypes[i]){
            case DT_STRING:
                size += schema->typeLength[i];
                break;
            case DT_INT:
                size += sizeof(int);
                break;
            case DT_FLOAT:
                size += sizeof(float);
                break;
            case DT_BOOL:
                size += sizeof(bool);
                break;
            default:
                break;
        }
    }
    return size;
}

/**
 * Creates schema for the given attributes
 */
extern Schema *createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys){

    Schema *schema = (Schema *) malloc(sizeof(Schema));
    bzero(schema,sizeof(Schema));

    schema->numAttr = numAttr;
    schema->attrNames = attrNames;
    schema->dataTypes = dataTypes;
    schema->typeLength = typeLength;
    schema->keySize = keySize;
    schema->keyAttrs = keys;

    return schema;
}
extern RC freeSchema (Schema *schema){
    free(schema);
    return RC_OK;
}

// dealing with records and attribute values
extern RC createRecord (Record **record, Schema *schema){
	*record = (Record *)  malloc(sizeof(Record));
	bzero(*record, sizeof(Record));
	(*record)->data = (char *)malloc((getRecordSize(schema)));
	bzero((*record)->data,getRecordSize(schema));
	return RC_OK;
}

extern RC freeRecord (Record *record){
	free(record->data);
	free(record);
	return RC_OK;
}

RC
attrOffset (Schema *schema, int attrNum, int *result)
{
    int offset = 0;
    int attrPos = 0;

    for(attrPos = 0; attrPos < attrNum; attrPos++)
        switch (schema->dataTypes[attrPos])
    {
        case DT_STRING:
            offset += schema->typeLength[attrPos];
            break;
        case DT_INT:
            offset += sizeof(int);
            break;
        case DT_FLOAT:
            offset += sizeof(float);
            break;
        case DT_BOOL:
            offset += sizeof(bool);
            break;
    }

    *result = offset;
    return RC_OK;
}

extern RC getAttr (Record *record, Schema *schema, int attrNum, Value **value){
	*value = (Value *)  malloc(sizeof(Value));
	bzero(*value, sizeof(Value));
	int attOffset;
	char *temp;
	int len = 0;
	char *stringV;

	attrOffset(schema, attrNum, &attOffset);
	temp = (record->data + attOffset);

	(*value)->dt = schema->dataTypes[attrNum];

	switch(schema->dataTypes[attrNum]){

		case DT_INT:
			memcpy(&((*value)->v.intV),temp, sizeof(int));
			break;
		case DT_STRING:
			len = schema->typeLength[attrNum];
			stringV = (char *) malloc(len + 1);
			bzero(stringV, (len + 1));
			strncpy(stringV, temp, len);
			stringV[len] = '\0';
			(*value)->v.stringV = stringV;
			break;
		case DT_FLOAT:
			memcpy(&((*value)->v.floatV),temp, sizeof(float));
			break;
		case DT_BOOL:
			memcpy(&((*value)->v.boolV),temp, sizeof(bool));
			break;
	}
	return RC_OK;
}

extern RC setAttr (Record *record, Schema *schema, int attrNum, Value *value){
	int attOffset; char * temp;
	char *stringV;
	int len = 0;
	attrOffset(schema, attrNum, &attOffset);
	temp = record->data + attOffset;

	switch(schema->dataTypes[attrNum]){

		case DT_INT:
			memcpy(temp,&(value->v.intV), sizeof(int));
			break;
		case DT_STRING:
			len = schema->typeLength[attrNum];
			stringV = (char *) malloc(len);
			bzero(stringV, len);
			stringV = value->v.stringV;
			memcpy(temp,(stringV), len);
			break;
		case DT_FLOAT:
			memcpy(temp,&((value->v.floatV)), sizeof(float));
			break;
		case DT_BOOL:
			memcpy(temp,&((value->v.boolV)), sizeof(bool));
			break;
	}

	return RC_OK;
}
