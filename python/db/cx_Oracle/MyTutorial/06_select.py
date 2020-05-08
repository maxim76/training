"""
Demonstrate selection from table using input parameter

Before running this demo, run demo 04_insert.py one or more times, to create some records in the table
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
cursor = connection.cursor()

sql = r"""select * from REQUESTS where DETAILS = :i_details order by DT"""
params = {':i_details' : 'Sample request text'}
cursor.execute(sql, params)

print("%-10s%-24s%s" % ('ID', 'Date/Time', 'Details'))
for row in cursor:
    #print(row)
    print("%-10s%-24s%s" % (row[0], row[1], row[2]))

connection.close()

