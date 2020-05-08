"""
Demonstrate execution of an arbitrary sql with input parameter
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"select 'GeneratedText' from dual connect by level <= :rowCount"
params = {'rowCount' : 10}
cursor.execute(sql, params)

for row in cursor:
    print(row)
