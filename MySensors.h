/*
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2017 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 */

/**
 * @defgroup MySensorsgrp MySensors
 * @ingroup publics
 * @{
 * @brief The primary public API declaration for the MySensors library
 *
 *
 */

/**
 * @file MySensors.h
 *
 * @brief API declaration for MySensors
 *
 * Include this header into your sketch to include the MySensors library and harness the power of
 * all those sensors!
 */
#ifndef MySensors_h
#define MySensors_h

#ifdef __cplusplus
#include <Arduino.h>
#endif

// general macros
#if !defined(_BV)
#define _BV(x) (1<<(x))	//!< _BV
#endif

#include "MyConfig.h"
#include "core/MySplashScreen.h"
#include "core/MySensorsCore.h"

// OTA Debug, has to be defined before HAL
#if defined(MY_OTA_LOG_SENDER_FEATURE) || defined(MY_OTA_LOG_RECEIVER_FEATURE)
#include "core/MyOTALogging.h"
#endif

// HARDWARE
#include "hal/architecture/MyHw.h"
#if defined(ARDUINO_ARCH_ESP8266)
#include "hal/architecture/ESP8266/MyHwESP8266.cpp"
#elif defined(ARDUINO_ARCH_AVR)
#include "drivers/AVR/DigitalWriteFast/digitalWriteFast.h"
#include "hal/architecture/AVR/MyHwAVR.cpp"
#elif defined(ARDUINO_ARCH_SAMD)
#include "drivers/extEEPROM/extEEPROM.cpp"
#include "hal/architecture/SAMD/MyHwSAMD.cpp"
#elif defined(ARDUINO_ARCH_STM32F1)
#include "hal/architecture/STM32F1/MyHwSTM32F1.cpp"
#elif defined(ARDUINO_ARCH_NRF5) || defined(ARDUINO_ARCH_NRF52)
#include "drivers/NVM/VirtualPage.cpp"
#include "drivers/NVM/NVRAM.cpp"
#include "hal/architecture/NRF5/MyHwNRF5.cpp"
#elif defined(__arm__) && defined(TEENSYDUINO)
#include "hal/architecture/Teensy3/MyHwTeensy3.cpp"
#elif defined(__linux__)
#include "hal/architecture/Linux/MyHwLinuxGeneric.cpp"
#endif

// OTA Debug second part, depends on HAL
#if defined(MY_OTA_LOG_SENDER_FEATURE) || defined(MY_OTA_LOG_RECEIVER_FEATURE)
#include "core/MyOTALogging.cpp"
#endif

#if defined(MY_LEDS_BLINKING_FEATURE)
#error MY_LEDS_BLINKING_FEATURE is now removed from MySensors core,\
define MY_DEFAULT_ERR_LED_PIN, MY_DEFAULT_TX_LED_PIN or\
MY_DEFAULT_RX_LED_PIN in your sketch instead to enable LEDs
#endif

#if defined(MY_DEFAULT_RX_LED_PIN) || defined(MY_DEFAULT_TX_LED_PIN) || defined(MY_DEFAULT_ERR_LED_PIN)
#include "core/MyLeds.cpp"
#else
#include "core/MyLeds.h"
#endif

#include "core/MyIndication.cpp"


// INCLUSION MODE
#if defined(MY_INCLUSION_MODE_FEATURE)
#include "core/MyInclusionMode.cpp"
#endif


// SIGNING
#include "core/MySigning.cpp"
#include "drivers/ATSHA204/sha256.cpp"
#if defined(MY_SIGNING_FEATURE)
// SIGNING COMMON FUNCTIONS
#if defined(MY_SIGNING_ATSHA204) && defined(MY_SIGNING_SOFT)
#error Only one signing engine can be activated
#endif
#if defined(MY_SIGNING_ATSHA204) && defined(__linux__)
#error No support for ATSHA204 on this platform
#endif

#if defined(MY_SIGNING_ATSHA204)
#include "core/MySigningAtsha204.cpp"
#include "drivers/ATSHA204/ATSHA204.cpp"
#elif defined(MY_SIGNING_SOFT)
#include "core/MySigningAtsha204Soft.cpp"
#endif
#endif


// FLASH
#if defined(MY_OTA_FIRMWARE_FEATURE)
#if defined(MY_OTA_USE_I2C_EEPROM)
#include "drivers/I2CEeprom/I2CEeprom.cpp"
#else
#include "drivers/SPIFlash/SPIFlash.cpp"
#endif
#include "core/MyOTAFirmwareUpdate.cpp"
#endif

