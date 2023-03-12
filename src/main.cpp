//#include "Farben.h"
#include <Arduino.h>
#include "Display.h"
#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <EEPROM.h>
//MQTT
#include <PubSubClient.h>
//fuer Uhrzeitabruf notwendig
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//Projektspezifisch
#include "HTML_Var.h"


//Projektvariablen
sprinklerConfig varConfig;
unsigned int Zisterne_Stand_Roh = 0;
uint8_t touchPorts[8] = {touchUp_wsgn, touchDown_gn, touchLeft_br, touchRight_wsbr, touchF1_bl, touchF2_wsbl, touchF3_or, touchF4_wsor};

//Touch-Variablen neu
float arrTouchAVG[8] = {0, 0, 0, 0, 0, 0, 0, 0};           //Array für die Durchschnitswerte pro Touch-Sensor
uint8_t TouchSensitivity = 2;                              //Empfindlichkeit der Touchtasten unter Glas = 1
uint8_t TouchFailureCount = 0;                            //Fehler der Touchtasten aufsummieren (Aktueller Wert > Initialwert)
const uint8_t TouchFailureCountMax = 100;                  //Maximale Fehleranzahl der Tochtasten
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
uint8_t ProgState[ProgItems];                              //Variablenarray zum speichern zum aktuellen Ausgangsdaten der Programme
uint8_t NanoOutputState = 0;                               //Relais sollstatus, nach Bestätigung von Nano wird Status übernommen
unsigned long ProgStart[ProgItems];                        //Variable zum speichern der Startzeit
uint8_t DebugMode = 0;                                     //Debugmode fuer reine Schriftausgabe am Display
unsigned long DisplayVerz = 120000;                        //Millisekunden die das Display nach letzter Betaetigung an bleibt
uint8_t NanoOutputRequested = 0;                           //Angabe ob eine Ausgangsabfrage offen ist und von Nano erwartet wird 0 = keine
unsigned long NanoOutputRequestTimeout = 0;                           //Zeitstempel wann die Anfrage ueberfaellig ist
unsigned int NanoOutputRequestTimebuffer = 1500;                      //Zeit in ms wann die Antwort angekommen sein muss
uint16_t NanoOutputRequestErrors = 0;                            //Anzahl von nicht Empfangenen Bestätigungen oder falschen Bestätigungen; Wird nach Empfang einer richtigen Nachricht zurückgesetzt
uint16_t NanoRequests_afterLastAnsw = 0;                         //Anzahl von Anfragen nach letzter Antwort von Nano
uint16_t maxWaterLevelRoh = 204;                               //Max Rueckgabewert bei Wasserstandsabfrage (in cm)
uint16_t minWaterLevelRoh = 20;                               //Min Rueckgabewert bei Wasserstandsabfrage (in cm)
uint16_t maxWaterLevelLiter = 8845;                          //Maximaler Fuellstand in Liter; Bei maxWassertiefe von 204 cm und min. 22 cm
int pGeneralVar[8] = {-1, -1, -1, -1, -1, -1, -1, -1};            //Verschiedene Wasserstaende und Verbraeuche
uint8_t SetOutputMan = 0;             //Variable um einen Ausgang manuell zu setzen
uint32_t NanoRequestFlags = 0;          //Anfragen die noch geschickt werden müssen, damit nicht alle auf einmal geschickt werden
char MQTTState[20] = "MQTT Ready";
char WifiState[20] = "Wifi Ready";
char WifiLastChange[50] = "";         //Aufnahme des Zeitstempels wann letztes mal die Verbindung neu aufgebaut wurde inklusive des WIFI Statuses

//-----------------------------------
//Variablen und Funktion zur Fehlersuche (Absturz Nano nach Empfang von Nachrichten)
uint8_t const maxMessages = 40;
char * sentMessages[maxMessages] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t lastMessage = maxMessages-1;
uint16_t countMessages = 0;

void SaveMessage(char * newMes, bool Nano = false)
{
  if((NanoOutputRequestErrors > 5)||(NanoRequests_afterLastAnsw > 5))
  return;

  uint8_t nextPos = 0;
  if(lastMessage < (maxMessages-1))
    nextPos = lastMessage + 1;
  
 
  if(sentMessages[nextPos]!=0)
    delete[] sentMessages[nextPos];

  sentMessages[nextPos] = new char[strlen(newMes)+30];
  if(Nano)
    sprintf(sentMessages[nextPos], "%d.%d.%d %d:%d nano -> \"%s\"", monthDay, currentMonth, currentYear, currentHour, currentMin, newMes);
  else
    sprintf(sentMessages[nextPos], "%d.%d.%d %d:%d -> \"%s\"", monthDay, currentMonth, currentYear, currentHour, currentMin, newMes);
  
  lastMessage = nextPos;
  countMessages++;
}
char * GetLastMessagesHTML()
{
  int signCount = 120;
  char * TextSum = 0;
  for (int i = 0; i < maxMessages; i++)
  {
    if(sentMessages[i])
      signCount += (strlen(sentMessages[i])+12);
  }
  TextSum = new char[signCount];
  sprintf(TextSum, "Die letzten gesendeten Nachrichten ueber UART (%d): <br>\n", signCount);
  uint8_t ReadPos = lastMessage;
  for (int i = 0; i< maxMessages; i++)
  {
    if(ReadPos < (maxMessages-1))
      ReadPos++;
    else
      ReadPos = 0;
    
    if(!sentMessages[ReadPos])
      continue;

    sprintf(TextSum, "%s%d -> %s <br>\n", TextSum, i, sentMessages[ReadPos]);
  }
  return TextSum;
}
//-----------------------------------

//Funktionsdefinitionen
void WiFi_Start_STA(char *ssid_sta, char *password_sta);
void WiFi_Start_AP(const char * ssid_AP);
bool WIFIConnectionCheck(bool with_reconnect);
void notFound(AsyncWebServerRequest *request);
void EinstSpeichern();
void EinstLaden();
char ResetVarLesen();
void ResetVarSpeichern(char Count);
void initDebugWindow();
void closeDebugWindow();
float AVGTouchPort(uint8_t Port, u32_t Delay);
int setOutput(char Channel, char State);
void CheckProg(SprinklerProgramm *Prog, unsigned long *Start);
void LEDSmooth(int Speed);
void LEDOff();
void rebuildMainMenu(Menu *Hauptmenu, sprinklerConfig *varConfig, int * Wasserstaende);
void UpdateButtonName(sprinklerConfig *varConfig);
uint8_t GetOutput(SprinklerProgramm *_Prog, unsigned long *_Start);
int sendtoNano(char Command, uint32_t Data = 0);
void SendRequests(uint32_t * Flags);
int getfromNano(uint32_t * pData);
uint32_t calcWaterLiter(int Wasserstand);
bool MQTTinit();  //Wenn verbunden Rückgabewert true
void MQTT_callback(char* topic, byte* payload, unsigned int length);
bool MQTT_sendMessage(int MQTT_MSGType, const uint8_t* MSG, uint8_t len);
bool MQTT_sendMessage(int MQTT_MSGType, int MSG);
bool MQTT_sendMessage(int MQTT_MSGType, float MSG);
bool MQTT_sendMessage(int MQTT_MSGType, uint8_t MSG);
bool MQTT_sendMessage(int MQTT_MSGType, uint32_t MSG);
void TouchInit(); //Initialisierung der Touchtasten
void WebserverRoot(AsyncWebServerRequest *request);
void WebserverLEDConfig(AsyncWebServerRequest *request);
void WebserverChannel(AsyncWebServerRequest *request);
void WebserverProg(AsyncWebServerRequest *request);
void WebserverSettings(AsyncWebServerRequest *request);
void WebserverDelProg(AsyncWebServerRequest *request);
void WebserverDebugStart(AsyncWebServerRequest *request);
void WebserverDebugToggleWindow(AsyncWebServerRequest *request);
void WebserverDebugText(AsyncWebServerRequest *request);
void WebserverDebugEnd(AsyncWebServerRequest *request);
void WebserverLastMessages(AsyncWebServerRequest *request);
void WebserverDisplayOff(AsyncWebServerRequest *request);
void WebserverDisplayOn(AsyncWebServerRequest *request);
void WebserverTouchInit(AsyncWebServerRequest *request);
void WebserverPOST(AsyncWebServerRequest *request);

//Erstellen Serverelement
AsyncWebServer server(8080);

// WiFi Variablen
//char ssid_sta[] = "SSID_WN";                                                             //"<your SSID>";
//char password_sta[] = ""; //"<Your Password";

//const char *ssid_AP = "ESP_Beregnung_01";
//const char *password_AP = ""; //Wenn Passwort, dann muss es eine Laenge von 8 - 32 Zeichen haben

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


