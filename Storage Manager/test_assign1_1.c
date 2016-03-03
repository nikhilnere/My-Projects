#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

// test name
char *testName;

/* test output files */
#define TESTPF "test_pagefile.bin"

/* prototypes for test functions */
static void testCreateOpenClose(void);
static void testSinglePageContent(void);
static void testAppendEmptyBlock(void);
static void testReadNextPreviousblock(void);
static void testNonExistingPage(void);
static void testEnsureCapacity(void);
static void testOpenAlredyOpenedFile(void);


/* main function running all tests */
int
main (void)
{
  testName = "";
  
  initStorageManager();
  
  testCreateOpenClose();
  testSinglePageContent();
  testAppendEmptyBlock();
  testReadNextPreviousblock();
  testNonExistingPage();
  testEnsureCapacity();
  testOpenAlredyOpenedFile();
  return 0;
}


/* check a return code. If it is not RC_OK then output a message, error description, and exit */
/* Try to create, open, and close a page file */
void
testCreateOpenClose(void)
{
  SM_FileHandle fh;

  testName = "test create open and close methods";

  TEST_CHECK(createPageFile (TESTPF));
  
  TEST_CHECK(openPageFile (TESTPF, &fh));
  ASSERT_TRUE(strcmp(fh.fileName, TESTPF) == 0, "filename correct");
  ASSERT_TRUE((fh.totalNumPages == 1), "expect 1 page in new file");
  ASSERT_TRUE((fh.curPagePos == 0), "freshly opened file's page position should be 0");

  TEST_CHECK(closePageFile (&fh));
  TEST_CHECK(destroyPageFile (TESTPF));

  // after destruction trying to open the file should cause an error
  ASSERT_TRUE((openPageFile(TESTPF, &fh) != RC_OK), "opening non-existing file should return an error.");

  TEST_DONE();
}

/* Try to create, open, and close a page file */
void
testSinglePageContent(void)
{
  SM_FileHandle fh;
  SM_PageHandle ph;
  int i;

  testName = "test single page content";

  ph = (SM_PageHandle) malloc(PAGE_SIZE);

  // create a new page file
  TEST_CHECK(createPageFile (TESTPF));
  TEST_CHECK(openPageFile (TESTPF, &fh));
  printf("created and opened file\n");
  
  // read first page into handle
  TEST_CHECK(readFirstBlock (&fh, ph));
  // the page should be empty (zero bytes)
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == 0), "expected zero byte in first page of freshly initialized page");
  printf("first block was empty\n");
    
  // change ph to be a string and write that one to disk
  for (i=0; i < PAGE_SIZE; i++)
    ph[i] = (i % 10) + '0';
  TEST_CHECK(writeBlock (0, &fh, ph));
  printf("writing first block\n");

  // read back the page containing the string and check that it is correct
  TEST_CHECK(readFirstBlock (&fh, ph));
  for (i=0; i < PAGE_SIZE; i++)
    ASSERT_TRUE((ph[i] == (i % 10) + '0'), "character in page read from disk is the one we expected.");
  printf("reading first block\n");

  // destroy new page file
  TEST_CHECK(destroyPageFile (TESTPF));  
  
  TEST_DONE();
}

/*Tests if an empty block is added to the file and the current page position is pointing to the appended block*/
void testAppendEmptyBlock(void)
{
	int i;	
	SM_FileHandle fh;
	SM_PageHandle ph;
	ph = (SM_PageHandle) malloc(PAGE_SIZE);
	testName = "test append empty block";
	printf("Testing append empty block\n");
	//create a new page file
	TEST_CHECK(createPageFile(TESTPF));
	TEST_CHECK(openPageFile(TESTPF, &fh));
	
	//Append an empty block to file
	TEST_CHECK(appendEmptyBlock(&fh));

	TEST_CHECK(readLastBlock (&fh, ph));
	// the page should be empty (zero bytes)
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE((ph[i] == 0), "expected zero byte in appended page ");
	printf("Appended block is empty\n");

	ASSERT_TRUE((getBlockPos(&fh) == 1), "Empty block is added and the current pointer points to the added empty block");
	printf("Empty block added successfully\n");

	TEST_CHECK(destroyPageFile (TESTPF));  

	TEST_DONE();
}

