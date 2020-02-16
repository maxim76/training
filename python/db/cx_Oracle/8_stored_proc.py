"""
Example demostrates Oracle stored procedure execution.
Packaged procedure dbms_utility.db_version(version OUT varchar2, compatibility OUT varchar2) is called.
Procedures are called with cx_Oracle.Cursor.callproc(proc, [params])  
"""
import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()

# Output parameters definition
l_version = cursor.var(cx_Oracle.STRING)
l_compatibility = cursor.var(cx_Oracle.STRING)


# All parameters to stored procedure will be passed as a list (both input and output parmeters)
parameters = [l_version, l_compatibility]

print("Procedure parameters:")
print(parameters)

print('Executing stored procedure... ', end='')
# callproc executes stored function
result = cursor.callproc(
    'DBMS_UTILITY.DB_VERSION',   # stored procedure that will be called
    parameters)                 # list of procedure parameters

print('Done.')

# Access output parameters
print("Version: %s" % l_version.getvalue())
print("Compatibility: %s" % l_compatibility.getvalue())
