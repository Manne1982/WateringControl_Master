#ifndef WiFiFunctions
#define WiFiFunctions

void WiFi_Start_STA(char *ssid_sta, char *password_sta);
void WiFi_Start_AP(const char * ssid_AP);
bool WIFIConnectionCheck(bool with_reconnect);



#include "WiFiFunctions.cpp"
#endif
