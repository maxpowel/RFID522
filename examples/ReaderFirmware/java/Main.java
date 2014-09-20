import java.io.IOException;

import com.wixet.rc522.RC522;
import com.wixet.rc522.response.AnticollResponse;
import com.wixet.rc522.response.RequestResponse;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.UnsupportedCommOperationException;


public class Main {

	public static void main(String[] args) throws NoSuchPortException, PortInUseException, UnsupportedCommOperationException, IOException {
		// TODO Auto-generated method stub
		
		CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier("/dev/ttyUSB0");
		SerialPort serialPort = (SerialPort) portIdentifier.open("RFID", 0);
		serialPort.setSerialPortParams(9600, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
		RC522 rfid = new RC522(serialPort.getInputStream(), serialPort.getOutputStream());
		
		rfid.restart();
		RequestResponse res = rfid.request();
		if(res.isOk()){
			System.out.println("Card found");
			System.out.println("Type: "+res.getCardType());
			AnticollResponse uuid = rfid.anticoll();
			if(uuid.isOk()){
				System.out.print("Uuid: ");
				printHex(uuid.getUuid());
				int size = rfid.selectTag(uuid.getUuid());
				if(size > 0){
					System.out.println("Card size: "+size);
					System.out.println("Reading blocks: ");
					byte key[] = {(byte)0xFF, (byte)0xFF, (byte)0xFF, (byte)0xFF, (byte)0xFF, (byte)0xFF};
					boolean auth = false;
					for(int n=0; n < 64; n++){
						//Only auth when changing sector
						if (n%4 == 0){
							System.out.println(n);
							auth = rfid.auth(uuid.getUuid(), n, key);
							System.out.println("Auth: "+(auth?"OK":"Fail"));
						}
						
						if(auth){
							System.out.println("Reading block "+n);
							byte dataRead[] = rfid.read(n);
							if(dataRead != null){
								System.out.println("OK");
								if(n == 2){
									printHex(dataRead);
								}
							}else System.out.println("FAIL");
						}
						
						if(n == 2){
							byte dataWrite[] = {
									(byte)0x01, 
									(byte)0x02, 
									(byte)0x03, 
									(byte)0x04, 
									(byte)0x05, 
									(byte)0x06,
									(byte)0x07,
									(byte)0x08,
									(byte)0x09,
									(byte)0x08,
									(byte)0x07,
									(byte)0x06,
									(byte)0x06,
									(byte)0x05,
									(byte)0x05,
									(byte)0x05,};
							System.out.println("Write: "+rfid.write(2, dataWrite));
						}
					}
					rfid.halt();
						    
				}else System.out.println("SelectTag error");
			}else System.out.println("Anticoll error");
		}else{
			System.out.println("No card found");
		}
			
		rfid.close();
		serialPort.close();
		
	}
	
	public static void printHex(byte uuid[]){
		for(byte b: uuid){
			String s = String.format("%02x ", b);
			System.out.print(s+" ");
		}
		System.out.println("");
	}

}
