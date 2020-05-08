"""
Demonstrate execution of an arbitrary sql
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()
cursor.execute('select sysdate from dual')

for row in cursor:
    print(row)

