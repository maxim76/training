'''
Implement class that keeps N minimum elements and their unique ID/index
'''

class KMin:
	def __init__(self, k):
		self.k = k
		self.elements = dict()
		self.currMaxVal = None

	def process(self, val, index):
		'''
		Tests yet another pair of element/index. 
		If it's better that what already cintained, that new element substitutes less priority element that already in storage
		'''
		if (len(self.elements) < self.k):
			self.elements[index]=val
			if (self.currMaxVal is None) or (val > self.currMaxVal):
				self.currMaxVal = val
				self.currMaxNdx = index
		else:
			if val < self.currMaxVal:
				self.elements.pop(self.currMaxNdx)
				self.elements[index]=val
				# Find index of new maximum element
				def getDictValue(param):
					return self.elements[param]
				self.currMaxNdx = max(self.elements, key=getDictValue)
				self.currMaxVal = self.elements[self.currMaxNdx]

	def __repr__(self):
		return str(self.elements)


import random
l=[random.randint(0,100) for i in range(20)]
print l
kMin = KMin(3)
for index, item in enumerate(l):
	kMin.process(item, index)
#print(kMin.elements)
print(kMin)