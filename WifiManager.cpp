/*
 * WifiManager.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#include "WifiManager.h"
#include "LedBar.h"

extern LedBar *ledBar;

#define HTTP_REQ_BUFFER_SIZE 			500
#define HTTP_REQ_PROCESS_TIMEOUT_MS 	1000
#define HTTP_REQ_MAX_PATH_LEN 			300
#define HTTP_REQ_MAX_ACTION_LEN 		10

//char httpRequestBuffer [BUFFER_SIZE+1];
//char httpRequestPath [MAX_PATH_LEN];
//char httpRequestAction[MAX_ACTION_LEN + 1];

//char *httpRequestBuffer;
//char *httpRequestPath;
//char *httpRequestAction;
//int httpRequestBufferIndex = 0;



WifiManager::WifiManager(AppSettingsStorage &settings):settings_(settings),
wifiConnectedHandler_(NULL),
resetRequiredHandler_(NULL) {
}

WifiManager::~WifiManager() {
//	if (mDNS_ != NULL) {
//		delete mDNS_;
//	}
	if (webServer_ != NULL) {
		delete webServer_;
	}
}

void WifiManager::begin() {
	DEBUG_SERIAL("WifiService begin...\r\n");

	_debugConfig(settings_.props.config, Serial);

	if (settings_.props.config.wifiConfigured != 1 || (settings_.props.config.ssidName && !settings_.props.config.ssidName[0])) {
		onWifiNotConnected();
	}
	else {
		tryToConnectWifi();
	}
}

void WifiManager::loop() {

	if (!m_ConnectedToWifi && m_ConnectingToWifi) {
		if (millis() - wifiConnCheckingMillis_ > 500) {
			checkWifiConn();
			wifiConnCheckingMillis_ = millis();
		}
		return;
	}

//	if (m_WifiConnectRequested) {
//		//give delay a bit.
//		if (millis() - wifiConnCheckingMillis_ > 500) {
//			DEBUG_SERIAL( "WIFI Connect requested\r\n" );
//			m_WifiConnectRequested = false;
//
//			tryToConnectWifi();
//		}
//	}

//	if (mDNS_ != NULL) {
//		mDNS_->poll();
//	}

	//handle web server
	if (webServer_ != NULL && !m_WifiConnectRequested) {
		processHttpRequest();
	}

	if (m_WifiConnectRequested) {
		//give delay a bit.
		if (millis() - wifiConnCheckingMillis_ > 500) {
			DEBUG_SERIAL( "WIFI Connect requested\r\n" );
			m_WifiConnectRequested = false;

			tryToConnectWifi();
		}
	}
}

/*
void WifiManager::processHttpRequest() {
	WiFiClient client = webServer_->available();
	if (client) {
		Serial.println("New Web Client!!"); // print a message out the serial port

		boolean pathFound = false;
		String path = "";

		String currentLine = ""; // make a String to hold incoming data from the client
		while (client.connected()) {    // loop while the client's connected
			if (client.available()) { // if there's bytes to read from the client,
				char c = client.read();             // read a byte, then
				//Serial.write(c);          // print it out the serial monitor
				if (c == '\n') {       // if the byte is a newline character
					Serial.print(".");

					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine.length() == 0) {

						Serial.print("==> Path: ");
						Serial.println(path);
						lastReqPath_ = path;
						handleGet(path, client);
						// break out of the while loop:
						break;
					} else { // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				} else if (c != '\r') { // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine

					if (!pathFound) {
						if (currentLine.indexOf("GET /") >= 0) {
							if (c != ' ') {
								path += c;
							}
							else {
								pathFound = true;
							}
						}
					}
				}

				// Check to see if the client request was "GET /H" or "GET /L":
//					if (currentLine.endsWith("GET /api/aplist")) {
//						//handleWebAPListJson(client);
//						//break;
//					}
//					if (currentLine.endsWith("GET / HTTP")) {
//						//handleWebRoot(client);
//						//break;
//					}
			}
			else {
				//Serial.print("X");
				//handleGet(lastReqPath_, client);
				break;
			}
		}

		// give the web browser time to receive the data
		delay(1);

		// close the connection:
		client.stop();
		Serial.println("\r\nWeb client disconnected");
	}
}
*/

