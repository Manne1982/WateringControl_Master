#ifndef WebFunctions
#define WebFunctions
#include "ESPAsyncWebServer.h"

void notFound(AsyncWebServerRequest *request);
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

char * GetLastMessagesHTML();


#include "WebFunctions.cpp"
#endif