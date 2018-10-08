// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable Cresson transport layer
#define MY_CRESSON

#if defined(HAVE_HWSERIAL1)
  #define MY_CRESSON_HWSERIAL Serial1
#else
  #define MY_CRESSON_TX_PIN   3
  #define MY_CRESSON_RX_PIN   4
#endif

#include <MySensors.h>
#include <uniqueID.h>
#include <SHT21.h>

#define MINUTE        60000
uint32_t SLEEP_TIME = 15*MINUTE; // Sleep time between reports (in milliseconds)

// Initialize motion message
MyMessage msgTemp(0, V_TEMP);
MyMessage msgHum(1, V_HUM);
SHT21 sht;

uint16_t oldBatteryVolt = 0; // mV

void before() {
  cresson.selfID = uniqueID() % 254 + 1; // 1 ~ 254
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, cresson.selfID);
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  sht.begin();
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo(F("SHT21 Sensor"), F("1.0"));

	// Register all sensors to gw (they will be created as child devices)
	present(0, S_TEMP);
  present(1, S_HUM);
}

void loop()
{
  uint32_t localtime = millis();

  // read battery
  uint16_t batteryVolt = hwCPUVoltage(); // mV
  uint8_t batteryPcnt = batteryPercent(batteryVolt);
#ifdef MY_DEBUG
  Serial.print(F("Battery Voltage: ")); Serial.print(batteryVolt);  Serial.println(F(" mV"));
  Serial.print(F("Battery Percent: ")); Serial.print(batteryPcnt);  Serial.println(F(" %"));
#endif
  if (oldBatteryVolt != batteryVolt) {
    sendBatteryLevel(batteryPcnt);
    oldBatteryVolt = batteryVolt;
  }
  
	// Read sensor
 	float temp = sht.getTemperature();
  float hum  = sht.getHumidity();

  if (sht.isOK()) {
    #ifdef MY_DEBUG
      Serial.print(F("Temperature: ")); Serial.print(temp); Serial.println(F(" oC"));
      Serial.print(F("Humidity: "   )); Serial.print(hum) ; Serial.println(F(" %"));
    #endif
    send(msgTemp.set(temp,1));
    send(msgHum.set(hum,1));
  } else {
    #ifdef MY_DEBUG
    Serial.println(F("SHT21: Error!"));
    #endif
  }

	// Sleep
  sleep(SLEEP_TIME + localtime - millis());
}

uint8_t batteryPercent(uint16_t batVolt) {
  const uint16_t BATPCNT_100 = 3100;
  const uint16_t BATPCNT_0   = 1800;
  if (batVolt >= BATPCNT_100) {
    return 100;
  } else if (batVolt <= BATPCNT_0) {
    return 0;
  } else {
    return (uint8_t)map(batVolt, BATPCNT_0, BATPCNT_100,  0,  100);
  }
}
