#include "MQTT_Functions.h"
#include "GlobalVariabels.h"


//MQTT-Funktionen
void MQTT_callback(char* topic, byte* payload, unsigned int length)
{
  int lengthTopic = strlen(varConfig.MQTT_rootpath);
  char SubscribeVar[lengthTopic+20];
//  char payloadTemp[length + 2];
//  for (int i =0; i < length; i++){
//    payloadTemp[i] = (char) payload[i];
//  }
//  payloadTemp[length] = 0;
  String payloadTemp = (char*) payload;
  //Debug-Bereich
  if(DebugMode)
  {
    DebugFenster->print("MQTT-topic: ");
    DebugFenster->printnl(topic);
    DebugFenster->print("Daten: ");
    DebugFenster->printnl(payloadTemp.c_str());   
    DebugFenster->print("Daten erkannt: ");
    DebugFenster->println(varOutput.SetOutputMan);   
  }
  //Ausgänge setzen
  sprintf(SubscribeVar, "%s/setOutput", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    if((payloadTemp.toInt() < 256)&&(payloadTemp.toInt()>=0))
      varOutput.SetOutputMan = payloadTemp.toInt();
    return;
  }
  //Ausgangsport setzen
  sprintf(SubscribeVar, "%s/setChannel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    if((payloadTemp.toInt() < 8)&&(payloadTemp.toInt()>=0))
    {
      varOutput.SetOutputMan |= 1<<payloadTemp.toInt();
    }
    return;
  }
  //Ausgangsport zurücksetzen
  sprintf(SubscribeVar, "%s/resetChannel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    if((payloadTemp.toInt() < 8)&&(payloadTemp.toInt()>=0))
    {
      varOutput.SetOutputMan &= ~(1<<payloadTemp.toInt());
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
    Break_60s = 0;
    if(payloadTemp.toInt())
    {
      switch (payloadTemp.toInt())
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
    Break_60s = 0;
    
    if(payloadTemp.toInt())
    {
      switch (payloadTemp.toInt())
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
  //Lower the waterlevel
  sprintf(SubscribeVar, "%s/setNewWaterlevel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {
    uint32_t OUT, LEVEL;
    Break_60s = 0;

    if(sscanf(payloadTemp.c_str(), "OUT:%u|LEVEL:%u", &OUT, &LEVEL)==2)
    {
      if((OUT < 256)&&(LEVEL < maxWaterLevelLiter))
      {
        if(varOutput.setNewWaterlevel(OUT, LEVEL))
          MQTT_sendText(MQTT_MSG_Error, "setNewWaterlevel Bedingung erfüllt!");
        else
          MQTT_sendText(MQTT_MSG_Error, "setNewWaterlevel Bedingung nicht erfüllt!");

      }
    }
    else
    {
      MQTT_sendText(MQTT_MSG_Error, "Empfangene Werte falsch!");
    }    
    return;
  }
  sprintf(SubscribeVar, "%s/resetNewWaterlevel", varConfig.MQTT_rootpath);
  if(!strcmp(topic, SubscribeVar))
  {  
    Break_60s = 0;
    varOutput.resetNewWaterlevel();
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
    sprintf(SubscribeVar, "%s/setNewWaterlevel", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    sprintf(SubscribeVar, "%s/resetNewWaterlevel", varConfig.MQTT_rootpath);
    MQTTclient.subscribe(SubscribeVar);
    return true;
  }
  else
    return false;
}
bool MQTT_sendText(int _MSGType, String Text)
{
  return MQTT_sendMessage(_MSGType, (const uint8_t *)Text.c_str(), Text.length());
}

bool MQTT_sendMessage(int MQTT_MSGType, const uint8_t* MSG, uint8_t len)
{
  int lenPath = strlen(varConfig.MQTT_rootpath);
  char strPathVar[lenPath+20];

  switch (MQTT_MSGType)
  {
  case MQTT_MSG_Error:
    sprintf(strPathVar, "%s/Error", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, false);
  case MQTT_MSG_Waterlevel:
    sprintf(strPathVar, "%s/Waterlevel", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_currentConsume:
    sprintf(strPathVar, "%s/currentConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_totalConsume:
    sprintf(strPathVar, "%s/totalConsume", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_stateOutput:
    sprintf(strPathVar, "%s/stateOutput", varConfig.MQTT_rootpath);
    return MQTTclient.publish(strPathVar, MSG, len, true);
  case MQTT_MSG_CounterValue:
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

