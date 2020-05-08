"""
Demostrate how to call Oracle stored function.
Functions require their return type to be defined in advance using class cx_Oracle.<TYPENAME>
Functions are called with cx_Oracle.Cursor.callfunc(func, return_type, parameters=[], keywordParameters={})  
"""
import cx_Oracle
import dbconfig

# create db connection
connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

# All parameters to stored function will be passed as a list
parameters = ['Request created by stored function', ]

print("Input parameters:")
print(parameters)

print('Executing stored function... ', end='')
# callfunc executes stored function
result = cursor.callfunc(
    'PS.addrequestfuncwithcommit',              # stored function that will be called
    cx_Oracle.NUMBER,                           # stored function return type - must be declared before function call
    parameters)                                 # list of function parameters (excluding returning parameter)

print('Done.')

print("Result is: %d" % result)
