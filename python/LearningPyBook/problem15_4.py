"""
# Original
L = [1, 2, 4, 8, 16, 32, 64]
X = 5
found = False
i = 0
while not found and i < len(L):
	if 2 ** X == L[i]:
		found = True
	else:
		i = i+1

if found:
	print('at index', i)
else:
	print(X, 'not found')
"""

"""
# Subtask a
L = [1, 2, 4, 8, 16, 32, 64]
X = 5
found = False
i = 0
while i < len(L):
	if 2 ** X == L[i]:
		print('at index', i)
		break
	else:
		i = i+1
else:
	print(X, 'not found')
"""
"""
# Subtask b
L = [1, 2, 4, 8, 16, 32, 64]
X = 5
for item in L:
	if 2 ** X == item:
		print('at index', L.index(2**X))
		break
else:
	print(X, 'not found')
"""
"""
# Subtask c
L = [1, 2, 4, 8, 16, 32, 64]
X = 5
if 2 ** X in L:
	print('at index', L.index(2**X))
else:
	print(X, 'not found')
"""
"""
# Subtask d
L=[]
for item in range(7): L.append(2 ** item)
X = 5
if 2 ** X in L:
	print('at index', L.index(2**X))
else:
	print(X, 'not found')
"""
# Subtask f
L=map( lambda x: 2 ** x, range(7) )
X = 5
if 2 ** X in L:
	print('at index', L.index(2**X))
else:
	print(X, 'not found')
