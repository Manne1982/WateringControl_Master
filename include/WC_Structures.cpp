#include "WC_Structures.h"

outputVariables::outputVariables():
SetOutputMan(0),                                  //Variable um einen Ausgang manuell zu setze
NanoOutputState(0),                               //Relais sollstatus, nach Bestätigung von Nano wird Status übernommen
NanoOutputRequestTimeout(0),                           //Zeitstempel wann die Anfrage ueberfaellig ist
NanoOutputRequestTimebuffer(1500),                      //Zeit in ms wann die Antwort angekommen sein muss
NanoOutputRequestErrors(0),                            //Anzahl von nicht Empfangenen Bestätigungen oder falschen Bestätigungen; Wird nach Empfang einer richtigen Nachricht zurückgesetzt
NanoOutputRequested(0),                           //Angabe ob eine Ausgangsabfrage offen ist und von Nano erwartet wird 0 = keine
SetWaterlevel_Output(0),                          //Variable um Ausgänge zu setzen um einen bestimmten Wasserstand zu erreichen
SetWaterlevel_StartTime(0),                 //Variable wann Wasserstandabsenkung gestartet hat
SetWaterlevel_MaxTime_Without_Change(600000),  //Max 10 Minuten (600000 ms) ohne Wasserstandsänderung erlaubt
SetWaterlevel_TargetLevel(10000),                //Wasserstand, der bei Wasserstandsänderung erreicht oder unterschritten werden muss 
SetWaterlevel_StartLevel(10000),                //Wasserstand, der bei Beginn der Wasserstandsänderung bestand 
SetWaterlevelLock(false),
WaterlevelLiter(0)
{
    for(int i=0; i < ProgItems; i++)
        ProgState[i] = 0;
}
outputVariables::~outputVariables()
{}

uint8_t outputVariables::getOutputSummary()
{
    uint8_t Temp = 0;
    for(int i = 0; i < ProgItems; i ++)
        Temp |= ProgState[i];

    return (Temp | SetOutputMan | SetWaterlevel_Output);
}

void outputVariables::setProgState(uint8_t Prog, uint8_t newState)
{
    if(Prog < ProgItems)
    {
        ProgState[Prog] = newState;
    }
}
uint8_t outputVariables::getProgState(uint8_t Prog)
{
    if(Prog < ProgItems)
        return ProgState[Prog];

    return 0;
}
int outputVariables::setNewWaterlevel(uint8_t Prog, uint16_t newWaterlevel)
{
    if((newWaterlevel < WaterlevelLiter) && Prog && !SetWaterlevelLock)
    {
        SetWaterlevel_StartTime = millis();
        SetWaterlevel_StartLevel = WaterlevelLiter;
        SetWaterlevel_TargetLevel = newWaterlevel;
        SetWaterlevel_Output = Prog;
        return 1;
    }
    return 0;
}
void outputVariables::resetNewWaterlevel()
{
    SetWaterlevel_StartTime = 0;
    SetWaterlevel_StartLevel = 0;
    SetWaterlevel_TargetLevel = 0;
    SetWaterlevel_Output = 0;
}
void outputVariables::checkWaterlevel()
{
    if((SetWaterlevel_Output&&(WaterlevelLiter <= SetWaterlevel_TargetLevel)))
    {
        SetWaterlevel_StartTime = 0;
        SetWaterlevel_StartLevel = 0;
        SetWaterlevel_TargetLevel = 0;
        SetWaterlevel_Output = 0;
    }
    else
    if(SetWaterlevel_Output && (millis()>= (SetWaterlevel_StartTime + SetWaterlevel_MaxTime_Without_Change))&&(SetWaterlevel_StartLevel == WaterlevelLiter))
    {
        SetWaterlevel_StartTime = 0;
        SetWaterlevel_StartLevel = 0;
        SetWaterlevel_TargetLevel = 0;
        SetWaterlevel_Output = 0;
        SetWaterlevelLock = true;
    }
}
void outputVariables::resetSetWaterlevelLock()
{
    SetWaterlevelLock = false;
}
void outputVariables::setWaterlevel(uint16_t newWaterlevel)
{
    WaterlevelLiter = newWaterlevel;
}
uint16_t outputVariables::getWaterlevel()
{
    return WaterlevelLiter;
}