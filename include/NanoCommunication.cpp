#include "NanoCommunication.h"
#include "GlobalVariabels.h"

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
  case set_ConvVar:
    sprintf(strTemp, "C5_%u", Data);
    break;
  case get_ConvVar:
    sprintf(strTemp, "C6_%u", Data);
    break;
  case set_watervolcompl:
    sprintf(strTemp, "C4_%u", Data);
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
void SaveMessage(char * newMes, bool Nano)
{
  if((varOutput.NanoOutputRequestErrors > 5)||(NanoRequests_afterLastAnsw > 5))
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

