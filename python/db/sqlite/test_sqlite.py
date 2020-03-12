import sqlalchemy

print "SQLite demo"
print "Use sqlalchemy. Version: ", sqlalchemy.__version__

from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

engine = create_engine('sqlite:///:memory:', echo=True)

from sqlalchemy.ext.declarative import declarative_base
Base = declarative_base()
from sqlalchemy import Column, Integer, String

class User(Base):
	__tablename__ = 'users'
	id = Column(Integer, primary_key=True)
	name = Column(String)
	fullname = Column(String)
	password = Column(String)

	def __repr__(self):
		return "<User(name='%s', fullname='%s', password='%s')>" % (
			self.name, self.fullname, self.password)

Base.metadata.create_all(engine)

ed_user = User(name='ed', fullname='Ed Jones', password='edspassword')

print "Username: ", ed_user.name
print "Password: ", ed_user.password

Session = sessionmaker(bind=engine)
session = Session()

session.add(ed_user)
our_user = session.query(User).filter_by(name='ed').first() 
print our_user
session.commit()

print "session.query(User).filter(User.name.in_(['ed', 'fakeuser'])).all()"
session.query(User).filter(User.name.in_(['ed', 'fakeuser'])).all()
