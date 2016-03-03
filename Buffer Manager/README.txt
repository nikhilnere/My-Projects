------------------------
WHAT IS BUFFER MANAGER ?
------------------------

	The buffer manager manages a fixed number of pages in memory that represent pages from a page file managed by the storage manager. The memory pages managed by the buffer manager are called page frames or frames for short. We call the combination of a page file and the page frames storing pages from that file a Buffer Pool. There is only one buffer pool for each page file. Each buffer pool uses one page replacement strategy that is determined when the buffer pool is initialized. There are two replacement strategies FIFO and LRU implemented in this buffer manager. 


------------
HOW TO RUN ?
------------
	Open terminal. Go to the location where applicaion files are present. Type below command to run testcases

		Command : make test


-----------------
HOW DOES IT WORK
-----------------

- When a pin page reques comes then the requested page is searched in the memory. If any of the frames contain that page then page pointer is returned
- If the requested page is not in the memory then the page is read from the disk and is put in the free frames
- If there are no empty frames avaialable then the page replacement strategy is used to replace an existing page in memory with the requested page
- There are two page replacement strategy used here - FIFO and LRU


-------------
FIFO STRATEGY
-------------

- The oldest page in the pool is picked to be replaced with the new page
- If the oldest page has a fixed count greater 0 that means if it being used then the next eligible page is picked by the FIFO algorithm


-------------
LRU STRATEGY
-------------
- As the name suggests the least recently used page is picked to replace with the new page
- If the least recenlty used page is being used then the next eliible page is picked.


-------------------
WHAT A USER CAN DO?
-------------------

1. Requesting a page : 

	When client requests for a page, buffer manager will first check whether the page is already in the page frame. In case the page is present, the pointer to page frame will be returned. If the page is not present, the buffer manager will first read it from the disk, and it will then decide in which page frame to load the page. After this is done, the buffer manager will then return the pointer to the page frame where the page is loaded.


2. Read / Modify page : 

	Once the page is loaded into page frame, client can read/ modify/ write them. After modifying the page, client will need to notify the buffer manager. If client does not notify about modifications, the changes made into page will be lost. When the buffer manager is notified about these modifications, it will evict the old pages and write the modified pages to the disk. Buffer manager will evict the page only when its fix count is 0. Otherwise, fix count being greater than zero indicates that the page is being used by other client(s). The modified page is called a dirty page.


-------------------
FILES IN THE FOLDER
-------------------
Below are the files and there purpose:

	- test_assign2_1.c
	------------------
		Contains the test cases to test the buffer manager application.

	-buffer_mgr.h
	-------------
		Provides buffer manager interface

	-buffer_mgr.c
	-------------
		Implements buffer manager interface. All buffer manager logic resides here

	- storage_mgr.h
	---------------
		Provides storage manager interface


	- storage_mgr.c
	---------------
		Implements storage manager interface. All storage manager logic resides here. 
	

	- test_helper.h, dberror.h, dberror.c
	-------------------------------------
		They provide testing and error handling framework for the storage manager application


	- buffer_mgr_stat.h
	-------------------
		buffer_mgr_stat.h provides several functions for outputting buffer or page content to stdout or into a string


	- buffer_mgr_stat.c
	-------------------
		Implements all the functions of the buffer_mgr_stat.h

	- makefile
	----------
		Compiles the code and executes it.



---------
FUNCTIONS
---------

1. Buffer Pool Functions
------------------------

	A. initBufferPool
	-----------------
	This function will create a buffer pool for an existing page file.
	1. numPages : Number of page frames in the buffer pool.
	2. strategy : The page replacement strategy used.
	3. pageFileName : File from which the pages will be cached into pool. This file must be existing. If the file is not existing, new file will not be created.
	4. stratData : The parameters for page replacement strategy are passed through stratData.


	B. shutdownBufferPool
	---------------------
	This function closes a buffer pool and releases all the associated resources with it. This will free the memory allocated for page frames. It will close the pool only if there are no more dirty pages in the buffer pool.

	
	C. forceFlushPool
	-----------------
	This function will force the buffer manager to write the dirty pages to the disk. All the dirty pages will be written to the disk only when their respective fix counts are 0.



2. Page Management Functions
----------------------------

	A. pinPage 
	----------
	This function is used to pin a page.
	1. pageNum : Its the page number with which the page is pinned.
	2. data : Its the field which points to the page frame in which the page is stored.


	B. unpinPage 
	------------
	It unpins the page 
	1. page : It is the page which is unpinned.
	2. pageNum : This is used to identify the page to be unpinned.


	C. markDirty 
	------------
	This function will mark a page as dirty. This means, page is been modified and requires to be written to the disk.


	D. forcePage 
	------------
	This will write the modified page to the disk.



3. Statistics Functions
-----------------------

	A. getFrameContents
	-------------------
	This function will return the statistic information about page numbers as an array of page numbers. ith element of the array will represent the number of pages stored in the ith frame.
	1. numPages: This is the size of the array. 


	B. getDirtyFlags
	----------------
	This function will return boolean type array. The ith boolean will represent whether the page in ith frame is dirty. If the page in ith frame is dirty, ith boolean value in array will be TRUE.
	1. numPages : It is the size of the array returned by this function.


	C. getFixCounts
	---------------
	This function will return an array of type int with information about fix count of pages stored in frames. The ith int in the array will represent fix count of the page stored in ith frame. For empty page frames, it will return 0.
	1. numPages :  This is the size of the array returned. 


	D. getNumReadIO 
	---------------
	This function will return the number of pages read from the disk since the buffer pool was initialized.