// GATEWAY - TRANSPORT
#if defined(MY_CONTROLLER_IP_ADDRESS) || defined(MY_CONTROLLER_URL_ADDRESS)
#define MY_GATEWAY_CLIENT_MODE	//!< gateway client mode
#endif

#if defined(MY_USE_UDP) && !defined(MY_GATEWAY_CLIENT_MODE)
#error You must specify MY_CONTROLLER_IP_ADDRESS or MY_CONTROLLER_URL_ADDRESS for UDP
#endif

#if defined(MY_GATEWAY_MQTT_CLIENT)
#if defined(MY_SENSOR_NETWORK)
// We assume that a gateway having a radio also should act as repeater
#define MY_REPEATER_FEATURE

#endif
// GATEWAY - COMMON FUNCTIONS
// We support MQTT Client using W5100, ESP8266 and Linux
#if !defined(MY_GATEWAY_CLIENT_MODE)
#error You must specify MY_CONTROLLER_IP_ADDRESS or MY_CONTROLLER_URL_ADDRESS
#endif

#if !defined(MY_MQTT_PUBLISH_TOPIC_PREFIX)
#error You must specify a topic publish prefix MY_MQTT_PUBLISH_TOPIC_PREFIX for this MQTT client
#endif

#if !defined(MY_MQTT_SUBSCRIBE_TOPIC_PREFIX)
#error You must specify a topic subscribe prefix MY_MQTT_SUBSCRIBE_TOPIC_PREFIX for this MQTT client
#endif

#if !defined(MY_MQTT_CLIENT_ID)
#error You must define a unique MY_MQTT_CLIENT_ID for this MQTT client
#endif

#include "core/MyGatewayTransport.cpp"
#include "core/MyProtocolMySensors.cpp"

#if defined(MY_GATEWAY_LINUX)
#include "drivers/Linux/EthernetClient.h"
#include "drivers/Linux/EthernetServer.h"
#include "drivers/Linux/IPAddress.h"
#endif
#include "drivers/PubSubClient/PubSubClient.cpp"
#include "core/MyGatewayTransportMQTTClient.cpp"
#elif defined(MY_GATEWAY_FEATURE)
// GATEWAY - COMMON FUNCTIONS
#include "core/MyGatewayTransport.cpp"

#include "core/MyProtocolMySensors.cpp"

// GATEWAY - CONFIGURATION
#if defined(MY_SENSOR_NETWORK)
// We assume that a gateway having a radio also should act as repeater
#define MY_REPEATER_FEATURE
#endif
#if !defined(MY_PORT)
#error You must define MY_PORT (controller or gatway port to open)
#endif
#if defined(MY_GATEWAY_ESP8266)
// GATEWAY - ESP8266
#include "core/MyGatewayTransportEthernet.cpp"
#elif defined(MY_GATEWAY_LINUX)
// GATEWAY - Generic Linux
#if defined(MY_USE_UDP)
#error UDP mode is not available for Linux
#endif
#include "drivers/Linux/EthernetClient.h"
#include "drivers/Linux/EthernetServer.h"
#include "drivers/Linux/IPAddress.h"
#include "core/MyGatewayTransportEthernet.cpp"
#elif defined(MY_GATEWAY_W5100)
// GATEWAY - W5100
#include "core/MyGatewayTransportEthernet.cpp"
#elif defined(MY_GATEWAY_ENC28J60)
// GATEWAY - ENC28J60
#if defined(MY_USE_UDP)
#error UDP mode is not available for ENC28J60
#endif
#include "core/MyGatewayTransportEthernet.cpp"
#elif defined(MY_GATEWAY_SERIAL)
// GATEWAY - SERIAL
#include "core/MyGatewayTransportSerial.cpp"
#endif
#endif

// TRANSPORT
#ifndef DOXYGEN
// count enabled transports
#if defined(MY_RADIO_RF24)
#define __RF24CNT 1		//!< __RF24CNT
#else
#define __RF24CNT 0		//!< __RF24CNT
#endif
#if defined(MY_RADIO_NRF5_ESB)
#define __NRF5ESBCNT 1 //!< __NRF5ESBCNT
#else
#define __NRF5ESBCNT 0 //!< __NRF5ESBCNT
#endif
#if defined(MY_RADIO_RFM69)
#define __RFM69CNT 1	//!< __RFM69CNT
#else
#define __RFM69CNT 0	//!< __RFM69CNT
#endif
#if defined(MY_RADIO_RFM95)
#define __RFM95CNT 1	//!< __RFM95CNT
#else
#define __RFM95CNT 0	//!< __RFM95CNT
#endif
#if defined(MY_RS485)
#define __RS485CNT 1	//!< __RS485CNT
#else
#define __RS485CNT 0	//!< __RS485CNT
#endif
#if defined(MY_CRESSON)
#define __CRESSONCNT 1	//!< __CRESSONCNT
#else
#define __CRESSONCNT 0	//!< __CRESSONCNT
#endif

