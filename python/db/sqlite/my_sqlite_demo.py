import sqlalchemy
from sqlalchemy import *


print "SQLite demo"

from sqlalchemy import create_engine

print "Creating engine"
engine = create_engine('sqlite:///data.db')
print "Connection establishment"
connection = engine.connect()

metadata = sqlalchemy.MetaData()

print "Creating table 'UsersTable'"
table1 = Table('UsersTable', metadata,
    Column('user_id', Integer, primary_key=True),
    Column('user_name', String(16), nullable=False),
    Column('email_address', String(60), key='email'),
    Column('password', String(20), nullable=False)
)
# drop if table exist first
table1.drop(engine, checkfirst = True)
#metadata.create_all(engine)
table1.create(engine, checkfirst = True)

print "Inserting data into table"
result = connection.execute("insert into UsersTable (user_id, user_name, password) values (1,'Aaa','xewd')")
result = connection.execute("insert into UsersTable (user_id, user_name, password) values (2,'Bbb','xewd')")
result = connection.execute("insert into UsersTable (user_id, user_name, password) values (3,'Ccc','xewd')")

print "Selecting data from table"
result = connection.execute("select user_name from UsersTable")
for row in result:
    print "	username:", row['user_name']

print "Dropping table 'UsersTable'"
#table1.drop(engine, checkfirst = True)

print "Closing conneciton"
connection.close()