// Parse the action and path from the first line of an HTTP request.
void parseFirstLine(char* line, char* action, char* path) {
	// Parse first word up to whitespace as action.
	char* lineaction = strtok(line, " ");
	if (lineaction != NULL)
		strncpy(action, lineaction, HTTP_REQ_MAX_ACTION_LEN);
	// Parse second word up to whitespace as path.
	char* linepath = strtok(NULL, " ");
	if (linepath != NULL) {
		strncpy(path, linepath, HTTP_REQ_MAX_PATH_LEN);
	}
}

bool parseRequest(char* buf, int bufSize, char* action, char* path) {
	// Check if the request ends with \r\n to signal end of first line.
	if (bufSize < 2)
		return false;
	if (buf[bufSize - 2] == '\r' && buf[bufSize - 1] == '\n') {
		Serial.println(buf);
		parseFirstLine(buf, action, path);
		return true;
	}
	return false;
}

void WifiManager::processHttpRequest() {

	if (webServer_ == NULL) {
		return;
	}

	WiFiClient client = webServer_->available();
	if (!client) {
		return;
	}

	if (!client.connected() || !client.available()) {
		return;
	}

#ifdef DEBUG_SERIAL
	Serial.println(F("New client connected"));
#endif

	int httpRequestBufferIndex = 0;

	//httpRequestBuffer = (char*) malloc(BUFFER_SIZE + 1);
//	memset(&httpRequestBuffer, 0, BUFFER_SIZE + 1);
//	//httpRequestPath = (char*) malloc(MAX_PATH_LEN + 1);
//	memset(&httpRequestPath, 0, MAX_PATH_LEN + 1);
//	memset(&httpRequestAction, 0, sizeof(httpRequestAction));

	char *httpRequestBuffer = new char[HTTP_REQ_BUFFER_SIZE + 1]();
	char *httpRequestPath = new char[HTTP_REQ_MAX_PATH_LEN + 1]();
	char *httpRequestAction = new char[HTTP_REQ_MAX_ACTION_LEN + 1]();

	// Set a timeout for reading all the incoming data.
	unsigned long endtime = millis() + HTTP_REQ_PROCESS_TIMEOUT_MS + 1000;

	// Read all the incoming data until it can be parsed or the timeout expires.
	bool parsed = false;
	while (!parsed && (millis() < endtime) && (httpRequestBufferIndex < HTTP_REQ_BUFFER_SIZE)) {

		if (client.available()) {
			httpRequestBuffer[httpRequestBufferIndex++] = client.read();
		}

		parsed = parseRequest(httpRequestBuffer, httpRequestBufferIndex, httpRequestAction, httpRequestPath);
	}

	//Serial.println(httpRequestBuffer);

	if (parsed) {
#ifdef DEBUG_SERIAL
		Serial.println(F("Processing HTTP request"));
		Serial.print(F("HTTP Action: ")); Serial.println(httpRequestAction);
		Serial.print(F("HTTP Path: ")); Serial.println(httpRequestPath);
#endif
		// Check the action to see if it was a GET request.
		if (strcmp(httpRequestAction, "GET") == 0) {

			handleGet(String(httpRequestPath), client);

		} else {
			// Unsupported action, respond with an HTTP 405 method not allowed error.
			client.println(F("HTTP/1.1 405 Method Not Allowed"));
			client.println(F(""));
		}
	}

	// Wait a short period to make sure the response had time to send before
	// the connection is closed (the CC3000 sends data asyncronously).
	delay(100);

	//free(httpRequestBuffer);
	//free(httpRequestPath);
	delete [] httpRequestBuffer;
	delete [] httpRequestPath;
	delete [] httpRequestAction;

#ifdef DEBUG_SERIAL
	Serial.println(F("Client disconnected"));
#endif
	// Close the connection when done.
	client.stop();
}

void WifiManager::setOnWifiConnectedHandler(wifi_callback_t handler) {
	wifiConnectedHandler_ = handler;
}

void WifiManager::setOnResetRequiredHandler(wifi_callback_t handler) {
	resetRequiredHandler_ = handler;
}

bool WifiManager::isWifiConnected() {
	return m_ConnectedToWifi && !m_ConnectingToWifi;
}

