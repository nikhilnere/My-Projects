#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buffer_mgr.h"
#include "storage_mgr.h"
#include "dberror.h"
#include "dt.h"

#define FRM_CONTENTS 0
#define DIRTY_FLAG 1
#define FIX_COUNT 2
#define MAX_FRAMES 100
#define LRU_WEIGHT 3

int writeIOCount, readIOCount;            // used to store IO count
int **bufferPoolStatistics;   // stores buffer pool statistics about the buffer pool
SM_FileHandle *fHandle;
int oldestFrame;	//used for FIFO replacement strategy

BM_PageHandle *frames[MAX_FRAMES];

/**
 * Initializes the buffer pool manager
 */
RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData){

	int i;

	fHandle = (SM_FileHandle *)malloc(sizeof(SM_FileHandle));
	openPageFile((char *)pageFileName, fHandle); //open pageFile

	int oldestFrame = 0;	//used for FIFO. Initially point to the first frame
	writeIOCount = 0;
	readIOCount = 0;
	bm->pageFile = (char *) pageFileName; //Name of the page file
	bm->numPages = numPages; //number of page frames
	bm->strategy = strategy; //page replacement strategy

	//allocatin memory for buffer pool statistics
	bufferPoolStatistics = (int**) malloc (4 * (sizeof (int*)));
	for(i=0;i<4;i++)
	{
		*(bufferPoolStatistics+i)=(int*)malloc(sizeof(int)*numPages);
	}

	for (i=0; i < numPages; i++){
		bufferPoolStatistics[FRM_CONTENTS][i] = -1;
		bufferPoolStatistics[DIRTY_FLAG][i] = 0;
		bufferPoolStatistics[FIX_COUNT][i] = 0;
		bufferPoolStatistics[LRU_WEIGHT][i] = 0;

		frames[i] = (BM_PageHandle *) malloc (sizeof(BM_PageHandle));	//Creating a frame
		frames[i]->data = (char*)calloc(PAGE_SIZE, sizeof(char));	//allocating memory of PAGE_SIZE in a frame
		frames[i]->pageNum = -1;	//initially frame does not contain any page
	}

	return RC_OK;
}

/**
 * Closes the buffer pool and frees the memory allocated
 */
RC shutdownBufferPool(BM_BufferPool *const bm){

	int i = 0;
	int shutDwnflag = 1;

	//checking if any frame is being used if so setting flag to false
	for(i = 0 ; i < bm->numPages ; i++){
		if (bufferPoolStatistics[FIX_COUNT][i] > 0)
			shutDwnflag = 0;	//can not shut down the buffer pool
	}

	if (shutDwnflag){
		printf("Shutdown command accepted\n");
		//shutting down the buffer pool

		//writing all dirty pages to the disk
		forceFlushPool(bm);
		free (bufferPoolStatistics);
		closePageFile(fHandle);
	}else{
		//TODO return error code that the buffer manager can be shut down
		printf("\nCan not shut down buffer pool manager\n");
	}

}

/**
 * Writes all the dirty pages to the file
 */
RC forceFlushPool(BM_BufferPool *const bm){

	int i;
	for (i = 0 ; i < bm -> numPages ; i++){
		if (bufferPoolStatistics[DIRTY_FLAG][i] && bufferPoolStatistics[FIX_COUNT][i] == 0){	//check if the page if dirty and is not being used
			writeBlock(frames[i]->pageNum, fHandle, frames[i]->data);
			bufferPoolStatistics[DIRTY_FLAG][i] = 0;
			writeIOCount++;
		}
	}
}

/**
 * Reads the given page from disk and load it in to a frame
 */
RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum){

	int i;

	//ensuring file size
	ensureCapacity(pageNum + 1, fHandle);

	switch (bm->strategy){
		case RS_FIFO:
			return pinPage_rs_FIFO(bm, page, pageNum);
			break;
		case RS_LRU:
			return pinPage_rs_LRU(bm, page, pageNum);
			break;
		default:
			return INVALID_STRATEGY;
			break;
	}


	return RC_OK;
}

/**
 * Applies FIFO page replacement strategy
 */
