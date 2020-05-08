"""
Demostrate how to call Oracle stored procedure.
Procedures are called with cx_Oracle.Cursor.callproc(proc, parameters=[], keywordParameters={})  
"""
import cx_Oracle
import dbconfig

# create db connection
connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

# Output parameters definition
l_id = cursor.var(cx_Oracle.NUMBER)
l_descr = cursor.var(cx_Oracle.STRING)

# All parameters to stored function will be passed as a list
parameters = ['Request created by stored procedure', l_id, l_descr]

print("Procedure parameters:")
print(parameters)

print('Executing stored procedure... ', end='')
# callfunc executes stored function
result = cursor.callproc(
    'PS.addrequestprocoutwithcommit',           # stored procedure that will be called
    parameters)                                 # list of procedure parameters

print('Done.')

print("Procedure returned ID: %d" % l_id.getvalue())
print("Procedure returned Descr: %s" % l_descr.getvalue())
