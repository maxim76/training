import cx_Oracle
import dbconfig

# create db connection
db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = db.cursor()
cursor.prepare('select OBJECT_NAME from user_objects where OBJECT_NAME like :object_name_template and rownum < 10')

paramValue='I%'
print('Executing prepared statement with template parameter value %s' % paramValue)
cursor.execute(None, {'object_name_template':paramValue})
for row in cursor:
    print(row)

paramValue='C%'
print('Executing prepared statement with template parameter value %s' % paramValue)
cursor.execute(None, {'object_name_template':paramValue})
for row in cursor:
    print(row)

