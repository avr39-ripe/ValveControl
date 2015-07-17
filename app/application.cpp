#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <Libraries/OneWire/OneWire.h>

///////////////////////////////////////////////////////////////////
// Set your SSID & Pass for initial configuration
#include "../include/configuration.h" // application configuration
///////////////////////////////////////////////////////////////////

#include "webserver.h"

Timer procTimer;
Timer relayTimer;

OneWire ds(ONEWIRE_PIN);

bool web_ap_started = false;
// Sensors string values
String StrT, StrRH, StrTime;
byte n;
unsigned long counter = 0;
float curr_temp = 26.07;

struct temp_sensor
{
  byte addr[8];
  char addr_str[17];
  float value;
  byte data[12];
};

temp_sensor temp_sensors[] ={ {{0x28, 0x9D, 0x14, 0x3E, 0x00, 0x00, 0x00, 0xDB},"Sensor1\0",0},
                              {{0x28, 0xE3, 0x1D, 0x3E, 0x00, 0x00, 0x00, 0xA3},"Sensor2\0",0},
                              {{0x28, 0x97, 0xDD, 0x3D, 0x00, 0x00, 0x00, 0x4D},"Sensor3\0",0}
};

void process();
void connectOk();
void connectFail();

//extern "C" void user_rf_pre_init(void)
//{
//}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
//	Serial.begin(9600); // 115200 by default
	Serial.systemDebugOutput(false); // Debug output to serial
	Serial.setTimeout(2000);

	ActiveConfig = loadConfig();
//	ActiveConfig.NetworkSSID = WIFI_SSID;
//	ActiveConfig.NetworkPassword = WIFI_PWD;

	WifiStation.config(ActiveConfig.NetworkSSID, ActiveConfig.NetworkPassword);
	WifiStation.enable(true);
//	WifiAccessPoint.enable(false);

	WifiStation.waitConnection(connectOk, 20, connectFail); // We recommend 20+ seconds for connection timeout at start
//	wifi_set_phy_mode(PHY_MODE_11G);
	WifiAccessPoint.config("ValveConfig", "", AUTH_OPEN);
	WifiAccessPoint.enable(true);
	startWebServer();


	procTimer.initializeMs(5000, process).start();
	process();
}

void process()
{

}

void readTemp()
{
	counter++;

	ds.reset();
	ds.skip();
	ds.write(0x44); // start conversion

//  system_soft_wdt_stop();
	delay(750);
//  system_soft_wdt_restart();

	for (n = 0; n < NUM_SENSORS; n++)
	{

		if (OneWire::crc8(temp_sensors[n].addr, 7) != temp_sensors[n].addr[7])
		{
			Serial.println("CRC is not valid!");
			temp_sensors[n].value = -1000;
		}

		ds.reset();
		ds.select(temp_sensors[n].addr);
		ds.write(0xBE); // Read Scratchpad

		for (i = 0; i < 9; i++)
		{
			temp_sensors[n].data[i] = ds.read();
		}

		float tempRead = ((temp_sensors[n].data[1] << 8)
				| temp_sensors[n].data[0]); //using two's compliment
		temp_sensors[n].value = tempRead / 16;


	}

	Serial.println();
	root.printTo(Serial);
}


void connectOk()
{
	debugf("connected");
//	WifiAccessPoint.enable(false);
	if(! web_ap_started)
	{
		web_ap_started = true;
		WifiAccessPoint.config("ValveConfig", "", AUTH_OPEN);
		WifiAccessPoint.enable(true);
		startWebServer();
		procTimer.restart();
	}
//	// At first run we will download web server content
//	if (!fileExist("index.html") || !fileExist("config.html") || !fileExist("bootstrap.css.gz") || !fileExist("jquery.js.gz"))
//	{
//	//If we miss some file in spiff_rom.bin image list FS content for debug
//		auto dir = fileList();
//		for (int i = 0; i < dir.size(); i++)
//			Serial.println(dir[i]);
//		downloadContentFiles();
//	}
//	else
//		startWebServer();
}

void connectFail()
{
	debugf("connection FAILED");

	if(! web_ap_started)
	{
		web_ap_started = true;
		WifiAccessPoint.config("ValveConfig", "", AUTH_OPEN);
		WifiAccessPoint.enable(true);
		startWebServer();
	}

	WifiStation.waitConnection(connectOk); // Wait connection
}
