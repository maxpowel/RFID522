RFID522
=======

An easy to use arduino library for the RC522 rfid reader.


Convert it into a rfid reader/writer for your computer
=======================================================

In the examples directory you can found the ReaderFirmware sketch. This sketch converts your
arduino + RC522 into a rfid reader/writer device (over serial port).

You can also attach an hc-05 for a wirless rfid reader (I use it to have an rfid reader in my android phone using the java library)

Build a device like that is very cheap (about 7€ arduino mini pro + serial to usb + RC522), very small
and hightly customizable (you can create your own primitives since you control the firwmare!).

Libraries available
=======================================================

When uploading the ReaderFirmware sketch you can use it as a RFID reader connected to your computer in the following languagues:

[Python](https://github.com/maxpowel/RFID522/tree/master/examples/ReaderFirmware/python)
[Java](https://github.com/maxpowel/RFID522/tree/master/examples/ReaderFirmware/java)