// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable Cresson transport layer
#define MY_CRESSON

#if defined(ARDUINO_ARCH_AVR) && defined(HAVE_HWSERIAL1)
  #define MY_CRESSON_HWSERIAL Serial1
#else
  #define MY_CRESSON_TX_PIN   3
  #define MY_CRESSON_RX_PIN   4
#endif

#include <MySensors.h>
#include <uniqueID.h>

#define MINUTE                60000
#define DIGITAL_INPUT_SENSOR  2   // The digital input you attached your motion sensor.  (Only 2 and 3 generates interrupt!)
#define CHILD_ID              0   // Id of the sensor child

uint32_t SLEEP_TIME = 1*MINUTE; // Sleep time between reports (in milliseconds)

// Initialize motion message
MyMessage msg(CHILD_ID, V_TRIPPED);

void before() {
  cresson.selfID = uniqueID() % 254 + 1; // 1 ~ 254
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, cresson.selfID);
}

void setup()
{
	pinMode(DIGITAL_INPUT_SENSOR, INPUT);      // sets the motion sensor digital pin as input
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo(F("Motion Sensor"), F("1.0"));

	// Register all sensors to gw (they will be created as child devices)
	present(CHILD_ID, S_MOTION);
}

void loop()
{
	// Read digital motion value
	bool tripped = digitalRead(DIGITAL_INPUT_SENSOR) == HIGH;

	send(msg.set(tripped?"1":"0"));  // Send tripped value to gw

	// Sleep until interrupt comes in on motion sensor. Send update every two minute.
	sleep(digitalPinToInterrupt(DIGITAL_INPUT_SENSOR), CHANGE, SLEEP_TIME);
}
