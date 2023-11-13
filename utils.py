import struct

def EncodeFakeCommand( payload ):
	try: payload = payload.encode( 'utf8' )
	except AttributeError: payload = bytes( payload )
	return bytes( [ len( payload ) ] ) + payload
	
def EncodeFakeSignalBlock( payload ):
	"""
	The payload can be a string, or a sequence of 8-bit numbers.
	A 14-byte header (including length encoding) will be prepended
	and a 2-byte footer will be appended.
	
	Use a 522-byte payload to match the length of the most usual
	kind of block (538 bytes).
	"""
	try: payload = payload.encode( 'utf8' )
	except AttributeError: payload = bytes( payload )
	payload += b'\x0D\x0A'
    restOfHeader = b'\xFF' * 10
    blockSize = 4 + len(restOfHeader) + len(payload)
	return b'\x23\x32' + struct.pack( '<H', blockSize ) + restOfHeader + payload
