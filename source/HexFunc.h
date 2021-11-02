#pragma once

#include<string>

using std::string;

typedef unsigned char BYTE;

const string HEX_DIGITS[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"};

string byteToHex(const BYTE& b);
BYTE hexStringToByte(const string& hex);
BYTE hexCharToByte(const char& hex);
