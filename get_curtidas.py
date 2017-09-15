#!/usr/bin/env python
#coding: utf-8

import requests, re, sys, signal
# import serial

def close_enough(orig, new, delta):
	return (new >= orig and new-orig < delta)

def handler(signum, frame, ser):
	# ser.close()
	sys.exit()

if __name__ == '__main__':
	last_curtidas = 1339
	page = 'https://www.facebook.com/autobotzufmg/'

	# ser = serial.Serial(
	# 	port='/dev/ttyUSB1',
	# 	baudrate=9600,
	# 	# parity=serial.PARITY_ODD,
	# 	# stopbits=serial.STOPBITS_TWO,
	# 	# bytesize=serial.SEVENBITS
	# )
	# ser.isOpen()

	signal.signal(signal.SIGINT, lambda sig, frame: handler(sig, frame, 'oi'))
	# signal.signal(signal.SIGINT, lambda sig, frame: handler(sig, frame, ser))

	while (1):
		code = requests.get(page)

		urls = re.findall(r"<div>\d+.\d+ pessoas curtiram isso</div>", code.content)
		found_matches = []
		
		for i,url in enumerate(urls):
			try:
				print i, ': ', url, '->', int(re.sub(r'\D', "", url) )
				found_matches.append(int(re.sub(r'\D', "", url) ))
			except:
				print 'Error convertind string to integer'

		candidatos = 0
		curtidas = 0
		for match in found_matches:
			if close_enough(last_curtidas, match, 30):
				if candidatos > 0:
					print 'Error, mode than onde candidate found on page'
					# ser.close()
					sys.exit()
				else:
					candidatos+=1
					curtidas = match
		if curtidas == 0:
			print 'Nothing found'
			curtidas = last_curtidas
		else:
			last_curtidas = curtidas
			print 'Found', curtidas, 'curtidas'
			# ser.write(str(curtidas))