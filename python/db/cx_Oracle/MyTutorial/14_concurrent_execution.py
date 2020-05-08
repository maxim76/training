"""
Demonstrate how multiple db operations can be executed simultaneously
Example uses cx_Oracle session pool.

cx_Oracle's connection pooling lets applications create and maintain a pool of connections to the database.
The internal implementation uses Oracle's session pool technology.
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
from threading import Thread, Lock

# create pool of connections
pool = cx_Oracle.SessionPool(dbconfig.username, dbconfig.password, dbconfig.database, min=2, max=5, increment=1, threaded=True)

# This sql statement imitates long running task
sql = r"""
    begin
        :o_id := ps.addrequestfunc(:i_request);
        dbms_lock.sleep(:i_delay);
        commit;
    end;
"""

def DBExecutor(executorID, lock):
    with pool.acquire() as connection:
        with lock:
            print("DBExecutor(%d) : started" % executorID)
        cursor = connection.cursor()
        l_id = cursor.var(cx_Oracle.NUMBER)
        params = {
            'i_request' : 'Request created by concurrent demo, executor #%d' % executorID,
            'i_delay' : 3,
            'o_id' : l_id
            }
        cursor.execute(sql, params)
        with lock:
            print("DBExecutor(%d) : done" % executorID)

executors = []
stdoutlock = Lock()
for i in range(3):
    t = Thread(target=DBExecutor, args=(i,stdoutlock))
    executors.append(t)
    print("main() : starting executor #%d" % i)
    t.start()

for t in executors:
    t.join()

print("main() : all executors completed")