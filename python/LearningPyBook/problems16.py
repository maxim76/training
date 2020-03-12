def test():
	a = 10
	print("a = %d" % a)
	print("b = %d" % test.b)

print(dir(test))
test.b = 20
print(dir(test))
test()
