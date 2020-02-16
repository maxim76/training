import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()
cursor.execute('select OBJECT_NAME from user_objects where rownum < 10')
for row in cursor:
    print(row)
