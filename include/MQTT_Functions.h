#ifndef MQTT_Functions
#define MQTT_Functions

#include <Arduino.h>
#include <PubSubClient.h>
#include "Display.h"


bool MQTTinit();  //Wenn verbunden Rückgabewert true
void MQTT_callback(char* topic, byte* payload, unsigned int length);
bool MQTT_sendMessage(int MQTT_MSGType, const uint8_t* MSG, uint8_t len);
bool MQTT_sendMessage(int MQTT_MSGType, int MSG);
bool MQTT_sendMessage(int MQTT_MSGType, float MSG);
bool MQTT_sendMessage(int MQTT_MSGType, uint8_t MSG);
bool MQTT_sendMessage(int MQTT_MSGType, uint32_t MSG);
bool MQTT_sendText(int _MSGType, String Text);



#include "MQTT_Functions.cpp"
#endif