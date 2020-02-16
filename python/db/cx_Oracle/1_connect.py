import cx_Oracle
import dbconfig

try:
    print("Trying to connect to DB...    ", end='')
    db = cx_Oracle.connect(dbconfig.username, dbconfig.password, dbconfig.database)
    print("Connected!")
    print("Oracle version : %s" % db.version)
    print("Database Server Name (DSN) : %s" % db.dsn)
    db.close()
except:
    print("Failed connect to DB")