void WifiManager::tryToConnectWifi() {

//	if (webServer_ != NULL) {
//		//mDNS_->stop();
//
//		delete webServer_;
//		webServer_ = NULL;
//	}

	delay(10);

	m_ConnectedToWifi = false;

#ifdef DEBUG_SERIAL
	Serial.println(F("Connecting as WiFi client...\r\n"));
	Serial.print(F("SSID Name: ")); Serial.println(settings_.props.config.ssidName);
#endif

	//WiFi.disconnect();
	uint8_t stat = WiFi.begin(settings_.props.config.ssidName, settings_.props.config.ssidPass);

	//check first
	//uint8_t stat = WiFi.status();
	if (stat == WL_CONNECTED) {
		m_ConnectedToWifi = true;
		m_ConnectingToWifi = false;

#ifdef DEBUG_SERIAL
		Serial.println(F("\r\nYay!!! WIFI is connected\r\n"));
#endif

		onWifiConnected();
		return;
	}

	//prepare to checking
	m_ConnectingToWifi = true;
	wifiConnTrial_ = 0;
	wifiConnCheckingMillis_ = millis();

	ledBar->startAnimation();
}

void WifiManager::checkWifiConn() {
	bool _giveUp = false;
	if (wifiConnTrial_ > 30000 / (500 * 2)) {
#ifdef DEBUG_SERIAL
		Serial.println(F("\r\nWIFI connection is timeout\r\n"));
#endif
		_giveUp = true;
	}

	//uint8_t stat = WiFi.waitForConnectResult();
	uint8_t stat = WiFi.status();
	//uint8_t stat = WiFi.begin(settings_.props.config.ssidName, settings_.props.config.ssidPass);

	/*if (stat == WL_DISCONNECTED) {
		DEBUG_SERIAL("Disconnected!\r\n");
		_giveUp = true;
	}
	else */
	if (stat == WL_NO_SSID_AVAIL) {
		DEBUG_SERIAL("SSID is GONE!\r\n");
		WiFi.disconnect();
		_giveUp = true;
	}
	else if (stat == WL_CONNECTED) {
		m_ConnectedToWifi = true;
		m_ConnectingToWifi = false;

#ifdef DEBUG_SERIAL
		Serial.println(F("\r\nYay!!! WIFI is connected\r\n"));
#endif
		onWifiConnected();

		return;
	}

	if (_giveUp) {
		m_ConnectingToWifi = false;
		m_ConnectedToWifi = false;

		onWifiNotConnected();
	}
	else {
#ifdef DEBUG_SERIAL
		Serial.print(".");
#endif
		wifiConnTrial_++;
	}
}

void WifiManager::onWifiConnected() {
	m_ConnectedToWifi = true;
	ledBar->stopAnimation();

	//Turn off DNS
//	if (mDNS_ != NULL) {
//		//mDNS_->stop();
//
//		delete mDNS_;
//		mDNS_ = NULL;
//	}

	IPAddress _localIP = WiFi.localIP();

//	StuffConfig *_cfg = &m_Service.getProps().config; //m_Config
//
//	boolean newConnection = (_cfg->wifiConfigured == 0);//if previously wifi not configured, it's just connected

	/* Just connected to WLAN */
	Serial.print("\r\nConnected to "); Serial.println(settings_.props.config.ssidName);
	Serial.print("IP address: "); Serial.println(_localIP);

	strcpy(settings_.props.config.ipAddress, toStringIp(_localIP).c_str());
	settings_.props.config.wifiConfigured = true;

//	strcpy(_cfg->ssidName, m_Config.ssidName);
//	strcpy(_cfg->ssidPass, m_Config.ssidPass);

	//service.setConfig(m_Config, true);

	settings_.save();

	if (wifiConnectedHandler_) {
		wifiConnectedHandler_(true);
	}
}

void WifiManager::onWifiNotConnected() {
	m_ConnectedToWifi = false;
	ledBar->stopAnimation();
	//m_Service.stopServe();

	setupAccessPoint();
	setupWebServer();
}

