/*
 * NetworkService.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#include "NetworkService.h"
#include "TimeService.h"
#include "WifiManager.h"
#include "Service.h"
#include <ArduinoJson.h>

extern TimeService timeSvc;
extern WifiManager wifiMgr;
extern Service svc;

// MQTT settings
//char mqttServer[] = "dycodeiot.azure-devices.net";
//char mqttClientName[] = "MKR1000-1";
//char mqttUsername[] = "dycodeiot.azure-devices.net/MKR1000-1"; // your MQTT username
//char mqttPassword[] = "SharedAccessSignature sr=dycodeiot.azure-devices.net%2fdevices%2fMKR1000-1&sig=vlFchg77cdiIG1Wa%2bB3IR2gRkvRkXrBMku5faoxJf18%3d&se=1461702034"; // your MQTT password
//char mqttPassword[] = "SharedAccessSignature sr=dycodeiot.azure-devices.net/devices/MKR1000-1&sig=mW%2FCDy1h9fR8ljyjN4WXB%2BdV4wcxrjtUsiLVY2NJcPk%3D&se=1459519206"; // your MQTT password

char mqttPubTopicTemplate[] = "devices/%s/messages/events/"; // your MQTT topic /<username>/topic
char mqttSubsTopicTemplate[] = "devices/%s/messages/devicebound/#";

WiFiSSLClient wifiSSLClient;

String lastC2dPayload = "";
volatile boolean needToProcessC2dPayload = false;

String getDateTimeString() {

	RTCZero rtc = timeSvc.getRtc();

//  int hInt = isAM()? hourFormat12(): (hourFormat12() + 12);
	int hInt = rtc.getHours();
	String h = ((hInt < 10) ? "0" : "") + String(hInt);
	String _dateTime = "20" + String(rtc.getYear()) + "-" + ((rtc.getMonth() < 10) ? "0" : "")
			+ String(rtc.getMonth()) + "-" + ((rtc.getDay() < 10) ? "0" : "") + String(rtc.getDay())
			+ "T" + h + ":" + ((rtc.getMinutes() < 10) ? "0" : "") + String(rtc.getMinutes())
			+ ":" + ((rtc.getSeconds() < 10) ? "0" : "") + String(rtc.getSeconds()) + ".000Z";

	return _dateTime;
}

NetworkService::NetworkService(AppSettingsStorage& settings):
settings_(settings),
mqttClient_(NULL) {

}

NetworkService::~NetworkService() {
	if (mqttClient_ != NULL) {
		delete mqttClient_;
		mqttClient_ = NULL;
	}
}

void NetworkService::begin() {
	lastMqttConnectionTrialMillis_ = 0;
	lastTelemetryPublishMillis_ = 0;
}

//volatile uint32_t lastShow = 0;
void NetworkService::loop() {
	//if (timeStatus() == timeNotSet) {
		//DEBUG_SERIAL("Resync time\r\n");
		//beginSyncTime();
	//}

//	if (millis() - lastShow > 5000) {
//		lastShow = millis();
//		Serial.print("Hour: ");
//		Serial.print(timeSvc.getRtc().getHours()); Serial.print(":");
//		Serial.print(timeSvc.getRtc().getMinutes()); Serial.print(":");
//		Serial.println(timeSvc.getRtc().getSeconds());
//	}


	if (mqttClient_ != NULL && wifiMgr.isWifiConnected()) {

		if (!mqttClient_->connected()) {
			//DEBUG_SERIAL("NOT CONNECTED to MQTT\r\n");
			if ((millis() - lastMqttConnectionTrialMillis_ > 15000) && !isMqttConnecting_) {

				DEBUG_SERIAL("RECONNECTING to MQTT\r\n");
				reconnectMqtt();
			}
		}

		if (mqttClient_->connected()) {

			mqttClient_->loop();

			/*
			//if (!_appPaired &&
			if ((millis() - lastTelemetryPublishMillis_ > DATA_PUBLISH_INTERVAL/2)) {

				DEBUG_SERIAL("Publishing props to MQTT\r\n");
				//uint16_t data = service.getWaterLevelPercentage();
				//publishDataToThingspeak(data);

				publishPropsToMqtt();
				lastTelemetryPublishMillis_ = millis();
			}
			*/

			if (lastTelemetryPublishMillis_ == 0 || (millis() - lastTelemetryPublishMillis_ > DATA_PUBLISH_INTERVAL)) {

				DEBUG_SERIAL("Publishing to MQTT\r\n");
				//uint16_t data = service.getWaterLevelPercentage();
				//publishDataToThingspeak(data);

				publishTelemetryToMqtt();
				lastTelemetryPublishMillis_ = millis();
			}
		}
	}

	if (needToProcessC2dPayload) {
		needToProcessC2dPayload = false;
		processCloud2DeviceMessage();
	}

	/*
	if (webClient_ != NULL) {
		while (webClient_->available()) {
			char c = webClient_->read();
			Serial.write(c);
		}

		// if the server's disconnected, stop the client:
		if (!webClient_->connected()) {
			DEBUG_SERIAL(F("Disconnecting from web server."));
			DEBUG_SERIAL("\r\n");
			webClient_->stop();

			delay(10);
			delete webClient_;
			webClient_ = NULL;
		}
	}*/
}

