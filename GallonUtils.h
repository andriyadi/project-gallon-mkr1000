//
// Created by Andri Yadi on 12/4/15.
//

#ifndef GALLON_GALLONUTILS_H_H
#define GALLON_GALLONUTILS_H_H

#include "Arduino.h"
#include "IPAddress.h"

int parseKeyVal(char *propsQueryString, char *results[][2], int resultsMaxCt);
/** IP to String? */
String toStringIp(IPAddress ip);
bool ipAddressFromString(const char *address, IPAddress *destAddr);
unsigned char h2int(char c);
void urldecode(char *urlbuf);

#endif //GALLON_GALLONUTILS_H_H
