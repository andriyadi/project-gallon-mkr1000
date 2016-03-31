//
// Created by Andri Yadi on 12/4/15.
//

#ifndef GALLON_CONSTANTS_H
#define GALLON_CONSTANTS_H

#define TEST_MODE				0 //change to 0 later

#define TIMEZONE_OFFSET			7 //Western Indonesia Time

#define INIT_EEPROM         	0

#define LED_BAR_I2C         	1

#define SSR_PIN              	A3
#define RESET_PIN				A1
#define BUTTON_PIN				A2

#define LDR_AVAILABLE         	1
#define LDR_PIN              	0

#define SENSOR_PROBES_COUNT            5//10
#define SENSOR_READ_SAMPLING_COUNT     10

#define DEBUG_SERIAL(...) Serial.print( __VA_ARGS__ )

#ifndef DEBUG_SERIAL
#define DEBUG_SERIAL(...)
#endif

#define DEBUG_SERIAL_LN Serial.println()

//#define DEBUG_SERIAL_DIRECT 			1

#define SENSOR_QUERY_INTERVAL 			5000
#define RESPOND_WATERLEVEL_INTERVAL 	2*SENSOR_QUERY_INTERVAL

#define DATA_PUBLISH_INTERVAL 	20000
#define TIME_SYNC_INTERVAL 		10*60*1000 //60*1000

//#define WIFI_SSID "BOLT!Super4G-1F20"
//#define WPA_PASSWORD "9i522424"

#define DEFAULT_WIFI_SSID "DyWare-AP4"
#define DEFAULT_WIFI_PASSWORD "p@ssw0rd"

//#define WIFI_SSID "DyWare-AP2"
//#define WPA_PASSWORD "957DaegCen"

//#define WIFI_SSID "HAME_F1_0e93"
//#define WPA_PASSWORD "7cd0e938"

//#define WIFI_SSID "Andri's iPhone 6s"
//#define WPA_PASSWORD "11223344"

#define DEFAULT_DEVICE_ID "gallon-01"
#define DEFAULT_DEVICE_NAME "My Smart Gallon"
#define DEFAULT_DEVICE_TYPES "SmartGallon"
#define DEFAULT_DEVICE_SUBTYPES "Generic"

#define DEFAULT_AP_NAME DEFAULT_DEVICE_ID //"homeX-gallon-03"
#define DEFAULT_AP_PASS "11223344"
#define DEFAULT_IP_ADDR "192.168.1.1"

#define MQTT_PORT 8883

#define API_HOST "home-x.cloudapp.net"
#define API_PORT 9000

#endif //GALLON_CONSTANTS_H
