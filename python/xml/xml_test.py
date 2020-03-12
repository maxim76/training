# ElementTree XML API Demo
# https://docs.python.org/2/library/xml.etree.elementtree.html

import xml.etree.ElementTree as ET

tree = ET.parse('country_data.xml')
root = tree.getroot()

print "\nRoot name:"
print root.tag

print "\nChilds:"
for child in root:
	print child.tag, child.attrib

print "\nFind  elements with a tag which are direct children of the current element:"
for country in root.findall('country'):
	rank = country.find('rank').text
	name = country.get('name')
	print name, rank

print "\nIterate recursively over all the sub-tree below it (its children, their children, and so on)"
for neighbor in root.iter('neighbor'):
	print neighbor.attrib
