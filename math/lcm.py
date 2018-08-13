"""
Calculate LCM (lowest common multiple)
"""

def gcd(a,b):
	divident = max(a,b)
	divisor = min(a,b)
	while (divisor):
		divident, divisor = divisor, divident % divisor

	return divident


def lcm( a, b ):
	gcdAB = gcd(a,b)
	return a*b/gcdAB


assert( lcm(48, 180) == 720 )
