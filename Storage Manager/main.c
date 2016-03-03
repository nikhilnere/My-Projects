#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define FILENAME_LENGTH 256

SM_FileHandle sm_FileHandle;

void main(){
SM_PageHandle memPage;
memPage = (char*)calloc(PAGE_SIZE, sizeof(char));
int choice, currentPagePos, blockNo, numberOfPages;
char fileName[FILENAME_LENGTH];
char writeChar;

while(1)
{
	printf("\n\n***********************************");
	printf("\n*         STORAGE MANAGER         *");
	printf("\n***********************************\n");	
	printf("1. Create page file\n"); 
	printf("2. Open page file\n");
	printf("3. Close page file\n");
	printf("4. Destroy page file\n");
	printf("5. Read block\n");
	printf("6. Get current block position\n");
	printf("7. Read first block\n");
	printf("8. Read previous block\n");
	printf("9. Read current block\n");
	printf("10. Read next block\n");
	printf("11. Read last block\n");
	printf("12. Write to a block\n");
	printf("13. Write to the current block\n");
	printf("14. Append empty block to the file\n");
	printf("15. Ensure capacity of the file\n");
	printf("16. Exit\n");
	printf("Enter your choice : ");
	scanf("%d", &choice);
	
	switch(choice){
		case 1: 
			printf("Enter file name to be created : ");
			scanf("%s", fileName);
			if(createPageFile (fileName) == RC_OK){
				printf("\nFile created successfully!!");
			}else{
				printf("\nCould not create file...Try again\n");
			}
			break;

		case 2:
			printf("Enter file name to be opened : ");
			scanf("%s", fileName);
			if(openPageFile(fileName, &sm_FileHandle) == RC_OK){
				printf("\nFile opened successfully!! It is now available for reading and writing operations..\n");
			}else{
				printf("\nFile not found...Please check the file name or the file is already open\n");
			}
			break;

		case 3:
			if(closePageFile(&sm_FileHandle) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try closing it..\n");
			}else{
				printf("\nFile is closed successfully!!");
			}
			break;

		case 4: 
			printf("Enter file name to be deleted : ");
			scanf("%s", fileName);
			if(destroyPageFile(fileName) == RC_FAILED_TO_DELETE){
				printf("\nFailed to delete the file...\n");
			}else{
				printf("\nThe file is deleted!!\n");
			}
			break;

		case 5:
			printf("Enter the block(page) number :");
			scanf("%d", &blockNo);
			if(readBlock(blockNo, &sm_FileHandle, memPage) == RC_OK){
				printf("\nPage contents : \n%s", memPage);
			}else{
				printf("\nSomething went wrong...Please open the file first or enter a correct page number..\n");
			}
			break;

		case 6: 
			currentPagePos = getBlockPos (&sm_FileHandle);
			if (currentPagePos == -1){
				printf("\nFirst open a file and then try..\n");				
			}else{
				printf("\nThe current page position : %d", currentPagePos);
			}
			break;

		case 7:
			if(readFirstBlock (&sm_FileHandle, memPage) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try..\n");
			}else{
				printf("\nPage contents : \n%s", memPage);
			}
			break;

		case 8:
			if(readPreviousBlock (&sm_FileHandle, memPage) == RC_OK){
				printf("\nPage contents : \n%s", memPage);				
			}else{
				printf("\nSomething went wrong...Please check if you have opened the file first or may be the current page is the first page..\n");
			}
			break;

		case 9:
			if(readCurrentBlock (&sm_FileHandle, memPage) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try..\n");
			}else{
				printf("\nPage contents : \n%s", memPage);
			}
			break;

		case 10:
			if (readNextBlock (&sm_FileHandle, memPage) == RC_OK){
				printf("\nPage contents : \n%s", memPage);				
			}else{
				printf("\nSomething went wrong...Please check if you have opened the file first or may be the current page is the last page..\n");
			}
			break;

		case 11:
			if (readLastBlock (&sm_FileHandle, memPage) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try..\n");
			}else{
				printf("\nPage contents : \n%s", memPage);
			}
			break;

		case 12:
			printf("Enter the block(page) number :");
			scanf("%d", &blockNo);
			printf("Enter a character to be written to the block :");
			scanf(" %c", &writeChar);
			memset(memPage, writeChar, PAGE_SIZE);
			if (writeBlock (blockNo, &sm_FileHandle, memPage) == RC_OK){
				printf("\nThe block has been filled with the given character !!");
			}else{
				printf("\nSomething went wrong...Please check if you have opened the file or enter a valid page number\n");
			}
			break;

		case 13:
			printf("Enter a character to be written to the block :");
			scanf(" %c", &writeChar);
			memset(memPage, writeChar, PAGE_SIZE);
			if(writeCurrentBlock (&sm_FileHandle, memPage) == RC_OK){
				printf ("\nCurrent block has been filled with the given character !!");
			}else{
				printf("\nFirst open the file and then try again\n");
			}
			break;

		case 14:
			if(appendEmptyBlock (&sm_FileHandle) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try..\n");
			}else{
				printf("\nEmpty block added to the file!!");
			}
			break;

		case 15:
			printf("Enter the number of pages:");
			scanf("%d", &numberOfPages);
			if(ensureCapacity (numberOfPages, &sm_FileHandle) == FIRST_OPEN_FILE){
				printf("\nFirst open a file and then try..\n");
			}else{
				printf ("\nThe total number pages are : %d", numberOfPages);
			}
			break;

		case 16:
			exit(0);

		default:
			printf("\nInvalid choice...Try again");
	}
}
}

