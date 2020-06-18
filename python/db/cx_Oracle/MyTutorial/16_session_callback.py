"""
Applications can set session state in each connection. Examples of session state are NLS settings from ALTER SESSION statements. 
Pooled connections will retain their session state after they have been released back to the pool. 

The SessionPool() parameter sessionCallback enables efficient setting of session state so that connections have a known session state, 
without requiring that state to be explicitly set after each acquire() call.

If the sessionCallback parameter is a Python procedure, it will be called whenever acquire() will return a newly created database connection that has not been used before. 
It is also called when connection tagging is being used and the requested tag is not identical to the tag in the connection returned by the pool.
"""

import cx_Oracle
import dbconfig
import time

# Session callback to set specific session state
def initSession(connection, requestedTag):
    print("initSession() called")
    cursor = connection.cursor()
    cursor.execute("ALTER SESSION SET NLS_DATE_FORMAT = 'YYYY-MM-DD HH24:MI'")

# create pool of connections
pool = cx_Oracle.SessionPool(dbconfig.username, dbconfig.password, dbconfig.database, min=3, max=3, increment=0, sessionCallback=initSession, encoding="UTF-8")

for i in range(5):
    print("Test %d" % i)
    with pool.acquire() as connection:
        cursor = connection.cursor()
        cursor.execute('select to_char(sysdate) from dual')
        for row in cursor:
            print(row[0])
