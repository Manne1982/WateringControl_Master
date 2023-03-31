#include "WebFunctions.h"
#include "GlobalVariabels.h"
#include "WaterControl.h"
#include "MQTT_Functions.h"
#include "HTML_Var.h"


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
  sprintf(HTMLTemp, html_Start, (pGeneralVar[WaterLevLiter]), ((100.0 * pGeneralVar[WaterLevLiter])/maxWaterLevelLiter), (pGeneralVar[WaterLev]), pGeneralVar[WaterVolTotal]);
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
      request->send_P(200, "text/html", "Debug-Mode gestartet, zum beenden '/DebugEnd' als Postfix eintragen<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
      break;
    case 1:
      Hauptansicht->WindowActiv = true;
      DebugFenster->WindowActiv = false;
      DebugMode = 2;
      Hauptansicht->GetFirstObject()->DrawAllObjects();
      request->send_P(200, "text/html", "Fenster umgeschaltet um zurueckzukehren nochmals '/DebugToggleWindow' eintragen<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
      break;
    case 2:
      Hauptansicht->WindowActiv = false;
      DebugFenster->WindowActiv = true;
      DebugMode = 1;
      DebugFenster->drawDebug();
      request->send_P(200, "text/html", "Fenster umgeschaltet um zurueckzukehren nochmals '/DebugToggleWindow' eintragen<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
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
  else
  {
    request->send_P(200, "text/html", "Debug-Mode nicht gestartet!<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
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
    request->send_P(200, "text/html", "Debug-Mode gestartet, zum beenden '/DebugEnd' als Postfix eintragen<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
  else
  {
    request->send_P(200, "text/html", "Debug-Mode bereits gestartet!<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
}
void WebserverDebugEnd(AsyncWebServerRequest *request)
{
  if(DebugMode)
  {
    closeDebugWindow();
    Hauptansicht->GetFirstObject()->DrawAllObjects();
    request->send_P(200, "text/html", "Debug-Mode beendet<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
  else
  {
    request->send_P(200, "text/html", "Debug-Mode nicht gestartet!<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
}
void WebserverDisplayOff(AsyncWebServerRequest *request)
{
  if(!DebugMode)
  {
    cnt_LED_Display = 0;
    digitalWrite(Display_Beleuchtung, 0);
    request->send_P(200, "text/html", "Display abgeschaltet<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
}
void WebserverDisplayOn(AsyncWebServerRequest *request)
{
  if(!DebugMode)
  {
    cnt_LED_Display = millis() + DisplayVerz;
    digitalWrite(Display_Beleuchtung, 1);
    request->send_P(200, "text/html", "Display angeschaltet<br><meta http-equiv=\"refresh\" content=\"2; URL=/\">");
  }
}
void WebserverTouchInit(AsyncWebServerRequest *request)
{
  TouchSperre = millis() + 5000;
  request->send_P(200, "text/html", "<HTML>\n<head>\n<meta http-equiv=\"refresh\" content=\"2 URL=/\">\n</head>\n<body>\nTouchtasten werden neu initialisiert!\n</body>\n");
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
          if(request->getParam(i)->value()!="xxxxxx")
          {
            if ((request->getParam(i)->value().length() <= 60)&&(request->getParam(i)->value().length() >= 8))
            { 
              strcpy(varConfig.WLAN_Password, request->getParam(i)->value().c_str());
            }
            else
            {
              request->send_P(200, "text/html", "Passwortlaenge muss zwischen 8 und 60 Zeichen liegen<form> <input type=\"button\" value=\"Go back!\" onclick=\"history.back()\"></form>");
              return;
            }
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