void setup(void)
{
  char ResetCount;

  //Display
  pinMode(Display_Beleuchtung, OUTPUT);
  digitalWrite(Display_Beleuchtung, 1);
  initDebugWindow();
  DebugFenster->printnl("UART initialisieren");
  Serial.begin(9600);
  delay(300);
  DebugFenster->printnl("Reset-Variable aus EEPROM lesen und erhöhen");
  ResetCount = ResetVarLesen();
  if((ResetCount < 0)||(ResetCount > 5))  //Prüfen ob Wert Plausibel, wenn nicht rücksetzen
    ResetCount = 0;

    //ResetCount++;
  DebugFenster->printnl("Reset-Variable in EEPROM schreiben und 5 Sekunden warten");
  ResetVarSpeichern(ResetCount);
  for(int i = 0; i<5; i++)
  {
    DebugFenster->print(".");
    delay(1000);
  }
  DebugFenster->printnl(".");
  DebugFenster->printnl("Einstellungen aus EEPROM einlesen");
  if (ResetCount < 5) //Wenn nicht 5 mal in den ersten 5 Sekunden der Startvorgang abgebrochen wurde
    EinstLaden();
  else
    DebugFenster->printnl("Achtung!!! Einstellungen wurden nicht eingelesen!!");
  ResetVarSpeichern(0);
  DebugFenster->printnl("WLAN-Einstellungen vornehmen und verbinden");
  //WLAN starten
  if (varConfig.WLAN_AP_Aktiv == 1)
    WiFi_Start_AP(varConfig.WLAN_SSID);
  else
    WiFi_Start_STA(varConfig.WLAN_SSID, varConfig.WLAN_Password);
  //WiFi_Start_STA(ssid_sta, password_sta);

  //Zeitserver Einstellungen
  DebugFenster->printnl("Zeitserver einstellen und verbinden");
  if (strlen(varConfig.NW_NTPServer))
    timeClient = new NTPClient(ntpUDP, (const char *)varConfig.NW_NTPServer);
  else
    timeClient = new NTPClient(ntpUDP, "fritz.box");
  delay(1000);

  timeClient->begin();
  timeClient->setTimeOffset(varConfig.NW_NTPOffset * 3600);

  //Nur für Test muss wieder gelöscht werden
  pGeneralVar[WaterDifDay] = WiFi.RSSI();


  //MQTT
  DebugFenster->printnl("MQTT initialisieren");
  MQTTinit();

  //OTA
  DebugFenster->printnl("OTA initialisieren");
  ArduinoOTA.setHostname("BeregnungOTA");
  ArduinoOTA.setPassword("Beregnung!123");
  ArduinoOTA.begin();
  for(int i = 0; i<5; i++)
  {
    //OTA
    ArduinoOTA.handle();
    DebugFenster->print(".");
    delay(1000);
  }

  //LED
  DebugFenster->printnl("RGB-LED initialisieren");
  ledcSetup(0, 5000, 8);
  ledcAttachPin(RGB_Red, 0);
  ledcSetup(1, 5000, 8);
  ledcAttachPin(RGB_Green, 1);
  ledcSetup(2, 5000, 8);
  ledcAttachPin(RGB_Blue, 2);

  DebugFenster->printnl("Ausgänge am Nano auf 0 setzen");
  sendtoNano(set_output, 0); //Ausgänge vom Nano bei Neustart auf 0 setzen

  //Webserver
  DebugFenster->printnl("Web-Server initialisieren");
  server.onNotFound(notFound);
  server.begin();
  server.on("/", HTTP_GET, WebserverRoot);
  server.on("/LED-Config", HTTP_GET, WebserverLEDConfig);
  server.on("/DisplayOn", HTTP_GET, WebserverDisplayOn);
  server.on("/DisplayOff", HTTP_GET, WebserverDisplayOff);
  server.on("/DebugStart", HTTP_GET, WebserverDebugStart);
  server.on("/LastMessages", HTTP_GET, WebserverLastMessages);
  server.on("/DebugText", HTTP_GET, WebserverDebugText);
  server.on("/DebugToggleWindow", HTTP_GET, WebserverDebugToggleWindow);
  server.on("/DebugEnd", HTTP_GET, WebserverDebugEnd);
  server.on("/TouchInit", HTTP_GET, WebserverTouchInit);
  server.on("/Channel", HTTP_GET, WebserverChannel);
  server.on("/Prog", HTTP_GET, WebserverProg);
  server.on("/DelProg", HTTP_GET, WebserverDelProg);
  server.on("/Settings", HTTP_GET, WebserverSettings);
  server.on("/POST", HTTP_POST, WebserverPOST);
  //Toucharray initialisieren
  DebugFenster->printnl("Touch-Tasten initialisieren");
  TouchInit();
  delay(5000);

  closeDebugWindow();
  //Grafische Oberflaeche
  Button1->b_Color = TFT_DARKCYAN;
  Button1->SetPossibleStates(3);
  Button1->Text = varConfig.Programm[0].ProgName;
  Button1->DrawObject();
  Button2->b_Color = TFT_DARKGREY;
  Button2->SetPossibleStates(3);
  Button2->Text = varConfig.Programm[1].ProgName;
  Button2->DrawObject();
  Button3->b_Color = TFT_DARKGREY;
  Button3->SetPossibleStates(3);
  Button3->Text = varConfig.Programm[2].ProgName;
  Button3->DrawObject();
  Button4->SetPossibleStates(2);
  Button4->b_Color = TFT_DARKGREEN;
  Button4->Text = "Infinity";
  Button4->DrawObject();
  Status->Schriftgroesse = 1;
  Status->DrawObject();
  Anzeige->SetLevelMax(100);
  Anzeige->SetLevelMin(0);
  Anzeige->SetLevelIst(0);
  Anzeige->Text = "Fuellstand";
  Anzeige->DrawObject();
  Hauptmenu->CreateMenus(ProgItems + 3);
  rebuildMainMenu(Hauptmenu, &varConfig, pGeneralVar);
  //Errechnen des maximalen Fuellstands der Zisterne
  maxWaterLevelLiter = calcWaterLiter(maxWaterLevelRoh - minWaterLevelRoh);

  Hauptmenu->MenuNavNext();
  Hauptmenu->DrawObject();
  cnt_LED_Display = millis() + DisplayVerz; //Damit das Display am Anfang eingeschaltet ist

}

