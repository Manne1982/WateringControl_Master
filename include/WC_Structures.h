#ifndef WC_Structures
#define WC_Structures
#include <Arduino.h>

#define countItems 10 //Anzahl der moeglichen Prorammzeilen
#define ProgItems 4 // Anzahl Programme


//Struktur fuer die Regnerprogramme
struct SprinklerItem{
  unsigned int delay = 0;
  unsigned int length = 0;
  unsigned char channel = 0;
};
struct SprinklerProgramm{
  SprinklerItem Zeilen[countItems]; 
  unsigned char ProgItemCount = 0; //Wieviel Sprinkleritems sind vorhanden
  unsigned int ProgDauer = 0;
  char ProgName[7] = "Prog";
};
struct sprinklerConfig {
  //Einstellungen NW-Einstellungen WLAN
  char WLAN_AP_Aktiv = 1;
  char WLAN_SSID[40] = "Regner";
  char WLAN_Password[70] = "";
  //Einstellungen NW-Einstellungen MQTT
  char MQTT_Server[50] = "192.168.178.2";
  uint16_t MQTT_Port = 1883;
  char MQTT_Username[20] = "Benutzer";
  char MQTT_Password[70] = "123456";
  char MQTT_fprint[70] = "";
  char MQTT_rootpath[100] = "/Garten/Beregnung";
  //Einstellungen NW-Einstellungen Netzwerk
  char NW_StatischeIP = 0;
  char NW_NetzName[20] = "Regner";
  char NW_IPAdresse[17] = "192.168.178.10";
  char NW_SubMask[17] = "255.255.255.0";
  char NW_Gateway[17] = "192.168.178.1";
  char NW_DNS[17] = "192.168.178.1";
  char NW_NTPServer[55] = "fritz.box";
  char NW_NTPOffset = 0;
  //Regner
  unsigned int Max_Liter = 8943;
  SprinklerProgramm Programm[ProgItems];
  char ChannelName[8][15]={"Kanal_1", "Kanal_2", "Kanal_3", "Kanal_4", "Kanal_5", "Kanal_6", "Kanal_7", "Kanal_8"};
  //LED-Einstellungen
  unsigned char LED_Prog = 0;
  unsigned int LED_Color[3] = {0, 255, 0};
  unsigned int LED_Speed = 125;
};
class outputVariables{
  public:
    outputVariables();
    ~outputVariables();
    uint8_t getOutputSummary();
    void setProgState(uint8_t Prog, uint8_t newState);
    uint8_t getProgState(uint8_t Prog);
    int setNewWaterlevel(uint8_t Prog, uint16_t newWaterlevel);
    void resetNewWaterlevel();
    void checkWaterlevel();
    void setWaterlevel(uint16_t newWaterlevel);
    uint16_t getWaterlevel();
    void resetSetWaterlevelLock();
    uint8_t SetOutputMan;                                  //Variable um einen Ausgang manuell zu setzen
    uint8_t NanoOutputState;                               //Relais sollstatus, nach Bestätigung von Nano wird Status übernommen
    unsigned long NanoOutputRequestTimeout;                           //Zeitstempel wann die Anfrage ueberfaellig ist
    unsigned int NanoOutputRequestTimebuffer;                      //Zeit in ms wann die Antwort angekommen sein muss
    uint16_t NanoOutputRequestErrors;                            //Anzahl von nicht Empfangenen Bestätigungen oder falschen Bestätigungen; Wird nach Empfang einer richtigen Nachricht zurückgesetzt
    uint8_t NanoOutputRequested;                           //Angabe ob eine Ausgangsabfrage offen ist und von Nano erwartet wird 0 = keine
  private:
    uint8_t ProgState[ProgItems];                              //Variablenarray zum speichern zum aktuellen Ausgangsdaten der Programme
    uint8_t SetWaterlevel_Output;                          //Variable um Ausgänge zu setzen um einen bestimmten Wasserstand zu erreichen
    unsigned long SetWaterlevel_StartTime;                 //Variable wann Wasserstandabsenkung gestartet hat
    unsigned long SetWaterlevel_MaxTime_Without_Change;  //Max 10 Minuten (600000 ms) ohne Wasserstandsänderung erlaubt
    uint16_t SetWaterlevel_TargetLevel;                //Wasserstand, der bei Wasserstandsänderung erreicht oder unterschritten werden muss 
    uint16_t SetWaterlevel_StartLevel;                //Wasserstand, der bei Beginn der Wasserstandsänderung bestand 
    bool SetWaterlevelLock;
    uint16_t WaterlevelLiter;
};
#include "WC_Structures.cpp"
#endif
