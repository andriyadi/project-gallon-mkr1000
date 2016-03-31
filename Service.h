/*
 * Service.h
 *
 *  Created on: Mar 30, 2016
 *      Author: andri
 */

#ifndef SERVICE_H_
#define SERVICE_H_

#include "StuffDefs.h"
#include "Adafruit_MPR121.h"
#include "AppSettingsStorage.h"
#include "constants.h"

class Service {
public:
	Service(AppSettingsStorage &settings);
	virtual ~Service();

	void begin();
	void loop();
	void startServe();
	void stopServe();

	void onStuffAttrChanged();
	void onDispenserLedChanged(boolean state);

	uint16_t getWaterLevelPercentage();

private:
	AppSettingsStorage &settings_;
	bool serviceStarted_ = false;
	Adafruit_MPR121 *mpr121_ = NULL;

	bool switchState_ = true;

	//LedBar *ledBar_ = NULL;
	uint16_t lastWaterLevelPercent_ = 0;

	volatile unsigned long lastSensorQueryTime_, lastRespondTime_;

	void doSomethingAboutWaterLevel(uint16_t percent);

	void _debugStuffProps(StuffProps props, Print& p);
	void _debugStuffConfig(StuffConfig config, Print &p);
};

#endif /* SERVICE_H_ */
