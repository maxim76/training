import numpy as np

print( "Simple array creation" )
a = np.array([2,3,4])
print( a.dtype, a )
a = np.array([1.2, 3.5, 5.1])
print( a.dtype, a )

print( "Create array with specified data type" )
a = np.array([2,3,4], dtype=np.int16)
print( a.dtype, a )
a = np.array([2,3,4], dtype=np.float)
print( a.dtype, a )

print( "2D array" )
a = np.array([(1.5,2,3), (4,5,6)])
print( a )

print( "Array reshaping" )
a = np.arange(15).reshape(3, 5)
print( a )

print( "Array properties" )
print( ".ndim = ", a.ndim )
print( ".shape = ", a.shape )
print( ".size = ", a.size )
print( ".dtype = ", a.dtype )
print( ".itemsize = ", a.itemsize )


print( "Creating predefined size array, no initialization" )
a = np.empty( (3,4) )
print( a )

print( "Creating predefined size array, filled with zeros" )
a = np.zeros( (3,4) )
print( a )

print( "Creating predefined size array, filled with ones" )
a = np.ones( (3,4) )
print( a )

print( "Creating array as a sequence of numbers" )
a = np.arange(10)
print( a )
a = np.arange(0, 2, 0.3)
print( a )

print( "Creating array as a distribution on defined element's count in linear space" )
a = np.linspace(0, 2, 9)
print( a )

print( "Creating array of random elements" )
a = np.random.random( (2, 3) )
print( a )


print( "Aggregation functions" )
print( "a.sum()=", a.sum() )
print( "a.min()=", a.min() )
print( "a.max()=", a.max() )


print( "Aggregation functions applied to specific axis only" )
print( a.max( axis=0 ) )
print( a.max( axis=1 ) )


print( "Using element-wise arithmetic operators" )
a = np.arange( 10 )
b = np.ones( 10 )
print (a + b )
print (a * b )	# Not a matrix mul, but just a element-wise product
print ( a**2 )
print ( a < 5 )

print( "Using built-in numpy element-wise functions" )
a = np.linspace( 0, 2*np.pi, 8 )
b = np.sin(a)
print( b )

print( "Matrix multiplication" )
a = np.array( [[1,1], 
				[0,1]] )
b = np.array( [[2,0], 
				[3,4]] )
c = a @ b
print( c )


print( "Multidimensional slicing" )
a = np.arange(24).reshape(4,6)
print( "Original:" )
print( a )
print( "Slice" )
print( a[1:3, 1:4] )

print( "Save array to file" )
np.save( open( "data.npy", "wb" ), a, allow_pickle=False )
print( "Read array from file" )
b = np.load( "data.npy" )
print( b )