uint8_t Test_Var = 0;
void loop()
{
  //OTA
  ArduinoOTA.handle();
  //Anweisungen werden alle 200 Millisekunden ausgefuehrt
  if (Break_200ms < millis())
  {
    Break_200ms = millis() + 200;
    if (TouchSperre < millis())
    {
      for (int i = 0; i < 8; i++)
      {
        if((arrTouchAVG[i] - touchRead(touchPorts[i])) > TouchSensitivity)
        {
          float currentTouch = AVGTouchPort(touchPorts[i], 1);
          if((arrTouchAVG[i] - currentTouch) > TouchSensitivity)
          { 
            if(DebugMode)
            {
              DebugFenster->print("Touchwert ");
              DebugFenster->print(i);
              DebugFenster->print(" Initialwert ");
              DebugFenster->print(arrTouchAVG[i]);
              DebugFenster->print(" aktueller Wert ");
              DebugFenster->println(currentTouch);
            }
            if (cnt_LED_Display < millis())
            {
              TouchSperre = millis() + 500;
              if (i == t_F1)
              {
                digitalWrite(Display_Beleuchtung, 1);
                cnt_LED_Display = millis() + DisplayVerz;
                break;
              }
            }
            else
            {
              TouchSperre = millis() + 500;
              switch (i)
              {
              case t_up:
                Hauptmenu->MenuNavUp();
                cnt_LED_Display = millis() + DisplayVerz;
                if (DebugMode)
                DebugFenster->printnl("t_up");
                break;
              case t_down:
                Hauptmenu->MenuNavDown();
                cnt_LED_Display = millis() + DisplayVerz;
                if (DebugMode)
                DebugFenster->printnl("t_down");
                break;
              case t_Right:
                Hauptmenu->MenuNavNext();
                cnt_LED_Display = millis() + DisplayVerz;
                if (DebugMode)
                DebugFenster->printnl("t_Right");
                break;
              case t_Left:
                Hauptmenu->MenuNavRev();
                cnt_LED_Display = millis() + DisplayVerz;
                if (DebugMode)
                DebugFenster->printnl("t_Left");
                break;
              case t_F1:
                Button1->PushButton();
                if (DebugMode)
                DebugFenster->printnl("t_F1");
                if (Button1->GetButtonState() == Switch_auto)
                  ProgStart[0] = millis();
                else
                  ProgStart[0] = 0;
                Break_60s = 0;
                cnt_LED_Display = millis() + DisplayVerz;
                break;
              case t_F2:
                if (DebugMode)
                DebugFenster->printnl("t_F2");
                Button2->PushButton();
                if (Button2->GetButtonState() == Switch_auto)
                  ProgStart[1] = millis();
                else
                  ProgStart[1] = 0;
                Break_60s = 0;
                cnt_LED_Display = millis() + DisplayVerz;
                break;
              case t_F3:
                if (DebugMode)
                DebugFenster->printnl("t_F3");
                Button3->PushButton();
                if (Button3->GetButtonState() == Switch_auto)
                  ProgStart[2] = millis();
                else
                  ProgStart[2] = 0;
                Break_60s = 0;
                cnt_LED_Display = millis() + DisplayVerz;
                break;
              case t_F4:
                if (DebugMode)
                DebugFenster->printnl("t_F4");
                Button4->PushButton();
                cnt_LED_Display = millis() + DisplayVerz;
                break;
              default:
                break;
              }
            }
          }
          else
          {
            if(Break_Touchtest < millis())
            {
              float ReadVar = AVGTouchPort(touchPorts[i], 1);
              if(((ReadVar - arrTouchAVG[i]) > 0.3))
              {
                if(TouchFailureCount >= TouchFailureCountMax)
                {
                  TouchFailureCount = 0;
                  TouchInit();
                }
                else
                  TouchFailureCount++;
                if(DebugMode)
                {
                  DebugFenster->print("Touchport_Fehler ");
                  DebugFenster->print(i);
                  DebugFenster->print(" Fehleranzahl ");
                  DebugFenster->print(TouchFailureCount);
                  DebugFenster->print(" Initialwert ");
                  DebugFenster->print(arrTouchAVG[i]);
                  DebugFenster->print(" aktueller Wert ");
                  DebugFenster->println(ReadVar);
                }
              }
            }
          }
        }
      }
      Break_Touchtest = (Break_Touchtest > millis())?Break_Touchtest:(millis() + 1000);
    }
  }
  //Anweisungen werden alle 1 Sekunden ausgefuehrt
  if (Break_1s < millis())
  {
    Break_1s = millis() + 1000;
    if ((cnt_LED_Display > millis())||DebugMode)
    {
      digitalWrite(Display_Beleuchtung, 1);
    }
    else
    {
      digitalWrite(Display_Beleuchtung, 0);
    }
  }
  //Anweisungen werden alle 20 Sekunden ausgefuehrt
  if (Break_60s < millis())
  {
    Break_60s = millis() + 60000;
    SendRequests(&NanoRequestFlags);

    //Vorbereitung Datum
    unsigned long epochTime = timeClient->getEpochTime();
    struct tm *ptm = gmtime((time_t *)&epochTime);
    monthDay = ptm->tm_mday;
    currentMonth = ptm->tm_mon + 1;
    currentYear = ptm->tm_year + 1900;
    currentHour = ptm->tm_hour;
    currentMin = ptm->tm_min;
    char Temp[50];

    //Ermitteln des Ausgangs
    switch (Button1->GetButtonState())
    {
    case Switch_auto:
      ProgState[0] = GetOutput(&varConfig.Programm[0], &ProgStart[0]);   //Setzen des Ausgangs fuer Programm 1
      if (ProgStart[0] == 0)
        Button1->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      ProgState[0] = 0;
    }
    switch (Button2->GetButtonState())
    {
    case Switch_auto:
      ProgState[1] = GetOutput(&varConfig.Programm[1], &ProgStart[1]);   //Setzen des Ausgangs fuer Programm 2
      if (ProgStart[1] == 0)
        Button2->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      ProgState[1] = 0;
    }
    switch (Button3->GetButtonState())
    {
    case Switch_auto:
      ProgState[2] = GetOutput(&varConfig.Programm[2], &ProgStart[2]);   //Setzen des Ausgangs fuer Programm 3
      if (ProgStart[2] == 0)
        Button3->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      ProgState[2] = 0;
    }
    //MQTT Verbindungskontrolle und neu verbinden
    if (MQTTclient.state() != 0)
    {
      if(WIFIConnectionCheck(false))
      {
        WifiState[0] = 'W';
        MQTTclient.disconnect();
        MQTTinit();
        if(DebugMode)
        {
          DebugFenster->print(timeClient->getFormattedTime().c_str());
          DebugFenster->printnl("MQTT-Verbindung neu erstellt");
        }
      }
      else
        WifiState[0] = 0;
    }
    else
    {
      sprintf(MQTTState, "MQTT Ready");
      WifiState[0] = 'W';
    }
    sprintf(Temp, "%d:%02d, %s %02d.%02d.%d | W=%d | M=%d", currentHour, currentMin, WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WiFi.status(), MQTTclient.state());
    Status->Text = Temp;
    Status->UpdateState();
    /*
    MQTT
    -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
    -3 : MQTT_CONNECTION_LOST - the network connection was broken
    -2 : MQTT_CONNECT_FAILED - the network connection failed
    -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
    0 : MQTT_CONNECTED - the client is connected
    1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
    2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
    3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
    4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
    5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect

    WiFi
    WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
    WL_IDLE_STATUS      = 0,
    WL_NO_SSID_AVAIL    = 1,
    WL_SCAN_COMPLETED   = 2,
    WL_CONNECTED        = 3,
    WL_CONNECT_FAILED   = 4,
    WL_CONNECTION_LOST  = 5,
    WL_DISCONNECTED     = 6
  */
  }
  //Anweisungen werden alle 10 Minuten ausgefuehrt
  if (Break_10m < millis())
  {
    Break_10m = millis() + 600000;
    int WIFITemp = WiFi.RSSI();
    if(pGeneralVar[WaterDifDay] != WIFITemp)
    {
      pGeneralVar[WaterDifDay] = WIFITemp;
      Hauptmenu->UpdateMenuPos(indexAGM, WaterDifDay);
    }
    sprintf(MQTTState, "MQTT %d", MQTTclient.state());
    NanoRequestFlags |= 1<<get_waterlevel;  //Anfordern Wasserstand in Zisterne
  }

  //Anweisungen werden alle 3600 Sekunden (1h) ausgefuehrt
  if (Break_h < millis())
  {
    Break_h = millis() + 3600000;
    timeClient->update();
    if(WIFIConnectionCheck(true))
    {
      NanoRequestFlags |= (1<<get_waterlevel) | (1<<get_watervolcompl);
    }
  }
  //LED aktivieren
  if (Button4->GetButtonState() == Switch_on)
    LEDSmooth(100);
  else
    LEDOff();
  //Update Grafische Oberflaeche
  Button1->UpdateState();
  Button2->UpdateState();
  Button3->UpdateState();
  Button4->UpdateState();

  //Wenn Aenderung am Ausgang
  if ((Status->Status != (ProgState[0] | ProgState[1] | ProgState[2] | SetOutputMan))&&NanoOutputRequested == 0)
  {
    //Setzen der Grafic auf den neuen Ausgang und uebertragung der Daten zu Nano
    NanoOutputState = ProgState[0] | ProgState[1] | ProgState[2] | SetOutputMan;
    sendtoNano(set_output, NanoOutputState);
    NanoOutputRequested = get_outputcheck;
    NanoOutputRequestTimeout = millis() + NanoOutputRequestTimebuffer;
    if(DebugMode)
    {
      DebugFenster->print("Prog1 = ");
      DebugFenster->println(ProgState[0]);
      DebugFenster->print("Prog2 = ");
      DebugFenster->println(ProgState[1]);
      DebugFenster->print("Prog3 = ");
      DebugFenster->println(ProgState[2]);
      DebugFenster->print("SetOutputMan = ");
      DebugFenster->println(SetOutputMan);
      DebugFenster->print("Status = ");
      DebugFenster->println(Status->Status);
    }
  }
  //Wenn die Antwort von Nano fehlt
  if((NanoOutputRequestTimeout < millis()) && NanoOutputRequested)
  {
    //Fehlerbehandlung muss noch programmiert werden
    if(DebugMode)
    {
      DebugFenster->print("Ausgänge wurden nicht bestätigt: ");
      DebugFenster->println(NanoOutputRequestErrors);      
    }
    sendtoNano(set_output, NanoOutputState);
    NanoOutputRequestTimeout = millis() + NanoOutputRequestTimebuffer + (NanoOutputRequestErrors*1000);
    NanoOutputRequestErrors++;
  }
  //MQTT wichtige Funktion
  if(WiFi.status()== WL_CONNECTED)  //MQTTclient.loop() wichtig damit die Daten im Hintergrund empfangen werden
  {
    if(!MQTTclient.loop())
    {
      MQTTclient.disconnect();
      MQTTinit();
    }
  }
  //Abfrage ob Daten von der seriellen Schnittstelle empfangen wurden
  if(Serial.available() > 0) //Wenn Zeichen angekommen
  {
    uint32_t Temp = 0;
    switch(getfromNano(&Temp))
    {
      case err_noerr:
        NanoOutputRequestTimeout = 0;        
        break;
      case get_outputcheck:
        if(DebugMode)
        {
          DebugFenster->printnl("Folgende Ausgaenge wurden bestaetigt.");
          DebugFenster->print("Gesendet: ");
          DebugFenster->print(NanoOutputState);
          DebugFenster->print(" | Empfangen: ");
          DebugFenster->println(Temp);
        }
        if(NanoOutputState == Temp)
        {
          Status->Status = NanoOutputState;
          Status->UpdateState();
          NanoOutputRequestErrors = 0;
          NanoOutputRequested = 0;
        }
        else
        {
          Status->Status = Temp;
          Status->UpdateState();
          sendtoNano(set_output, NanoOutputState);
          NanoOutputRequested = get_outputcheck;
          NanoOutputRequestTimeout = millis() + NanoOutputRequestTimebuffer;
          NanoOutputRequestErrors++;
        }
        MQTT_sendMessage(MQTT_stateOutput, (uint8_t) Temp);
        break;
      case err_waterlevel:
        if(DebugMode)
        {
          DebugFenster->printnl("get_errwaterlevel-Fehler vom Nano empfangen!");
        }
        break;
      case get_waterlevel:
        if((Temp > (maxWaterLevelRoh + 10))||(Temp < minWaterLevelRoh))
        {
          if(DebugMode)
          {
            DebugFenster->printnl("Wasserstand Rueckgabewert ausserhalb erlaubtem Bereichs.");
            DebugFenster->print("Empfangen: ");
            DebugFenster->println(Temp);
          }
          pGeneralVar[WaterLevRaw] = -1;
          Hauptmenu->UpdateMenuPos(indexAGM, WaterLevRaw);
        }
        else
        {
          pGeneralVar[WaterLevRaw] = Temp;
          pGeneralVar[WaterLev] = maxWaterLevelRoh - Temp;
          pGeneralVar[WaterLevLiter] = calcWaterLiter(pGeneralVar[WaterLev]);
          Anzeige->SetLevelIst(100.0* ((float) pGeneralVar[WaterLevLiter]/maxWaterLevelLiter));
          Anzeige->DrawObject();
          Hauptmenu->UpdateMenuPos(indexAGM, WaterLevRaw);
          Hauptmenu->UpdateMenuPos(indexAGM, WaterLev);
          Hauptmenu->UpdateMenuPos(indexAGM, WaterLevLiter);
          if(DebugMode)
          {
            DebugFenster->print("Wasserstand auf ");
            DebugFenster->print(Temp);
            DebugFenster->print("gesetzt.");
          }
        }
        MQTT_sendMessage(MQTT_Waterlevel, pGeneralVar[WaterLevLiter]); //Übertragen des Wasserstands in l
        break;
      case get_watervolcompl:
        if(Temp >= 0)
        {
          uint32_t WaterVolNew = (float) Temp/1000 + 0.5;
          if(pGeneralVar[WaterVolTotal] != WaterVolNew)
          {
            pGeneralVar[WaterVolTotal] = WaterVolNew;
            char strTemp[50] = "";
            sprintf(strTemp, "%.1f", ((float) WaterVolNew/1000));
            Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[WaterVolTotal]->Value1 = strTemp;
            Hauptmenu->UpdateMenuPos(indexAGM, WaterVolTotal);
            MQTT_sendMessage(MQTT_totalConsume, WaterVolNew); //Übertragen des Wasserverbrauchs in l
          }
          if(DebugMode)
          {
            DebugFenster->print("Wasserverbrauch ");
            DebugFenster->print(Temp);
            DebugFenster->printnl(" ml erhalten und in l gespeichert.");
          }
        }
        break;
      case get_watervolcurrent:
        if(Temp >= 0)
        {
          uint32_t WasserCurNeu = Temp;
          MQTT_sendMessage(MQTT_currentConsume, Temp); //Übertragen des Wasserverbrauchs in ml
        //  MQTT_sendMessage(MQTT_totalConsume, Temp); //Übertragen des Wasserverbrauchs in ml
          if(pGeneralVar[WaterVolCur] != WasserCurNeu)
          {
            pGeneralVar[WaterVolCur] = WasserCurNeu;
            Hauptmenu->UpdateMenuPos(indexAGM, WaterVolCur);
          }
          if(DebugMode)
          {
            DebugFenster->print("Aktueller Wasserverbrauch ");
            DebugFenster->print(Temp);
            DebugFenster->printnl(" ml/s erhalten und gespeichert.");
          }
        }
        break;
      case get_CounterValue:
        if(Temp >= 0)
        {
          MQTT_sendMessage(MQTT_CounterValue, Temp); 
          if(DebugMode)
          {
            DebugFenster->print("Aktueller Zählerstand ");
            DebugFenster->println(Temp);
          }
        }
        break;
      case err_read:
        break;
      default:
      break;
    }
  }

}
//---------------------------------------------------------------------
//Debug Fenster
void initDebugWindow()
{
  Hauptansicht->WindowActiv = false;
  DebugFenster = new Window(320, 480);
  DebugFenster->setDebug();
  DebugMode = 1;
  DebugFenster->tft.setTextSize(1);
}
void closeDebugWindow()
{
  delete DebugFenster;
  DebugFenster = 0;
  DebugMode = 0;
  Hauptansicht->WindowActiv = true;
}
//---------------------------------------------------------------------
//Funktion zum Umrechen vom Wasserstand in cm auf Liter
uint32_t calcWaterLiter(int Wasserstand)
{
  float TempWaterSt = (float) Wasserstand / 100;
  float TempSummand[3] = {0, 0, 0};
  TempSummand[0] = 5.0/3 * TempWaterSt * TempWaterSt * TempWaterSt;
  TempSummand[1] = 5.25 * TempWaterSt * TempWaterSt;
  TempSummand[2] = 0.826 * TempWaterSt;
  if(DebugMode)
  {
    DebugFenster->print("Wasserstand: ");
    DebugFenster->print(Wasserstand);
    DebugFenster->print("\nSummand 1: ");
    DebugFenster->print(TempSummand[0]);
    DebugFenster->print("\nSummand 2: ");
    DebugFenster->print(TempSummand[1]);
    DebugFenster->print("\nSummand 3: ");
    DebugFenster->println(TempSummand[2]);
  }
  return 1000 * (TempSummand[1] + TempSummand[2] - TempSummand[0]);
}
//---------------------------------------------------------------------
//Funktion zum empfangen der Befehler von Nano
//Rueckgabewert entspricht der Art der Daten, Daten werden ueber den Pointer zurueckgegeben
int getfromNano(uint32_t * pData)
{
  char Temp[50];
  char Checksumme = 0;
  int length = Serial.readBytes(Temp, 50);
  SaveMessage(Temp, true);
  NanoRequests_afterLastAnsw = 0; //Zur Fehlersuche bezüglich Nanoabstürze
  if(DebugMode)
  {
    Temp[length] = 0;
    DebugFenster->printnl("Daten von Nano empfangen: ");
    DebugFenster->printnl(Temp);
  }
  if(length == 0)
  {
    if(DebugMode)
    {
      DebugFenster->printnl("UART Fehler, keine Zeichen empfangen obwohl available > 0");
    }
    return err_read;
  }
  //Errechnen und Auswertung der Checksumme, diese sollte 0 ergeben
  for(int i = 0; i < length-1; i++)
    Checksumme += Temp[i];
  if(Checksumme)
  {
    if(DebugMode)
    {
      DebugFenster->print("UART Fehler, Checksummenfehler: ");
      DebugFenster->print(Checksumme);
    }
    return err_read;
  }
  else
  Temp[length-1] = 0;   //Wenn die Checksumme passt, wird der Wert auf 0 gesetzt um bei der weiteren Auswertung nicht zu stoeren
  switch (Temp[0])
  {
  case 'A':
    if(Temp[1] == '1')
    {
      if(sscanf(Temp, "A1_%u", pData) != 1)
      {
        if(DebugMode)
        {
          DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'A')!");
          DebugFenster->printnl(Temp);
          DebugFenster->print(*pData);
        }
        return err_read; 
      }
      return get_outputcheck;
    }
    else
    {
      if(DebugMode)
      {
        DebugFenster->printnl("UART Fehler, Syntax falsch!");
      }
      return err_read;      
    }
    break;
  case 'B':
    if(Temp[1] == '1')
    {
      if(sscanf(Temp, "B1_%u", pData) != 1) //Wenn mehr oder weniger Rueckgabewerte erkannt Fehler
      {
        if(DebugMode)
        {
          DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'B')!");
          DebugFenster->printnl(Temp);
          DebugFenster->print(*pData);
        }
        return err_read; 
      }
      if(DebugMode)
      {
        DebugFenster->print("get_waterlevel empfangen: ");
        DebugFenster->println(*pData);
      }
      return get_waterlevel;
    }
    else
    {
      if(DebugMode)
      {
        DebugFenster->printnl("UART Fehler, Syntax falsch!");
      }
      return err_read;      
    }
    break;
  case 'C': //Muss erst noch geprueft werden, welche WErte vom Nano abgerufen werden sollen
    switch(Temp[1])
    {
      case '1':
        if(sscanf(Temp, "C1_%u", pData) != 1)
        {
          if(DebugMode)
          {
            DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'C')!");
            DebugFenster->printnl(Temp);
            DebugFenster->print(*pData);
          }
          return err_read; 
        }
        return get_watervolcompl;
      case '3':
        if(sscanf(Temp, "C3_%u", pData) != 1)
        {
          if(DebugMode)
          {
            DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'C')!");
            DebugFenster->printnl(Temp);
            DebugFenster->print(*pData);
          }
          return err_read; 
        }
        return get_watervolcurrent;
      case '7':
        if(sscanf(Temp, "C7_%u", pData) != 1)
        {
          if(DebugMode)
          {
            DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'C')!");
            DebugFenster->printnl(Temp);
            DebugFenster->print(*pData);
          }
          return err_read; 
        }
        return get_ConvVar;
      default:
        return err_read; 
    }
  case 'D': //Muss erst noch geprueft werden, welche WErte vom Nano abgerufen werden sollen
    switch(Temp[1])
    {
      case '1': //Einstellungen empfangen, muss noch programmiert werden
        break;
      case '2': //Empfang der Zählerwerte
        if(sscanf(Temp, "D2_%u", pData) != 1)
        {
          if(DebugMode)
          {
            DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'D2')!");
            DebugFenster->printnl(Temp);
            DebugFenster->print(*pData);
          }
          return err_read; 
        }
        return get_CounterValue;
      default:
        return err_read; 
    }
  case 'S':
    if(length > 2)
    {
      if(sscanf(Temp, "S_%u", pData) != 1)
      {
        if(DebugMode)
        {
          DebugFenster->printnl("UART Fehler, Syntax falsch (SSCANF)(case 'S')!");
          DebugFenster->printnl(Temp);
          DebugFenster->print(*pData);
        }
        return err_read; 
      }
      return *pData;
    }
    else
    {
      return err_read;
    }
    break;
  default:
      if(DebugMode)
      {
        DebugFenster->printnl("UART Fehler, Syntax falsch (Falsche Anweisung)!");
      }
      return err_read;      
    break;
  }
  return 0;
}
//Funktion zum senden der Befehle zu Nano
int sendtoNano(char Command, uint32_t Data)
{
  char strTemp[15] = "";
  NanoRequests_afterLastAnsw++; //Fehlersuche zu Nanoabstürzen
  switch(Command)
  {
  case send_syscheck:
    strcpy(strTemp, "S");
    break;
  case set_output:
    sprintf(strTemp, "A0_%u", Data);
//    MQTT_sendMessage(MQTT_stateOutput, Data); //Übertragen Status Ausgänge
    break;
  case get_waterlevel:
    strcpy(strTemp, "B0");
    break;
  case get_watervolcompl:
    strcpy(strTemp, "C0");
    break;
  case get_watervolcurrent:
    strcpy(strTemp, "C2");
    break;
  case set_Config:
    sprintf(strTemp, "D0_%u", Data);
    break;
  case set_watervolcompl:
    sprintf(strTemp, "D0_%u", Data);
    break;
  case get_CounterValue:
    sprintf(strTemp, "D2_%u", Data);
    break;
  default:
    break;
  }
  int Length = strlen(strTemp);
  unsigned char checkSum = 0;
  for (int i = 0; i < Length; i++)
  {
    checkSum += strTemp[i];
  }
  strTemp[Length] = (256 - checkSum);
  strTemp[Length + 1] = 0;
  Serial.write(strTemp, Length + 2);
  SaveMessage(strTemp);
  int err = Serial.getWriteError();
  if(DebugMode)
  {
    DebugFenster->print("Uebertragen: ");
    DebugFenster->printnl(strTemp);
    DebugFenster->print("Uebertragene Zeichenanzahl: ");
    DebugFenster->println((Length + 2));    
    //DebugFenster->print(256 - checkSum);
    DebugFenster->print("Uebertragungsfehler: ");
    DebugFenster->println(err);
  }
  return err;
}
//---------------------------------------------------------------------
//Funktion Um Werte vom Nano abzurufen duch Requestflags
void SendRequests(uint32_t * Flags)
{
  if(!(*Flags))
  return;
  for(int i = 0; i < 25; i++)
  {
    if((1<<i)& *Flags)
    {
      switch(i)
      {
        case get_ConvVar:
          sendtoNano(get_ConvVar, 0);
          *Flags &= ~(1<<i);
          return;
        case get_CounterValue:
          sendtoNano(get_CounterValue, 0);
          *Flags &= ~(1<<i);
          return;
//        case get_outputcheck:
//          sendtoNano(get_outputcheck, 0);
//          *Flags &= ~(1<<get_outputcheck);
//          return;
        case get_waterlevel:
          sendtoNano(get_waterlevel, 0);
          *Flags &= ~(1<<i);
          return;
        case get_watervolcompl:
          sendtoNano(get_watervolcompl, 0);
          *Flags &= ~(1<<i);
          return;
        case get_watervolcurrent:
          sendtoNano(get_watervolcurrent, 0);
          *Flags &= ~(1<<i);
          return;
        default:
         break;
      }
    }
  }
  *Flags = 0;
  return;
}
//---------------------------------------------------------------------
//Regner-Programmfunktion
uint8_t GetOutput(SprinklerProgramm *_Prog, unsigned long *_Start)
{
  unsigned long Ende = *_Start + (_Prog->ProgDauer * 60000);
  unsigned long CurrentTime = millis();
  uint8_t Temp_Out = 0;
  if(*_Start == 0)
  return 0;

  if (CurrentTime > Ende) //Wenn die Zeit bereits abgelaufen ist, Statzeit zuruecksetzen und 0 Ausgaenge zurueckgeben
  {
    *_Start = 0;
    return 0;
  }
  int StandinProzent = (float)100 / (_Prog->ProgDauer * 60000) * (CurrentTime - *_Start);
  for (int i = 0; i < _Prog->ProgItemCount; i++)
  {
    if ((StandinProzent >= _Prog->Zeilen[i].delay) && (StandinProzent < (_Prog->Zeilen[i].delay + _Prog->Zeilen[i].length)))
      Temp_Out |= 1 << (_Prog->Zeilen[i].channel - 1);
  }
  return Temp_Out;
}
//---------------------------------------------------------------------
//Wifi Funtkionen
void WiFi_Start_STA(char *ssid_sta, char *password_sta)
{
  unsigned long timeout;
  unsigned int Adresspuffer[4];
  if (varConfig.NW_StatischeIP)
  {
    sscanf(varConfig.NW_IPAdresse, "%d.%d.%d.%d", &Adresspuffer[0], &Adresspuffer[1], &Adresspuffer[2], &Adresspuffer[3]);
    IPAddress IP(Adresspuffer[0], Adresspuffer[1], Adresspuffer[2], Adresspuffer[3]);
    sscanf(varConfig.NW_Gateway, "%d.%d.%d.%d", &Adresspuffer[0], &Adresspuffer[1], &Adresspuffer[2], &Adresspuffer[3]);
    IPAddress IPGate(Adresspuffer[0], Adresspuffer[1], Adresspuffer[2], Adresspuffer[3]);
    sscanf(varConfig.NW_SubMask, "%d.%d.%d.%d", &Adresspuffer[0], &Adresspuffer[1], &Adresspuffer[2], &Adresspuffer[3]);
    IPAddress IPSub(Adresspuffer[0], Adresspuffer[1], Adresspuffer[2], Adresspuffer[3]);
    sscanf(varConfig.NW_Gateway, "%d.%d.%d.%d", &Adresspuffer[0], &Adresspuffer[1], &Adresspuffer[2], &Adresspuffer[3]);
    IPAddress IPDNS(Adresspuffer[0], Adresspuffer[1], Adresspuffer[2], Adresspuffer[3]);
    WiFi.config(IP, IPDNS, IPGate, IPSub);
  }
  WiFi.mode(WIFI_STA); // Client
  WiFi.hostname(varConfig.NW_NetzName);
  WiFi.begin(ssid_sta, password_sta);
  timeout = millis() + 12000L;
  while (WiFi.status() != WL_CONNECTED && millis() < timeout)
  {
    delay(10);
  }
  if (WiFi.status() == WL_CONNECTED)
  {
    //    server.begin();
    //    my_WiFi_Mode = WIFI_STA;
#ifdef BGTDEBUG
    Serial.print("Connected IP - Address : ");
    for (int i = 0; i < 3; i++)
    {
      Serial.print(WiFi.localIP()[i]);
      Serial.print(".");
    }
    Serial.println(WiFi.localIP()[3]);
#endif
  }
  else
  {
    WiFi.mode(WIFI_OFF);
#ifdef BGTDEBUG
    Serial.println("WLAN-Connection failed");
#endif
  }
}
void WiFi_Start_AP(const char * ssid_AP)
{
//  char *ssid_AP = "ESP_Beregnung_01";
  const char *password_AP = ""; //Wenn Passwort, dann muss es eine Laenge von 8 - 32 Zeichen haben

  WiFi.mode(WIFI_AP); // Accesspoint
                      //  WiFi.hostname(varConfig.NW_NetzName);

  WiFi.softAP(ssid_AP, password_AP);
  server.begin();
  IPAddress myIP = WiFi.softAPIP();
  //  my_WiFi_Mode = WIFI_AP;

#ifdef BGTDEBUG
  Serial.print("Accesspoint started - Name : ");
  Serial.print(ssid_AP);
  Serial.print(" IP address: ");
  Serial.println(myIP);
#endif
}
bool WIFIConnectionCheck(bool with_reconnect = true)
{
  if(WiFi.status()!= WL_CONNECTED)
  {
    if(with_reconnect)
    {
      WiFi.reconnect();
      sprintf(WifiLastChange, "(%d) %d:%d %d.%d.%d", WiFi.status(), currentHour, currentMin, monthDay, currentMonth, currentYear);
    }
    return false;
  }
  return true;
}
//---------------------------------------------------------------------
//Programmmenue wird neu erstellt
void rebuildMainMenu(Menu *Hauptmenu, sprinklerConfig *varConfig, int * Wasserstaende)
{
  //Allgemein
  Hauptmenu->pointArraySubMenu[indexAGM]->MenuTitle = MenuName[0];
  Hauptmenu->pointArraySubMenu[indexAGM]->deleteAllMenuPos();
  Hauptmenu->pointArraySubMenu[indexAGM]->createMenuPos(8);
  for (int i = 0; i < 8; i++)
  {
    Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[i]->Name = MenuName[i + 1];
    Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[i]->CenterOffset = 35;
    Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[i]->inputType = 0x01;
    Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[i]->pInt[0] = &Wasserstaende[i];
  }
  Hauptmenu->pointArraySubMenu[indexAGM]->pointArrayMenuPos[WaterVolTotal]->inputType = 0x00;
  //LED
  Hauptmenu->pointArraySubMenu[indexLED]->MenuTitle = MenuName[9];
  Hauptmenu->pointArraySubMenu[indexLED]->deleteAllMenuPos();
  Hauptmenu->pointArraySubMenu[indexLED]->createMenuPos(5);
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[0]->Name = MenuName[10];
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[0]->CenterOffset = 50;
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[0]->Value1 = LEDMode[varConfig->LED_Prog];
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[1]->Name = MenuName[11];
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[1]->CenterOffset = 50;
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[1]->inputType = 1;
  Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[1]->pInt[0] = (int *)&varConfig->LED_Speed;
  for (int i = 0; i < 3; i++)
  {
    Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[2 + i]->Name = LEDMode[i + 1];
    Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[2 + i]->CenterOffset = 50;
    Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[2 + i]->inputType = 1;
    Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[2 + i]->pInt[0] = (int *)&varConfig->LED_Color[i];
  }

  //Programme
  for (int i = 0; i < ProgItems; i++)
  {
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->MenuTitle = varConfig->Programm[i].ProgName;
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->deleteAllMenuPos();
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->createMenuPos(varConfig->Programm[i].ProgItemCount + 2);
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[0]->CenterOffset = 50;
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[0]->Name = MenuName[12];
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[0]->inputType = 1;
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[0]->pInt[0] = (int *)&varConfig->Programm[i].ProgDauer;
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[1]->MenuPosType = 1;
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[1]->Name = MenuName[13];
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[1]->Value1 = MenuName[14];
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[1]->Value2 = MenuName[15];
    Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[1]->Selectable = false;

    for (int j = 0; j < varConfig->Programm[i].ProgItemCount; j++)
    {
      Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[j + 2]->MenuPosType = 1;
      Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[j + 2]->inputType = 7;

      Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[j + 2]->pName = (char *)&varConfig->ChannelName[varConfig->Programm[i].Zeilen[j].channel - 1];
      Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[j + 2]->pInt[0] = (int *)&varConfig->Programm[i].Zeilen[j].delay;
      Hauptmenu->pointArraySubMenu[i + indexProgStart]->pointArrayMenuPos[j + 2]->pInt[1] = (int *)&varConfig->Programm[i].Zeilen[j].length;
    }
  }
  //Netzwerk
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->MenuTitle = MenuName[16];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->deleteAllMenuPos();
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->createMenuPos(10);
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[0]->Name = MenuName[17];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[0]->CenterOffset = 50;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[0]->Value1 = Ein_Aus[(int8_t)varConfig->WLAN_AP_Aktiv];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[1]->Name = MenuName[18];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[1]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[1]->Value1 = varConfig->WLAN_SSID;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[1]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[2]->Name = MenuName[19];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[2]->CenterOffset = 50;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[2]->Value1 = Ein_Aus[(int8_t)varConfig->NW_StatischeIP];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[3]->Name = MenuName[20];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[3]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[3]->Value1 = varConfig->NW_IPAdresse;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[3]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[4]->Name = MenuName[21];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[4]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[4]->Value1 = varConfig->NW_NetzName;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[4]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[5]->Name = MenuName[22];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[5]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[5]->Value1 = varConfig->NW_SubMask;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[5]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[6]->Name = MenuName[23];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[6]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[6]->Value1 = varConfig->NW_Gateway;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[6]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[7]->Name = MenuName[24];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[7]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[7]->Value1 = varConfig->NW_DNS;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[7]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[8]->Name = MenuName[25];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[8]->MenuPosType = 1;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[8]->Value1 = varConfig->NW_NTPServer;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[8]->Value2 = "";
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[9]->Name = MenuName[26];
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[9]->CenterOffset = 50;
  Hauptmenu->pointArraySubMenu[indexProgStart + ProgItems]->pointArrayMenuPos[9]->Value1 = IntToStr(varConfig->NW_NTPOffset);
}
void UpdateButtonName(sprinklerConfig *varConfig)
{
  Button1->Text = varConfig->Programm[0].ProgName;
  Button1->DrawObject();
  Button2->Text = varConfig->Programm[1].ProgName;
  Button2->DrawObject();
  Button3->Text = varConfig->Programm[2].ProgName;
  Button3->DrawObject();
}
//---------------------------------------------------------------------
//LED Funktionen des Infinity Spiegels
void LEDOff()
{
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
}
void LEDSmooth(int Speed)
{
  //Smooth
  if (millis() > LED_Mode_Time_Var)
  {
    LED_Mode_Time_Var = millis() + Speed;
    switch (LED_Mode_State)
    {
    case 0:
      SoftPWM_RGB[0] = 0;
      SoftPWM_RGB[1] = 0;
      SoftPWM_RGB[2] = 255;
      ledcWrite(0, SoftPWM_RGB[0]);
      ledcWrite(1, SoftPWM_RGB[1]);
      ledcWrite(2, SoftPWM_RGB[2]);
      LED_Mode_State++;
      break;
    case 1:
      if (SoftPWM_RGB[0] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]++;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 2:
      if (SoftPWM_RGB[2] == 0)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[2]--;
        ledcWrite(2, SoftPWM_RGB[2]);
      }
      break;
    case 3:
      if (SoftPWM_RGB[1] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[1]++;
        ledcWrite(1, SoftPWM_RGB[1]);
      }
      break;
    case 4:
      if (SoftPWM_RGB[0] == 0)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]--;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 5:
      if (SoftPWM_RGB[2] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[2]++;
        ledcWrite(2, SoftPWM_RGB[2]);
      }
      break;
    case 6:
      if (SoftPWM_RGB[0] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]++;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 7:
      if (SoftPWM_RGB[1] == 0)
        LED_Mode_State = 1;
      else
      {
        SoftPWM_RGB[1]--;
        SoftPWM_RGB[0]--;
        ledcWrite(1, SoftPWM_RGB[1]);
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    default:
      break;
    }
  }
}
//---------------------------------------------------------------------
//Einstellungen laden und Speichern im EEPROM bzw. Flash
void EinstSpeichern()
{
  unsigned long int Checksumme = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfig;
  for (int i = 0; i < sizeof(varConfig); i++)
    Checksumme += pointer[i];

  //EEPROM initialisieren
  EEPROM.begin(sizeof(varConfig) + 14);

  EEPROM.put(0, varConfig);
  EEPROM.put(sizeof(varConfig) + 1, Checksumme);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
void EinstLaden()
{
  sprinklerConfig varConfigTest;
  unsigned long int Checksumme = 0;
  unsigned long int ChecksummeEEPROM = 0;
  unsigned char *pointer;
  pointer = (unsigned char *)&varConfigTest;
  //EEPROM initialisieren
  unsigned int EEPROMSize;
  EEPROMSize = sizeof(varConfig) + 14;
  EEPROM.begin(EEPROMSize);

  EEPROM.get(0, varConfigTest);
  EEPROM.get(sizeof(varConfigTest) + 1, ChecksummeEEPROM);

  for (int i = 0; i < sizeof(varConfigTest); i++)
    Checksumme += pointer[i];
  if ((Checksumme == ChecksummeEEPROM) && (Checksumme != 0))
  {
    EEPROM.get(0, varConfig);
  }
  else
    Serial.println("Fehler beim Dateneinlesen");

  delay(200);
  EEPROM.end(); // Free RAM copy of structure
}
//---------------------------------------------------------------------
//Resetvariable die hochzaehlt bei vorzeitigem Stromverlust um auf Standard-Wert wieder zurueckzustellen.
void ResetVarSpeichern(char Count)
{
  EEPROM.begin(sizeof(varConfig) + 14);

  EEPROM.put(sizeof(varConfig) + 10, Count);

  EEPROM.commit(); // Only needed for ESP8266 to get data written
  EEPROM.end();    // Free RAM copy of structure
}
char ResetVarLesen()
{
  unsigned int EEPROMSize;
  char temp = 0;
  EEPROMSize = sizeof(varConfig) + 14;
  EEPROM.begin(EEPROMSize);
  EEPROM.get(EEPROMSize - 4, temp);
  delay(200);
  EEPROM.end(); // Free RAM copy of structure
  return temp;
}
//---------------------------------------------------------------------
//Touch Funktionen
float AVGTouchPort(uint8_t Port, uint32_t Count)
{
  if(!Count || Count >10)
  Count = 1;

  uint16_t arrTemp[10];  //Einzelwerte
  float AVGValue[Count];  //Durchschnittswerte
  float Summary[Count];   //Summen
  //Je nach eingabe Anzahl der Durchschnittsmessungen
  for (int j = 0; j < Count; j++)
  {
    AVGValue[j]=0;
    Summary[j]=0;
    //Zehn mal Touchwert lesen für Durchschnitt
    for (int i = 0; i<10; i++)
    {
      do
      {
        arrTemp[i] = touchRead(Port);
      }
      while (arrTemp[i] < 5); //Plausiprüfung, wenn Wert unter 5 das Lesen wiederholen.
      Summary[j] += arrTemp[i];
    }
    AVGValue[j]= Summary[j]/10; //Durchschnittsberechnung
    //Die Werte nochmals überprüfen ob alle ähnlich hoch sind.
    for (int i = 0; i<10; i++) 
    {
      while(abs(AVGValue[j] - arrTemp[i])>2) //Wenn Wert > 2 über Durchschnitt Wert austauschen gegen neuen Lesewert
      {
        Summary[j] -= arrTemp[i];
        arrTemp[i] = touchRead(Port);
        Summary[j] += arrTemp[i];
        AVGValue[j] = Summary[j]/10;
      }
    }
  }
  //Kleinsten Wert auf Summary[0]
  for(int i = 1; i < Count; i++)
  {
    if(Summary[i]< Summary[0])
      Summary[0]= Summary[i];

  }

  return Summary[0]/10;
}
//---------------------------------------------------------------------
//MQTT-Funktionen
void MQTT_callback(char* topic, byte* payload, unsigned int length)
{
  int lengthTopic = strlen(varConfig.MQTT_rootpath);
  char SubscribeVar[lengthTopic+20];
  char payloadTemp[length + 2];
  for (int i =0; i < length; i++){
    payloadTemp[i] = (char) payload[i];
  }
  payloadTemp[length] = 0;
  //Debug-Bereich
  if(DebugMode)
  {
    DebugFenster->print("MQTT-topic: ");
    DebugFenster->printnl(topic);
    DebugFenster->print("Daten: ");
    DebugFenster->printnl(payloadTemp);   
    DebugFenster->print("Daten erkannt: ");
    DebugFenster->println(SetOutputMan);   
  }
  //Ausgänge setzen
  sprintf(SubscribeVar, "%s/setOutput", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    unsigned int var = 0;
    if(sscanf(payloadTemp, "%u", &var)==1)
    {
      if(var < 256)
        SetOutputMan = var;
    }
    return;
  }
  //Ausgangsport setzen
  sprintf(SubscribeVar, "%s/setChannel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    unsigned int var = 0;
    if(sscanf(payloadTemp, "%u", &var)==1)
    {
      if(var < 8)
      {
        SetOutputMan |= 1<<var;
      }
    }
    return;
  }
  //Ausgangsport zurücksetzen
  sprintf(SubscribeVar, "%s/resetChannel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    unsigned int var = 0;
    if(sscanf(payloadTemp, "%u", &var)==1)
    {
      if(var < 8)
      {
        SetOutputMan &= ~(1<<var);
      }
    }
    return;
  }
  //LED-Mode einstellen
  sprintf(SubscribeVar, "%s/setLEDMode", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    //code
    return;
  }
  //Programm starten
  sprintf(SubscribeVar, "%s/setProg", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    unsigned int var = 0;
    Break_60s = 0;
    if(sscanf(payloadTemp, "%u", &var)==1)
    {
      switch (var)
      {
      case 1:
        Button1->SetButtonState(Switch_auto);
        ProgStart[0] = millis();
        break;
      case 2:
        Button2->SetButtonState(Switch_auto);
        ProgStart[1] = millis();
        break;
      case 3:
        Button3->SetButtonState(Switch_auto);
        ProgStart[2] = millis();
        break;
      
      default:
        break;
      }
    }    
    return;
  }
  //Programm abbrechen
  sprintf(SubscribeVar, "%s/resetProg", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    unsigned int var = 0;
    Break_60s = 0;
    if(sscanf(payloadTemp, "%u", &var)==1)
    {
      switch (var)
      {
      case 1:
        Button1->SetButtonState(Switch_off);
        ProgStart[0] = 0;
        break;
      case 2:
        Button2->SetButtonState(Switch_off);
        ProgStart[1] = 0;
        break;
      case 3:
        Button3->SetButtonState(Switch_off);
        ProgStart[2] = 0;
        break;
      
      default:
        break;
      }
    }    
    return;
  }
}
bool MQTTinit()
{
  if(MQTTclient.connected())
    MQTTclient.disconnect();
  IPAddress IPTemp;
  if(IPTemp.fromString(varConfig.MQTT_Server))
  {
    MQTTclient.setServer(IPTemp, varConfig.MQTT_Port);
  }
  else{
    MQTTclient.setServer(varConfig.MQTT_Server, varConfig.MQTT_Port);
  }
  MQTTclient.setCallback(MQTT_callback);
  unsigned long int StartTime = millis();
  while ((millis() < (StartTime + 5000)&&(!MQTTclient.connect(WiFi.macAddress().c_str(), varConfig.MQTT_Username, varConfig.MQTT_Password)))){
    delay(200);
  }
  if(MQTTclient.connected()){
    int laenge = strlen(varConfig.MQTT_rootpath);
    char SubscribeVar[laenge+15];
    sprintf(SubscribeVar, "%s/setOutput", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/setLEDMode", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/setProg", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/resetProg", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/setChannel", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/resetChannel", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    return true;
  }
  else
    return false;
}
bool MQTT_sendMessage(int MQTT_MSGType, const uint8_t* MSG, uint8_t len)
{
  int lenPath = strlen(varConfig.MQTT_rootpath);
  char strPathVar[lenPath+20];

  switch (MQTT_MSGType)
  {
  case MQTT_Error:
    sprintf(strPathVar, "%s/Error", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, false);
  case MQTT_Waterlevel:
    sprintf(strPathVar, "%s/Waterlevel", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_currentConsume:
    sprintf(strPathVar, "%s/currentConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_totalConsume:
    sprintf(strPathVar, "%s/totalConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_stateOutput:
    sprintf(strPathVar, "%s/stateOutput", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_CounterValue:
    sprintf(strPathVar, "%s/CounterValue", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  
  default:
    return false;
  }
}
bool MQTT_sendMessage(int MQTT_MSGType, int MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, uint8_t MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, uint32_t MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
bool MQTT_sendMessage(int MQTT_MSGType, float MSG)
{
  return MQTT_sendMessage(MQTT_MSGType, (const uint8_t*) IntToStr(MSG).c_str(), IntToStr(MSG).length());
}
//Initialisierung der Touchtasten
void TouchInit()
{
  for(int i = 0; i < 8; i++){
    arrTouchAVG[i] = AVGTouchPort(touchPorts[i], 5);
    if(DebugMode)
    {
      DebugFenster->print("Taste ");
      DebugFenster->print(i);
      DebugFenster->print(" = ");
      DebugFenster->println(arrTouchAVG[i]);
    }
  }
}

//---------------------------------------------------------------------
void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}
void WebserverRoot(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50 + 50)];
  char *HTMLTemp = new char[(strlen(html_Start) + 100)];
  char *HTMLString = new char[(strlen(html_header) + 50)+(strlen(html_Start) + 100)];
  //Vorbereitung Datum
  unsigned long epochTime = timeClient->getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  HTMLTemp[0] = 0;
  sprintf(Header_neu, html_header, timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WifiState, MQTTState, WifiLastChange, NanoRequests_afterLastAnsw, countMessages);
  //1. Fuellstand in L; 2. Fuellstand in Prozent; Wasserstand in mm; Fuellstand aenderung
  sprintf(HTMLTemp, html_Start, (pGeneralVar[WaterLevLiter]), (100.0/maxWaterLevelLiter*pGeneralVar[WaterLevLiter]), (pGeneralVar[WaterLev]), pGeneralVar[WaterVolTotal]);
  //Zusammenfassen der Einzelstrings
  sprintf(HTMLString, "%s%s", Header_neu, HTMLTemp);
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLTemp;
  delete[] HTMLString;
  delete[] Header_neu;
}

void WebserverLEDConfig(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50 + 50)];
  char *Body_neu = new char[(strlen(html_LED_Config) + 100)];
  char *HTMLString = new char[(strlen(html_header) + 50)+(strlen(html_LED_Config) + 100)];
  //Vorbereitung Datum
  unsigned long epochTime = timeClient->getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  // Temp Array fuer die auswahl des LED-Programms
  unsigned char TmpM[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  TmpM[varConfig.LED_Prog] = 1;
  sprintf(Header_neu, html_header, timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WifiState, MQTTState, WifiLastChange, NanoRequests_afterLastAnsw, countMessages);
  sprintf(Body_neu, html_LED_Config, varConfig.LED_Color[0], varConfig.LED_Color[1], varConfig.LED_Color[2], Un_Checked[TmpM[1]].c_str(), Un_Checked[TmpM[2]].c_str(), Un_Checked[TmpM[3]].c_str(), Un_Checked[TmpM[4]].c_str(), Un_Checked[TmpM[5]].c_str(), Un_Checked[TmpM[6]].c_str(), Un_Checked[TmpM[7]].c_str(), Un_Checked[TmpM[8]].c_str(), varConfig.LED_Speed);
  sprintf(HTMLString, "%s%s", Header_neu, Body_neu);
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLString;
  delete[] Body_neu;
  delete[] Header_neu;
}

void WebserverChannel(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50 + 50)];
  char *Body_neu = new char[((strlen(html_Prog2) + 50)*8)];
  char *HTMLString = new char[(strlen(html_header) + 50) + strlen(html_Prog1) + ((strlen(html_Prog2) + 50)*8) + strlen(html_Prog2_1)];
  //Vorbereitung Datum
  unsigned long epochTime = timeClient->getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  Body_neu[0] = 0;
  sprintf(Header_neu, html_header, timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WifiState, MQTTState, WifiLastChange, NanoRequests_afterLastAnsw, countMessages);
  for (int i = 0; i < 8; i++)
  {
    //1. Vorheriger String; 2. ProgNr; 3. ProgName
    sprintf(Body_neu, html_Prog2, Body_neu, i + 1, i + 1, &varConfig.ChannelName[i][0]);
  }
  //Zusammenfassen der Einzelstrings
  sprintf(HTMLString, "%s%s%s%s", Header_neu, html_Prog1, Body_neu, html_Prog2_1);
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLString;
  delete[] Body_neu;
  delete[] Header_neu;
}
void WebserverProg(AsyncWebServerRequest *request)
{
  char *HTMLString = new char[(strlen(html_header)+50 + 50)+(strlen(html_Prog3)+30)+((strlen(html_Prog4_1)+30)*ProgItems*countItems)+((strlen(html_Prog4)+30)*ProgItems)+(strlen(html_Prog5)+10)];
  char *HTMLTemp1 = new char[((strlen(html_Prog4_1)+30)*ProgItems*countItems)];
  //Vorbereitung Datum
  unsigned long epochTime = timeClient->getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  sprintf(HTMLString, html_header, timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WifiState, MQTTState, WifiLastChange, NanoRequests_afterLastAnsw, countMessages);
  sprintf(HTMLString, html_Prog3, HTMLString);
  for (int i = 0; i < ProgItems; i++) //ProgItems; i++)
  {
    HTMLTemp1[0] = 0;
    for (int j = 0; j < countItems; j++)
    {
      if (varConfig.Programm[i].Zeilen[j].channel > 0)
      {
        sprintf(HTMLTemp1, html_Prog4_1, HTMLTemp1, varConfig.Programm[i].Zeilen[j].delay, varConfig.Programm[i].Zeilen[j].length, varConfig.Programm[i].Zeilen[j].channel, i + 1, j + 1);
      }
      else
        break;
    }
    //1. Vprheriger String; 2. & 3. Programmnummer; 4. Programmname; 5. & 6. Programmnummer; 7. Programmdauer; 8. Programmkomponenten; 9 Programmnummer
    sprintf(HTMLString, html_Prog4, HTMLString, i + 1, i + 1, varConfig.Programm[i].ProgName, i + 1, varConfig.Programm[i].ProgDauer, HTMLTemp1, i + 1, i + 1, i + 1);
  }
  sprintf(HTMLString, html_Prog5, HTMLString);
  //Zusammenfassen der Einzelstrings
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLString;
  delete[] HTMLTemp1;
}
void WebserverSettings(AsyncWebServerRequest *request)
{
  char *Header_neu = new char[(strlen(html_header) + 50 + 50)];
  char *Body_neu = new char[(strlen(html_NWconfig)+250)];
  char *HTMLString = new char[(strlen(html_header) + 50)+(strlen(html_NWconfig)+250)];
  //Vorbereitung Datum
  unsigned long epochTime = timeClient->getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  int currentYear = ptm->tm_year + 1900;
  char *pntSelected[5];
  for (int i = 0; i < 5; i++)
    if (i == (varConfig.NW_NTPOffset + 2))
      pntSelected[i] = (char *)varSelected[1].c_str();
    else
      pntSelected[i] = (char *)varSelected[0].c_str();
  sprintf(Header_neu, html_header, timeClient->getFormattedTime().c_str(), WeekDays[timeClient->getDay()].c_str(), monthDay, currentMonth, currentYear, WifiState, MQTTState, WifiLastChange, NanoRequests_afterLastAnsw, countMessages);
  sprintf(Body_neu, html_NWconfig, Un_Checked[(short)varConfig.WLAN_AP_Aktiv].c_str(), varConfig.WLAN_SSID, Un_Checked[(short)varConfig.NW_StatischeIP].c_str(), varConfig.NW_IPAdresse, varConfig.NW_NetzName, varConfig.NW_SubMask, varConfig.NW_Gateway, varConfig.NW_DNS, varConfig.NW_NTPServer, pntSelected[0], pntSelected[1], pntSelected[2], pntSelected[3], pntSelected[4], varConfig.MQTT_Server, varConfig.MQTT_Port, varConfig.MQTT_Username, varConfig.MQTT_rootpath);
  sprintf(HTMLString, "%s%s", Header_neu, Body_neu);
  request->send_P(200, "text/html", HTMLString);
  delete[] HTMLString;
  delete[] Body_neu;
  delete[] Header_neu;
}
void WebserverDelProg(AsyncWebServerRequest *request)
{
  unsigned int varProg, varZeile;
  sscanf(request->url().c_str(), "/DelProg/%d/%d", &varProg, &varZeile);
  if ((varProg <= ProgItems) && (varZeile <= varConfig.Programm[varProg - 1].ProgItemCount))
  {
    for (int i = varZeile; i < varConfig.Programm[varProg - 1].ProgItemCount; i++)
    {
      varConfig.Programm[varProg - 1].Zeilen[i - 1] = varConfig.Programm[varProg - 1].Zeilen[i];
    }
    varConfig.Programm[varProg - 1].ProgItemCount--;
    varConfig.Programm[varProg - 1].Zeilen[varConfig.Programm[varProg - 1].ProgItemCount].channel = 0;
    request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\Prog\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\Prog\">");
  }
  else
  {
    request->send_P(200, "text/html", "Fehler! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
  }
  rebuildMainMenu(Hauptmenu, &varConfig, pGeneralVar);
  EinstSpeichern();
}
void WebserverDebugToggleWindow(AsyncWebServerRequest *request)
{
  switch(DebugMode)
  {
    case 0:
      Hauptansicht->WindowActiv = false;
      DebugFenster = new Window(320, 480);
      DebugFenster->setDebug();
      DebugMode = 1;
      request->send_P(200, "text/html", "Debug-Mode gestartet, zum beenden '/DebugEnd' als Postfix eintragen");
      break;
    case 1:
      Hauptansicht->WindowActiv = true;
      DebugFenster->WindowActiv = false;
      DebugMode = 2;
      Hauptansicht->GetFirstObject()->DrawAllObjects();
      request->send_P(200, "text/html", "Fenster umgeschaltet um zurueckzukehren nochmals '/DebugToggleWindow' eintragen");
      break;
    case 2:
      Hauptansicht->WindowActiv = false;
      DebugFenster->WindowActiv = true;
      DebugMode = 1;
      DebugFenster->drawDebug();
      request->send_P(200, "text/html", "Fenster umgeschaltet um zurueckzukehren nochmals '/DebugToggleWindow' eintragen");
      break;
    default:
      break;
  }
}
void WebserverDebugText(AsyncWebServerRequest *request)
{
  if(DebugMode)
  {
    int len = strlen(DebugFenster->getDebugText());
    char *HTMLString = new char[len + 500];
    int j = 0;
    for(int i = 0; i < len; i++)
    {
      HTMLString[j] = DebugFenster->getDebugText()[i];
      j++;
      if(DebugFenster->getDebugText()[i] == '\n')
      {
        HTMLString[j] = '<';
        HTMLString[j+1] = 'b';
        HTMLString[j+2] = 'r';
        HTMLString[j+3] = '>';
        j+=4;
      }
    }
    HTMLString[j] = 0;
//Automatische aktualisierung des Textes wurde auskommentiert
//    char *HTMLString_2 = new char[j + 100];
//    sprintf(HTMLString_2, "<HTML>\n<head>\n<meta http-equiv=\"refresh\" content=\"5\">\n</head>\n<body>\n%s\n</body>\n", HTMLString);
    request->send_P(200, "text/html", HTMLString);
    delay(200);
    delete[] HTMLString;
//    delete[] HTMLString_2;
  }
}

void WebserverLastMessages(AsyncWebServerRequest *request)
{
  char * strLastMessages = GetLastMessagesHTML();
  request->send_P(200, "text/html", strLastMessages);
  delete[] strLastMessages;
}

void WebserverDebugStart(AsyncWebServerRequest *request)
{
  if(!DebugMode)
  {
    initDebugWindow();
    request->send_P(200, "text/html", "Debug-Mode gestartet, zum beenden '/DebugEnd' als Postfix eintragen");
  }
}
void WebserverDebugEnd(AsyncWebServerRequest *request)
{
  if(DebugMode)
  {
    closeDebugWindow();
    Hauptansicht->GetFirstObject()->DrawAllObjects();
    request->send_P(200, "text/html", "Debug-Mode beendet");
  }
}
void WebserverDisplayOff(AsyncWebServerRequest *request)
{
  if(!DebugMode)
  {
    cnt_LED_Display = 0;
    digitalWrite(Display_Beleuchtung, 0);
    request->send_P(200, "text/html", "Display abgeschaltet");
  }
}
void WebserverDisplayOn(AsyncWebServerRequest *request)
{
  if(!DebugMode)
  {
    cnt_LED_Display = millis() + DisplayVerz;
    digitalWrite(Display_Beleuchtung, 1);
    request->send_P(200, "text/html", "Display angeschaltet");
  }
}
void WebserverTouchInit(AsyncWebServerRequest *request)
{
  TouchSperre = millis() + 5000;
  request->send_P(200, "text/html", "<HTML>\n<head>\n<meta http-equiv=\"refresh\" content=\"5\">\n</head>\n<body>\nTouchtasten werden neu initialisiert!\n</body>\n");
  delay(500);
  TouchInit();
}
void WebserverPOST(AsyncWebServerRequest *request)
{
  int parameter = request->params();
  unsigned short int *submitBereich;
  if (parameter)
  {
    submitBereich = (unsigned short int *)request->getParam(0)->name().c_str();
    switch (*submitBereich)
    {
    case subps:
      if (parameter != 3)
      {
        request->send_P(200, "text/html", "Anzahl Rueckgabewerte passt nicht! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
        break;
      }
      unsigned char Temp;
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name().startsWith("PS_Verz_"))
        {
          Temp = request->getParam(i)->name()[8] - 0x31;
          if (Temp < ProgItems)
            varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].delay = request->getParam(i)->value().toInt();
          if (varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].delay > 100)
            varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].delay = 100;
        }
        else if (request->getParam(i)->name().startsWith("PS_Dauer_"))
        {
          Temp = request->getParam(i)->name()[9] - 0x31;
          if (Temp < ProgItems)
            varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].length = request->getParam(i)->value().toInt();
          if ((varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].length + varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].delay) > 100)
            varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].length = 100 - varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].delay;
        }
        else if (request->getParam(i)->name().startsWith("PS_Kanal_"))
        {
          Temp = request->getParam(i)->name()[9] - 0x31;
          if ((Temp < ProgItems) && (request->getParam(i)->value().toInt() < 9))
          {
            varConfig.Programm[Temp].Zeilen[varConfig.Programm[Temp].ProgItemCount].channel = request->getParam(i)->value().toInt();
            varConfig.Programm[Temp].ProgItemCount++;
          }
          else
          {
            request->send_P(200, "text/html", "Fehler, Kanalnummer ungueltig! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
            break;
          }
        }
        else
        {
          request->send_P(200, "text/html", "Fehler! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          break;
        }
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\Prog\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\Prog\">");
      rebuildMainMenu(Hauptmenu, &varConfig, pGeneralVar);
      EinstSpeichern();
      break;
    case subpd:
      if (parameter != 2)
      {
        request->send_P(200, "text/html", "Anzahl Rueckgabewerte passt nicht! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
        break;
      }
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name().startsWith("PD_Prog_Name_"))
        {
          Temp = request->getParam(i)->name()[13] - 0x31;
          if ((Temp < ProgItems) && (request->getParam(i)->value().length() < 7))
          {
            strcpy(varConfig.Programm[Temp].ProgName, request->getParam(i)->value().c_str());
            UpdateButtonName(&varConfig);
          }
          else
          {
            request->send_P(200, "text/html", "Fehler! Vorgang abgebrochen, Name zu lang!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
            break;
          }
        }
        else if (request->getParam(i)->name().startsWith("PD_Prog_Dauer_"))
        {
          Temp = request->getParam(i)->name()[14] - 0x31;
          if ((Temp < ProgItems) && (request->getParam(i)->value().toInt() >= 10) && (request->getParam(i)->value().toInt() <= 300))
            varConfig.Programm[Temp].ProgDauer = request->getParam(i)->value().toInt();
        }
        else
        {
          request->send_P(200, "text/html", "Fehler! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          break;
        }
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\Prog\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\Prog\">");
      rebuildMainMenu(Hauptmenu, &varConfig, pGeneralVar);
      EinstSpeichern();
      break;
    case sublm:
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "LM_Prog")
        {
          unsigned int Temp = request->getParam(i)->value().toInt();
          varConfig.LED_Prog = (Temp < 9) ? Temp : 8;
          Hauptmenu->pointArraySubMenu[indexLED]->pointArrayMenuPos[0]->Value1 = LEDMode[varConfig.LED_Prog];
        }
        else if (request->getParam(i)->name() == "LM_Speed")
          varConfig.LED_Speed = request->getParam(i)->value().toInt();
        else
        {
          request->send_P(200, "text/html", "Fehler! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          break;
        }
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\LED-Config\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\LED-Config\">");
      EinstSpeichern();
      break;
    case sublf:
      if (parameter != 3)
      {
        request->send_P(200, "text/html", "Anzahl Rueckgabewerte passt nicht! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
        break;
      }
      for (int i = 0; i < parameter; i++)
      {
        unsigned int Temp = request->getParam(i)->value().toInt();
        varConfig.LED_Color[i] = (Temp < 256) ? Temp : 255;
      }
      varConfig.LED_Prog = 0;
      request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\LED-Config\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\LED-Config\">");
      EinstSpeichern();
      break;
    case subcn:
      if (parameter != 8)
      {
        request->send_P(200, "text/html", "Anzahl Rueckgabewerte passt nicht! Vorgang abgebrochen!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
        break;
      }
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->value().length() < 10)
          strcpy(&varConfig.ChannelName[i][0], request->getParam(i)->value().c_str());
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen!<br><a href=\\Channel\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"1; URL=\\Channel\">");
      EinstSpeichern();
      break;
    case subwl:
      varConfig.WLAN_AP_Aktiv = 0;
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "wlAP")
          varConfig.WLAN_AP_Aktiv = 1;
        else if (request->getParam(i)->name() == "wlSSID")
        {
          if (request->getParam(i)->value().length() < 40)
            strcpy(varConfig.WLAN_SSID, request->getParam(i)->value().c_str());
          else
          {
            request->send_P(200, "text/html", "SSID zu lang<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
            return;
          }
        }
        else if (request->getParam(i)->name() == "wlPassword")
        {
          if (request->getParam(i)->value().length() <= 60)
            strcpy(varConfig.WLAN_Password, request->getParam(i)->value().c_str());
          else
          {
            request->send_P(200, "text/html", "Passwort zu lang<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
            return;
          }
        }
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      request->send_P(200, "text/html", "Daten wurden uebernommen und ESP wird neu gestartet!<br><a href=\\Settings\\>Zurueck</a> <meta http-equiv=\"refresh\" content=\"15; URL=\\\">"); //<a href=\>Startseite</a>
      EinstSpeichern();
      ESP.restart();
      break;
    case subnw:
    {
      char tmp_StatischeIP = 0;
      String tmp_IPAdressen[4];
      String tmp_NTPServer;
      String tmp_NetzName;
      int tmp_NTPOffset;
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "nwSIP")
          tmp_StatischeIP = 1;
        else if (request->getParam(i)->name() == "nwIP")
          tmp_IPAdressen[0] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNetzName")
          tmp_NetzName = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwSubnet")
          tmp_IPAdressen[1] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwGateway")
          tmp_IPAdressen[2] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwDNS")
          tmp_IPAdressen[3] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNTPServer")
          tmp_NTPServer = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "nwNTPOffset")
          sscanf(request->getParam(i)->value().c_str(), "%d", &tmp_NTPOffset);
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      if (tmp_StatischeIP)
        if ((tmp_IPAdressen[0].length() == 0) || (tmp_IPAdressen[1].length() == 0))
        {
          request->send_P(200, "text/html", "Bei Statischer IP-Adresse wird eine IP-Adresse und eine Subnet-Mask benoetigt!<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      varConfig.NW_StatischeIP = tmp_StatischeIP;
      strcpy(varConfig.NW_IPAdresse, tmp_IPAdressen[0].c_str());
      strcpy(varConfig.NW_NetzName, tmp_NetzName.c_str());
      strcpy(varConfig.NW_SubMask, tmp_IPAdressen[1].c_str());
      strcpy(varConfig.NW_Gateway, tmp_IPAdressen[2].c_str());
      strcpy(varConfig.NW_DNS, tmp_IPAdressen[3].c_str());
      strcpy(varConfig.NW_NTPServer, tmp_NTPServer.c_str());
      varConfig.NW_NTPOffset = tmp_NTPOffset;
      request->send_P(200, "text/html", "Daten wurden uebernommen und ESP wird neu gestartet!<br><meta http-equiv=\"refresh\" content=\"10; URL=\\\">"); //<a href=\>Startseite</a>
    }
      EinstSpeichern();
      ESP.restart();
      break;
    case submq:
    {
      String Temp[6];
      for (int i = 0; i < parameter; i++)
      {
        if (request->getParam(i)->name() == "mqServer")
          Temp[0] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqPort")
          Temp[1] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqUser")
          Temp[2] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqPassword")
          Temp[3] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqRootpath")
          Temp[4] = request->getParam(i)->value();
        else if (request->getParam(i)->name() == "mqFPrint")
          Temp[5] = request->getParam(i)->value();
        else
        {
          request->send_P(200, "text/html", "Unbekannter Rueckgabewert<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
          return;
        }
      }
      if((Temp[0].length()<49)&&(Temp[0].length()>5))
        strcpy(varConfig.MQTT_Server, Temp[0].c_str());
      if((Temp[1].length()<6)&&(Temp[1].length()>1))
        varConfig.MQTT_Port = Temp[1].toInt();
      if((Temp[2].length()<19)&&(Temp[2].length()>5))
        strcpy(varConfig.MQTT_Username, Temp[2].c_str());
      if((Temp[3].length()<=60)&&(Temp[3].length()>5)&&(Temp[3]!= "xxxxxx"))
        strcpy(varConfig.MQTT_Password, Temp[3].c_str());
      if((Temp[4].length()<95)&&(Temp[4].length()>5))
        strcpy(varConfig.MQTT_rootpath, Temp[4].c_str());
      if((Temp[5].length()<=65)&&(Temp[5].length()>5)&&(Temp[5]!= "xxxxxx"))
        strcpy(varConfig.MQTT_fprint, Temp[5].c_str());
    }
      EinstSpeichern();
      if(MQTTinit())
        request->send_P(200, "text/html", "Daten wurden uebernommen, Verbindung zu MQTT-Server hergestellt!<br><meta http-equiv=\"refresh\" content=\"10; URL=\\\">"); //<a href=\>Startseite</a>
      else
        request->send_P(200, "text/html", "Daten wurden uebernommen, Verbindung zu MQTT-Server konnte nicht hergestellt werden!<br><meta http-equiv=\"refresh\" content=\"10; URL=\\\">"); //<a href=\>Startseite</a>
      break;
    
    default:
      char strFailure[50];
      sprintf(strFailure, "Anweisung unbekannt, Empfangen: %u", *submitBereich);
      request->send_P(200, "text/html", strFailure);
      break;
    }
  }
}

