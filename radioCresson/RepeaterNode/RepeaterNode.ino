/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to create a node thay repeates messages
 * from nodes far from gateway back to gateway.
 * It is important that nodes that has enabled repeater mode calls
 * process() frequently. Repeaters should never sleep.
 */

// Enable debug prints to serial monitor
#define MY_DEBUG

// Enable and select radio type attached
#define MY_CRESSON

#if defined(HAVE_HWSERIAL1)
  #define MY_CRESSON_HWSERIAL Serial1
#else
  #define MY_CRESSON_TX_PIN   7
  #define MY_CRESSON_RX_PIN   8
#endif

#define MINUTE 60000
uint32_t SLEEP_TIME = 15*MINUTE; // Sleep time between reports (in milliseconds)

#include <MySensors.h>
#include <uniqueID.h>

void before() {
  cresson.selfID = uniqueID() % 254 + 1; // 1 ~ 254
  cresson.mhmode = MHREPEATER;
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, cresson.selfID);
}

void setup()
{
}

void presentation()
{
	//Send the sensor node sketch version information to the gateway
	sendSketchInfo("CMEV Repeater Node", "2.0");
  present(0, S_ARDUINO_REPEATER_NODE);
}

uint16_t oldBatteryVolt = 0; // mV

void loop()
{
  uint32_t localtime = millis();
  
  uint16_t batteryVolt = hwCPUVoltage(); // mV
  uint8_t batteryPcnt = batteryPercent(batteryVolt);
  
#ifdef MY_DEBUG
  Serial.print("Battery Voltage: ");
  Serial.print(batteryVolt);
  Serial.println(" mV");

  Serial.print("Battery percent: ");
  Serial.print(batteryPcnt, DEC);
  Serial.println(" %");
#endif

  if (oldBatteryVolt != batteryVolt) {
    sendBatteryLevel(batteryPcnt);
    oldBatteryVolt = batteryVolt;
  }
  
  // Sleep CPU
  hwSleep(SLEEP_TIME + localtime - millis());
}

uint8_t batteryPercent(uint16_t batVolt) {
  const uint16_t BATPCNT_100 = 13200;
  const uint16_t BATPCNT_0   = 10500;
  if (batVolt >= BATPCNT_100) {
    return 100;
  } else if (batVolt <= BATPCNT_0) {
    return 0;
  } else {
    return (uint8_t)map(batVolt, BATPCNT_0, BATPCNT_100,  0,  100);
  }
}