void WifiManager::setupWebServer() {
	if (webServer_ == NULL) {
		webServer_ = new WiFiServer(80);
		webServer_->begin();
	}

//	if (mDNS_ == NULL) {
//		mDNS_ = new WiFiMDNSResponder();
//	}
//
//	if (!mDNS_->begin(settings_.props.config.apName)) {
//		Serial.println(F("Failed to start MDNS responder!"));
//	}
//	else {
//		Serial.print(F("Server listening at http://"));
//		Serial.print(settings_.props.config.apName);
//		Serial.println(".local/");
//	}
}

void WifiManager::setupAccessPoint() {
#ifdef DEBUG_SERIAL
    Serial.print(F("Configuring access point: "));
    Serial.println(F(settings_.props.config.apName));
#endif

//    IPAddress apIP = IPAddress();
//    if (!ipAddressFromString(settings_.props.config.ipAddress, &apIP)) {
//        apIP = IPAddress(192, 168, 9, 1);
//    }
//
//    IPAddress netMsk(255, 255, 255, 0);
//
//#if (DEBUG_SERIAL_DIRECT)
//    apIP.printTo(Serial);
//    Serial.println();
//    netMsk.printTo(Serial);
//    Serial.println();
//#endif

    delay(10);

    //WiFi.config(apIP, apIP, apIP, netMsk);
    //WiFi.config(apIP, netMsk);

    if (WiFi.beginAP(settings_.props.config.apName, 3) != WL_CONNECTED) {
    //if (WiFi.beginAP(settings_.props.config.apName, 0, settings_.props.config.apPass) != WL_CONNECTED) {
        Serial.println(F("Creating access point failed"));
        // don't continue
        //while (true);
    }
    else {
    	Serial.println(F("Access point ready!"));
    }

    delay(100); // Without delay I've seen the IP address blank

#ifdef DEBUG_SERIAL
    Serial.print("AP IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
#endif

//    if (!m_DNSServer) {
//        m_DNSServer = new DNSServer();
//    }
//    /* Setup the DNS server redirecting all the domains to the apIP */
//    m_DNSServer->setErrorReplyCode(DNSReplyCode::NoError);
//    m_DNSServer->start(DNS_PORT, "*", apIP);

}

void WifiManager::_debugConfig(StuffConfig _config, Print& p) {
#ifdef DEBUG_SERIAL
    p.print(F("SSID Name: "));
    p.println(_config.ssidName);
    p.print(F("SSID Pass: "));
    p.println(_config.ssidPass);
    p.print(F("AP Name: "));
    p.println(_config.apName);
    p.print(F("Config Initialized: "));
    p.println(_config.configInitialized);
    p.print(F("Wifi Configured: "));
    p.println(_config.wifiConfigured);
#endif
}

void WifiManager::handleGet(String path, WiFiClient client) {
	if (path.equals("/")) {
		handleGetRoot(client);
	}
	else if (path.equals("/api/aplist")) {
		handleGetAPListJson(client);
	}
	else if (path.startsWith("/api/setup/wifi")) {
		path.replace("/api/setup/wifi?", "");
		handleSetupWifi(path, client);
	}
	else {
		handleNotFound(client);
	}
}

void WifiManager::handleGetRoot(WiFiClient client) {
	DEBUG_SERIAL(F("\r\nHandle path /\r\n"));

	String respString = "<strong>Nothing here</strong>";

	// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
	// and a content-type so the client knows what's coming, then a blank line:
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println(String("Content-Length: ") + String(respString.length()));
	client.println("Connection: close");
	client.println();

	// the content of the HTTP response follows the header:
	client.print(respString);

	// The HTTP response ends with another blank line:
	client.println();
}

void WifiManager::handleGetAPListJson(WiFiClient client) {

	DEBUG_SERIAL(F("\r\nHandle path /api/aplist\r\n"));


	// the content of the HTTP response follows the header:

	String json = "[]";

	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1) {
		DEBUG_SERIAL("Couldn't get a WiFi network\r\n");
	}
	else {
		// print the list of networks seen:
		DEBUG_SERIAL("Number of available networks:");
		DEBUG_SERIAL(numSsid);
		DEBUG_SERIAL_LN;

		json = "[";
		// print the network number and name for each network found:
		for (int i = 0; i < numSsid; i++) {

			String encType;
			uint8_t encTypeInt = WiFi.encryptionType(i);

			switch(encTypeInt) {
				case ENC_TYPE_WEP: {
					encType = "WEP";
					break;
				}
				case ENC_TYPE_TKIP: {
					encType = "WPA";
					break;
				}
				case ENC_TYPE_CCMP: {
					encType = "WPA2";
				    break;
				}
				case ENC_TYPE_NONE: {
					encType = "None";
					break;
				}
				case ENC_TYPE_AUTO: {
					encType = "Auto";
					break;
				}
				default: {
					encType = "Unknown";
					break;
				}
			}

			int quality = 0;
			if(WiFi.RSSI(i) <= -100)
			{
				quality = 0;
			}
			else if(WiFi.RSSI(i) >= -50)
			{
				quality = 100;
			}
			else
			{
				quality = 2 * (WiFi.RSSI(i) + 100);
			}

			json += "{\"ssid\": \"" +  String(WiFi.SSID(i))  +  "\", \"encType\": \"" + String(encType) + "\", \"quality\": " +
					String(quality) + "}";
			if (i < numSsid-1) {
				json += ", ";
			}
		}

		json += "]";

		DEBUG_SERIAL(json);
	}

	client.println("HTTP/1.1 200 OK");
	client.println("Content-type: application/json");
	client.println("Cache-Control: no-cache, no-store, must-revalidate");
	client.println();

	client.print(json);

	// The HTTP response ends with another blank line:
	client.println();
}

