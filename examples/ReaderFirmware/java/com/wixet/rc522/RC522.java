package com.wixet.rc522;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import com.wixet.rc522.response.AnticollResponse;
import com.wixet.rc522.response.RequestResponse;

public class RC522 {

	InputStream is;
	OutputStream os;
	public RC522(InputStream is, OutputStream os){
		this.is = is;
		this.os = os;
	}
	
	public RequestResponse request() throws IOException{
		RequestResponse res = new RequestResponse();
		if(sendCommand(Commands.REQUEST)){
			res.setStatus(true);
			byte cardType[] = readBytes(2);
			switch(toInt(cardType[0], cardType[1])){
			  case 0x4400:
			    res.setCardType(CardType.MIFARE_ULTRALIGHT);
			  break;
			  
			  case 0x0400:
				  res.setCardType(CardType.MIFARE_S50);
			  break;
			  
			  case 0x0200:
				  res.setCardType(CardType.MIFARE_S70);
			  break;
			  
			  case 0x0800:
				  res.setCardType(CardType.MIFARE_PRO);
			  break;
			  
			  case 0x4403:
				  res.setCardType(CardType.MIFARE_DESFIRE);
			  break;
			}
		}
		return res;
	}
	
	
	public AnticollResponse anticoll() throws IOException{
		AnticollResponse res = new AnticollResponse();		
		if(sendCommand(Commands.ANTICOLL)){
			res.setStatus(true);
			res.setUuid(readBytes(4));//Uuid length is 4 bytes
		}
		return res;
	}
	
	public int selectTag(byte uuid[]) throws IOException{
		os.write(Commands.SELECT_TAG);
		os.write(uuid);
		return is.read();

	}
	
	public boolean auth(byte uuid[], int blockAddr, byte key[]) throws IOException{		
			os.write(Commands.AUTH);
			//Send uuid
			os.write(uuid);
			//Send block address
			os.write(blockAddr);
			//Send key
			os.write(key);
			return is.read() == Commands.MI_OK;
	}
	
	public byte[] read(int blockAddr) throws IOException{
		byte block[] = null;
		//This command answers when the block addr is sent too
		os.write(Commands.READ);
		if(sendCommand(blockAddr)){
			//Block size is 16
			block = readBytes(16);
		}
		return block;
	}
	
	public boolean write(int blockAddr, byte data[]) throws IOException{
		//This command answers when all data is sent
		os.write(Commands.WRITE);
		os.write(blockAddr);
		os.write(data);
		return is.read() == Commands.MI_OK;
	}
    
        		
	
    public void halt() throws IOException{
        os.write(Commands.HALT);
    }
        
    public void restart() throws IOException{
        os.write(Commands.RESTART);
    }
    
    public void close() throws IOException{
    	os.close();
    	is.close();
    }
	
	protected int toInt(byte hb, byte lb)
	{
		return ((int)hb << 8) | ((int)lb & 0xFF);
	}
	
	protected byte[] readBytes(int nBytes) throws IOException{
		byte bytes[] = new byte[nBytes];
		int bytesRead = 0;
		while(bytesRead < nBytes){
			bytesRead += is.read(bytes, bytesRead, nBytes-bytesRead);
		}
		return bytes;
	}
	
	protected boolean sendCommand(int command) throws IOException{
		os.write(command);
		return is.read() == Commands.MI_OK;
	}
}
