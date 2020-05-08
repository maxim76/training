"""
Demonstrate creation of cursor.

Below is explanation what cursor is.
Oracle cursors
==============
Oracle cursor is a pointer that points to a result of a query. PL/SQL has two types of cursors: implicit cursors and explicit cursors.
Implicit cursors
----------------
Whenever Oracle executes an SQL statement such as SELECT INTO, INSERT, UPDATE, and DELETE, it automatically creates an implicit cursor.
Oracle internally manages the whole execution cycle of implicit cursors and reveals only the cursor’s information and statuses such as SQL%ROWCOUNT, SQL%ISOPEN, SQL%FOUND, and SQL%NOTFOUND.
The implicit cursor is not elegant when the query returns zero or multiple rows which cause NO_DATA_FOUND or TOO_MANY_ROWS exception respectively.

Explicit cursors
----------------
An explicit cursor is an SELECT statement declared explicitly in the declaration section of the current block or a package specification. 
For an explicit cursor, you have control over its execution cycle from OPEN, FETCH, and CLOSE.

cx_Oracle cursors
=================
In cx_Oracle library, cursor is the object that allows statements to be executed and results (if any) fetched. cx_Oracle cursor can be considered as a gateway to Oracle cursor.
Connection object is responsible for cursors creation. Arbitrary number of cursors can be created using cursor() method of the Connection object.
Simple programs will do fine with just a single cursor, which can be used over and over again. Larger projects might however require several distinct cursors.

Some of cursor's methods are:
execute
callfunc
callproc
fetchone
fetchall
prepare
parse
bindnames
bindvars
...
"""

import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
print("Connected!")
cursor = connection.cursor()
print("Cursor is created!")
print("Cursor connection : %s" % cursor.connection)
print("Cursor description : %s" % cursor.description)
cursor.close()
connection.close()