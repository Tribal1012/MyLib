#!/usr/bin/python
from scapy.all import *
from random import randint
from time import sleep
import sys
import getopt
import re


def banner():
	print "==================================================="
	print "|               Simple_UDP_Fuzzer                 |"
	print "==================================================="

def usage():
	print >>sys.stderr, "[-] Simple_UDP_Fuzzer.py [-h|-p|-s|-S] -d dst_ip -D dst_port"
	print >>sys.stderr, "                         -h           : Help"
	print >>sys.stderr, "                         -p {tcp|udp} : TCP/UDP Protocol"
	print >>sys.stderr, "                         -s src_ip    : Source IP"
	print >>sys.stderr, "                         -S src_port  : Source Port"
	print >>sys.stderr, "                         -d dst_ip    : Destination IP"
	print >>sys.stderr, "                         -D dst_port  : Destination Port"
	sys.exit(-1)

def check_address(address):
	ip = None
	match = re.match("^(\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})$", address)
	if match:
		ip = match.group(1)
	else:
		print >>sys.stderr, "invalid address " + address 
	
	return ip

def get_random_data(n=16):
	return ''.join([chr(randint(0,255)) for i in xrange(n)])

def create_packet(protocol=None, src_ip=None, dst_ip=None, src_port=None, dst_port=None):
	if src_ip:
		packet = IP(src=src_ip, dst=dst_ip)
	else:
		packet = IP(dst=dst_ip)
	
	if protocol is None: protocol = 'tcp'
	if protocol == 'tcp':
		if src_port:
			packet = packet/TCP(sport=src_port, dport=dst_port)
		else:
			packet = packet/TCP(dport=dst_port)
	elif protocol == 'udp':
		if src_port:
			packet = packet/UDP(sport=src_port, dport=dst_port)
		else:
			packet = packet/UDP(dport=dst_port)
	else:
		print "Unknown protocol"
		return None

	packet = packet/get_random_data(randint(1,1024))

	return packet

def fuzzing(netinfo):
	while True:
		packet = create_packet(**netinfo)

		with open("result.log", "w") as fh:
			fh.write(str(packet))

		packet.show()
		result = sr1(packet,timeout=1)
		if result:
			result.show()
		sleep(1)

def config_netinfo():
	argv = sys.argv
	try:
		opts, args = getopt.getopt(argv[1:],
			"p:s:d:S:D:h",
			["protocol=", "src_ip=", "dst_ip=", "src_port=", "dst_port=", "help"])
	except getopt.error:
		usage()

	netinfo = {'protocol':'tcp'}
	for flag, value in opts:
		if flag in ['-p', '--protocol']:
			if value not in ['tcp', 'udp']:
				usage()
			netinfo['protocol'] = value
		elif flag in ['-s', '--src_ip']:
			netinfo['src_ip'] = check_address(value)
		elif flag in ['-d', '--dst_ip']:
			netinfo['dst_ip'] = check_address(value)
		elif flag in ['-S', '--src_port']:
			netinfo['src_port'] = int(value)
		elif flag in ['-D', '--dst_port']:
			netinfo['dst_port'] = int(value)
		elif flag in ['-h', '--help']:
			usage()
		else:
			usage()
	
	if 'dst_ip' not in netinfo and 'dst_port' not in netinfo:
		usage()

	print "[+] Configured..."
	
	return netinfo

def main():
	banner()
	netinfo = config_netinfo()
	fuzzing(netinfo)

if __name__ == "__main__":
	main()