/*
void NetworkService::retrieveAccountDetails() {
	if (webClient_ == NULL) {
		webClient_ = new WiFiClient();
	}

	char _urlPath[50];
	snprintf(_urlPath, sizeof _urlPath, "/api/iothub/%s", settings_.props.device.id);

	DEBUG_SERIAL(_urlPath); DEBUG_SERIAL_LN;

	if (webClient_->connect(API_HOST, API_PORT)) {
		DEBUG_SERIAL(F("connected to Web Server\r\n"));
		// Make a HTTP request:
		webClient_->println("GET " + String(_urlPath) + " HTTP/1.1");
		webClient_->println("Accept: application/json");
		webClient_->println("Host: " + String(API_HOST));
		webClient_->println("Connection: close");
		webClient_->println();
	 }
}
*/

void NetworkService::onWifiConnected(boolean newConnection) {
	//retrieveAccountDetails();
	setupMqttClient();
	reconnectMqtt();
}

void NetworkService::setupMqttClient() {
	DEBUG_SERIAL("Setup MQTT\r\n");

	if (strcmp(settings_.props.config.mqttServer, "") == 0) {
		DEBUG_SERIAL(F("NO MQTT SERVER set!!\r\n"));
		return;
	}

	if (mqttClient_ == NULL) {
		mqttClient_ = new PubSubClient(settings_.props.config.mqttServer, MQTT_PORT, wifiSSLClient);

		//_mqttClient.setServer(m_Config.mqttServer, m_Config.mqttPort);
		mqttClient_->setCallback(NetworkService::mqttCallback);
	}
}

void NetworkService::reconnectMqtt() {

	isMqttConnecting_ = true;
	lastMqttConnectionTrialMillis_ = millis();

	const char* _clientName = settings_.props.device.id;
	String _userId = String(settings_.props.config.mqttServer) + "/" + _clientName;

#if (DEBUG_SERIAL_DIRECT)
	Serial.print(F("Reconnecting to MQTT Broker. Client name: "));
	Serial.println(_clientName);
	Serial.println(_userId);
	Serial.println(settings_.props.account.userToken);
#endif

	int numTrials = 0;
	//while (!mqttClient_->connect(_clientName, mqttUsername, mqttPassword)) {
	while (!mqttClient_->connect(_clientName, _userId.c_str(), settings_.props.account.userToken)) {
		Serial.print(".");
		//Serial.print(mqttClient.state());

		numTrials++;
		if (numTrials >= 3) {
			break;
		}
	}

	if (numTrials >= 3) {
		//not connected
#ifdef DEBUG_SERIAL
		Serial.println(F("NOT CONNECTED to MQTT Broker"));
#endif
	}
	else {

		char _topicBuff[256];
		snprintf(_topicBuff, sizeof _topicBuff, mqttSubsTopicTemplate,
				settings_.props.device.id);

#ifdef DEBUG_SERIAL
		Serial.print(F("Connected to MQTT broker.\r\nTopic: "));
		Serial.println(_topicBuff);
#endif

		mqttClient_->subscribe(_topicBuff);

		publishPropsToMqtt();

		//lastPropsPublishMillis_ = millis();
		lastTelemetryPublishMillis_ = millis();
	}

	isMqttConnecting_ = false;
}

