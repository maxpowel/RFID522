import serial;
import struct;

class RFID522:
    """ Simple class for control the device. """
    """ Take care about the serial speed (must be the same here and in the device)"""
    
    #All available commands
    CMD_REQUEST = struct.pack("B",0)
    CMD_ANTICOLL = struct.pack("B",1)
    CMD_SELECT_TAG = struct.pack("B",2)
    CMD_AUTH = struct.pack("B",3)
    CMD_READ = struct.pack("B",4)
    CMD_WRITE = struct.pack("B",5)
    CMD_HALT = struct.pack("B",6)
    CMD_RESTART = struct.pack("B",7)
    #Value returned when all is ok
    MI_OK = 0
    
    def __init__(self, port, speed= 9600):
        self.ser = serial.Serial(port, speed);
        
    def close(self):
	self.ser.close();
    
    def request(self):
      self.ser.write(self.CMD_REQUEST);
      res = struct.unpack("B",self.ser.read());
      status = res[0]
      res = {
	"status": status == self.MI_OK
      }
      if status == self.MI_OK:
	res["cardType"] = struct.unpack("BB",self.ser.read(2))
	
      return res
	
    def anticoll(self):
      self.ser.write(self.CMD_ANTICOLL);
      res = struct.unpack("B",self.ser.read());
      status = res[0]
      res = {
	"status": status == self.MI_OK
      }
      if status == self.MI_OK:
	uuid = struct.unpack("BBBB", self.ser.read(4))
	res["uuid"] = uuid

      return res
      
    def selectTag(self, uuid):
      self.ser.write(self.CMD_SELECT_TAG);
      self.ser.write(struct.pack("BBBB", uuid[0], uuid[1], uuid[2], uuid[3]));
      res = struct.unpack("B",self.ser.read());
      return res[0]
      
    def auth(self, uuid, blockAddr, key):
      self.ser.write(self.CMD_AUTH);
      #send uuid
      self.ser.write(struct.pack("BBBB", uuid[0], uuid[1], uuid[2], uuid[3]));
      #send blockAddr
      self.ser.write(struct.pack("B", blockAddr));
      #send key
      self.ser.write(struct.pack("BBBBBB", key[0], key[1], key[2], key[3], key[4], key[5]));
      res = struct.unpack("B",self.ser.read());
      return res[0] == self.MI_OK
      
    def read(self,blockAddr):
      self.ser.write(self.CMD_READ);
      #send blockAddr
      self.ser.write(struct.pack("B", blockAddr));
      
      res = struct.unpack("B",self.ser.read());
      status = res[0]
      res = {
	"status": status == self.MI_OK
      }
      if status == self.MI_OK:
	data = struct.unpack("BBBBBBBBBBBBBBBB", self.ser.read(16))
	res["data"] = data

      return res

    def write(self,blockAddr,data):
      self.ser.write(self.CMD_WRITE);
      #send blockAddr
      self.ser.write(struct.pack("B", blockAddr));
      
      #send data
      self.ser.write(struct.pack("BBBBBBBBBBBBBBBB", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]));
      
      res = struct.unpack("B",self.ser.read());
      status = res[0]
      res = {
	"status": status == self.MI_OK
      }

      return res
      
      
    def halt(self):
      self.ser.write(self.CMD_HALT);
      
    def restart(self):
      self.ser.write(self.CMD_RESTART);
      


# Main program, just a basic exampe
rfid = RFID522("/dev/ttyUSB0");
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
