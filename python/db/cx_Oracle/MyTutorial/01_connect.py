"""
Demonstrate connection and disconnection to Oracle DB
"""
import cx_Oracle
import dbconfig

connection = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
print("Connected!")
print("Oracle version : %s" % connection.version)
print("Database Server Name (DSN) : %s" % connection.dsn)
connection.close()