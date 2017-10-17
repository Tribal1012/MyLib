def hexdump(string):
	for line in range(0, ((len(string)/16)+1), 1):
		print '%04x: ' % line + ' '.join(x.encode('hex') for x in string[(line*16):((line*16)+16)])
	#print ' '.join(x.encode('hex') for x in string)

hexdump("A"*100+"B"*100)
