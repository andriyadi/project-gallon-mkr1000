/*
 * TimeUtils.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#include "TimeService.h"

#include "constants.h"

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
const char* ntpServerName = "time.nist.gov";

//const int timeZone = 7;     // Central European Time

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 * Lookup the IP address for the host name instead */

//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address

char packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;
unsigned int localPort = 2390;      // local port to listen for UDP packets

TimeService::TimeService(int tz):
		timeZone_(tz) {

}

TimeService::~TimeService() {

}

void TimeService::setRealTimeClock(uint32_t epoch) {

	time_t rawtime;
	struct tm * timeinfo;

	rawtime = (time_t) epoch;
	timeinfo = localtime(&rawtime);

	Serial.println(F("Current local time and date"));
	Serial.print(asctime(timeinfo));

	rtc_.setEpoch(epoch);

//	rtc_.setSeconds(timeinfo->tm_sec);
//	rtc_.setMinutes(timeinfo->tm_min);
//	rtc_.setHours((timeinfo->tm_hour) + timeZone_);
//
//	rtc_.setDay(timeinfo->tm_mday);
//	rtc_.setMonth((timeinfo->tm_mon) + 1); //tm_mon months since January - [ 0 to 11 ]
//	rtc_.setYear((timeinfo->tm_year) - 100); //tm_year years since 1900 and format is yy
}

void TimeService::begin() {
	rtc_.begin();
}

void TimeService::loop() {
	if (prevLastSyncMillis_ > 0) { //prev already started by calling sync directly from outside
		if (millis() - prevLastSyncMillis_ > TIME_SYNC_INTERVAL) {
			sync();
		}

		//if still zero
		if (sysTime_ == 0 && (millis() - prevLastSyncMillis_ > (60*1000))) {
			Serial.println(F("Hey, Epoch is still zero. Syncing..."));
			sync();
		}
	}
}

void TimeService::sync() {

    udp.begin(localPort);
    Serial.print("Local port: ");
    Serial.println(localPort);

    //get a random server from the pool
    WiFi.hostByName(ntpServerName, timeServerIP);

    prevLastSyncMillis_ = millis();

    Serial.println(F("Waiting for sync"));

    //setSyncProvider(getNtpTime);

    int numberOfTries = 0, maxTries = 6;
	do {
		sysTime_ = getNtpTime();
		numberOfTries++;
	} while ((sysTime_ == 0) || (numberOfTries > maxTries));

	udp.stop();

	if (sysTime_ == 0 || (numberOfTries > maxTries)) {
		Serial.print(F("NTP may be unreachable! Cannot set RTC"));
	}
	else {
		Serial.print(F("Epoch received: "));
		Serial.println(sysTime_);
		setRealTimeClock(sysTime_);
	}
}

// send an NTP request to the time server at the given address
void TimeService::sendNTPpacket(IPAddress& address)
{
    Serial.println(F("Sending NTP packet..."));
    // set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    udp.beginPacket(address, 123); //NTP requests are to port 123
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
}

RTCZero& TimeService::getRtc() {
	return rtc_;
}

time_t TimeService::getNtpTime()
{
	while (udp.parsePacket() > 0) ; // discard any previously received packets
	Serial.println("Transmit NTP Request");

	sendNTPpacket(timeServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			Serial.println("Receive NTP Response");
			udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];

			const unsigned long seventyYears = 2208988800UL;
			return secsSince1900 - seventyYears + (timeZone_ * 3600UL);
		}
	}

	Serial.println(F("No NTP Response :-("));
	return 0; // return 0 if unable to get the time

//	unsigned long epoch = 0;
//
//	udp.begin(localPort);
//	sendNTPpacket (timeServerIP); // send an NTP packet to a time server
//
//	// wait to see if a reply is available
//	delay(1000);
//	uint32_t beginWait = millis();
//	while (millis() - beginWait < 1500) {
//		int size = udp.parsePacket();
//		if (size >= NTP_PACKET_SIZE) {
//			Serial.println("NTP time received");
//			// We've received a packet, read the data from it
//			udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
//
//			//the timestamp starts at byte 40 of the received packet and is four bytes,
//			// or two words, long. First, esxtract the two words:
//
//			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
//			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
//			// combine the four bytes (two words) into a long integer
//			// this is NTP time (seconds since Jan 1 1900):
//			unsigned long secsSince1900 = highWord << 16 | lowWord;
//
//			// now convert NTP time into everyday time:
//			// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
//			const unsigned long seventyYears = 2208988800UL;
//			// subtract seventy years:
//			epoch = secsSince1900 - seventyYears;
//
//			break;
//		}
//	}
//
//	return epoch;
}