RC pinPage_rs_FIFO(BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum){

	/**
	 * This FIFO algorithm picks the oldest page to replace it with a new page.
	 * If the oldest page is being used then it finds the next available older page.
	 * If all the pages are being used then an error code is returned saying ALL PAGES IN USE CANNOT REPLACE
	 */
	int framesFull = 1;
	int foundInFrame = 0;
	int nextAvailableFrame;
	int i;

	//searching if the page is present in the frame
	for(i=0; i< bm->numPages ; i++){
		if (frames[i]->pageNum == pageNum){
			page->data = frames[i]->data;
			page->pageNum = frames[i]->pageNum;
			foundInFrame = 1;
			bufferPoolStatistics[FIX_COUNT][i]++;
			return RC_OK;
		}
	}

	//If the page is not found in any of the frames, then read it from the disk and put in a frame
	if(!foundInFrame){
		for (i=0; i < bm->numPages; i++){
			if(frames[i]->pageNum == -1){
				readBlock(pageNum, fHandle, frames[i]->data);	//read page from pageFile in the frame
				frames[i]->pageNum = pageNum;
				bufferPoolStatistics[FRM_CONTENTS][i] = pageNum;
				bufferPoolStatistics[FIX_COUNT][i]++;
				readIOCount++;
				page->data = frames[i]->data;
				page->pageNum = frames[i]->pageNum;
				framesFull = 0;

				return RC_OK;
			}
		}
		//Apply replacement strategy if the frames are full
		if (framesFull){
			//checking if the oldest frame is being used.
			if (bufferPoolStatistics[FIX_COUNT][oldestFrame] == 0){
				//If the page to be removed is dirty then write the new page content to the file
				if(bufferPoolStatistics[DIRTY_FLAG][oldestFrame]){
					page->pageNum = frames[oldestFrame]->pageNum;
					forcePage (bm, page);
				}
				readBlock(pageNum, fHandle, frames[oldestFrame]->data);	//read page from pageFile in the frame
				frames[oldestFrame]->pageNum = pageNum;
				bufferPoolStatistics[FRM_CONTENTS][oldestFrame] = pageNum;
				bufferPoolStatistics[FIX_COUNT][oldestFrame]++;
				readIOCount++;
				page->data = frames[oldestFrame]->data;
				page->pageNum = frames[oldestFrame]->pageNum;

				oldestFrame = (oldestFrame + 1) % bm->numPages; //calculate the next oldest frame for future replacement
				return RC_OK;
			}else{
				//The oldest frame is in use so finding the next available older frame
				nextAvailableFrame = oldestFrame;
				for(i=0;i<bm->numPages-1;i++){
					nextAvailableFrame = (nextAvailableFrame + 1) % bm->numPages; //checking the next oldest frames
					if (bufferPoolStatistics[FIX_COUNT][nextAvailableFrame] == 0){
						//If the page to be removed is dirty then write the new page content to the file
						if(bufferPoolStatistics[DIRTY_FLAG][nextAvailableFrame]){
							page->pageNum = frames[nextAvailableFrame]->pageNum;
							forcePage (bm, page);
						}
						readBlock(pageNum, fHandle, frames[nextAvailableFrame]->data);	//read page from pageFile in the frame
						frames[nextAvailableFrame]->pageNum = pageNum;
						bufferPoolStatistics[FRM_CONTENTS][nextAvailableFrame] = pageNum;
						bufferPoolStatistics[FIX_COUNT][nextAvailableFrame]++;
						readIOCount++;
						page->data = frames[nextAvailableFrame]->data;
						page->pageNum = frames[nextAvailableFrame]->pageNum;
						return RC_OK;
					}
				}
			}
		}
	}
	return ALL_PAGES_IN_USE_CANNOT_REPLACE;
}

/**
 * Applies LRU page replacement strategy
 */
RC pinPage_rs_LRU(BM_BufferPool *const bm, BM_PageHandle *const page,const PageNumber pageNum){

	/**
	 * This LRU algorithm works on weight given to each page. The latest pinned page will have the higher weight that is equal to the total number of
	 * frames in the pool. The frame with lower weight will be picked to be replaced. The lower weight will be 1.
	 * If a page is pinned then it will have maximum weight and the weight of the other pages will be reduced by one.
	 * if page with the lowest weight is being used then next available page with lower weight will be picked.
	 * If all the pages are being used then an error code is returned saying ALL PAGES IN USE CANNOT REPLACE.
	 */

	int framesFull = 1;
	int foundInFrame = 0;
	int i,j, lowWeight, replaced;

	//searching if the page is present in the frame
	for(i=0; i< bm->numPages ; i++){
		if (frames[i]->pageNum == pageNum){
			page->data = frames[i]->data;
			page->pageNum = frames[i]->pageNum;
			foundInFrame = 1;
			bufferPoolStatistics[FIX_COUNT][i]++;

			//Calculating weight for replacement strategy
			bufferPoolStatistics[LRU_WEIGHT][i] = bm->numPages;	//maximum weight since it is recently read
			for (j = 0; j < bm->numPages; j++){
				if (i != j && bufferPoolStatistics[LRU_WEIGHT][j]>0){
					bufferPoolStatistics[LRU_WEIGHT][j]--;	//reducing weight of other pages by 1
				}
			}
			return RC_OK;
		}
	}

	//If the page is not found in any of the frames, then read it from the disk and put in a frame
	if(!foundInFrame){
		for (i=0; i < bm->numPages; i++){
			if(frames[i]->pageNum == -1){
				readBlock(pageNum, fHandle, frames[i]->data);	//read page from pageFile in the frame
				frames[i]->pageNum = pageNum;
				bufferPoolStatistics[FRM_CONTENTS][i] = pageNum;
				bufferPoolStatistics[FIX_COUNT][i]++;
				readIOCount++;
				page->data = frames[i]->data;
				page->pageNum = frames[i]->pageNum;
				framesFull = 0;

				//Calculating weight for replacement strategy
				bufferPoolStatistics[LRU_WEIGHT][i] = bm->numPages;	//maximum weight since it is recently read
				for (j = 0; j < bm->numPages; j++){
					if (i != j && bufferPoolStatistics[LRU_WEIGHT][j]>0){
						bufferPoolStatistics[LRU_WEIGHT][j]--;	//reducing weight of the other pages by 1
					}
				}
				return RC_OK;
			}
		}
		//Apply replacement strategy if the frames are full
		if (framesFull){
			replaced = 0;
			lowWeight = 1;
			while (!replaced){
				for(i=0; i<bm->numPages; i++){
					if(bufferPoolStatistics[LRU_WEIGHT][i] == lowWeight && bufferPoolStatistics[FIX_COUNT][i] == 0){
						if (bufferPoolStatistics[DIRTY_FLAG][i]){
							page->pageNum = frames[i]->pageNum;	//force page if it is dirty and then replace
							forcePage (bm, page);
						}
						readBlock(pageNum, fHandle, frames[i]->data);	//read page from pageFile in the frame
						frames[i]->pageNum = pageNum;
						bufferPoolStatistics[FRM_CONTENTS][i] = pageNum;
						bufferPoolStatistics[FIX_COUNT][i]++;
						readIOCount++;
						page->data = frames[i]->data;
						page->pageNum = frames[i]->pageNum;
						replaced = 1;		//replaced page.. stop searching for the next lower
						//Calculating weight for replacement strategy
						bufferPoolStatistics[LRU_WEIGHT][i] = bm->numPages;	//maximum weight since it is recently read
						for (j = 0; j < bm->numPages; j++){
								if (i != j && bufferPoolStatistics[LRU_WEIGHT][j]>lowWeight){
								bufferPoolStatistics[LRU_WEIGHT][j]--;	//reducing weight of the other pages by 1
							}
						}
						return RC_OK;
					}
				}
				lowWeight++; //if the lowest weight page is being used then search the next lower
			}
		}
	}

	return ALL_PAGES_IN_USE_CANNOT_REPLACE;
}

