import numpy as np

print( "Create 2D array P [5x3]" )
p = np.array([[300,301,302],[310,311,312],[320,321,322],[330,331,332],[340,341,342]], dtype=np.int16)
print( p )
print( "" )

print( "Create 2D array Q [2x3]" )
q = np.array([[200,201,202],[210,211,212]], dtype=np.int16)
print( q )
print( "" )

print( "Calculate differencies P[1] - Q[1]" )
print( np.abs(p[1] - q[1]) )
print( "" )

print( "Calculate sum of differencies P[1] - Q[1]" )
print( np.sum(p[1] - q[1]))
print( "" )


print( "Calculate differencies P[1] - Q" )
print( np.abs(p[1] - q) )
print( "" )


print( "Calculate sum of differencies P[1] - Q" )
print( np.sum( np.abs(p[1] - q) ) )
print( "" )

print( "Compute the sum of each row in resulting array" )
print( np.sum( np.abs(p[1] - q), axis=1 ) )
print( "" )




print( "Create 2D array where elementss are L1 distance from every item of P to every item of Q" )
pSize = p.shape[0]
qSize = q.shape[0]
dists=np.zeros((pSize, qSize), dtype=np.int16)
for i in range(pSize):
	for j in range(qSize):
		dists[i][j] = np.sum( np.abs(p[i] - q[j]) )

print( dists )
print( "" )

print( "Same as above, Approach 2" )
# Fill the whole row of dists without additional loops or list comprehensions
pSize = p.shape[0]
qSize = q.shape[0]
dists=np.zeros((pSize, qSize), dtype=np.int16)
for i in range(pSize):
	# Fill the whole row if distances. To make it, matrix if distances between tested p[i] and whole q is calculated, 
	# and then sum of each rows in this matrix is calculated, producing vector of sums of differencies
	dists[i] = np.sum( np.abs(p[i] - q), axis=1 )
print( dists )
print( "" )


print( "Same as above, Approach 3" )
# Implement computing all distances with no loops!
pSize = p.shape[0]
qSize = q.shape[0]
dists=np.zeros((pSize, qSize), dtype=np.int16)
featureSize=p.shape[1]
s = np.tile(p, qSize) - q.reshape(1,-1)
s = s.reshape(-1, featureSize)
s = np.sum( abs(s), axis=1)
dists = s.reshape(-1, qSize)
print( dists )
print( "" )

