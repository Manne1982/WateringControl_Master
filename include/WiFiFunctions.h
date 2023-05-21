#ifndef WiFiFunctions
#define WiFiFunctions

void WiFi_Start_STA(const char *ssid_sta, const char *password_sta);
void WiFi_Start_AP(const char * ssid_AP, const char *password_AP = "");
bool WIFIConnectionCheck(bool with_reconnect);



#include "WiFiFunctions.cpp"
#endif
