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

#define CHILD_ID_SWITCH 0

#define EPROM_SWITCH_STATE 1

#define Switch_OFF 0
#define Switch_ON 1

#define SN "Binary Switch"
#define SV "1.0"

#define MINUTE 60000
uint32_t SLEEP_TIME = 15*MINUTE; // Sleep time between reports (in milliseconds)

int16_t LastSwitchState=Switch_OFF;
MyMessage SwitchMsg(CHILD_ID_SWITCH, V_STATUS);

void before() {
  cresson.selfID = uniqueID() % 254 + 1; // 1 ~ 254
  hwWriteConfig(EEPROM_NODE_ID_ADDRESS, cresson.selfID);
}

void setup()
{
	//Retreive our last Switch state from the eprom
	int SwitchState=loadState(EPROM_SWITCH_STATE);
	if (SwitchState<=1) {
		LastSwitchState=SwitchState;
	}

	//Here you actualy switch on/off the Switch with the last known dim level
	SetCurrentState2Hardware();
  pinMode(LED_BUILTIN, OUTPUT);
}

void presentation()
{
	// Send the Sketch Version Information to the Gateway
	sendSketchInfo(SN, SV);
	present(CHILD_ID_SWITCH, S_BINARY );
}

uint32_t localtime = 0;
void loop()
{
  // Send update every minute.
  if (millis() - localtime > SLEEP_TIME) {
    SendCurrentState2Controller();
    localtime = millis();
  }
}

void receive(const MyMessage &message)
{
	if (message.type == V_STATUS) {
		int lstate= atoi( message.data );
		if ((lstate<0)||(lstate>1)) {
			return;
		}
		LastSwitchState=lstate;
		saveState(EPROM_SWITCH_STATE, LastSwitchState);
	}

	//Here you set the actual Switch state/level
	SetCurrentState2Hardware();
}

void SetCurrentState2Hardware()
{
	if (LastSwitchState==Switch_OFF) {
    digitalWrite(LED_BUILTIN, LOW);
	} else {
    digitalWrite(LED_BUILTIN, HIGH);
	}
  SendCurrentState2Controller();
}

void SendCurrentState2Controller() {
  send(SwitchMsg.set(LastSwitchState?"1":"0"));
}