void NetworkService::publishTelemetryToMqtt() {
#ifdef DEBUG_SERIAL
	Serial.print("Date: ");
	Serial.println(getDateTimeString());
#endif

	//uint16_t waterLevel = service.getWaterLevelPercentage();
	uint16_t waterLevel = settings_.props.attr.waterLevelPercent;

//	uint16_t rawValues[10];
//	for (uint8_t i=0; i<10; i++){
//	  rawValues[i] = service.getRawValues(i);
//	}

	//bool ldrValue = service.getLDRValue();
	bool ldrValue = settings_.props.attr.ldr;

	String json = "{";
	json += "\"deviceId\":\"" + String(settings_.props.device.id) + "\",";
	json += "\"state\":" + String(settings_.props.attr.state) + ",";
	json += "\"waterLevelPercent\":" + String(waterLevel) + ",";
	json += "\"datetime\":\"" + getDateTimeString() + "\",";
//	json += "\"rawValues\": [" + String(rawValues[0]);
//	for (uint8_t i=1; i<10; i++){
//	  json += "," + String(rawValues[i]);
//	}
//	json += "],";
	json += "\"ldr\":" + String(ldrValue);
	json += "}";

	const char *payload = json.c_str();
	Serial.println(payload);

	//"2015-10-04T01:01:01.000Z", //2015-10-04T23:06:32.000Z

	char _topicBuff[512];
	snprintf(_topicBuff, sizeof _topicBuff, mqttPubTopicTemplate,
					settings_.props.device.id);

#ifdef DEBUG_SERIAL
		Serial.print(F("Topic: "));
		Serial.println(_topicBuff);
#endif

	if (mqttClient_->publish(_topicBuff, payload)) {
		DEBUG_SERIAL("Published Metric to MQTT\r\n");
		//indicatorLed.setColor(Color::blue);
	}
	else {
		DEBUG_SERIAL(F("Publish Metric to MQTT failed!!!\r\n"));
		DEBUG_SERIAL(F("Error code: "));
		DEBUG_SERIAL(mqttClient_->state());
		DEBUG_SERIAL_LN;

		//indicatorLed.setColor(Color::red);
	}
}

void NetworkService::onNeedToPublishTelemetry() {
	publishTelemetryToMqtt();
	lastTelemetryPublishMillis_ = millis();
}

void NetworkService::processCloud2DeviceMessage() {
	if (lastC2dPayload.equals("")) {
		return;
	}

	DEBUG_SERIAL(F("Payload to process: "));
	DEBUG_SERIAL(lastC2dPayload);
	DEBUG_SERIAL_LN;

	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(lastC2dPayload);
	if (root.containsKey("name")) {
		String cmdName = root["name"];

		if (cmdName.equals("control")) {
			JsonObject& parameters = root["parameters"];
			if (parameters.containsKey("state")) {
				bool state = parameters["state"];
				settings_.props.attr.state = state;
				Serial.print(F("Control state: "));
				Serial.println(state);

				svc.onStuffAttrChanged();
			}
		}
	}

	lastC2dPayload = "";
}

void NetworkService::mqttCallback(char* topic, byte* payload, unsigned int length) {

	if (length == 0) {
		return;
	}

//	byte* p = (byte*)malloc(length+1);
//	memset(p, 0, length);
//	//os_memcpy(p, payload, length);
//
//	unsigned int idx = 0;
//	for (idx = 0; idx < length; idx++) {
//
//		if (payload[idx] == '}') {
//			//Serial.println("End of payload");
//			p[idx] = payload[idx];
//			break;
//		}
//
//		p[idx] = payload[idx];
//	}

//	char realPayload[length];
//	strncpy(realPayload, reinterpret_cast<const char*>(payload), length);

	String realPayload = "";
	realPayload.reserve(length);
	for (uint32_t i = 0; i < length; i++) {
		realPayload += (char)payload[i];
	}

	lastC2dPayload = realPayload;
	needToProcessC2dPayload = true;

	//p[idx++] = '}';

#ifdef DEBUG_SERIAL
	Serial.print(F("Payload: "));
	Serial.println(realPayload);
	Serial.print(F("Payload length: "));
	Serial.println(length);
#endif
}

void NetworkService::publishPropsToMqtt() {

}
