/*
 * Service.cpp
 *
 *  Created on: Mar 30, 2016
 *      Author: andri
 */

#include "Service.h"
#include "LedBar.h"
#include "NetworkService.h"

extern LedBar *ledBar;
extern NetworkService networkSvc;

Service::Service(AppSettingsStorage &settings):settings_(settings) {
	// TODO Auto-generated constructor stub

}

Service::~Service() {
	// TODO Auto-generated destructor stub
}

void Service::begin() {
	mpr121_ = new Adafruit_MPR121();

	//    m_MPR121->begin(0x5A);

	if (!mpr121_->begin(0x5A)) {
		DEBUG_SERIAL(F("MPR121 not found, check wiring?\r\n"));
		delete mpr121_;
		mpr121_ = NULL;

	}
	else {
		DEBUG_SERIAL(F("MPR121 found!\r\n"));
	}

//#if LED_BAR_I2C
//	m_LedBar = new LedBar(0x38);    //atau 0x20
//	Serial.println("LEDBAR Control using PCF8574");
//#else
//	m_LedBar = new LedBar(pins, (sizeof(pins) / sizeof(uint8_t)));
//	Serial.println("LEDBAR Control using GPIO");
//#endif
//	m_LedBar->begin();

	//m_LedBar->startAnimation();

	lastSensorQueryTime_ = millis();
	lastRespondTime_ = millis();

	pinMode(SSR_PIN, OUTPUT);
	digitalWrite(SSR_PIN, HIGH);

	settings_.props.attr.state = true;
	_debugStuffProps(settings_.props, Serial);
}

void Service::loop() {
	if (mpr121_ == NULL) {
		return;
	}

	if (!serviceStarted_) {
		return;
	}

	//below are things to serve
	if ((millis() - lastSensorQueryTime_) > SENSOR_QUERY_INTERVAL) {

		boolean shouldResponse = (boolean) (millis() - lastRespondTime_ > RESPOND_WATERLEVEL_INTERVAL);
		uint16_t percent = getWaterLevelPercentage();

#ifdef DEBUG_SERIAL
		Serial.print("Water level = "); Serial.println(percent);
#endif

		if (shouldResponse) {
			doSomethingAboutWaterLevel(percent);
			lastRespondTime_ = millis();
		}

		lastSensorQueryTime_ = millis();
	}

	delay(10);
}

void Service::startServe() {
	serviceStarted_ = true;
}

void Service::stopServe() {
	serviceStarted_ = false;
}

void Service::onStuffAttrChanged() {
//	if (settings_.props.attr.state)

	switchState_ = settings_.props.attr.state;
	digitalWrite(SSR_PIN, switchState_);
}

void Service::onDispenserLedChanged(boolean state) {
#ifdef DEBUG_SERIAL
	Serial.println(state? "Hot Water Ready!!!": "Oh no, water is not ready!");
#endif

	settings_.props.attr.ldr = state;

	settings_.save();

	//if (millis() - lastRespondTime_ > RESPOND_WATERLEVEL_INTERVAL/5) {
		networkSvc.onNeedToPublishTelemetry();
	//}
}

void Service::doSomethingAboutWaterLevel(uint16_t percent) {
	uint16_t thresh = settings_.props.attr.alertThreshold == 0? 20: settings_.props.attr.alertThreshold;

	if (ledBar && !ledBar->isAnimating()) {
		ledBar->setPercentage(percent);
		ledBar->setBlink((boolean) (percent <= thresh));
	}

	//strcpy(m_StuffProps.device.name, "Oi Oi Oi");
	settings_.props.attr.waterLevelPercent = percent;

//	if (percent <= thresh && m_AlertHandler) {
//		boolean shouldSend = (LastWaterLevelPercent_ > thresh);
//		m_AlertHandler(percent, shouldSend);
//	}

	if (percent <= thresh || switchState_ == false){
	  digitalWrite(SSR_PIN, LOW);
	  //Serial.println("Mati....");
	}
	else {
	  digitalWrite(SSR_PIN, HIGH);
	  //Serial.println("Nyala....");
	}

	lastWaterLevelPercent_ = percent;
}

void Service::_debugStuffProps(StuffProps props, Print& p) {
#ifdef DEBUG_SERIAL
    p.print(F("Device id: "));
    p.println(props.device.id);
    p.print(F("Device name: " ));
    p.println(props.device.name);
    p.print(F("Water Level: " ));
    p.println(props.attr.waterLevelPercent);
    //_debugStuffAttr(props.attr, p);
    _debugStuffConfig(props.config, p);
#endif
}

uint16_t Service::getWaterLevelPercentage() {

	uint8_t sensorProbesCount = SENSOR_PROBES_COUNT;

    uint16_t *avgSensorValues = (uint16_t *) malloc(sensorProbesCount * sizeof(float_t));

    for (uint8_t i = 0; i < sensorProbesCount; i++) {

        avgSensorValues[i] = mpr121_->filteredData(i);
        //m_StuffProps.attr.rawValues[i] = avgSensorValues[i];

#ifdef DEBUG_SERIAL
		Serial.print(avgSensorValues[i]);Serial.print("\t");
#endif
    }

#ifdef DEBUG_SERIAL
    //DEBUG_SERIAL("%d\t", (uint16_t)avgSensorValues[i]);
    Serial.println();
#endif

    uint16_t sum = 0;

    for(uint8_t i = 0; i < sensorProbesCount; i++) {
        sum += (uint16_t)avgSensorValues[i];
    }

#ifdef DEBUG_SERIAL
		Serial.print("Sum all values = "); Serial.println(sum);
#endif

    free(avgSensorValues);

    uint16_t emptyThresh = 537;
    uint16_t fullThresh = 316;

    float_t waterLevelPercent = ((1.0f*abs(emptyThresh - sum)/(1.0f*abs(emptyThresh - fullThresh))))*100.0f - 8.0f;
    uint16_t reminder = (uint16_t)waterLevelPercent % 10;
    uint16_t front = waterLevelPercent - reminder;
    waterLevelPercent = front + (reminder > 5? 10: 0);

    //waterLevelPercent = ((uint16_t)waterLevelPercent - (uint16_t)waterLevelPercent % 10);

    return (uint16_t) waterLevelPercent > 100? 100: (uint16_t) waterLevelPercent;
}

void Service::_debugStuffConfig(StuffConfig config, Print& p) {
}
