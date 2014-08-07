RFID522
=======

A C++ implementation of the ggrfid.ino.

All functionality has been wraped into a library class. All funcions provided by
ggrfid.ino has been included as methods so you can scan tags, read and write with
a few funtion calls.

Use the original C code is very uncomfortable to use in arduino, it's the only reason I've done this arduino library.

Check the example for more details.

The author of original code is Dr.Leong (WWW.B2CQSHOP.COM) and I hope that my work
will not make him angry!


Convert it into a rfid reader/writer for your computer
=======================================================

In the examples directory you can found the ReaderFirmware sketch. This sketch converts your
arduino + RC522 into a rfid reader/writer device (over serial port). I also provide a python
example about how to use it (its very simple and you can port it to any other language whithout
any problem).

You can also attach an hc-05 for a wirless rfid reader (I use it to have an rfid reader in my android phone)

Build a device like that is very cheap (about 7€ arduino mini pro + serial to usb + RC522), very small
and hightly customizable (you can create your own primitives since you control the firwmare!).
