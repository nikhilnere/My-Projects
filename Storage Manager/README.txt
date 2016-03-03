------------------------
WHAT IS STORAEG MANAGER?
------------------------
	This is a simple storage manager which is capable of reading blocks from a file on disk into memory and writing blocks from memory to a file on disk. The storage manager deals with pages (blocks) of fixed size. In addition to reading and writing pages from a file, it provides methods for creating, opening, and closing files. The storage manager maintains several types of information for an open file like total number of pages in the file, current page position (for reading and writing), the file name, and FILE pointer. 


-----------
HOW TO RUN?
-----------
	Open terminal. Go to the location where applicaion files are present. There are two commands to run the application one to execute the test cases and the other to start the applicaion with user interface

	1) Run test cases :
		Command : make test

	2) Run application throught user interface:
		Command : make testui


--------------------------
HOW TO USE USER INTERFACE?
--------------------------
It is a menu driven program that takes choices from user for performing tasks.  
Following are the choices provided to the user:	

	1. Create page file 
		Creates a new page file with an empty page in it. Accepts file name as input.

	2. Open page file
		Opens the page file specified by user. If the file does not present on the disk it gives a message "File does not found". 

	3. Close page file
		Closes the page file.

	4. Destroy page file
		Deletes the page file specified by the user.

	5. Read block
		Reads the block number given as input. Any random block can be read with this function.

	6. Get current block position
		Returns the current block position.

	7. Read first block
		Reads first block in the file 

	8. Read previous block
		Reads one block previous to the "curPagePos"th block of file.

	9. Read current block
		Reads "curPagePos"th block of file.

	10. Read next block
		Reads one block next to the "curPagePos"th block of file.

	11. Read last block
		Reads the last block of the file.

	12. Write to a block
		Writes the content to the specified block. Accepts block number as input.

 	13. Write to the current block
		Writes the current block pointed by curPagePos to the file.
	
	14. Append empty block
		Appends an empty block to the file.

	15. Ensure capacity of the file
		Accepts the capacity i.e. number of pages user wants a file to have. If the file has lesser pages, then this function will make sure the capacity by adding extra pages.
	
	16. Exit
		This choice will exit the program.



-------------------
FILES IN THE FOLDER
------------------- 
	Below are the files and there purpose:

	- main.c
	---------
		Provides user interface to access the application. It gives user the choice of operations to perform. After taking input from user, it links user choices to the appropriate functionality through internal calls.


	- test_assign1_1.c
	------------------
		Contains the test cases to test the storage manager application.


	- storage_mgr.h
	---------------
		Provides an storage manager interface


	- storage_mgr.c
	---------------
		Implements storage manager interface. All the logic resides here. 
	

	- test_helper.h, dberror.h, dberror.c
	-------------------------------------
		They provide testing and error handling framework for the storage manager application


	- makefile
	----------
		Compiles the code and executes it.
