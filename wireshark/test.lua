local p_mtp = Proto( "test", "Test proto" )
local f_mtp = p_mtp.fields

f_mtp.msg	= ProtoField.none("cboss.mtp.msg",	"testMessage",		base.DEC)
f_mtp.len	= ProtoField.uint8("cboss.mtp.len",	"Length",		base.DEC)
f_mtp.tag	= ProtoField.uint8("cboss.mtp.tag",	"Tag",		base.DEC)


function p_mtp.dissector ( buf, pktinfo, tree )
	if buf:len() == 0 then return end
	item = tree:add( f_mtp.msg,  buf( 0, 10) )
	item:add( f_mtp.len,  buf( 0, 1) )
	item:add( f_mtp.tag,  buf( 1, 1) )
	--pktinfo.cols.info:set( "Test" )
	--pktinfo.cols.info:set( f_mtp.len.Fields():__tostring() )
	--pktinfo.cols.info:set( f_mtp.msg.text )
	pktinfo.cols.info:set( item.text )
	
end

-- Initialization routine
function p_mtp.init()
end

local udp_dissector_table = DissectorTable.get( "udp.port" )
udp_dissector_table:add( "25994", p_mtp )
