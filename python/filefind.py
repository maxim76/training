import glob
PATH2REPLAY = r"\\qahome\qa\process_replay_replays\*.replay"
print 'List files in ' + PATH2REPLAY
#fileList = glob.glob( '*.py' )
fileList = glob.glob( PATH2REPLAY )
for item in fileList:
	print item
