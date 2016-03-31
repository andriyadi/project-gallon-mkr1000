//
// Created by Andri Yadi on 12/16/15.
//

#ifndef SMARTGALLON_TIMEUTILS_H
#define SMARTGALLON_TIMEUTILS_H

#include <WiFi101.h>
#include <WiFiUdp.h>
#include <time.h>
#include <RTCZero.h>

class TimeService {

public:
	TimeService(int timeZone);
	~TimeService();
	void begin();
	void sync();
	void setRealTimeClock(uint32_t epoch);
	void loop();
	RTCZero &getRtc();

private:
	int timeZone_;
	RTCZero rtc_;

	uint32_t prevLastSyncMillis_ = 0;
	uint32_t sysTime_ = 0;

	// send an NTP request to the time server at the given address
	void sendNTPpacket(IPAddress& address);
	time_t getNtpTime();
};

#endif //SMARTGALLON_TIMEUTILS_H
