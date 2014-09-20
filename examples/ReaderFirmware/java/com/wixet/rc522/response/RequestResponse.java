package com.wixet.rc522.response;

import com.wixet.rc522.CardType;

public class RequestResponse extends Response{
	public CardType cardType;

	public CardType getCardType() {
		return cardType;
	}

	public void setCardType(CardType cardType) {
		this.cardType = cardType;
	}


}