/*Try to read next and previous block*/
void testReadNextPreviousblock(void){
	int i;	
	SM_FileHandle fh;
	SM_PageHandle ph;
	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	testName = "test read next and previous block";
	printf("Testing read next and previous block\n");

	//create a new page file
	TEST_CHECK(createPageFile(TESTPF));
	TEST_CHECK(openPageFile(TESTPF, &fh));
	
	//Ensure file size
	TEST_CHECK(ensureCapacity(2, &fh));
	
	//Write to the first block
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = 'a';
	TEST_CHECK(writeBlock(0, &fh, ph));
	printf("writing first block\n");

	//write to the second block
	for (i=0; i < PAGE_SIZE; i++)
		ph[i] = 'b';
	TEST_CHECK(writeBlock(1, &fh, ph));
	printf("writing to second block\n");

	//Now current pointer points to the second page.
	//read previous page
	TEST_CHECK(readPreviousBlock(&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE(ph[i] == 'a', "Expected data..previous page is being read");
	printf("Previous page is read\n");

	//read next page
	TEST_CHECK(readNextBlock(&fh, ph));
	for (i=0; i < PAGE_SIZE; i++)
		ASSERT_TRUE(ph[i] == 'b', "Expected data..next page is being read");
	printf("Next page is read\n");

	TEST_CHECK(destroyPageFile (TESTPF));  
	
	TEST_DONE();	
}

/*try to read-write a non existing page*/
void testNonExistingPage(void){
	SM_FileHandle fh;
	SM_PageHandle ph;
	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	testName = "test read-write non existing page";
	printf("testing read-write non existing page\n");

	//create a new page file
	TEST_CHECK(createPageFile(TESTPF));
	TEST_CHECK(openPageFile(TESTPF, &fh));
	
	//reading non existing pages
	ASSERT_TRUE(readNextBlock(&fh, ph) == RC_READ_NON_EXISTING_PAGE, "Next page does not exists - Expected!!");
	ASSERT_TRUE(readPreviousBlock(&fh, ph) == RC_READ_NON_EXISTING_PAGE, "Previos page does not exists - Expected!!");
	ASSERT_TRUE(readBlock(2, &fh, ph) ==  RC_READ_NON_EXISTING_PAGE, "File does not have 3rd page - Expected!!");
	
	//writing to a non existing page
	ASSERT_TRUE(writeBlock(3, &fh, ph) == RC_WRITE_NON_EXISTING_PAGE, "File does not have 4th page - Expected!!");
	printf("test read-write non existing page - Successful\n");

	TEST_CHECK(destroyPageFile (TESTPF)); 

	TEST_DONE();
}

/*try ensuring the capacity of the file*/
void testEnsureCapacity(void){
	SM_FileHandle fh;
	SM_PageHandle ph;
	ph = (SM_PageHandle) malloc(PAGE_SIZE);

	testName = "test ensure file capacity";
	printf("testing ensure file capacity\n");

	//create a new page file
	TEST_CHECK(createPageFile(TESTPF));
	TEST_CHECK(openPageFile(TESTPF, &fh));
	
	//ensure capacity
	TEST_CHECK(ensureCapacity(4,&fh));
	ASSERT_TRUE(getBlockPos(&fh) == 3, "Current block position is 3..File size ensured!!");
	printf("test ensure file capacity - Successful\n");

	TEST_CHECK(destroyPageFile (TESTPF)); 

	TEST_DONE();	
}

/*try to open a file which is already opened */
void testOpenAlredyOpenedFile(void){
	SM_FileHandle fh;
	testName = "test alredy open file";
	printf("test alredy open file\n");
	
	//create a new page file
	TEST_CHECK(createPageFile(TESTPF));
	
	//open the file
	TEST_CHECK(openPageFile(TESTPF, &fh));

	//try to open the same file again
	ASSERT_TRUE(openPageFile(TESTPF, &fh) == RC_FILE_ALREADY_OPEN, "Return code - file is alredy open");
	printf("test alredy opened file - Successful\n");
	
	TEST_CHECK(destroyPageFile (TESTPF)); 

	TEST_DONE();
}


