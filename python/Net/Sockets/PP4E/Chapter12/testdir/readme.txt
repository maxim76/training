----------------
dir([object]) 
----------------
fetch a list of all the names available inside scope of <object> (or current scope if no object arg passed)

Examples:
dir(), dir(<module_name>), dir(<var_name>), dir(<class_name/class_object_name>)

----------------
help(<method_name>)
----------------
Examples:
S = 'qwerty'
help(S.replace) or help(''.replace)
L=[1,2,3]
help(L.append) or help([].append)

----------------
type(<object>)
----------------
Prints type of object


Comprehensions (list comprehension expression)
----------------
List comprehensions derive from set notation; they are a way to build a new list by
running an expression on each item in a sequence, one at a time, from left to right.
Example:
>>> [[x ** 2, x ** 3] for x in range(4)]
[[0, 0], [1, 1], [4, 8], [9, 27]]

>>> M = [[1, 2, 3], # A 3 × 3 matrix, as nested lists
		 [4, 5, 6], # Code can span lines if bracketed
		 [7, 8, 9]]
>>> col2 = [row[1] for row in M] # Collect the items in column 2
>>> col2
[2, 5, 8]

