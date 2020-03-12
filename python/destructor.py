import gc

class Test:
	def __init__(self, name):
		self.name = name
		print("Constructor : " + self.name)
	def __del__(self):
		print("Destructor : " + self.name)

print("1")
t = Test("A")
print("References: " + str(len(gc.get_referrers(t))))
print("2")
t = Test("B")
print("References: " + str(len(gc.get_referrers(t))))
print("3")
