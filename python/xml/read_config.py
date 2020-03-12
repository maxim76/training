import xml.etree.ElementTree as ET

class ConfigLoader( object ):

	def __init__( self, configFile ):
		self.tree = ET.parse( configFile )
		if self.tree is None:
			print "Cannot read/parse file %s" % configFile

	def getMachineList( self ):
		section = self.tree.find( "direct/machineList" )
		if section is None:
			print "direct/machineList sectio is not found"
			return None
		machineList = [ {'name' : machine.get( "hostname" ), 'nCore' : machine.find( "nCore" ).text} for machine in section.findall( "machine" )]

		return machineList
# ConfigLoader


# Unit-test
if __name__ == "__main__":
	print ConfigLoader( "settings.xml" ).getMachineList()
