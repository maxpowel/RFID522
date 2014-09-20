package com.wixet.rc522.response;

public abstract class Response {
	protected boolean status;

	public boolean getStatus() {
		return status;
	}

	public void setStatus(boolean status) {
		this.status = status;
	}
	
	public boolean isOk(){
		return status;
	}

}
