#ifndef NanoCommunication
#define NanoCommunication
#include <Arduino.h>

int sendtoNano(char Command, uint32_t Data = 0);
void SendRequests(uint32_t * Flags);
int getfromNano(uint32_t * pData);
void SaveMessage(char * newMes, bool Nano = false);



#include "NanoCommunication.cpp"
#endif