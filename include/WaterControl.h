#ifndef WaterControl
#define WaterControl
#include <Arduino.h>
#include "GlobalVariabels.h"


enum{
  MQTT_MSG_Error,
  MQTT_MSG_Logging,
  MQTT_MSG_Waterlevel,
  MQTT_MSG_currentConsume,
  MQTT_MSG_totalConsume,
  MQTT_MSG_stateOutput,
  MQTT_MSG_CounterValue
};

struct SprinklerItem;
struct SprinklerProgramm;
struct sprinklerConfig;
struct outputVariables;


void TouchInit(); //Initialisierung der Touchtasten

void EinstSpeichern();
void EinstLaden();
char ResetVarLesen();
void ResetVarSpeichern(char Count);
void initDebugWindow();
void closeDebugWindow();

void rebuildMainMenu(Menu *Hauptmenu, sprinklerConfig *varConfig, int * Wasserstaende);
void UpdateButtonName(sprinklerConfig *varConfig);
float AVGTouchPort(uint8_t Port, u32_t Delay);

uint8_t GetOutput(SprinklerProgramm *_Prog, unsigned long *_Start);
uint32_t calcWaterLiter(int Wasserstand);


#include "WaterControl.cpp"
#endif