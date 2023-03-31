//#include "Farben.h"
#include <Arduino.h>
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
#include "Display.h"
#include "WaterControl.h"
#include "WebFunctions.h"
#include "GlobalVariabels.h"
#include "MQTT_Functions.h"
#include "WC_Structures.h"
#include "NanoCommunication.h"
#include "WiFiFunctions.h"
#include "LEDFunctions.h"

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
  //maxWaterLevelLiter = calcWaterLiter(maxWaterLevelRoh - minWaterLevelRoh);
  pGeneralVar[WaterLevLiter] = calcWaterLiter(maxWaterLevelRoh - minWaterLevelRoh);

  Hauptmenu->MenuNavNext();
  Hauptmenu->DrawObject();
  cnt_LED_Display = millis() + DisplayVerz; //Damit das Display am Anfang eingeschaltet ist
  MQTT_sendText(MQTT_MSG_Logging, "Water control rebooted!");

}

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
              float ReadVar = AVGTouchPort(touchPorts[i], 5);
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
      Break_Touchtest = (Break_Touchtest > millis())?Break_Touchtest:(millis() + 60000);
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
      varOutput.setProgState(0, GetOutput(&varConfig.Programm[0], &ProgStart[0]));   //Setzen des Ausgangs fuer Programm 1
      //varOutput.ProgState[0] = GetOutput(&varConfig.Programm[0], &ProgStart[0]);   //Setzen des Ausgangs fuer Programm 1
      if (ProgStart[0] == 0)
        Button1->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      varOutput.setProgState(0, 0);
    }
    switch (Button2->GetButtonState())
    {
    case Switch_auto:
      varOutput.setProgState(1, GetOutput(&varConfig.Programm[1], &ProgStart[1]));   //Setzen des Ausgangs fuer Programm 2
      if (ProgStart[1] == 0)
        Button2->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      varOutput.setProgState(1, 0);
    }
    switch (Button3->GetButtonState())
    {
    case Switch_auto:
      varOutput.setProgState(2, GetOutput(&varConfig.Programm[2], &ProgStart[2]));   //Setzen des Ausgangs fuer Programm 3
      if (ProgStart[2] == 0)
        Button3->SetButtonState(Switch_off);
      break;
    case Switch_on:
      break;
    case Switch_off:
      varOutput.setProgState(2, 0);
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
  if ((Status->Status != varOutput.getOutputSummary())&&varOutput.NanoOutputRequested == 0)
  {
    //Setzen der Grafic auf den neuen Ausgang und uebertragung der Daten zu Nano
    varOutput.NanoOutputState = varOutput.getOutputSummary();
    sendtoNano(set_output, varOutput.NanoOutputState);
    varOutput.NanoOutputRequested = get_outputcheck;
    varOutput.NanoOutputRequestTimeout = millis() + varOutput.NanoOutputRequestTimebuffer;
    if(DebugMode)
    {
      DebugFenster->print("Prog1 = ");
      DebugFenster->println(varOutput.getProgState(0));
      DebugFenster->print("Prog2 = ");
      DebugFenster->println(varOutput.getProgState(1));
      DebugFenster->print("Prog3 = ");
      DebugFenster->println(varOutput.getProgState(2));
      DebugFenster->print("SetOutputMan = ");
      DebugFenster->println(varOutput.SetOutputMan);
      DebugFenster->print("Status = ");
      DebugFenster->println(Status->Status);
    }
  }
  //Wenn die Antwort von Nano fehlt
  if((varOutput.NanoOutputRequestTimeout < millis()) && varOutput.NanoOutputRequested)
  {
    //Fehlerbehandlung muss noch programmiert werden
    if(DebugMode)
    {
      DebugFenster->print("Ausgänge wurden nicht bestätigt: ");
      DebugFenster->println(varOutput.NanoOutputRequestErrors);      
    }
    MQTT_sendText(MQTT_MSG_Error, "Output states were not confirmed!");
    sendtoNano(set_output, varOutput.NanoOutputState);
    varOutput.NanoOutputRequestTimeout = millis() + varOutput.NanoOutputRequestTimebuffer + (varOutput.NanoOutputRequestErrors*1000);
    varOutput.NanoOutputRequestErrors++;
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
        varOutput.NanoOutputRequestTimeout = 0;        
        break;
      case get_outputcheck:
        if(DebugMode)
        {
          DebugFenster->printnl("Folgende Ausgaenge wurden bestaetigt.");
          DebugFenster->print("Gesendet: ");
          DebugFenster->print(varOutput.NanoOutputState);
          DebugFenster->print(" | Empfangen: ");
          DebugFenster->println(Temp);
        }
        if(varOutput.NanoOutputState == Temp)
        {
          Status->Status = varOutput.NanoOutputState;
          Status->UpdateState();
          varOutput.NanoOutputRequestErrors = 0;
          varOutput.NanoOutputRequested = 0;
        }
        else
        {
          Status->Status = Temp;
          Status->UpdateState();
          sendtoNano(set_output, varOutput.NanoOutputState);
          varOutput.NanoOutputRequested = get_outputcheck;
          varOutput.NanoOutputRequestTimeout = millis() + varOutput.NanoOutputRequestTimebuffer;
          varOutput.NanoOutputRequestErrors++;
        }
        MQTT_sendMessage(MQTT_MSG_stateOutput, (uint8_t) Temp);
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
          varOutput.setWaterlevel(calcWaterLiter(pGeneralVar[WaterLev]));
          varOutput.checkWaterlevel();
          pGeneralVar[WaterLevLiter] = varOutput.getWaterlevel(); //calcWaterLiter(pGeneralVar[WaterLev]);
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
        MQTT_sendMessage(MQTT_MSG_Waterlevel, pGeneralVar[WaterLevLiter]); //Übertragen des Wasserstands in l
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
            MQTT_sendMessage(MQTT_MSG_totalConsume, WaterVolNew); //Übertragen des Wasserverbrauchs in l
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
          MQTT_sendMessage(MQTT_MSG_currentConsume, Temp); //Übertragen des Wasserverbrauchs in ml
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
          MQTT_sendMessage(MQTT_MSG_CounterValue, Temp); 
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
