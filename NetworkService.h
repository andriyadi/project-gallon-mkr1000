/*
 * NetworkService.h
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#ifndef NETWORKSERVICE_H_
#define NETWORKSERVICE_H_

#include <PubSubClient.h>
#include "AppSettingsStorage.h"
#include "WiFi101.h"

class NetworkService {
public:
	NetworkService(AppSettingsStorage &settings);
	virtual ~NetworkService();

	void begin();
	void loop();
	void onWifiConnected(boolean newConnection);
	void onNeedToPublishTelemetry();

private:
	AppSettingsStorage &settings_;
	PubSubClient *mqttClient_ = NULL;
	//WiFiClient *webClient_ = NULL;

	volatile unsigned long lastMqttConnectionTrialMillis_, lastTelemetryPublishMillis_;
	volatile boolean isMqttConnecting_ = false;

	//void retrieveAccountDetails();

	void setupMqttClient();
	void reconnectMqtt();
	void publishTelemetryToMqtt();

	static void mqttCallback(char* topic, byte* payload, unsigned int length);
	void publishPropsToMqtt();
	void processCloud2DeviceMessage();
};

#endif /* NETWORKSERVICE_H_ */
