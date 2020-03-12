from optparse import OptionParser
parser = OptionParser()
parser.add_option("-f", "--file", dest="filename",
                  help="write report to FILE", metavar="FILE")
parser.add_option("-q", "--quiet",
                  action="store_false", dest="verbose", default=True,
                  help="don't print status messages to stdout")
parser.add_option("--srcip", dest="srcIP", help="Source IP address")
parser.add_option("--srcport", dest="srcPort", help="Source port")
parser.add_option("--dstip", dest="dstIP", help="Destination IP address")
parser.add_option("--dstport", dest="dstPort", help="Destination port")
parser.add_option("--data", dest="data", help="Hex dump to send")

(options, args) = parser.parse_args()

print options
print type(options)
