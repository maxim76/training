"""
Calculate GCD (Greatest common divisor) using Euclid's algorithm
"""

def gcd(a,b):
	'''
	divident = max(a,b)
	divisor = min(a,b)
	while(divident % divisor != 0):
		newDivisor = divident % divisor
		divident = divisor
		divisor = newDivisor
	return divisor
	'''
	divident = max(a,b)
	divisor = min(a,b)
	while (divisor):
		divident, divisor = divisor, divident % divisor

	return divident

assert( gcd(18, 84) == 6 )
assert( gcd(48, 180) == 12 )
