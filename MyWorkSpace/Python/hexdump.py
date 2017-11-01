data = "H4sIAAAAAAAAAHVTwY7TMBD9F3ePrZW4TdL2hhBFe0BCqhAHhCzjTBpTx7Zsp2W12iO/wKdw4XfgPxi7LVvYRTlEM5M3896byYd7wjs9hp5HNQBZl1WxrKu6XjarupySf5I1K4op0eoTWd+TG3zzA/igrCFrUtKGlnMyPeVbiEJpTEs70IPrrQGqxWhkD56OUelAt2CC9eFdCiaT1h6NtqJ9dQATJ5OtHb2EjdIwmZRldW6L/V6Y1lvVpoRw7mo+o8Vl+ACxt20e3gJ5mJJWhaiMjFylLFvCfNEwAR2UCIE0EdPGfuHKhCi05p0yKvRY5NELuc+4WVXWRbmqlmiB89aBjwpCcgKjdsTushfGQFINBrHOBhUzuRla6SGMOs0Jo5QQAsm0nBZ3vLN+QJcxIaLgIUs/gf7n8SDkUyHxziGMoC0YJHOMSNsj8S1KgPhSuDh6SCYF6QEM70HteqTUsLRVYZTDrwuaH5IYd+CTi1rJ/RXsqNrYI1+WnEhz3H53mfXZUWlpFDbYLh6FBypOC6Muk5CPJJ5dXwsHJeHpnmpWL1LdhivMgi4oO2u9dg2PVmF5XrFmXhfzgi0ZazL8qDqsRD8CBgbi0fo9P/v2/nZze272uMhZMvZmEGbshEzMkyFBDGE0u+eO8rJSJ3bAzTicCJ0PBL+8/APnQfmuyqzrrzZqCEnBoiqK6sS+WbAk9UYK71Vm8evr958/vmV6eOaJ7PbN7PWKVZuUUwGP2IfIW3FH1p3QAR7+nEg+avLw8Td3AV6H/gMAAA=="

def hexdump(string):
	for line in range(0, ((len(string)/16)+1), 1):
		print '%04x: ' % line + ' '.join(x.encode('hex') for x in string[(line*16):((line*16)+16)])
	#print ' '.join(x.encode('hex') for x in string)

hexdump(data.decode('base64'))
