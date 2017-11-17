from struct import pack, unpack
import idautils
import idaapi

def SearchString(curser):
	#curser = here()
	b1 = (curser >> 24) & 0xff
	b2 = (curser >> 16) & 0xff
	b3 = (curser >> 8) & 0xff
	b4 = curser & 0xff
	
	print '[+] String Address : ' + hex(curser)
	binstr = '%x %x %x %x' % (b4, b3, b2, b1)

	tmplist = []
	tmplist.append(FindBinary(0, SEARCH_DOWN | SEARCH_CASE, binstr, 16))

	iscontinue = True
	while iscontinue:
		tmp = FindBinary(0, SEARCH_DOWN | SEARCH_CASE | SEARCH_NEXT, binstr, 16)
		if tmp == -1 or tmp in tmplist:
			iscontinue = False
		else:
			tmplist.append(tmp)

	if len(tmplist) == 1:
		print '[+] Find Binary Address : ' + hex(tmplist[0])
	else:
		print '[+] Find Binary Address : '
		print tmplist

	Jump(tmplist[0])

	return len(tmplist)

def main():
	SearchString(here())

if __name__ == '__main__':
	main()