#if (__RF24CNT + __NRF5ESBCNT + __RFM69CNT + __RFM95CNT + __RS485CNT + __CRESSONCNT > 1)
#error Only one forward link driver can be activated
#endif
#endif //DOXYGEN

// SANITY CHECK
#if defined(MY_REPEATER_FEATURE) || defined(MY_GATEWAY_FEATURE)
#define MY_TRANSPORT_SANITY_CHECK		//!< enable regular transport sanity checks
#endif

// TRANSPORT INCLUDES
#if defined(MY_RADIO_RF24) || defined(MY_RADIO_NRF5_ESB) || defined(MY_RADIO_RFM69) || defined(MY_RADIO_RFM95) || defined(MY_RS485) || defined(MY_CRESSON)
#include "hal/transport/MyTransportHAL.h"
#include "core/MyTransport.h"

// PARENT CHECK
#if defined(MY_PARENT_NODE_IS_STATIC) && (MY_PARENT_NODE_ID == AUTO)
#error Parent is static but no parent ID defined, set MY_PARENT_NODE_ID.
#endif

#if defined(MY_TRANSPORT_DONT_CARE_MODE)
#error This directive is deprecated, set MY_TRANSPORT_WAIT_READY_MS instead!
#endif

// RAM ROUTING TABLE
#if defined(MY_RAM_ROUTING_TABLE_FEATURE) && defined(MY_REPEATER_FEATURE)
// activate feature based on architecture
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_NRF5) || defined(ARDUINO_ARCH_STM32F1) || defined(TEENSYDUINO) || defined(__linux__)
#define MY_RAM_ROUTING_TABLE_ENABLED
#elif defined(ARDUINO_ARCH_AVR)
#if defined(__avr_atmega1280__) || defined(__avr_atmega1284__) || defined(__avr_atmega2560__)
// >4kb, enable it
#define MY_RAM_ROUTING_TABLE_ENABLED
#else
// memory limited, enable with care
// #define MY_RAM_ROUTING_TABLE_ENABLED
#endif // __avr_atmega1280__, __avr_atmega1284__, __avr_atmega2560__
#endif // ARDUINO_ARCH_AVR
#endif
#ifdef DOXYGEN
/**
 * @def MY_RAM_ROUTING_TABLE_ENABLED
 * @brief Automatically set if RAM routing table is enabled
 *
 * @see MY_RAM_ROUTING_TABLE_FEATURE
 */
#define MY_RAM_ROUTING_TABLE_ENABLED
#endif

// SOFTSPI
#ifdef MY_SOFTSPI
#if defined(ARDUINO_ARCH_ESP8266)
#error Soft SPI is not available on ESP8266
#endif
#include "drivers/AVR/DigitalIO/DigitalIO.h"
#endif

// POWER PIN
#ifndef DOXYGEN
#if defined(MY_RF24_POWER_PIN) || defined(MY_RFM69_POWER_PIN) || defined(MY_RFM95_POWER_PIN) || defined(MY_RADIO_NRF5_ESB) || defined(MY_CRESSON_POWER_PIN)
#define RADIO_CAN_POWER_OFF (true)
#else
#define RADIO_CAN_POWER_OFF (false)
#endif
#endif

