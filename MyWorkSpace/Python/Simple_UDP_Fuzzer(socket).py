#!/usr/bin/python
from random import randint
from time import sleep
from struct import pack, unpack
from construct import *
import sys
import getopt
import re
import socket
import netifaces as nif
import binascii

EthernetHeader = Struct(
	"daddr" / Bytes(6),
	"saddr" / Bytes(6),
	"ethertype" / Enum(Int16ub,
		IPv4=0x0800,
		ARP=0x0806
	)
)
ARP_Packet = Struct(
	"htype" / Int16ub,
	"ptype" / Int16ub,
	"hlen" / Int8ub,
	"plen" / Int8ub,
	"oper" / Int16ub,
	"sha" / Bytes(6),
	"spa" / Bytes(4),
	"tha" / Bytes(6),
	"tpa" / Bytes(4)
)

def hexdump(string):
	for line in range(0, ((len(string)/16)+1), 1):
		print '%04x: ' % (line*16) + ' '.join(x.encode('hex') for x in string[(line*16):((line*16)+16)])
	#print ' '.join(x.encode('hex') for x in string)

def banner():
	print "==================================================="
	print "|                Simple_UDP_Fuzzer                |"
	print "==================================================="

def usage():
	print >>sys.stderr, "[-] Simple_UDP_Fuzzer.py [-h|-p|-m] -s src_ip -d dst_ip -S src_ip -D dst_port"
	print >>sys.stderr, "                     -h           : Help"
	print >>sys.stderr, "                     -p {tcp|udp} : TCP/UDP Protocol"
	print >>sys.stderr, "                     -s src_ip    : Source IP"
	print >>sys.stderr, "                     -S src_port  : Source Port"
	print >>sys.stderr, "                     -d dst_ip    : Destination IP"
	print >>sys.stderr, "                     -D dst_port  : Destination Port"
	print >>sys.stderr, "                     -m max       : Max length of packet's data"
	sys.exit(-1)

def check_address(address):
	ip = None
	match = re.match("^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})$", address)
	if match:
		ip = match.group(1)
	else:
		print >>sys.stderr, "invalid address " + address 
		return None
	
	return ip

def get_random_data(n=16):
	return ''.join([chr(randint(0,255)) for i in xrange(n)])

def calc_checksum(protocol=None, src_ip=None, dst_ip=None, header=None, data=None):
	checksum = 0
	total = 0
	if (protocol != None) and (src_ip != None) and (dst_ip != None):
		psh_saddr = socket.inet_aton(src_ip)
		psh_daddr = socket.inet_aton(dst_ip)
		psh_zero = 0
		psh_protocol = socket.IPPROTO_UDP if (protocol == 'udp') else socket.IPPROTO_TCP
		psh_length = len(header) + len(data)
	
		psh_header = pack('!4s4sBBH', psh_saddr, psh_daddr, psh_zero, psh_protocol, psh_length)
		tmp = unpack('!6H', psh_header)

		# get psh_header's checksum
		for item in tmp:
			total += item

	if (header != None):
		# get tcp or udp's header checksum
		tmp = unpack('!4H', header)
		for item in tmp:
			total += item

	if (data != None):
		tmp_data = data + '\x00'*(16 - (len(data) % 16))
		tmp = unpack('!' + 'H'*(len(tmp_data)/2), tmp_data)
		# get data's checksum 
		for item in tmp:
			total += item

	checksum = (total >> 16) + (total & 0xffff)
	checksum = (checksum ^ 0xffff)
	
	return checksum

def create_raw_ip_header(protocol, src_ip, dst_ip, data_len):
	ip_ihl = 5
	ip_ver = 4
	ip_tos = 0
	ip_tot_len = 20 + 8 + data_len
	ip_id = 44444
	ip_frag_off = 0
	ip_ttl = 128
	ip_proto = socket.IPPROTO_UDP if (protocol == 'udp') else socket.IPPROTO_TCP
	ip_check = 0
	ip_saddr = socket.inet_aton(src_ip)
	ip_daddr = socket.inet_aton(dst_ip)
	 
	ip_ihl_ver = (ip_ver << 4) + ip_ihl
	 
	# the ! in the pack format string means network order
	ip_header = pack('!BBHHHBBH4s4s' , ip_ihl_ver, ip_tos, ip_tot_len, ip_id, ip_frag_off, ip_ttl, ip_proto, ip_check, ip_saddr, ip_daddr)
	tmp = unpack('!10H', ip_header)

	# get ip checksum
	total = 0
	for item in tmp:
		total += item
	
	ip_check = (total >> 16) + (total & 0xffff)
	ip_check = (ip_check ^ 0xffff)

	# repack
	ip_header = pack('!BBHHHBBH4s4s' , ip_ihl_ver, ip_tos, ip_tot_len, ip_id, ip_frag_off, ip_ttl, ip_proto, ip_check, ip_saddr, ip_daddr)

	return ip_header

def create_raw_udp_header(protocol, src_ip, dst_ip, src_port, dst_port, data):
	udp_sport = src_port
	udp_dport = dst_port
	udp_length = 8 + len(data)
	udp_checksum = 0

	udp_header = pack('!4H', udp_sport, udp_dport, udp_length, udp_checksum)
	# get udp_checksum
	udp_checksum = calc_checksum(protocol, src_ip, dst_ip, udp_header, data)
	# repack udp_header
	udp_header = pack('!4H', udp_sport, udp_dport, udp_length, udp_checksum)

	return udp_header

