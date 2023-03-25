
#include "WaterControl.h"
#include "GlobalVariabels.h"


//---------------------------------------------------------------------
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

//Programmmenue für Display wird neu erstellt
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
//---------------------------------------------------------------------
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
