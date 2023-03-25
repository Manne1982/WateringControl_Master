#include "LEDFunctions.h"
#include "GlobalVariabels.h"

//---------------------------------------------------------------------
//LED Funktionen des Infinity Spiegels
void LEDOff()
{
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
}
void LEDSmooth(int Speed)
{
  //Smooth
  if (millis() > LED_Mode_Time_Var)
  {
    LED_Mode_Time_Var = millis() + Speed;
    switch (LED_Mode_State)
    {
    case 0:
      SoftPWM_RGB[0] = 0;
      SoftPWM_RGB[1] = 0;
      SoftPWM_RGB[2] = 255;
      ledcWrite(0, SoftPWM_RGB[0]);
      ledcWrite(1, SoftPWM_RGB[1]);
      ledcWrite(2, SoftPWM_RGB[2]);
      LED_Mode_State++;
      break;
    case 1:
      if (SoftPWM_RGB[0] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]++;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 2:
      if (SoftPWM_RGB[2] == 0)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[2]--;
        ledcWrite(2, SoftPWM_RGB[2]);
      }
      break;
    case 3:
      if (SoftPWM_RGB[1] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[1]++;
        ledcWrite(1, SoftPWM_RGB[1]);
      }
      break;
    case 4:
      if (SoftPWM_RGB[0] == 0)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]--;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 5:
      if (SoftPWM_RGB[2] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[2]++;
        ledcWrite(2, SoftPWM_RGB[2]);
      }
      break;
    case 6:
      if (SoftPWM_RGB[0] == 255)
        LED_Mode_State++;
      else
      {
        SoftPWM_RGB[0]++;
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    case 7:
      if (SoftPWM_RGB[1] == 0)
        LED_Mode_State = 1;
      else
      {
        SoftPWM_RGB[1]--;
        SoftPWM_RGB[0]--;
        ledcWrite(1, SoftPWM_RGB[1]);
        ledcWrite(0, SoftPWM_RGB[0]);
      }
      break;
    default:
      break;
    }
  }
}

