import rfid

# Main program, just a basic exampe
rfid = rfid.RFID522("/dev/ttyUSB0");
rfid.restart();
res = rfid.request();
if res["status"]:
  res = rfid.anticoll()
  print res
  if res["status"]:
    print rfid.selectTag(res["uuid"])
    n = 0
    while n != 64:
      if n%4 == 0:
	auth = rfid.auth(res["uuid"], n, [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
	print "Auth: ", auth
      else:
	 auth = True
      if auth:
	print "Reading block ", n
	rfid.read(n)
	n = n+1
	#print "writing block 2"
	#print rfid.write(2, [0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])
    rfid.halt()
else:
  print "No card found"
rfid.close();