// Transport drivers
#if defined(MY_RADIO_RF24)
#if defined(MY_RF24_ENABLE_ENCRYPTION)
#include "drivers/AES/AES.cpp"
#endif
#include "drivers/RF24/RF24.cpp"
#include "hal/transport/MyTransportRF24.cpp"
#elif defined(MY_RADIO_NRF5_ESB)
#if !defined(ARDUINO_ARCH_NRF5)
#error No support for nRF5 radio on this platform
#endif
#if defined(MY_NRF5_ESB_ENABLE_ENCRYPTION)
#include "drivers/AES/AES.cpp"
#endif
#include "drivers/NRF5/Radio.cpp"
#include "drivers/NRF5/Radio_ESB.cpp"
#include "hal/transport/MyTransportNRF5_ESB.cpp"
#elif defined(MY_RS485)
#if !defined(MY_RS485_HWSERIAL)
#if defined(__linux__)
#error You must specify MY_RS485_HWSERIAL for RS485 transport
#endif
#include "drivers/AltSoftSerial/AltSoftSerial.cpp"
#endif
#include "hal/transport/MyTransportRS485.cpp"
#elif defined(MY_RADIO_RFM69)
#if defined(MY_RFM69_NEW_DRIVER)
#include "drivers/RFM69/new/RFM69_new.cpp"
#else
#include "drivers/RFM69/old/RFM69_old.cpp"
#endif
#include "hal/transport/MyTransportRFM69.cpp"
#elif defined(MY_RADIO_RFM95)
#if defined(MY_RFM95_ENABLE_ENCRYPTION)
#include "drivers/AES/AES.cpp"
#endif
#include "drivers/RFM95/RFM95.cpp"
#include "hal/transport/MyTransportRFM95.cpp"
#elif defined(MY_CRESSON)
#include <crstream.h>
#if !defined(MY_CRESSON_HWSERIAL)
#include <SoftwareSerial.h>
#if !defined(MY_CRESSON_RX_PIN)
#error The CRESSON transport is not configured as HWSERIAL, you must specify MY_CRESSON_RX_PIN (MCU TX pin)
#endif
#if !defined(MY_CRESSON_TX_PIN)
#error The CRESSON transport is not configured as HWSERIAL, you must specify MY_CRESSON_TX_PIN (MCU RX pin)
#endif
#endif
#include "hal/transport/MyTransportCresson.cpp"
#endif

// PASSIVE MODE
#if defined(MY_PASSIVE_NODE) && !defined(DOXYGEN)
#define MY_TRANSPORT_UPLINK_CHECK_DISABLED
#define MY_PARENT_NODE_IS_STATIC
#undef MY_REGISTRATION_FEATURE
#undef MY_SIGNING_FEATURE
#undef MY_OTA_FIRMWARE_FEATURE
#if (defined(MY_GATEWAY_FEATURE) || defined(MY_REPEATER_FEATURE))
#error This node is configured as GW/repeater, MY_PASSIVE_NODE cannot be set simultaneously
#endif
#if (MY_NODE_ID == AUTO)
#error MY_PASSIVE_NODE configuration requires setting MY_NODE_ID
#endif
#endif

#include "core/MyTransport.cpp"
#endif

// Make sure to disable child features when parent feature is disabled
#if !defined(MY_SENSOR_NETWORK)
#undef MY_OTA_FIRMWARE_FEATURE
#undef MY_REPEATER_FEATURE
#undef MY_SIGNING_NODE_WHITELISTING
#undef MY_SIGNING_FEATURE
#endif

#if !defined(MY_GATEWAY_FEATURE)
#undef MY_INCLUSION_MODE_FEATURE
#undef MY_INCLUSION_BUTTON_FEATURE
#endif

#if !defined(MY_CORE_ONLY)
#if !defined(MY_GATEWAY_FEATURE) && !defined(MY_SENSOR_NETWORK)
#error No forward link or gateway feature activated. This means nowhere to send messages! Pretty pointless.
#endif
#endif

#include "core/MyCapabilities.h"
#include "core/MyMessage.cpp"
#include "core/MySplashScreen.cpp"
#include "core/MySensorsCore.cpp"

// HW mains
#if !defined(MY_CORE_ONLY)
#if defined(ARDUINO_ARCH_AVR)
#include "hal/architecture/AVR/MyMainAVR.cpp"
#elif defined(ARDUINO_ARCH_SAMD)
#include "hal/architecture/SAMD/MyMainSAMD.cpp"
#elif defined(ARDUINO_ARCH_ESP8266)
#include "hal/architecture/ESP8266/MyMainESP8266.cpp"
#elif defined(ARDUINO_ARCH_NRF5)
#include "hal/architecture/NRF5/MyMainNRF5.cpp"
#elif defined(__linux__)
#include "hal/architecture/Linux/MyMainLinux.cpp"
#elif defined(ARDUINO_ARCH_STM32F1)
#include "hal/architecture/STM32F1/MyMainSTM32F1.cpp"
#elif defined(TEENSYDUINO)
#include "hal/architecture/Teensy3/MyMainTeensy3.cpp"
#endif
#endif

#endif

/** @}*/