def create_packet(protocol=None, src_mac=None, dst_mac=None, src_ip=None, dst_ip=None, src_port=None, dst_port=None, max=65000):
	print max
	data_len = randint(0,max)
	data = get_random_data(data_len)

	ether_header = EthernetHeader.build({
		"daddr": ether_aton(dst_mac),
		"saddr": ether_aton(src_mac),
		"ethertype": "IPv4"
	})
	ip_header = create_raw_ip_header(protocol, src_ip, dst_ip, data_len)
	udp_header = create_raw_udp_header(protocol, src_ip, dst_ip, src_port, dst_port, data)

	packet = ip_header + udp_header + data

	print "[+] ==================== packet ===================="
	print hexdump(packet)

	return packet

def ether_aton(mac_str):
	return pack('6B', *(int(x, 16) for x in mac_str.split(':')))

def ehter_ntoa(mac_str):
	return ':'.join('%02x' % x for x in unpack('6B', mac_str))

# thx https://stackoverflow.com/questions/10137594/python-get-mac-address-of-only-the-connected-local-nic/18031868#18031868
def get_my_mac(ip):
	for i in nif.interfaces():
		try:
			if_mac = nif.ifaddresses(i)[nif.AF_LINK][0]['addr']
			if_ip = nif.ifaddresses(i)[nif.AF_INET][0]['addr']
		except: #ignore ifaces that dont have MAC or IP
			if_mac = if_ip = None
		if if_ip == ip:
			return if_mac.encode('ascii', 'ignore')

	return None

# ARP Request
def get_target_mac(my_mac, my_ip, target_ip):
	s = socket.socket(socket.AF_INET, socket.SOCK_RAW)
	
	ether_header = EthernetHeader.build({
		"daddr": ether_aton("ff:ff:ff:ff:ff:ff"),
		"saddr": ether_aton(my_mac),
		"ethertype": "ARP"
	})
	arp_packet = ARP_Packet.build({
		"htype": 0x0001,
		"ptype": 0x0800,
		"hlen": 0x06,
		"plen": 0x04,
		"oper": 0x0001,
		"sha": ether_aton(my_mac),
		"spa": socket.inet_aton(my_ip),
		"tha": ether_aton("00:00:00:00:00:00"),
		"tpa": socket.inet_aton(target_ip)
	})
	packet = ether_header + arp_packet
#	hexdump(packet)
#	s.bind(("eth0", socket.SOCK_RAW))
#	s.send(packet)
#	print s.recv()
#	s.close()

	# wtf..? where is socket.AF_PACKET?
	return "00:0C:29:EE:3A:6E"

def fuzzing(netinfo):
	s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_IP)

	while True:
		packet = create_packet(**netinfo)

		with open("result.log", "w") as fh:
			fh.write(str(packet))

		result = s.sendto(packet, (netinfo['dst_ip'], netinfo['dst_port']))
		sleep(1)

def check_netinfo(protocol=None, src_ip=None, dst_ip=None, src_port=None, dst_port=None, max=65000):
	if protocol != 'udp':
		return False
	if src_ip == None or dst_ip == None:
		return False
	if src_port == None or dst_port == None:
		return False
	if max == None or max == 0:
		return False
	
	return True

def config_netinfo():
	argv = sys.argv
	try:
		opts, args = getopt.getopt(argv[1:],
			"p:s:d:S:D:m:h",
			["protocol=", "src_ip=", "dst_ip=", "src_port=", "dst_port=", "max=", "help"])
	except getopt.error:
		usage()

	netinfo = {'max':100}
	netinfo['protocol'] = 'udp'
	for flag, value in opts:
		if flag in ['-p', '--protocol']:
			if value not in ['tcp', 'udp']:
				usage()
			netinfo['protocol'] = value
		elif flag in ['-s', '--src_ip']:
			netinfo['src_ip'] = value if (check_address(value) != None) else None
		elif flag in ['-d', '--dst_ip']:
			netinfo['dst_ip'] = value if (check_address(value) != None) else None
		elif flag in ['-S', '--src_port']:
			netinfo['src_port'] = int(value)
		elif flag in ['-D', '--dst_port']:
			netinfo['dst_port'] = int(value)
		elif flag in ['-m', '--max']:
			netinfo['max'] = int(value)
		elif flag in ['-h', '--help']:
			usage()
		else:
			usage()

	print "[+] Configured..."
	try:
		check_netinfo(**netinfo)
	except:
		usage()
		return None

	netinfo['src_mac'] = get_my_mac(netinfo['src_ip'])
	netinfo['dst_mac'] = get_target_mac(netinfo['src_mac'], netinfo['src_ip'], netinfo['dst_ip'])
	print "[+] my_mac : " + netinfo['src_mac']
	print "[+] dst_mac : " + netinfo['dst_mac']

	return netinfo

def main():
	banner()
	netinfo = config_netinfo()
	if netinfo != None:
		fuzzing(netinfo)

if __name__ == "__main__":
	main()
