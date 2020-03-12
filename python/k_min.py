"""
Class to keep K minimal elements
"""

class KMin:
	def __init__( self, k ):
		self.k = k
		self.elements = dict()
		self.currMax = None		# Keep maximim element that is candidate to being replaced

	def process( val, index ):
		if len(self.elements) < self.k:
			self.elements[index] = val
			self.currMax = max(self.currMax, val) if self.currMax is not None else val
		else:
			if val < self.currMax:
				self.currMax = v
