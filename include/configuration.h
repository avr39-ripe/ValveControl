#ifndef INCLUDE_CONFIGURATION_H_
#define INCLUDE_CONFIGURATION_H_

#include <user_config.h>
#include <SmingCore/SmingCore.h>

// Put you SSID and Password here
#define WIFI_SSID "infjust"
#define WIFI_PWD "jujust12"

// Pin for communication with DHT sensor
//#define DHT_PIN 1 // UART0 TX pin
#define DHT_PIN 2

// Pin for trigger control output
//#define CONTROL_PIN 3 // UART0 RX pin
#define CONTROL_PIN 0

#define VALVE_CONFIG_FILE ".valve.conf" // leading point for security reasons :)

struct ValveConfig
{
	ValveConfig()
	{
		set_temp = 0;
		temp_delta = 0;
		temp_interval = 0;
		switch_interval = 0;
	}

	String NetworkSSID;
	String NetworkPassword;
// ValveControl settings
	float set_temp;
	float temp_delta;
	int temp_interval;
	int switch_interval;
};

ValveConfig loadConfig();
void saveConfig(ValveConfig& cfg);

extern ValveConfig ActiveConfig;

#endif /* INCLUDE_CONFIGURATION_H_ */
