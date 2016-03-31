/*
 * SmartGallom_MKR1000.ino
 *
 *  Created on: Mar 28, 2016
 *      Author: Andri Yadi
 *      Copyright 2016 DycodeX. All rights reserved
 */

#include "Arduino.h"
#include <Wire.h>
#include "LedBar.h"
#include "AppSettingsStorage.h"
#include "WifiManager.h"
#include "NetworkService.h"
#include "TimeService.h"
#include "Service.h"
#include "constants.h"

volatile uint32_t lastHardResetReq = 0;
volatile boolean hardResetRequested = false;

LedBar *ledBar;
TimeService timeSvc(TIMEZONE_OFFSET);
AppSettingsStorage appSetting;
WifiManager wifiMgr(appSetting);
NetworkService networkSvc(appSetting);
Service svc(appSetting);

void onWifiConnected(boolean newConn) {
	Serial.println(F("WiFi just connected..."));
	timeSvc.sync();
	svc.startServe();
	networkSvc.onWifiConnected(newConn);
}

void doHardReset() {
	DEBUG_SERIAL(F("HARD Resetting...\r\n"));
	ledBar->allOff();

	delay(500);

	pinMode(RESET_PIN, OUTPUT);
	digitalWrite(RESET_PIN, LOW);
}

void onWifiNeedReset(boolean hardReset) {
	DEBUG_SERIAL(F("HARD Reset requested...\r\n"));

//	delay(1000);
//	pinMode(RESET_PIN, OUTPUT);
//	digitalWrite(RESET_PIN, LOW);

	hardResetRequested = true;
	lastHardResetReq = millis();
}

void dispenserLedOn();
void dispenserLedOff() {

	DEBUG_SERIAL(F("LDR OFF!!!\r\n"));
	//uint8_t state = digitalRead(LDR_PIN);
	svc.onDispenserLedChanged(false);

	detachInterrupt(LDR_PIN);
	attachInterrupt(LDR_PIN, dispenserLedOn, FALLING);
}

void dispenserLedOn(){
	DEBUG_SERIAL(F("LDR ON!!!\r\n"));
    //uint8_t state = digitalRead(LDR_PIN);
    svc.onDispenserLedChanged(true);

    detachInterrupt(LDR_PIN);
    attachInterrupt(LDR_PIN, dispenserLedOff, RISING);
}

void handleGenericButtonLongPressed() {
	appSetting.init();

	delay(1000);
	//restart
	onWifiNeedReset(true);
}

volatile uint32_t lastGenericButtonChanged = 0, lastGenericButtonClicked = 0;
void genericButtonInterrupted() {

	if (millis() - lastGenericButtonChanged <= 100) {
		return;
	}
	lastGenericButtonChanged = millis();

	uint8_t state = digitalRead(BUTTON_PIN);
	if (state) {
		lastGenericButtonClicked = millis();
		DEBUG_SERIAL(F("Generic Button clicked\r\n"));
	}
	else {
		lastGenericButtonClicked = 0;
		DEBUG_SERIAL(F("Generic Button released\r\n"));
	}
}

void setup()
{
	Serial.begin(115200);
//	while (!Serial) {
//		; // wait for serial port to connect. Needed for native USB port only
//	}

	delay(3000);
	hardResetRequested = false;

	//pinMode(RESET_PIN, OUTPUT);
	//digitalWrite(RESET_PIN, HIGH);

	Wire.begin();

	Serial.println("Gallon is ready to rock and roll!");
	appSetting.load();

#if LED_BAR_I2C
    ledBar = new LedBar(0x20);    //atau 0x38
#else
    ledBar = new LedBar(pins, (sizeof(pins) / sizeof(uint8_t)));
#endif

    ledBar->begin();
    //ledBar->startAnimation();

	pinMode(6, OUTPUT);
	digitalWrite(6, HIGH);

#if (LDR_AVAILABLE)
	pinMode(LDR_PIN, INPUT_PULLDOWN);
    attachInterrupt(LDR_PIN, dispenserLedOn, FALLING);
#endif

    pinMode(BUTTON_PIN, INPUT_PULLDOWN);
    attachInterrupt(BUTTON_PIN, genericButtonInterrupted, CHANGE);

	svc.begin();

	wifiMgr.setOnWifiConnectedHandler(onWifiConnected);
	wifiMgr.setOnResetRequiredHandler(onWifiNeedReset);

	timeSvc.begin();
	wifiMgr.begin();

	networkSvc.begin();
}

volatile uint32_t lastBlink = 0;
volatile bool ledOn = false;
// The loop function is called in an endless loop
void loop()
{
	//Add your repeated code here
	ledBar->loop();

	svc.loop();
	wifiMgr.loop();
	networkSvc.loop();
	timeSvc.loop();

	//only for heartbeat
	if (millis() - lastBlink> 3000) {
		lastBlink = millis();
		digitalWrite(6, ledOn? HIGH: LOW);
		ledOn = !ledOn;
	}

	if (hardResetRequested && (millis() - lastHardResetReq >= 5000)) {
		hardResetRequested = false;
		doHardReset();
	}

	if (lastGenericButtonClicked != 0 && millis() - lastGenericButtonClicked > 2000) {
		lastGenericButtonClicked = 0;
		DEBUG_SERIAL(F("Long pressed\r\n"));
		handleGenericButtonLongPressed();
	}
}
