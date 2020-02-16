import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()
cursor.execute(
    'select OBJECT_NAME from user_objects where OBJECT_NAME like :object_name_template and rownum < 10',
    {'object_name_template':'%I'})

print('configured binding:')
print(cursor.bindnames())

print('Select result:')
for row in cursor:
    print(row)
