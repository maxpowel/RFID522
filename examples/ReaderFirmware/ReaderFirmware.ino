/* 
 * Written by Alvaro Garcia maxpowel@gmail.com
 * 
 * This firmware converts your arduino + RC522 into an
 * rfid reader/writer. I use it for mifare but it may work
 * for all rfid tags supported by RC522
 *
 * I use an arduino mini pro + serial to usb + RC522 for a
 * cheap, small and high customizable device. Even you can
 * replace serial to usb for an hc-05 and get a wireless
 * rfid reader (but you have to provide a battery and some
 * other stuff)
 */
#include <SPI.h>
#include <RFID522.h>

// set up variables using the SD utility library functions:
RFID522 rfid(10,5);
 
                            
void setup()
{
  //Set a higher speed for faster transmission.
  //Full dump takes about 2,2s at 9600 and 0,6 at 115200
  //but if you are planning to use software serial take care about speed limitation
  Serial.begin(9600);
  // start the SPI library:
  SPI.begin();

  rfid.init();  
}


#define CMD_REQUEST 0
#define CMD_ANTICOLL 1
#define CMD_SELECT_TAG 2
#define CMD_AUTH 3
#define CMD_READ 4
#define CMD_WRITE 5
#define CMD_HALT 6
#define CMD_RESTART 7

uchar request(uchar *str){
  return rfid.request(PICC_REQIDL, str);	
}

uchar anticoll(uchar *uuid){
  return rfid.anticoll(uuid);
}

int selectTag(uchar *uuid){
  return rfid.selectTag(uuid);
}

uchar auth(uchar *uuid, uchar blockAddr, uchar *key){
  return rfid.auth(PICC_AUTHENT1A, blockAddr, key, uuid);
}

uchar status;
uchar uuid[5];//5th byte is a CRC byte
uchar key[6];
uchar blockAddr;
uchar blockData[16];

void loop(void) {
  
  if(Serial.available()){
    char cmd = Serial.read();
    switch(cmd){
      case CMD_REQUEST:
        uchar cardType[2];
        status = request(cardType);
        Serial.write(status);
        if(status == MI_OK){
          Serial.write(cardType,2);
        }
      break;
      
      case CMD_ANTICOLL:
        
        status = anticoll(uuid);
        Serial.write(status);
        if(status == MI_OK)
          Serial.write(uuid,4);//uuid is only 4 bytes
        
      break;
      
      case CMD_SELECT_TAG:
        //Read UUID
        while(Serial.available() < 4);
        for(int i=0; i < 4; i++){
          uuid[i] = Serial.read();
        }
        uchar cardSize;
        cardSize = selectTag(uuid);
        Serial.write(cardSize);
      break;
      
      case CMD_AUTH:
        //Read uuid
        while(Serial.available() < 4);
        for(int i=0; i < 4; i++){
          uuid[i] = Serial.read();
        }
        //Read block
        while(!Serial.available());
        blockAddr = Serial.read();
        //Read key
        while(Serial.available() < 6);
        for(int i=0; i < 6; i++){
          key[i] = Serial.read();
        }
        
        status = auth(uuid, blockAddr, key);
        Serial.write(status);
        
      break;
      
      case CMD_READ:
        //Read blockaddr
        while(!Serial.available());
        blockAddr = Serial.read();
        status = rfid.read(blockAddr, blockData);
        Serial.write(status);
        if(status == MI_OK){
          Serial.write(blockData,16);
        }
      break;
      
      case CMD_WRITE:
        //Read blockaddr
        while(!Serial.available());
        blockAddr = Serial.read();
        
        //Read data to write
        while(Serial.available() < 16);
        for(int i=0; i < 16; i++){
          blockData[i] = Serial.read();
        }
        
        status = rfid.write(blockAddr, blockData);
        Serial.write(status);
      break;
      
      case CMD_HALT:
        rfid.halt();
      break;
      
      case CMD_RESTART:
        rfid.init();
      break;
    }
  }
}
