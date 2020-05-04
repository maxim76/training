"""
Example demostrates usage of Oracle session pool.

cx_Oracle’s connection pooling lets applications create and maintain a pool of connections to the database. 
The internal implementation uses Oracle’s session pool technology. 
In general, each connection in a cx_Oracle connection pool corresponds to one Oracle session.

A connection pool is created by calling SessionPool()
Connections can then be obtained from a pool by calling acquire()
Connections acquired from the pool should be released back to the pool using SessionPool.release() or Connection.close() 
when they are no longer required. Otherwise, they will be released back to the pool automatically when all of the 
variables referencing the connection go out of scope. The session pool can be completely closed using SessionPool.close().

In the example, two SQL statements are run in parallel, each uses it's own oracle session, obtained from the pool.
"""

import cx_Oracle
import dbconfig
import threading

# create pool of connections
pool = cx_Oracle.SessionPool(dbconfig.username, dbconfig.password, dbconfig.database, min=2, max=5, increment=1, threaded=True)

def DBTask1():
    with pool.acquire() as connection:
        cursor = connection.cursor()
        print("DBTask1(): Start execution...")
        cursor.callproc("dbms_lock.sleep", (5,))
        print("DBTask1(): done")

def DBTask2():
    with pool.acquire() as connection:
        cursor = connection.cursor()
        print("DBTask2(): Start execution...")
        cursor.execute('select OBJECT_NAME, OBJECT_TYPE from user_objects where rownum < 20')
        for row in cursor:
            print(row)
        print("DBTask2(): done")

thread1 = threading.Thread(target=DBTask1)
thread1.start()

thread2 = threading.Thread(target=DBTask2)
thread2.start()

thread1.join()
thread2.join()

print("Example completed")