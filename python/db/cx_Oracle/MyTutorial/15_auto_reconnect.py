"""
Demonstrate how pooled connections reconnect automatically after db outages.

Theory: FAN allows cx_Oracle to provide high availability features without the application being aware of an outage. 
Unused, idle connections in a connection pool will be automatically cleaned up. 
A future SessionPool.acquire() call will establish a fresh connection to a surviving database instance without the application being aware of any service disruption.

In this tutorial, single instance is used.
1. Run example, it will establich 3 connections to DB and will request sysdate every 1 sec. Oracle sessions view can be used to inspect that 3 sesisons are created.
2. Do 'shutdown immediate;' on oracle. Example will print DB is not available.
3. Do 'startup;' on oracle. Example will recover sysdate request and printing. Oracle sessions view can be used to inspect that 3 NEW sesisons are created.
"""

import cx_Oracle
import dbconfig
import time

# create pool of connections
pool = cx_Oracle.SessionPool(dbconfig.username, dbconfig.password, dbconfig.database, min=3, max=3, increment=0, threaded=True)

while True:
    try:
        with pool.acquire() as connection:
            cursor = connection.cursor()
            cursor.execute('select sysdate from dual')
            for row in cursor:
                print(row[0])
    except:
        print("DB is not available")
    time.sleep(1)
