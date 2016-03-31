/*
 * GallonUtils.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#include "GallonUtils.h"

int parseKeyVal(char *propsQueryString, char *results[][2], int resultsMaxCt) {
    int ct = 0;

    while (propsQueryString && *propsQueryString && ct < resultsMaxCt) {
        results[ct][0] = strsep(&propsQueryString, "&");
        results[ct][1] = strchrnul(results[ct][0], '=');
        if (*results[ct][1]) *results[ct][1]++ = '\0';

        ct++;
    }

    return ct;
}

/** IP to String? */
String toStringIp(IPAddress ip) {
    String res = "";
    for (int i = 0; i < 3; i++) {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);
    return res;
}

bool ipAddressFromString(const char *address, IPAddress *destAddr)
{
    // TODO: add support for "a", "a.b", "a.b.c" formats

    byte bytes[4];
    uint16_t acc = 0; // Accumulator
    uint8_t dots = 0;

    while (*address)
    {
        char c = *address++;
        if (c >= '0' && c <= '9')
        {
            acc = acc * 10 + (c - '0');
            if (acc > 255) {
                // Value out of [0..255] range
                return false;
            }
        }
        else if (c == '.')
        {
            if (dots == 3) {
                // Too much dots (there must be 3 dots)
                return false;
            }
            bytes[dots++] = acc;
            acc = 0;
        }
        else
        {
            // Invalid char
            return false;
        }
    }

    if (dots != 3) {
        // Too few dots (there must be 3 dots)
        return false;
    }

    bytes[3] = acc;

    IPAddress ipAddr = IPAddress(bytes[0], bytes[1], bytes[2], bytes[3]);
    *destAddr = ipAddr;

    return true;
}

unsigned char h2int(char c)
{
    if (c >= '0' && c <='9'){
        return((unsigned char)c - '0');
    }
    if (c >= 'a' && c <='f'){
        return((unsigned char)c - 'a' + 10);
    }
    if (c >= 'A' && c <='F'){
        return((unsigned char)c - 'A' + 10);
    }
    return(0);
}

void urldecode(char *urlbuf)
{
    char c;
    char *dst;
    dst=urlbuf;
    while ((c = *urlbuf)) {
        if (c == '+') c = ' ';
        if (c == '%') {
            urlbuf++;
            c = *urlbuf;
            urlbuf++;
            c = (h2int(c) << 4) | h2int(*urlbuf);
        }
        *dst = c;
        dst++;
        urlbuf++;
    }
    *dst = '\0';
}
