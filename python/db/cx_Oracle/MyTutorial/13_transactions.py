"""
Demostrate how to use transactions.
"""
import cx_Oracle
import dbconfig

# create db connection
connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

# All parameters to stored function will be passed as a list
parameters = ['Request created by transaction demo', ]

try:
    print("Start transaction")
    connection.begin()

    print('Executing stored function... ', end='')
    result = cursor.callfunc('PS.addrequestfunc', cx_Oracle.NUMBER, parameters)
    print('Done.')

    print('Commit transaction')
    connection.commit()

    print("Result is: %d" % result)

except:
    print('DB operation failed')
    print('Rollback transaction')
    connection.rollback()