/**
 * Decreases the fix count of the page indicating that the user is no more using the page
 */
RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page){
	int i;
	int foundFlag = 0;
	//TODO if a gage is unpinned then what to do? should it be written to the disk? and removed from the pool?
	for (i=0; i < bm->numPages; i++){
		if(frames[i]->pageNum == page->pageNum && bufferPoolStatistics[FIX_COUNT][i]>0){
			bufferPoolStatistics[FIX_COUNT][i]--;
			foundFlag = 1;
			break;
		}
	}
	if (foundFlag){
		return RC_OK;
	}else{
		return PAGE_NOT_PRESENT_IN_FRAME;
	}
}

/**
 * Marks the given page dirty. User has changed the contents of the page
 */
RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page){
	int i;
	int foundFlag = 0;
	for (i=0; i < bm->numPages; i++){
		if (frames[i]->pageNum == page->pageNum){
			frames[i]->data = page->data;
			bufferPoolStatistics[DIRTY_FLAG][i] = 1;
			foundFlag = 1;
			break;
		}
	}
	if (foundFlag){
		return RC_OK;
	}else{
		return PAGE_NOT_PRESENT_IN_FRAME;
	}

}

/**
 * Writes the given page to disk
 */
RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page){
	int i;
	for(i=0 ; i < bm->numPages ; i++){
		if (frames[i]->pageNum == page->pageNum){
			if(bufferPoolStatistics[FIX_COUNT][i] == 0){
				writeBlock(page->pageNum, fHandle, frames[i]->data);
							bufferPoolStatistics[DIRTY_FLAG][i] = 0;
							writeIOCount++;
							return RC_OK;
			}else{
				return PAGE_IN_USE_CANNOT_FLUSH;
			}
		}
	}
	return PAGE_NOT_PRESENT_IN_FRAME;
}

/**
 * Returns the frame contents
 */
PageNumber *getFrameContents (BM_BufferPool *const bm){
	return bufferPoolStatistics[FRM_CONTENTS];
}

/**
 * Returns the dirty flags
 */
bool *getDirtyFlags (BM_BufferPool *const bm){
	int i;
	bool *dirtyFlags = (bool *)malloc(bm->numPages *(sizeof(bool *)));

	for(i=0 ; i<bm->numPages ; i++){
		if(bufferPoolStatistics[DIRTY_FLAG][i]){
			dirtyFlags[i] = true;
		}else{
			dirtyFlags[i] = false;
		}
	}
	return dirtyFlags;
}

/**
 * Returns the fix count
 */
int *getFixCounts (BM_BufferPool *const bm){
	return bufferPoolStatistics[FIX_COUNT];

}

/**
 * returns readIO count
 */
int getNumReadIO (BM_BufferPool *const bm){
	return readIOCount;
}

/**
 * returns writeIO count
 */
int getNumWriteIO (BM_BufferPool *const bm){
	return writeIOCount;
}

int *getWeight (){
	return bufferPoolStatistics[LRU_WEIGHT];
}
