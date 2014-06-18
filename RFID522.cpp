/*
 * The original author of the code is Dr.Leong (WWW.B2CQSHOP.COM) 
 * Me, Álvaro García (maxpowel@gmail.com) and I only have wrapped his code into 
 * a C++ class.
 * 
 */ 
#include "RFID522.h"
#include <SPI.h>

RFID522::RFID522(int selectPin, int resetPin) {
  chipSelectPin = selectPin;
  NRSTPD = resetPin;
  
    
  pinMode(chipSelectPin,OUTPUT);             // Set digital pin 10 as OUTPUT to connect it to the RFID /ENABLE pin 
  digitalWrite(chipSelectPin, LOW);          // Activate the RFID reader
  pinMode(NRSTPD,OUTPUT);               // Set digital pin 10 , Not Reset and Power-down
  digitalWrite(NRSTPD, HIGH);
}


void RFID522::writeRegister(uchar addr, uchar val)
{
	digitalWrite(chipSelectPin, LOW);
	SPI.transfer((addr<<1)&0x7E);	
	SPI.transfer(val);
	digitalWrite(chipSelectPin, HIGH);
}

uchar RFID522::readRegister(uchar addr)
{
	uchar val;
	digitalWrite(chipSelectPin, LOW);
	//Address Format：1XXXXXX0
	SPI.transfer(((addr<<1)&0x7E) | 0x80);	
	val =SPI.transfer(0x00);  
	digitalWrite(chipSelectPin, HIGH);  
	return val;	
}

void RFID522::setBitMask(uchar reg, uchar mask)  
{
    uchar tmp;
    tmp = readRegister(reg);
    writeRegister(reg, tmp | mask);  // set bit mask
}

void RFID522::clearBitMask(uchar reg, uchar mask)  
  {
      uchar tmp;
      tmp = readRegister(reg);
      writeRegister(reg, tmp & (~mask));  // clear bit mask
  } 
  
void RFID522::antennaOn(void)
{
	uchar temp;

	temp = readRegister(TxControlReg);
	if (!(temp & 0x03))
	{
		setBitMask(TxControlReg, 0x03);
	}
}

void RFID522::antennaOff(void)
{
	clearBitMask(TxControlReg, 0x03);
}

void RFID522::reset(void)
{
    writeRegister(CommandReg, PCD_RESETPHASE);
}

void RFID522::init(void)
{
	digitalWrite(NRSTPD,HIGH);

	reset();
		
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    writeRegister(TModeReg, 0x8D);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    writeRegister(TPrescalerReg, 0x3E);	//TModeReg[3..0] + TPrescalerReg
    writeRegister(TReloadRegL, 30);           
    writeRegister(TReloadRegH, 0);
	
	writeRegister(TxAutoReg, 0x40);		//100%ASK
	writeRegister(ModeReg, 0x3D);		//CRC初始值0x6363	???

	//clearBitMask(Status2Reg, 0x08);		//MFCrypto1On=0
	//writeRegister(RxSelReg, 0x86);		//RxWait = RxSelReg[5..0]
	//writeRegister(RFCfgReg, 0x7F);   		//RxGain = 48dB

	antennaOn();		//打开天线
}
  
