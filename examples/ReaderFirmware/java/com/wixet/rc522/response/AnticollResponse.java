package com.wixet.rc522.response;

public class AnticollResponse extends Response{
	protected byte[] uuid;

	public byte[] getUuid() {
		return uuid;
	}

	public void setUuid(byte[] uuid) {
		this.uuid = uuid;
	}

}
