import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()

try:
    print("Start transaction")
    db.begin()

    print("Execute SQL statement")
    cursor.execute('select sysdate from dual')
    print("Successfully executed")

    print('Commit transaction')
    db.commit()

    print('Printing result:')
    for row in cursor:
        print(row)

except:
    print('DB operation failed')
    print('Rollback transaction')
    db.rollback()