uchar RFID522::toCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen)
{
    uchar status = MI_ERR;
    uchar irqEn = 0x00;
    uchar waitIRq = 0x00;
    uchar lastBits;
    uchar n;
    uint i;

    switch (command)
    {
	case PCD_AUTHENT:		//认证卡密
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:	//发送FIFO中数据
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }
  
    writeRegister(CommIEnReg, irqEn|0x80);	//允许中断请求
    clearBitMask(CommIrqReg, 0x80);			//清除所有中断请求位
    setBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO初始化
    
	writeRegister(CommandReg, PCD_IDLE);	//NO action;取消当前命令	???

	//向FIFO中写入数据
    for (i=0; i<sendLen; i++)
    {   
		writeRegister(FIFODataReg, sendData[i]);    
	}

	//执行命令
	writeRegister(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {    
		setBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts  
	}   
    
	//等待接收数据完成
	i = 2000;	//i根据时钟频率调整，操作M1卡最大等待时间25ms	???
    do 
    {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
	n = readRegister(CommIrqReg);
	i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    clearBitMask(BitFramingReg, 0x80);			//StartSend=0
	
    if (i != 0)
    {    
	if(!(readRegister(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
	{
	    status = MI_OK;
	    if (n & irqEn & 0x01)
	    {   
				status = MI_NOTAGERR;			//??   
			}

	    if (command == PCD_TRANSCEIVE)
	    {
		n = readRegister(FIFOLevelReg);
		lastBits = readRegister(ControlReg) & 0x07;
		if (lastBits)
		{   
					*backLen = (n-1)*8 + lastBits;   
				}
		else
		{   
					*backLen = n*8;   
				}

		if (n == 0)
		{   
					n = 1;    
				}
		if (n > RFID522_MAX_LEN)
		{   
					n = RFID522_MAX_LEN;   
				}
				
				//读取FIFO中接收到的数据
		for (i=0; i<n; i++)
		{   
					backData[i] = readRegister(FIFODataReg);    
				}
	    }
	}
	else
	{   
			status = MI_ERR;  
		}
	
    }
	
    //setBitMask(ControlReg,0x80);           //timer stops
    //writeRegister(CommandReg, PCD_IDLE); 

    return status;
}
  
uchar RFID522::request(uchar reqMode, uchar *TagType)
{
      uchar status;  
      uint backBits;			//接收到的数据位数

      writeRegister(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???
      
      TagType[0] = reqMode;
      status = toCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

      if ((status != MI_OK) || (backBits != 0x10))
      {    
	      status = MI_ERR;
      }

      return status;
}
  
uchar RFID522::anticoll(uchar *serNum)
{
    uchar status;
    uchar i;
	uchar serNumCheck=0;
    uint unLen;
    

    //clearBitMask(Status2Reg, 0x08);		//TempSensclear
    //clearBitMask(CollReg,0x80);			//ValuesAfterColl
	writeRegister(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = toCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

    if (status == MI_OK)
	{
		//Check card serial number
		for (i=0; i<4; i++)
		{   
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{   
			status = MI_ERR;    
		}
    }

    //setBitMask(CollReg, 0x80);		//ValuesAfterColl=1

    return status;
} 

void RFID522::calulateCRC(uchar *pIndata, uchar len, uchar *pOutData)
{
    uchar i, n;

    clearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    setBitMask(FIFOLevelReg, 0x80);			//清FIFO指针
    //writeRegister(CommandReg, PCD_IDLE);

	//向FIFO中写入数据	
    for (i=0; i<len; i++)
    {   
		writeRegister(FIFODataReg, *(pIndata+i));   
	}
    writeRegister(CommandReg, PCD_CALCCRC);

	//等待CRC计算完成
    i = 0xFF;
    do 
    {
	n = readRegister(DivIrqReg);
	i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//读取CRC计算结果Description
    pOutData[0] = readRegister(CRCResultRegL);
    pOutData[1] = readRegister(CRCResultRegM);
}

uchar RFID522::selectTag(uchar *serNum)
{
    uchar i;
	uchar status;
	uchar size;
    uint recvBits;
    uchar buffer[9]; 

	//clearBitMask(Status2Reg, 0x08);			//MFCrypto1On=0

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i=0; i<5; i++)
    {
	buffer[i+2] = *(serNum+i);
    }
	calulateCRC(buffer, 7, &buffer[7]);		//??
    status = toCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    
    if ((status == MI_OK) && (recvBits == 0x18))
    {   
		size = buffer[0]; 
	}
    else
    {   
		size = 0;    
	}

    return size;
}

uchar RFID522::auth(uchar authMode, uchar BlockAddr, uchar *Sectorkey, uchar *serNum)
{
    uchar status;
    uint recvBits;
    uchar i;
	uchar buff[12]; 

	//Verify instruction block address + sector + password + card serial number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i=0; i<6; i++)
    {    
		buff[i+2] = *(Sectorkey+i);   
	}
    for (i=0; i<4; i++)
    {    
		buff[i+8] = *(serNum+i);   
	}
    status = toCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

    if ((status != MI_OK) || (!(readRegister(Status2Reg) & 0x08)))
    {   
		status = MI_ERR;   
	}
    
    return status;
}
  
uchar RFID522::read(uchar blockAddr, uchar *recvData)
{
    uchar status;
    uint unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    calulateCRC(recvData,2, &recvData[2]);
    status = toCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

    if ((status != MI_OK) || (unLen != 0x90))
    {
	status = MI_ERR;
    }
    
    return status;
}
  
uchar RFID522::write(uchar blockAddr, uchar *writeRegisterData)
{
    uchar status;
    uint recvBits;
    uchar i;
	uchar buff[18]; 
    
    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    calulateCRC(buff, 2, &buff[2]);
    status = toCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    {   
		status = MI_ERR;   
	}
	
    if (status == MI_OK)
    {
	for (i=0; i<16; i++)		//向FIFO写16Byte数据
	{    
		buff[i] = *(writeRegisterData+i);   
	}
	calulateCRC(buff, 16, &buff[16]);
	status = toCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
	
		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{   
			status = MI_ERR;   
		}
    }
    
    return status;
}
  
void RFID522::halt(void)
{
      uchar status;
  uint unLen;
  uchar buff[4]; 

  buff[0] = PICC_HALT;
  buff[1] = 0;
  calulateCRC(buff, 2, &buff[2]);

  status = toCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}

//SDClass SD;
