#include <user_config.h>
#include <SmingCore/SmingCore.h>

///////////////////////////////////////////////////////////////////
// Set your SSID & Pass for initial configuration
#include "../include/configuration.h" // application configuration
///////////////////////////////////////////////////////////////////

#include "webserver.h"

Timer procTimer;
bool web_ap_started = false;
// Sensors string values
String StrT, StrRH, StrTime;
int counter = 0;
float curr_temp = 26.07;

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
	if(Serial.available() > 0)
	  {
		procTimer.stop();
		char buf[255];
		int length = Serial.readBytesUntil('\r', buf, 255);

		if ( length > 0)
	    {
	    	buf[length]='\0';
//	    	Serial.print("Serial data: ");
//	    	Serial.print(buf);

	    	DynamicJsonBuffer jsonBuffer;
	    	JsonObject& root = jsonBuffer.parseObject(buf);
	    	if (!root.success())
	    	{
//	    	    Serial.print("parseObject() failed");
	    	}
	    	else
	    	{
//	    		root.printTo(Serial);
//	    		Serial.println();

	    		if(root["settings"].success())
	    		{
					JsonObject& settings = root["settings"];
					ActiveConfig.set_temp = settings["set_temp"];
					ActiveConfig.temp_delta = settings["temp_delta"];
					ActiveConfig.temp_interval = settings["temp_interval"];
					ActiveConfig.switch_interval = settings["switch_interval"];

					counter++;
	    		}
	    		if(root["temperature"].success())
	    		{
	    			JsonObject& temperature = root["temperature"];
	    			curr_temp = temperature["curr_temp"];

	    			counter++;
	    		}
	    	}

	    }
//		Serial.println(counter);
		procTimer.start();
	  }
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
