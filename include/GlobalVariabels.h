#ifndef GlobalVariabels
#define GlobalVariabels

#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include <NTPClient.h>

#include "Display.h"
#include "WaterControl.h"
#include "WC_Structures.h"


//Projektvariablen
sprinklerConfig varConfig;
unsigned int Zisterne_Stand_Roh = 0;

//-----------------------------------
//Variablen und Funktion zur Fehlersuche (Absturz Nano nach Empfang von Nachrichten)
const uint8_t  maxMessages = 40;
char * sentMessages[maxMessages] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t lastMessage = maxMessages-1;
uint16_t countMessages = 0;

//-----------------------------------
enum {touchUp_wsgn = 33, touchDown_gn = 32, touchRight_wsbr = 15, touchLeft_br = 4, touchF1_bl = 13, touchF2_wsbl = 12, touchF3_or = 14, touchF4_wsor = 27, RGB_Red = 22, RGB_Green = 16, RGB_Blue = 17, Display_Beleuchtung = 21};
enum {indexAGM = 0, indexLED = 1, indexProgStart = 2, indexNWK = 3};
enum{  //Status Nanokommunikation
  send_syscheck,
  set_output,
  set_watervolcompl,
  set_Config,
  set_ConvVar,
  get_outputcheck,
  get_waterlevel,
  get_watervolcompl,
  get_watervolcurrent,
  get_ConvVar,
  get_ConfigCheck,
  get_CounterValue,
  err_noerr,
  err_read,
  err_TransferESP,
  err_waterlevel,
  err_watervol,
  err_EEPROM,
  err_FunctionInactiv
};
enum
{
  t_up,
  t_down,
  t_Left,
  t_Right,
  t_F1,
  t_F2,
  t_F3,
  t_F4
};
enum{  //Menuepositionen Allgemein
  WaterLevLiter, 
  WaterVolTotal,
  WaterVolCur,
  WaterDifDay,
  WaterDifWeek, 
  WaterDifMonth, 
  WaterLev, 
  WaterLevRaw 
};
enum{  //Menuepositionen LED
  LEDModus, 
  LEDSpeed,
  LEDRed,
  LEDGreen,
  LEDBlue
};


//Touch-Variablen neu
float arrTouchAVG[8] = {0, 0, 0, 0, 0, 0, 0, 0};           //Array für die Durchschnitswerte pro Touch-Sensor
uint8_t TouchSensitivity = 2;                              //Empfindlichkeit der Touchtasten unter Glas = 1
uint8_t TouchFailureCount = 0;                            //Fehler der Touchtasten aufsummieren (Aktueller Wert > Initialwert)
const uint8_t TouchFailureCountMax = 50;                  //Maximale Fehleranzahl der Tochtasten
uint8_t SoftPWM_Counter = 0;                               //Zaehler der immer um 1 hochgezaehlt wird
uint8_t SoftPWM_RGB[3] = {0, 0, 0};                        //3 Variablem mit dem Ausschaltwert (0 = immer aus, 255 = immer ein) 0 = Rot, 1 = Gruen, 2 = Blau
uint8_t SoftPWM_Ports[3] = {RGB_Red, RGB_Green, RGB_Blue}; //Die 3 Ports in ein Array geschriben um sie einfacher zu bearbeiten
uint8_t LED_Mode_State = 0;                                //Wenn ein LED-Modus aktiviert ist, wird hier der aktuelle Stand hochgezaehlt (0=Neu initialisieren)
unsigned long LED_Mode_Time_Var = 0;                       //Variable fuer LED-Programm (Smooth)
unsigned long TouchSperre = 0;                             // sobald ein Touch erkannt, wird eine erneute erkennung verhindert bis Variable < millis()
unsigned long Break_200ms = 0;                             //Variable fuer Dinge die alle 200ms ausgefuehrt werden
unsigned long Break_1s = 0;                                //Variable fuer Dinge die alle 1s ausgefuehrt werden
unsigned long Break_60s = 0;                               //Variable fuer Dinge die alle 60s ausgefuehrt werden
unsigned long Break_10m = 0;                               //Variable fuer Dinge die alle 10m ausgefuehrt werden
unsigned long Break_h = 0;                                 //Variable fuer Dinge die alle 1h ausgefuehrt werden
unsigned long Break_Touchtest = 0;                         //Pause bei überprüfung der Touchvariablen
int monthDay = 0;                                          //Variablen für die aktuelle Zeit
int currentMonth = 0;
int currentYear = 0;
int currentHour = 0;
int currentMin = 0;
unsigned long cnt_LED_Display = 0;                         //Wenn diese Variable < millis() ist geht das Display aus
int KeyLockState = 0;                                      //Wenn > 0 Sperre aktiv
outputVariables varOutput;
unsigned long ProgStart[ProgItems];                        //Variable zum speichern der Startzeit
uint8_t DebugMode = 0;                                     //Debugmode fuer reine Schriftausgabe am Display
unsigned long DisplayVerz = 120000;                        //Millisekunden die das Display nach letzter Betaetigung an bleibt
uint16_t NanoRequests_afterLastAnsw = 0;                         //Anzahl von Anfragen nach letzter Antwort von Nano
uint16_t maxWaterLevelRoh = 204;                               //Max Rueckgabewert bei Wasserstandsabfrage (in cm)
uint16_t minWaterLevelRoh = 22;                               //Min Rueckgabewert bei Wasserstandsabfrage (in cm)
const uint16_t maxWaterLevelLiter = 8845;                          //Maximaler Fuellstand in Liter; Bei maxWassertiefe von 204 cm und min. 22 cm
int32_t pGeneralVar[8] = {-1, -1, -1, -1, -1, -1, -1, -1};            //Verschiedene Wasserstaende und Verbraeuche
uint32_t NanoRequestFlags = 0;          //Anfragen die noch geschickt werden müssen, damit nicht alle auf einmal geschickt werden
char MQTTState[20] = "MQTT Ready";
char WifiState[20] = "Wifi Ready";
char WifiLastChange[50] = "";         //Aufnahme des Zeitstempels wann letztes mal die Verbindung neu aufgebaut wurde inklusive des WIFI Statuses
uint8_t touchPorts[8] = {touchUp_wsgn, touchDown_gn, touchLeft_br, touchRight_wsbr, touchF1_bl, touchF2_wsbl, touchF3_or, touchF4_wsor};


//Erstellen Serverelement
AsyncWebServer server(8080);

//Uhrzeit Variablen
WiFiUDP ntpUDP;
NTPClient *timeClient;

//MQTT Variablen
WiFiClient wifiClient;
PubSubClient MQTTclient(wifiClient);

//Grafik-Oberflaeche
Window *Hauptansicht = new Window(320, 480);
Button *Button1 = new Button(Hauptansicht, 0, 32, 72, 72);
Button *Button2 = new Button(Hauptansicht, 0, 104, 72, 72);
Button *Button3 = new Button(Hauptansicht, 0, 176, 72, 72);
Button *Button4 = new Button(Hauptansicht, 0, 248, 72, 72);
Level *Anzeige = new Level(Hauptansicht, 380, 32, 290);
Bar *Status = new Bar(Hauptansicht);
Menu *Hauptmenu = new Menu(Hauptansicht);

Window *DebugFenster;



#endif