void WifiManager::handleNotFound(WiFiClient client) {
	client.println("HTTP/1.1 404 Not Found");
	client.println("Content-Type: text/html");
	client.println("Connection: close");
	client.println();

	// the content of the HTTP response follows the header:
	client.print("<strong>Not Found</strong>");

	// The HTTP response ends with another blank line:
	client.println();
}

void WifiManager::handleSetupWifi(String qs, WiFiClient client) {

	int maxKeysCount = 6;
	char *props[maxKeysCount][2];
	char buf[300];
	strcpy(buf, qs.c_str());

	int resultsCt = parseKeyVal(buf, props, maxKeysCount);
	for (int i = 0; i < resultsCt; i++) {
	    char *prop = props[i][0];
	    char *val = props[i][1];
	    urldecode(val);

	    Serial.print("Key: "); Serial.println(prop);
	    Serial.print("Val: "); Serial.println(val);

	    if (strcmp(prop, "ssidName") == 0){
	    	strcpy(settings_.props.config.ssidName, val);
		}
	    if (strcmp(prop, "ssidPass") == 0){
	    	strcpy(settings_.props.config.ssidPass, val);
	    }
//	    if (strcmp(prop, "userToken") == 0){
//	    	strcpy(settings_.props.account.userToken, val);
//	    }
//	    if (strcmp(prop, "userId") == 0){
//	    	strcpy(settings_.props.account.userId, val);
//	    }

	    if (strcmp(prop, "sasToken") == 0) {
			strcpy(settings_.props.account.userToken, val);
		}
		if (strcmp(prop, "sasExpiry") == 0){
			settings_.props.account.tokenExpiry = atol(val);
		}
		if (strcmp(prop, "name") == 0){
			strcpy(settings_.props.config.iotHubName, val);
		}
		if (strcmp(prop, "url") == 0){
			strcpy(settings_.props.config.mqttServer, val);
		}
	}

	//TODO: UNCOMMENT!
	settings_.props.config.wifiConfigured = true;
	settings_.save();

	String json = "{\"status\": \"OK\", \"id\": \"" + String(settings_.props.device.id) + "\"}";

	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: application/json");
	client.println(String("Content-Length: ") + String(json.length()));
	client.println("Cache-Control: no-cache, no-store, must-revalidate");
	client.println("Connection: close");
	client.println();

	// the content of the HTTP response follows the header:
	client.print(json);

	// The HTTP response ends with another blank line:
	client.println();

//	WiFi.disconnect();
//	delay(100);
//
//	wifiConnCheckingMillis_ = millis() + 3*1000;
//	m_WifiConnectRequested = true;

	if (resetRequiredHandler_) {
		resetRequiredHandler_(true);
	}
}
