def getParams( **kwargs ):
	for key, value in kwargs.iteritems():
		print( "Attrib=%s  Value=%s" % (key, value) )
		setattr( obj, key, value )

class Obj:
	pass

class Obj2:
	def __init__( self, **kwargs ):
		for key, value in kwargs.iteritems():
			setattr( self, key, value )

obj = Obj()
getParams( sParam1='Value1', sParam2='Value2', iParam1=123 )
print( "obj contains the following attributes: " + str ( dir(obj) ) )

obj2 = Obj2( sParam1='Value11', sParam2='Value22', iParam1=123, iParam2 = 456 )
print( "obj2 contains the following attributes: " + str ( dir(obj2) ) )

for attr, value in obj2.__dict__.iteritems():
	print( attr, value )
