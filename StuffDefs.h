//
// Created by Andri Yadi on 12/4/15.
//

#ifndef GALLON_STUFFDEFS_H
#define GALLON_STUFFDEFS_H

#include <Arduino.h>
#include "string.h"

#include "constants.h"

struct StuffAttr {
    uint16_t waterLevelPercent;
    uint16_t alertThreshold;
    boolean state;
    int8_t command;
    uint16_t rawValues[10];
    boolean ldr;
};

struct StuffConfig
{
    char ssidName[20];        //sn
    char ssidPass[12];        //sp
    char ipAddress[15];
    boolean configured = 0;
    boolean wifiConfigured = 0;
    boolean configInitialized = 0;
    char apName[20];
    char apPass[10];
    char iotHubName[50];
    char mqttServer[100];
    //char smsDestinations[100];
    boolean appPaired = 0;
    uint8_t sensorOrientation = 1;
    //uint16_t emptyThresholds[10] = {170,122,128,125,149,72,70,73,75,100};   //buat botol kecil, beda device beda nilai nya
    uint16_t emptyThresholds[6] = {259,	206, 242, 294, 290, 287};
    //uint16_t emptyThresholds[10] = {193,165,157,142,187,72,70,73,75,100};     //buat galon kecil
    //uint16_t emptyThresholds[10] = {150,115,114,157,109,104,103,100,108,183};     //buat galon besar
    //uint16_t emptyThresholds[10] = {127,95,94,93,93,91,85,83,83,108};     //buat galon besar terbaru
    //uint16_t emptyThresholds[10] = {127,99,97,93,95,91,87,86,100,115};     //buat galon besar 7 sensor
};

struct StuffDevice {
    char id[41];
    char name[31];
    char types[31];
    char subtypes[31];
};

struct StuffAccount {
    char userId[31];
    char userToken[161];
    uint32_t tokenExpiry;
};

struct StuffProps
{
    StuffDevice device;

    StuffConfig config;
    StuffAttr attr
    = {
		100,
		20,
		0
    };

    StuffAccount account;
    //uint16_t crc;

    void init() {
//		attr = {
//				100,
//				20,
//				0
//		};

//		device = {
//				"x61110a6-4d0a-4382-bb73-534204fef25z",
//				"My Smart Gallon",
//				"SmartGallon",
//				"Generic"
//		};

    	strcpy(device.id, DEFAULT_DEVICE_ID);
		strcpy(device.name, DEFAULT_DEVICE_NAME);
		strcpy(device.types, DEFAULT_DEVICE_TYPES);
		strcpy(device.subtypes, DEFAULT_DEVICE_SUBTYPES);

		strcpy(config.apName, DEFAULT_AP_NAME);
		strcpy(config.apPass, DEFAULT_AP_PASS);
		strcpy(config.ssidName, DEFAULT_WIFI_SSID);
		strcpy(config.ssidPass, DEFAULT_WIFI_PASSWORD);
		strcpy(config.ipAddress, DEFAULT_IP_ADDR);

		config.configured = false;
		config.configInitialized = true;

		//strncpy(account.userId, "stub2", 30);
		//strncpy(account.userToken, "xiVATgZ7GlxhOxKYQnJzaIgP3iuszOq2", 32);
		//strcpy(account.userToken, "stub2");

#if TEST_MODE
		strcpy(config.iotHubName, "dycodeiot");
		strcpy(config.mqttServer, "dycodeiot.azure-devices.net");
		config.wifiConfigured = true;

		strcpy(account.userId, "dycodeiot.azure-devices.net/gallon-01");
		strcpy(account.userToken, "SharedAccessSignature sr=dycodeiot.azure-devices.net/devices/gallon-01&sig=lG1RlLpC8uyq6zoPtKI4FfCoFCR7QyZfaPuzhEEFVxM%3D&se=1459525685");
		account.tokenExpiry = 1459525685;

#else
		strcpy(config.iotHubName, "");
		strcpy(config.mqttServer, "");
		config.wifiConfigured = false;

		strcpy(account.userId, "");
		strcpy(account.userToken, "");
		account.tokenExpiry = 0;
#endif

	}
};

#endif //GALLON_STUFFDEFS_H
