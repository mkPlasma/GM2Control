#include"HexFunc.h"


string byteToHex(const BYTE& b){
	return HEX_DIGITS[b / 16] + HEX_DIGITS[b % 16];
}

BYTE hexCharToByte(const char& hex){

	if(hex >= '0' && hex <= '9')
		return hex - '0';

	if(hex >= 'a' && hex <= 'f')
		return 10 + (hex - 'a');

	if(hex >= 'A' && hex <= 'F')
		return 10 + (hex - 'A');

	return (BYTE)-1;
}

BYTE hexStringToByte(const string& hex){
	return hexCharToByte(hex[0]) * 16 + hexCharToByte(hex[1]);
}
