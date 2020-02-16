"""
Example demostrates Oracle stored function execution.
Builtin function CONCAT(string1, string2) is called. Appends string2 to the end of string1.
Functions are called with cx_Oracle.Cursor.callfunc(proc, returnType, [params]). 
Functions require their return type to be defined in advance using class cx_Oracle.<TYPENAME>
"""
import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()

# All parameters to stored function will be passed as a list
parameters = ['ParamValue1', 'ParamValue2']

print("Input parameters:")
print(parameters)

print('Executing stored function... ', end='')
# callfunc executes stored function
result = cursor.callfunc(
    'CONCAT',               # stored function that will be called
    cx_Oracle.STRING,       # stored function return type - must be declared before function call
    parameters)             # list of function parameters (excluding returning parameter)

print('Done.')

print("Result is: %s" % result)
