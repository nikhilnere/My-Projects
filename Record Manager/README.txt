------------------------
WHAT IS RECORD MANAGER ?
------------------------
	We have created a Record manager that handles tables with a fixed schema in which clients can insert records, delete records, update records, and can scan through the records. A scan will have search condition. The records which fulfills this condition will be returned. Tables are stored in seperate files accessed by record manager through buffer manager


------------
HOW TO RUN ?
------------
	Open terminal. Go to the location where applicaion files are present. Type below command to run testcases

		Command : make test
		
	To run the expr test cases use below command
		
		Command : make test_expr

	To delete the output files type below command
	
		Command : make clean

-----------------
HOW DOES IT WORK?
-----------------

- Record : A record ID that consists of a page number and slot number along with binary representation of its attributes according to schema.
- Schema : It consists of number of attributes named as numAttr. Attribute name (attrNames) and data type (dataTypes) is recorded for each attribute.
- Key :  A schema can have a key which is represented as an array of integers. This array represents the positions of the attributes of the key (keyAttrs).
- Expressions : It can be constants, references to attribute values and operator invocations.
- Operators : These can be either comparison or boolean operators with One or more expressions as input. 
- Record manager : It provides five types of functions(table and record manager management, handling the records in a table, scans, schemas and attribute values with creating records). 

-------------------
WHAT A USER CAN DO?
-------------------
	A user can create a schema and insert, update or delete records. Thsese records are managed with RIDs. Also, a user can scan through these records using a search condition called as expression. Matching records will then be displayed to the user as a result of scan.


-------------------
FILES IN THE FOLDER
-------------------
Below are the files and there purpose:

	- test_assign3_1.c
	------------------
		Contains the test cases to test the record manager application.

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

	- dt.h
	------
		Contains all the datatypes used throughout the application

	- expr.h
	--------
		expr.h declares data structures and functions to deal with expressions for scans.

	- expr.c
	--------
		expr.c implements the functions declared in expr.h		

	-record_mgr.h
		record_mgr.h declares five types of functions in the record manager: functions for 
			- table and record manager management, 
			- handling the records in a table,
			- scans,
			- dealing with schemas,
			- dealing with attribute values and creating records
	- makefile
	----------
		Compiles the code and executes it.


---------
FUNCTIONS
---------

1. Table and Record Manager Functions
-------------------------------------
	There are functions to initialize and shutdown a record manager and to create, open, and close a table.

	A. getNumTuples
	---------------
	This function returns the number of tuples in the table


2. Record Functions
-------------------
	These are functions to retrieve a record with a certain RID; delete, insert or update a record with RID

	A. initRecordManager
	--------------------
	This function initializes Record Manager
	1. mgmtData : Data passed as an argument

	B. shutdownRecordManager
	------------------------
	This function is used to close the Record Manager

	C. createTable
	--------------
	This function is used to create a schema
	1. name : name of the schema
	2. schema : The instance of schema to be created

	D. openTable
	------------
	This function is used to open the table for edition
	1. rel : reference to the table to be opened
	2. name : name of the table

	E. closeTable
	-------------
	This function is used to close down the table that was open before
	1. rel : reference to the table to be opened

	F. deleteTable
	--------------
	This function is used to delete an existing schema
	1. name : name of the schema

	G. getNumTuples
	---------------
	This function returns number of tuples in a table
	1. rel : reference to the table

	H. insertRecord
	---------------
	This function is used to insert a record.
	1. rel : reference to the table
	2. record : Its the instance of record that is supposed to be inserted

	I. deleteRecord
	---------------
	This function is used to delete a record.
	1. rel : reference to the table
	2. RID : Its the record ID that is supposed to be deleted

	J. updateRecord
	---------------
	This function is used to update a record.
	1. rel : reference to the table
	2. record : Its the instance of record that is supposed to be updated

	K. getRecord
	-------------
	This function is used to insert a record.
	1. rel : reference to the table
	2. RID : Its the record ID that is supposed to be inserted
	3. record : Its the instance of record that is supposed to be updated

3. Scan Functions
-----------------
	These are functions with which a user can initiate a scan to retrieve all tuples from a table given a certain condition that must be fulfilled.

	A. startScan
	------------
	This method initializes the scan.
	1. RM_TableData : Reference to the table
	2. RM_ScanHandle : A data sttructure passed to method to startScan
	3. Expr : Condition to fulfill
	
	B. next
	-------
	This function returns next tuple that fulfills the condition of scan
	1. RM_ScanHandle : A data sttructure passed to method to startScan
	2. Record : Reference to the record

	C. closeScan
	------------
	This function will close the scan.
	1. RM_ScanHandle : A data sttructure passed to method to startScan

4. Schema Functions
-------------------
	These are the helper functions to return the size in bytes of records for a given schema and create a new schema.
	
	A. getRecordSize
	----------------
	This function returns size of a schema
	1. Schema : Reference to the schema

	B. createSchema 
	---------------
	This function creates a schema and returns a reference to it
	1. numAttr : Number of attributes
	2. attrNames : Names of attributes
	3. dataTypes : Datatypes of atributes
	4. typeLength : length of datatype
	5. keySize : number of keys
	6. keys : All the keys

	C. freeSchema
	-------------
	This function frees up a schema
	1. Schema : Reference to the schema to be freed

5. Attribute Functions
----------------------
	These are the functions used to get or set the attribute values of a record and create a new record for a given schema. 

	A. createRecord 
	---------------
	This function creates a record
	1. Record : Instance of the record
	2. Schema : Schema in which record is to be created

	B. freeRecord 
	-------------
	This function frees up a record
	1. Record : Reference to the record to be freed up

	C. getAttr 
	----------
	This function returns the attribute of a record from a schema
	1. Record : Reference to the record
	2. Schema : reference to the schema
	3. attrNum : The number of attributes
	4. Value : A double pointer of value

	D. setAttr
	---------- 
	This function updates an attribute
	1. Record : Reference to the record
	2. Schema : Reference to the schema
	3. attrNum : Number of attribute
	4. Value Reference to the value

	
--------------
EXTRA CREDITS
--------------

	Tombstones and TIDs:
	--------------------

		1. Tombstones are stored in a linked list. The list consists of tomstoneNodes where each tomstoneNode cotains an RID and a pointer to the next tomstoneNode. This list is used in the record functions.

		2. While inserting in a record first the Tombstones list is traversed to see if there are any deleted records, if found the new record is inserted in its place
		
		3. While fetching a record (getRecord), if the record is found in the tomstones list then the search is termineted because the record to be searched is already deleted


