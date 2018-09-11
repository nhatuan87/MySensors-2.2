// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable Cresson transport layer
#define MY_CRESSON

#if defined(HAVE_HWSERIAL1)
  #define MY_CRESSON_HWSERIAL Serial1
#else
  #define MY_CRESSON_TX_PIN   7
  #define MY_CRESSON_RX_PIN   8
#endif

#include <MySensors.h>
#include <uniqueID.h>
#include <SHT21.h>  // include SHT21 library

uint32_t SLEEP_TIME = 60000; // Sleep time between reports (in milliseconds)

// Initialize motion message
MyMessage msgTemp(1, V_TEMP);
MyMessage msgHum(2, V_HUM);
SHT21 sht;

void before() {
  cresson.selfID = uniqueID() & 0x00FF;
  cresson.destID = GATEWAY_ADDRESS;
  cresson.begin();
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, cresson.selfID);
}

void setup()
{

}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo(F("SHT21 Sensor"), F("1.0"));

	// Register all sensors to gw (they will be created as child devices)
	present(1, S_TEMP);
  present(2, S_HUM);
}

void loop()
{
	// Read temperature
	float temp = sht.getTemperature();
  float hum  = sht.getHumidity();

	send(msgTemp.set(temp,1));
  send(msgHum.set(hum,1));

	// Sleep
	sleep(SLEEP_TIME);
}
