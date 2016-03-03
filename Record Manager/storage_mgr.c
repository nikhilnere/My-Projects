#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

//File header contains total number of pages
#define METADATA_SIZE 4
//SM_FileHandle used to reset file handle when the file is destroyed.
SM_FileHandle *sm_fileHandle;

void initStorageManager (){
}

/*
Creates a page file with one empty page in it 
*/
RC createPageFile (char *fileName){

	int totalNumPages = 1;
	//create file
	FILE *file = fopen(fileName, "w");

	if(file == NULL)
		return FILE_CREATION_FAILED;

	//allocate a block of memory with the size same as PAGE_SIZE
	char *page = (char *) malloc(PAGE_SIZE);
	bzero(page, PAGE_SIZE);
	memset(page, '\0', PAGE_SIZE);
	
	//Write header and empty block
	fwrite(&totalNumPages, sizeof(int), 1, file);
	fwrite(page, sizeof(char), PAGE_SIZE, file);

	//close file
	fclose(file);
	free(page);

	return RC_OK;
}

/*
Opens page file.
	NOTE - Before opening a page file make sure the same file is not alredy open
*/
RC openPageFile (char *fileName, SM_FileHandle *fHandle){

	//Check if the file exists
	struct stat buffer;
	if (stat (fileName, &buffer) != 0)
		return RC_FILE_NOT_FOUND;

	//check if the file is already open
	if (fHandle->fileName == fileName)
		return RC_FILE_ALREADY_OPEN;

	//open page file
	FILE *file = fopen(fileName, "r+");
	int totalNumPages;
	//read the metadat from the header
	fread(&totalNumPages, sizeof(int), 1, file);

	//set the file handle
	fHandle->fileName = fileName;
	fHandle->totalNumPages = totalNumPages;
	fHandle->curPagePos = 0;
	fHandle->mgmtInfo = file;

	//SM_FileHandle used to reset file handle when the file is destroyed.
	sm_fileHandle = fHandle;

	//printf("\nTotal number of pages in the file : %d\n", totalNumPages);

	return RC_OK;
}

/*
Closes page file and resets the file handle
*/
RC closePageFile (SM_FileHandle *fHandle){
	//check if file is not open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;
	
	//close file
	fclose(fHandle->mgmtInfo);
	
	//reset file handle
	fHandle->fileName = NULL;
	fHandle->totalNumPages = 0;
	fHandle->curPagePos = 0;
	fHandle->mgmtInfo = NULL;
	free(fHandle);
	return RC_OK;
}

/*
Deletes page file
*/
RC destroyPageFile (char *fileName){
	//remove the file
	if (remove(fileName) == 0){
		return 	RC_OK;
	}
	return RC_FAILED_TO_DELETE;
}

/*
Reads the given block from page file
*/
RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){

	//check if the file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	//check if the page does not exist
	if(pageNum < 0 || pageNum > fHandle->totalNumPages - 1)
		return RC_READ_NON_EXISTING_PAGE;
	
	//calculate the offset
	long int offset = ((pageNum) * PAGE_SIZE) + METADATA_SIZE;
	fseek(fHandle->mgmtInfo, offset, SEEK_SET);

	//read block
	fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
	
	//Updating file handle
	fHandle->curPagePos = pageNum;

	return RC_OK;
}

/*
Returns current block position
*/
int getBlockPos (SM_FileHandle *fHandle){
	//check if file is open
	if (fHandle->fileName == NULL)
		return -1;

	return fHandle->curPagePos;
}

/*
Reads first block from page file
*/
RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	return readBlock(0,fHandle, memPage);;
}

/*
Reads previous block from page file
*/
RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	//check if current page is the first page
	if (fHandle->curPagePos == 0)
		return RC_READ_NON_EXISTING_PAGE; 

	return readBlock(fHandle->curPagePos - 1, fHandle, memPage);;
}

/*
Reads current block from page file
*/
RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	return readBlock(fHandle->curPagePos,fHandle, memPage);;
}

/*
Reads next block from page file
*/
RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;
	
	//check if current page is the last page
	if ((fHandle->curPagePos) == fHandle->totalNumPages)
		return RC_READ_NON_EXISTING_PAGE;

	return readBlock(fHandle->curPagePos + 1, fHandle, memPage);;
}

/*
Reads last block from page file
*/
RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;
	return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);;
}

/*
Writes to the given block in page file
*/
RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage){
	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;
	//check if memPage is not NULL
	if (memPage == NULL)
		return NULL_PAGE_HANDLE;
	//check if page exists in the file
	if(pageNum < 0 || pageNum > fHandle->totalNumPages - 1)
		return RC_WRITE_NON_EXISTING_PAGE;
	//calculate offset
	long int offset = ((pageNum) * PAGE_SIZE) + METADATA_SIZE;
	fseek(fHandle->mgmtInfo, offset, SEEK_SET);
	//write to file
	fwrite(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
	//Updating file handle
	fHandle->curPagePos = pageNum;
	return RC_OK;
}

/*
Writes to the current block in page file
*/
RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
	
	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	//check if memPage is NULL
	if (memPage == NULL)
		return NULL_PAGE_HANDLE;

	return writeBlock(fHandle->curPagePos,fHandle, memPage);
}

/*
Appends an empty block to page file
*/
RC appendEmptyBlock (SM_FileHandle *fHandle){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	//allocate a block of memory of the size of PAGE_SIZE
	char *page = (char *) malloc(PAGE_SIZE);
	bzero(page, PAGE_SIZE);
	memset(page, '\0', PAGE_SIZE);
	
	fHandle->totalNumPages++;

	//update header
	fseek (fHandle->mgmtInfo, 0, SEEK_SET);
	fwrite(&fHandle->totalNumPages, sizeof(int), 1, fHandle->mgmtInfo);

	//write empty page to the file
	fseek (fHandle->mgmtInfo, 0, SEEK_END);
	fwrite(page, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
	fHandle->curPagePos = fHandle->totalNumPages-1;
	free(page);
	return RC_OK;
}

/*
Ensures capacity of page file. If needed adds pages to the file to ensure the capacity
*/
RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle){

	//check if file is open
	if (fHandle->fileName == NULL)
		return FIRST_OPEN_FILE;

	int i;	
	//calculate the difference between expected number of pages and actual number of pages
	int difference = numberOfPages - fHandle->totalNumPages;
	if (difference > 0){
		for (i = 0; i < difference; i++){
			appendEmptyBlock(fHandle);
		}
	}
	//printf("\nFile size ensured!!");
	return RC_OK;
}
