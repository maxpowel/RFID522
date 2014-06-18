/* This example shows a basic usage (find and read a tag) */

#include <SPI.h>
#include <RFID522.h>

/* Connect SPI pins MISO, MOSI and SCK */
/* IRQ pin is not required (I guess it colud be used for some interrupt) */
/* This device ONLY works with 3,3V even for data. Otherwise it simple will not work */

/* CS pin is called SDA in some boards */
#define CS_PIN 10
#define RESET_PIN 5
// set up variables using the SD utility library functions:

RFID522 rfid(CS_PIN,RESET_PIN);

 uchar sectorKeyA[16][16] = {
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                                 { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                            };
                            
void setup()
{
  Serial.begin(9600);
  //start the SPI library:
  SPI.begin();

  rfid.init();  
}


void loop(void) {
  uchar uuid[5];
  uchar blockData[RFID522_MAX_LEN];
  uchar status;
  uchar str[RFID522_MAX_LEN];
  uchar blockAddr;	//Select the operating block address 0 to 63
  
  //Find cards, return the card type	
  status = rfid.request(PICC_REQIDL, str);	
  if (status == MI_OK)
  {
    Serial.println("Card detected");
    Serial.print("Type: ");
    word type = word(str[0], str[1]);
    switch(type){
      case 0x4400:
        Serial.print("Mifare UltraLight");
      break;
      
      case 0x0400:
        Serial.print("Mifare_One (S50)");
      break;
      
      case 0x0200:
        Serial.print("Mifare_One (S70)");
      break;
      
      case 0x0800:
        Serial.print("Mifare_Pro(X)");
      break;
      
      case 0x4403:
        Serial.print("Mifare_DESFire");
      break;
      default:
        Serial.print("ERROR: ");
        Serial.print(str[0],HEX);
        Serial.print(" ");
        Serial.print(str[1],HEX);
    }
    Serial.println("");

    //Anti-collision, return the card's serial number 4 bytes
    status = rfid.anticoll(str);
    memcpy(uuid, str, 5);
    if (status == MI_OK)
    {
      Serial.print("Card UUID: ");
      int i = 0;
      for(i = 0; i < 4; i++){
        Serial.print(uuid[i], HEX);
      }
      Serial.println("");
      
      uchar size;
      size = rfid.selectTag(uuid);
      if (size != 0)
      {
        Serial.print("The size of the card is  :   ");
        Serial.print(size,DEC);
        Serial.println(" K ");
        Serial.println("Reading card");
        //Select block
        uchar *actualKey;
        uchar keyNumber = 0;
        for(blockAddr = 0; blockAddr < 64; blockAddr++){
          if (blockAddr % 4 != 0){
            //Do auth and read
            status = rfid.auth(PICC_AUTHENT1A, blockAddr, actualKey, uuid);
            if (status == MI_OK){
              
              status = rfid.read(blockAddr, blockData);
              if (status == MI_OK)
              {
                Serial.println("Read from the card, the data is : ");
                for (i=0; i<16; i++)
      	        {
                  Serial.print(blockData[i],HEX);
                  Serial.print(" , ");
      	        }
                Serial.println(" ");
              }else{
                Serial.print("Error reading block ");
                Serial.println(blockAddr);
              }
            }else{
              Serial.println("AUTH FAILED");
              Serial.println(blockAddr);
              Serial.print("Key: ");
              Serial.print(actualKey[0],HEX);
              Serial.print(actualKey[1],HEX);
              Serial.print(actualKey[2],HEX);
              Serial.print(actualKey[3],HEX);
              Serial.print(actualKey[4],HEX);
              Serial.print(actualKey[5],HEX);
              Serial.println("");      
            }
            
          }else{
            //Load next key and auth
            actualKey = sectorKeyA[keyNumber];
            keyNumber++;
            
          }
        }
        
      }
       rfid.halt();
       rfid.init();//Dont know why I have to call init or the program is freezed
      delay(1000);

    
   }else Serial.println("Anticollision failed");
 }
}