
#ifndef __RFID522_H__
#define __RFID522_H__

#include <Arduino.h>
#include "Commands.h"

#define	uchar	unsigned char
#define	uint	unsigned int
#define RFID522_MAX_LEN 16

class RFID522 {

private:
  // These are required for initialisation and use of sdfatlib
  /*Sd2Card card;
  SdVolume volume;
  SdFile root;*/
  byte chipSelectPin;
  byte NRSTPD;
  
  // my quick&dirty iterator, should be replaced
  //SdFile getParentDir(const char *filepath, int *indx);
public:
  RFID522(int selectPin, int resetPin); 
  
  /*
  * Function name：Write_MFRC5200
  * Description：MFRC522 a register to write a byte of data
  * Input：Register address - addr; val - value to be written
  * Return：No
  */
  void writeRegister(uchar addr, uchar val);


  /*
  * Function name：readRegister
  * Description：Read one byte of data from a register MFRC522
  * Input：addr--Register Address
  * Return：Returns to read a byte of data to
  */
  uchar readRegister(uchar addr);

  /*
  * Function name：setBitMask
  * Description：RC522 register bit set
  * Input：Register address - reg; mask - set value
  * Return：No
  */
  void setBitMask(uchar reg, uchar mask);


  /*
  * Function name：clearBitMask
  * Description：清RC522寄存器位
  * Input：reg--寄存器地址;mask--清位值
  * Return：No
  */
  void clearBitMask(uchar reg, uchar mask);


  /*
  * Function name：antennaOn
  * Description：开启天线,每次启动或关闭天险发射之间应至少有1ms的间隔
  * Input：No
  * Return：No
  */
  void antennaOn(void);

  /*
  * Function name：antennaOff
  * Description：关闭天线,每次启动或关闭天险发射之间应至少有1ms的间隔
  * Input：No
  * Return：No
  */
  void antennaOff(void);


  /*
  * Function name：ResetMFRC522
  * Description：复位RC522
  * Input：No
  * Return：No
  */
  void reset(void);


  /*
  * Function name：InitMFRC522
  * Description：初始化RC522
  * Input：No
  * Return：No
  */
  void init(void);



  /*
  * Function name：toCard
  * Description：RC522和ISO14443卡通讯
  * Input：command--MF522命令字，
  *			 sendData--通过RC522发送到卡片的数据, 
  *			 sendLen--发送的数据长度		 
  *			 backData--接收到的卡片返回数据，
  *			 backLen--返回数据的位长度
  * Return：Successful return MI_OK
  */
  uchar toCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen);

  /*
  * Function name：request
  * Description：Find cards, readRegister the card type number
  * Input：reqMode--Way to find the card，
  *			 TagType--Return Card Type
  *			 	0x4400 = Mifare_UltraLight
  *				0x0400 = Mifare_One(S50)
  *				0x0200 = Mifare_One(S70)
  *				0x0800 = Mifare_Pro(X)
  *				0x4403 = Mifare_DESFire
  * Return：Successful return MI_OK
  */
  uchar request(uchar reqMode, uchar *TagType);




  /*
  * Function name：anticoll
  * Description：Anti-collision detection, readRegistering the serial number of the selected card is the card
  * Input：serNum--Returns 4 bytes card serial number, the fifth byte checksum byte
  * Return：Successful return MI_OK
  */
  uchar anticoll(uchar *serNum);


  /*
  * Function name：calulateCRC
  * Description：用MF522计算CRC
  * Input：pIndata--要读数CRC的数据，len--数据长度，pOutData--计算的CRC结果
  * Return：No
  */
  void calulateCRC(uchar *pIndata, uchar len, uchar *pOutData);


  /*
  * Function name：selectTag
  * Description：Election card, readRegister the card memory capacity
  * Input：serNum--Incoming card serial number
  * Return：Successful return card capacity
  */
  uchar selectTag(uchar *serNum);


  /*
  * Function name：auth
  * Description：Verification card password
  * Input：authMode--Password Authentication Mode
		  0x60 = A key verification
		  0x61 = B key verificatio 
	      BlockAddr--Block Address
	      Sectorkey--Sector password
	      serNum--Card serial number, 4 bytes
  * Return：Successful return MI_OK
  */
  uchar auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum);


  /*
  * Function name：read
  * Description：Read block data
  * Input：Block address - blockAddr; recvData - block data readRegister out
  * Return：Successful return MI_OK
  */
  uchar read(uchar blockAddr, uchar *recvData);


  /*
  * Function name：write
  * Description：写块数据
  * Input：blockAddr--块地址;writeRegisterData--向块写16字节数据
  * Return：Successful return MI_OK
  */
  uchar write(uchar blockAddr, uchar *writeRegisterData);


  /*
  * Function name：halt
  * Description：Command card into hibernation
  * Input：No
  * Return：No
  */
  void halt(void);

};

//extern SDClass SD;

#endif
