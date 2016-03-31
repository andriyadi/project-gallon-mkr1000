/*
 * WifiManager.h
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#ifndef WIFIMANAGER_H_
#define WIFIMANAGER_H_

#include "Arduino.h"
#include "AppSettingsStorage.h"
#include "GallonUtils.h"
#include <WiFi101.h>

#include <WiFiMDNSResponder.h>

//#include <functional>

class WifiManager {
public:

#ifdef _GLIBCXX_FUNCTIONAL
  typedef std::function<void(const boolean)> wifi_callback_t;
#else
  typedef void(*wifi_callback_t)(const boolean);
#endif

	WifiManager(AppSettingsStorage &settings);
	virtual ~WifiManager();

	void begin();
	void loop();
//	typedef std::function<void(boolean)> WifiConnectedHandler;
	void setOnWifiConnectedHandler(wifi_callback_t handler);
	void setOnResetRequiredHandler(wifi_callback_t handler);
	bool isWifiConnected();

private:
	AppSettingsStorage &settings_;
	wifi_callback_t wifiConnectedHandler_, resetRequiredHandler_;

	WiFiServer *webServer_ = NULL;
	//WiFiMDNSResponder *mDNS_ = NULL;

	volatile boolean m_WifiConnectRequested = false;
	volatile boolean m_ConnectedToWifi = false;
	volatile boolean m_ConnectingToWifi = false;
	volatile unsigned long wifiConnCheckingMillis_ = 0;
	byte wifiConnTrial_ = 0;

	void tryToConnectWifi();
	void checkWifiConn();
	void onWifiConnected();
	void onWifiNotConnected();
	void setupWebServer();
	void processHttpRequest();
    void setupAccessPoint();
	void _debugConfig(StuffConfig _config, Print& p);

	//web handler
	void handleNotFound(WiFiClient client);
	void handleGet(String path, WiFiClient client);
	void handleGetRoot(WiFiClient client);
	void handleGetAPListJson(WiFiClient client);
	void handleSetupWifi(String qs, WiFiClient client);
};

#endif /* WIFIMANAGER_H_ */
