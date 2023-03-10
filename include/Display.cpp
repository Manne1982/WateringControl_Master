

#include "Display.h"

TFT_eSPI Window::tft = TFT_eSPI();

Window::Window(uint16_t height, uint16_t width):
WindowActiv(true),
d_height(height),
d_width(width),
FirstObject(0),
DebugWindow(false)
{ 
    tft.init();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 10);
    DebugText = new char[2];
    DebugText[0] = 0;
    DebugText[1] = 0;
}
Window::~Window()
{
    delete[] DebugText;
    if(FirstObject)
    {
        while(FirstObject->GetOCount() > 1)
            delete FirstObject->GetLastObject();

            delete FirstObject;
    }
}
void Window::SetFirstObject(Object * _Set)
{FirstObject = _Set;}

void Window::printnl(const char Text[])
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.print(Text);
        tft.print("\n");
    }
    int newLen = strlen(Text) + strlen(DebugText) + 2;
    char strtemp[newLen];
    sprintf(strtemp, "%s%s\n", DebugText, Text);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::print(const char Text[])
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.print(Text);
    }
    int newLen = strlen(Text) + strlen(DebugText) + 2;
    char strtemp[newLen];
    sprintf(strtemp, "%s%s", DebugText, Text);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::print(int Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%d", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%d", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::print(uint32_t Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%u", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%d", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::print(float Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%f", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%f", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::print(double Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%f", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%f", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::println(int Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%d\n", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%d\n", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::println(uint32_t Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%u\n", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%u\n", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::println(float Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%f\n", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%f\n", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::println(double Zahl)
{ 
    if(!DebugWindow)
        return;
    if(WindowActiv)
    {
        if(tft.getCursorY() > (d_height-10))
        {
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0,10);
        }
        tft.setTextSize(1);
        tft.printf("%f\n", Zahl);
    }
    int newLen = strlen(DebugText) + 15;
    int Index = 0;
    if(newLen > 400)
    {
        Index = newLen - 400;
        newLen = 401;
    }
    char strtemp[newLen];
    sprintf(strtemp, "%s%f\n", &DebugText[Index], Zahl);
    delete[] DebugText;
    DebugText = new char[newLen];
    sprintf(DebugText, "%s", strtemp);
}
void Window::setDebug()
{
    if(FirstObject)
        throw;
    DebugWindow = true;
}
void Window::drawDebug()
{
    if(WindowActiv && DebugWindow)
    {
        tft.setTextSize(1);
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 10);
        tft.print(DebugText);
        char * TextPos = DebugText;
        while(tft.getCursorY() > (d_height - 10))
        {
            TextPos = strchr(TextPos, '\n') + 1;
            tft.fillScreen(TFT_BLACK);
            tft.setCursor(0, 10);
            if(TextPos)
                tft.print(TextPos);
            else
                tft.print("Fehler bei der Seitenanpassung");
        }
        if(TextPos)
            sprintf(DebugText, "%s", TextPos);
    }
}
char * Window::getDebugText()
{
    return DebugText;
}
Object * Window::GetFirstObject() const
{return FirstObject;}

uint16_t Object::o_count = 0;
Object * Object::GetObject(uint16_t arrPos)
{
    if((arrPos==0)||(arrPos > o_count))
    return 0;   
    
    if(arrPos == 1)
    return this;
    else
    return o_after->GetObject(arrPos - 1);
    
}
Object * Object::GetLastObject()
{
    return GetObject(o_count);
}    
void Object::DrawAllObjects()
{
    DrawObject();
    if(o_after)
        o_after->DrawAllObjects();
}    
    

uint16_t Object::GetOCount() const
{return o_count;}

Object::Object(Window * _Fenster):
visible(true),
varWindow(_Fenster),
o_after(0),
o_height(0),
o_width(0),
posX(0),
posY(0)
{
  if (o_count)
  {
      o_before = _Fenster->GetFirstObject()->GetLastObject(); 
      o_before->o_after = this; 
  }
  else
  {
      _Fenster->SetFirstObject(this);
  }
  
  o_count++;
}
Object::Object(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _height, uint16_t _width):
visible(true),
varWindow(_Fenster),
o_after(0),
o_height(_height),
o_width(_width),
posX(_posX),
posY(_posY)
{
  if (o_count)
  {
      o_before = _Fenster->GetFirstObject()->GetLastObject(); 
      o_before->o_after = this; 
  }
  else
  {
      _Fenster->SetFirstObject(this);
  }
  o_count++;
}
Object::~Object()
{
    if (o_before == nullptr)
        varWindow->SetFirstObject(o_after);
    else
        o_before->o_after = o_after;

    if (o_after)
        o_after->o_before = o_before;

    o_count--;
}

Button::Button(Window * _Fenster):
Object(_Fenster),
Schriftgroesse(1),
Button_State(Switch_off),
possibleStates(2),
ToggleTime(0),
ToggleState(0)
{
    o_height=100;
    o_width=100;
    Text = "Button";
    b_Color = TFT_DARKGREY;
    Text_Color = TFT_WHITE;
    Schriftgrat = 2;
}

Button::Button(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeW, uint16_t _sizeH):
Object(_Fenster, _posX, _posY, _sizeH, _sizeW),
Schriftgroesse(1),
Button_State(Switch_off),
possibleStates(2),
ToggleTime(0),
ToggleState(0)
{
    Text = "Button";
    b_Color = TFT_DARKGREY;
    Text_Color = TFT_WHITE;
    Schriftgrat = 2;
}
void Button::PushButton()
{
    switch (Button_State)
    {
    case Switch_off:
        if(possibleStates == 3)
        Button_State = Switch_auto;
        else
        Button_State = Switch_on;
        break;
    case Switch_on:
        Button_State = Switch_off;
        break;    
    case Switch_auto:
        Button_State = Switch_on;
        break;    
    default:
        break;
    }
    UpdateState(1);
}
void Button::SetButtonState(uint8_t newState)
{
    if(newState < possibleStates)
    Button_State = newState;
    UpdateState(1);
}
void Button::SetPossibleStates(uint8_t newValue)
{
    if(newValue <= 3)
    {
        possibleStates = newValue;
        if(Button_State >= possibleStates)
        {
            Button_State = 0;
            DrawObject();
        }
    }
}
void Button::UpdateState(uint8_t _all = 0){
    int Stufen = -8;
    if(!visible || !varWindow->WindowActiv)
    return;
    if(!_all &&(Button_State != 2))
    return;
    switch(Button_State)
    {
        default:
        case Switch_off:
            ToggleTime = 0;
            Stufen = -8;
            break;
        case Switch_on:
            ToggleTime = 0;
            Stufen = 8;
            break;
        case Switch_auto:
            if ((ToggleTime)&&(ToggleTime < millis()))
            {
                ToggleState++;
                if(ToggleState == 2)
                {
                    ToggleState = 0;
                    Stufen = -8;
                    ToggleTime = millis() + 500;
                }
                else
                {
                    Stufen = 8;
                    ToggleTime = millis() + 500;
                }
            }
            else
            {
                if(ToggleTime)
                return;
                else
                ToggleTime = millis() + 500;
            }
            
            break;
    }
    //Zeichne Button + Umriss
    uint16_t TempColor = ChangeBrightness(b_Color, Stufen);
    const uint16_t j = 10;
    for (int i = j; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, TempColor);
        TempColor = ChangeBrightness(TempColor, Stufen);
    }
    varWindow->tft.drawRect(posX + j, posY + j, o_width - 2*j, o_height - 2*j, TempColor);
    varWindow->tft.drawRect(posX + j-1, posY + j-1, o_width - (2*j)+2, o_height - (2*j)+2, TempColor);
    varWindow->tft.drawLine(posX, posY, posX + j, posY + j, TempColor);
    varWindow->tft.drawLine(posX + o_width, posY, posX + o_width - j, posY + j, TempColor);
    varWindow->tft.drawLine(posX, posY + o_height, posX + j, posY + o_height - j, TempColor);
    varWindow->tft.drawLine(posX + o_width, posY + o_height, posX + o_width - j, posY + o_height - j, TempColor);

}
void Button::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;
    varWindow->tft.fillRect(posX, posY, o_width, o_height, b_Color);
    UpdateState(1);
    //Text schreiben
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.setTextColor(Text_Color);
    varWindow->tft.drawCentreString(Text, posX + (o_width / 2), posY + (o_height / 2) - 5, Schriftgrat);
}
Level::Level(Window * _Fenster):
Object(_Fenster),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
Color_Level_Back(TFT_LIGHTGREY),
Color_Level_Front(TFT_BLUE),
Level_min(0),
Level_max(100),
Level_ist(80)
{
    o_height=300;
    o_width=100;
    Text = "Button";
}
Level::Level(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH):
Object(_Fenster, _posX, _posY, _sizeH, 100),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
Color_Level_Back(TFT_LIGHTGREY),
Color_Level_Front(TFT_BLUE),
Level_min(0),
Level_max(100),
Level_ist(80)
{
    Text = "Level";
}
void Level::SetLevelMin(uint16_t _min)
{
    Level_min = _min;

    if(Level_ist < Level_min)
    Level_ist = Level_min;

    if(Level_max < Level_min)
    Level_max = Level_min+1;
}
void Level::SetLevelMax(uint16_t _max)
{
    Level_max = _max;

    if(Level_ist > Level_max)
    Level_ist = Level_max;

    if((Level_max < Level_min)&&(_max > 0))
    Level_min = Level_max-1;
}
void Level::SetLevelIst(uint16_t _ist)
{
    if((_ist <= Level_max)&&(_ist >= Level_min)&&(_ist != Level_ist))
    {
        Level_ist = _ist;
        DrawObject();
    }
}
void Level::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;

    //Zeichne Button + Umriss
    uint16_t BorderColor = ChangeBrightness(Color_Background, -32);
    uint16_t Color_Level_Front_dark = ChangeBrightness(Color_Level_Front, -48);
    uint16_t PosY_Full = posY + 55;
    uint16_t PosY_Empty = posY + o_height -25;
    uint16_t PosY_Is = PosY_Empty - ((float)(PosY_Empty-PosY_Full)/(Level_max-Level_min)*(Level_ist-Level_min));
    char Temp[10];
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.fillRect(posX, posY, o_width, o_height, Color_Background);
    for (int i = 3; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, BorderColor);
    }
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.drawCentreString(Text, posX + (o_width / 2), posY + 8, Schriftgrat);
    //Zeichne Pegelgrafik
    varWindow->tft.fillRoundRect(posX + 60, posY + 40, 30, o_height - 50, 15, Color_Level_Back);
    varWindow->tft.fillRoundRect(posX + 60, PosY_Empty - 15, 30, 30, 15, Color_Level_Front);
    varWindow->tft.fillRect(posX + 60, PosY_Full, 30, PosY_Empty - PosY_Full, Color_Level_Back);
    varWindow->tft.fillRect(posX + 60, PosY_Is, 30, PosY_Empty - PosY_Is, Color_Level_Front);
    varWindow->tft.drawRoundRect(posX + 60, posY + 40, 30, o_height - 50, 15, TFT_BLACK);
    varWindow->tft.drawLine(posX + 55, PosY_Full, posX + 90, PosY_Full, TFT_BLACK);
    varWindow->tft.drawLine(posX + 55, PosY_Empty, posX + 90, PosY_Empty, TFT_BLACK);
    varWindow->tft.drawLine(posX + 55, PosY_Full + (PosY_Empty-PosY_Full)/2, posX + 90, PosY_Full + (PosY_Empty-PosY_Full)/2, TFT_BLACK);
    varWindow->tft.setTextColor(TFT_BLACK);
    sprintf(Temp,"%d",Level_max);
    varWindow->tft.drawRightString(Temp, posX + 53,PosY_Full, Schriftgrat);
    sprintf(Temp,"%d",Level_min);
    varWindow->tft.drawRightString(Temp, posX + 53,PosY_Empty, Schriftgrat);
    sprintf(Temp,"%d",(Level_min+Level_max)/2);
    varWindow->tft.drawRightString(Temp, posX + 53, PosY_Full + (PosY_Empty-PosY_Full)/2, Schriftgrat);

    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.setTextFont(2);
    sprintf(Temp,"%d",Level_ist);
    varWindow->tft.drawRightString(Temp, posX + 53, PosY_Is, Schriftgrat);
    
    
    for(int i=0;i<4;i++)
    {
        varWindow->tft.drawLine(posX + 55, PosY_Is - 2 + i, posX + 90, PosY_Is - 2 + i, Color_Level_Front_dark);
    }
}

Bar::Bar(Window * _Fenster):
Object(_Fenster),
Text("Uhrzeit"),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
Color_On(TFT_GREEN),
Color_Off(TFT_RED),
Status(0x00)
{
    o_height=32;
    o_width=480;
}

Bar::Bar(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH, uint16_t _sizeW):
Object(_Fenster, _posX, _posY, _sizeH, _sizeW),
Text("Uhrzeit"),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
Color_On(TFT_GREEN),
Color_Off(TFT_RED),
Status(0x00)
{
}
void DrawStateCircle(Window * _window, uint16_t _color, uint16_t _posX, uint16_t _posY, uint16_t _radius)
{
    _window->tft.fillCircle(_posX, _posY, _radius, _color);
    _window->tft.drawCircle(_posX, _posY, _radius, TFT_BLACK);
}
void Bar::UpdateState()
{
    if(!visible || !varWindow->WindowActiv)
    return;
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.fillRect(posX+4, posY+4, o_width - 255, o_height-8, Color_Background);
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.setCursor(posX + 10, posY + o_height/2 - 8*Schriftgroesse);
    varWindow->tft.print(Text);
    for (int i = 0; i < 8; i++)
    {
        uint16_t Color = Color_Off;
        if (Status & (1 << i))
        Color = Color_On;

        DrawStateCircle(varWindow, Color, posX + o_width - 20 - (i * 30), posY + (o_height/2), 10);    
    }
}
void Bar::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;
    //Zeichne Button + Umriss
    uint16_t BorderColor = ChangeBrightness(Color_Background, -32);
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.fillRect(posX, posY, o_width, o_height, Color_Background);
    for (int i = 3; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, BorderColor);
    }
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.setCursor(posX + 10, posY + o_height/2 - 8*Schriftgroesse);
    varWindow->tft.print(Text);
 //   varWindow->tft.drawCentreString(Text, posX + (o_width / 2), o_height - 5, Schriftgrat);
    for (int i = 0; i < 8; i++)
    {
        uint16_t Color = Color_Off;
        if (Status & (1 << i))
        Color = Color_On;

        DrawStateCircle(varWindow, Color, posX + o_width - 20 - (i * 30), posY + (o_height/2), 10);    
    }
    
}

MenuButton::MenuButton(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH):
Object(_Fenster, _posX, _posY , _sizeH, 20),
Button_State(Switch_off)
{
    b_Color = TFT_DARKGREY;
}
MenuButton::MenuButton(Window * _Fenster):
Object(_Fenster),
Button_State(Switch_off)
{
    o_height=100;
    o_width=20;
    b_Color = TFT_DARKGREY;
}
void MenuButton::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;

    varWindow->tft.fillRect(posX, posY, o_width, o_height, b_Color);
    int Stufen = -8;
    if (!Button_State)
        Stufen = -8;
    else
        Stufen = 8;

    //Zeichne Button + Umriss
    uint16_t TempColor = ChangeBrightness(b_Color, Stufen);
    const uint16_t j = 7;
    for (int i = j; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, TempColor);
        TempColor = ChangeBrightness(TempColor, Stufen);
    }
    varWindow->tft.drawRect(posX + j, posY + j, o_width - 2*j, o_height - 2*j, TempColor);
    varWindow->tft.drawRect(posX + j-1, posY + j-1, o_width - (2*j)+2, o_height - (2*j)+2, TempColor);
    varWindow->tft.drawLine(posX, posY, posX + j, posY + j, TempColor);
    varWindow->tft.drawLine(posX + o_width, posY, posX + o_width - j, posY + j, TempColor);
    varWindow->tft.drawLine(posX, posY + o_height, posX + j, posY + o_height - j, TempColor);
    varWindow->tft.drawLine(posX + o_width, posY + o_height, posX + o_width - j, posY + o_height - j, TempColor);
}
void MenuButton::SetButtonState(uint8_t newState)
{
    if(newState < 2)
    Button_State = newState;
    DrawObject();
}

Menu::Menu(Window * _Fenster):
Object(_Fenster),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
countSubMenu(0),
activMenu(0),
activLayer(0),
Focus(false)
{
    o_height=320-32;  //Gesamthöhe minus die Infobar
    o_width=480-100-72; //Gesamtbreite minus Füllstandsanzeige und Buttons
    posY = 32;
    posX = 72;
}
Menu::Menu(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH, uint16_t _sizeW):
Object(_Fenster, _posX, _posY, _sizeH, _sizeW),
Schriftgrat(2),
Schriftgroesse(1),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
countSubMenu(0),
activMenu(0),
activLayer(0),
Focus(false)
{
}
void Menu::CreateMenus(uint8_t _count = 5)
{
    if(!_count)
    return;
    if((_count + countSubMenu) > 10)
    _count = 10 - countSubMenu;
    uint16_t ButtonSize = (o_height - 6)/(countSubMenu + _count);
    for(int i = 0; i < _count + countSubMenu; i++)
    {
        if(i<countSubMenu)
        {
            delete pointArrayMenuButton[i];
            delete pointArraySubMenu[i];
        }
        pointArrayMenuButton[i]= new MenuButton(varWindow, posX+3, posY+3 + ((countSubMenu + i)*ButtonSize), ButtonSize);
        pointArraySubMenu[i]= new SubMenu(varWindow, posX + 23, posY +3, o_height - 6, o_width - 26);
        pointArraySubMenu[i]->visible = false;
        pointArraySubMenu[i]->MenuTitle = "Submenu" + IntToStr(i);
    } 
    pointArraySubMenu[activMenu]->visible = true;
    countSubMenu = _count + countSubMenu;
}
void Menu::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;
    //Zeichne Umriss
    uint16_t BorderColor = ChangeBrightness(Color_Background, -32);    
    varWindow->tft.fillRect(posX, posY, o_width, o_height, Color_Background);
    for (int i = 3; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, BorderColor);
    }
    if(countSubMenu)
    for (int i = 0; i < countSubMenu; i++)
    {
        if(i == activMenu){
            pointArrayMenuButton[i]->SetButtonState(Switch_on);
            pointArraySubMenu[i]->DrawObject();
        }
        pointArrayMenuButton[i]->DrawObject();
    }
}
void Menu::setActivSubMenu(uint8_t _SubMenu)
{
    if(_SubMenu < countSubMenu)
    {
        pointArraySubMenu[activMenu]->visible = false;
        pointArraySubMenu[activMenu]->Focus = false;
        pointArrayMenuButton[activMenu]->SetButtonState(Switch_off);
        activMenu = _SubMenu;
        pointArraySubMenu[activMenu]->visible = true;
        pointArraySubMenu[activMenu]->Focus = true;
        pointArrayMenuButton[activMenu]->SetButtonState(Switch_on);
    }
}
SubMenu * Menu::getActivSubMenu()
{
    if(activMenu >= 0)
    return pointArraySubMenu[activMenu];

    return 0;
}
void Menu::MenuNavUp()
{
    switch (activLayer)
    {
    case 0:
        if(activMenu)
            setActivSubMenu(activMenu-1);
        else
            setActivSubMenu(countSubMenu-1);

        DrawObject();
        break;
    case 1:
        for(int i = 1; i<=getActivSubMenu()->getCountMenuPos();i++)
        {
            int var = (getActivSubMenu()->getCountMenuPos()+getActivSubMenu()->getActivPosInt()-i)%getActivSubMenu()->getCountMenuPos();
            if(getActivSubMenu()->pointArrayMenuPos[var]->Selectable)
            {
                getActivSubMenu()->setActivPos(var);
                i = getActivSubMenu()->getCountMenuPos()+1;
            }
        }
        break;
    case 2: //Muss noch realisiert werden
        break;
    
    default:
        break;
    }
    
}
void Menu::MenuNavDown()
{
    switch (activLayer)
    {
    case 0:
        if(activMenu == countSubMenu-1)
            setActivSubMenu(0);
        else
            setActivSubMenu(activMenu+1);

        DrawObject();
        break;
    case 1:
        if(getActivSubMenu()->getActivPosInt()==(getActivSubMenu()->getCountMenuPos()-1))
            getActivSubMenu()->setActivPos(0);//Wenn der oberste Menüpunkt aktiv ist, fange unten wieder an
        else
            getActivSubMenu()->setActivPos(getActivSubMenu()->getActivPosInt()+1);//setze die vorherige Position aktiv
        
        break;
    case 2: //Muss noch realisiert werden
        break;
    
    default:
        break;
    }
}
void Menu::MenuNavNext()
{
    switch (activLayer)
    {
    case 0:
        getActivSubMenu()->setActivPos(0);
        activLayer = 1;
        break;
    case 1: //Muss noch realisiert werden
        break;
    case 2: //Muss noch realisiert werden
        break;
    
    default:
        break;
    }
    DrawObject();
}
void Menu::MenuNavRev()
{
    switch (activLayer)
    {
    case 1:
        getActivSubMenu()->setActivPos(-1);
        activLayer = 0;
        break;
    case 2: //Muss noch realisiert werden
        break;
    
    default:
        break;
    }
}
void Menu::UpdateMenuPos(uint8_t SubM, uint8_t Pos)
{
    if((SubM == activMenu)&&(SubM < countSubMenu))
    {
        pointArraySubMenu[SubM]->UpdateMenuPos(Pos);
    }
}

SubMenu::SubMenu(Window * _Fenster):
Object(_Fenster),
Schriftgrat(2),
Schriftgroesse(2),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
PosPerPage(5),
Focus(false),
countMenuPos(0),
activPos(-1)
{
    o_height=320-32;  //Gesamthöhe minus die Infobar
    o_width=480-100-72; //Gesamtbreite minus Füllstandsanzeige und Buttons
    posY = 32;
    posX = 72;
}
SubMenu::SubMenu(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH, uint16_t _sizeW):
Object(_Fenster, _posX, _posY, _sizeH, _sizeW),
Schriftgrat(2),
Schriftgroesse(2),
Color_Text(TFT_WHITE),
Color_Background(TFT_DARKGREY),
PosPerPage(5),
Focus(false),
countMenuPos(0),
activPos(-1)
{
}
void SubMenu::createMenuPos(uint8_t _count = 5)
{
    if(!_count)
    return;
    if((_count + countMenuPos) > MaxMenuPos)
    _count = MaxMenuPos - countMenuPos;
    uint16_t PosHeight = (o_height - 40) / PosPerPage;
    uint16_t PosHeightRest = (o_height - 40) % PosPerPage;
    if(PosHeight>100)
    {
        PosHeight = 100;
        PosHeightRest = 0;
    }
    for(int i = 0; i < _count + countMenuPos; i++)
    {
        if(i<countMenuPos)
        delete pointArrayMenuPos[i];
        pointArrayMenuPos[i]= new MenuPos(varWindow, posX, posY+40 + PosHeightRest + (i%PosPerPage * PosHeight), PosHeight, o_width);
    } 
    countMenuPos = _count + countMenuPos;
}
void SubMenu::deleteAllMenuPos()
{
    for (int i = 0; i < countMenuPos; i++)
    {
        delete pointArrayMenuPos[i];
        pointArrayMenuPos[i]=0;
    }
    countMenuPos = 0;
}
void SubMenu::setActivPos(int _Pos)
{
    bool newPage = false;
    int Abort = 0;

    if(_Pos < countMenuPos)
    {
        if(_Pos >= 0)
        while(!pointArrayMenuPos[_Pos]->Selectable && (Abort<countMenuPos)) //Gehe zur nächsten Postiion wenn diese nicht Wählbar ist
        {
            _Pos = (_Pos+1)%countMenuPos;
            Abort++;
        }
        if(Abort>= countMenuPos) //Wenn keine Position wählbar ist, abbrechen
        return;
        if(((activPos/PosPerPage)!=(_Pos/PosPerPage))&&(_Pos >= 0))
            newPage = true;
        if(activPos >= 0)
        {
            pointArrayMenuPos[activPos]->Focus = false;
            if(newPage)
                DrawObject();
            else
                pointArrayMenuPos[activPos]->DrawObject();
        }
        activPos = _Pos;
        if(activPos >= 0)
        {
            pointArrayMenuPos[activPos]->Focus = true;
            if(newPage)
                DrawObject();
            else
                pointArrayMenuPos[activPos]->DrawObject();
        }
        else
        DrawObject();
    }
}
MenuPos * SubMenu::getActivPos()
{
    if(activPos>=0)
    return pointArrayMenuPos[activPos];

    return 0;
}

void SubMenu::DrawObject()
{
    int _activPos;
    if(!visible || !varWindow->WindowActiv)
    return;
    if(activPos <0)
    _activPos=0;
    else
    _activPos=activPos;
    //Zeichne Umriss
    varWindow->tft.fillRect(posX, posY, o_width, o_height, Color_Background);
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.drawCentreString(MenuTitle, posX + (o_width/2), posY + 20 - (4*Schriftgroesse), 1); //Überschrift Menü
    int Start = (_activPos)/PosPerPage*PosPerPage;
    int End = (_activPos)/PosPerPage*PosPerPage + PosPerPage;
    End = End>countMenuPos?countMenuPos:End;
    if(countMenuPos)
    for (int i = Start; i < End; i++)
    {
        pointArrayMenuPos[i]->DrawObject();
    }
}
void SubMenu::UpdateMenuPos(uint8_t Pos)
{
    //Wenn die geänderte Position auf der gleichen Seite ist zeichne es neu
    if(((int)(activPos / PosPerPage) == (int)(Pos / PosPerPage))&&(Pos < countMenuPos))
    {
        pointArrayMenuPos[Pos]->DrawObject();
    }
}
MenuPos::MenuPos(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeH, uint16_t _sizeW):
Object(_Fenster, _posX, _posY , _sizeH, _sizeW),
pInt{0, 0},
pName(0),
CenterOffset(0),
Schriftgrat(1),
Schriftgroesse(2),
b_Color(TFT_DARKGREY),
Color_Text(TFT_WHITE),
MenuPosType(0),
Focus(false),
Selectable(true),
Edit_State(0),
inputType(0),
ToggleTime(0),
ToggleState(0)
{
    Name = "MPos";
    Value1 = "Value1";
    Value2 = "Value2";
    
}
MenuPos::MenuPos(Window * _Fenster):
Object(_Fenster),
pInt{0, 0},
pName(0),
CenterOffset(0),
Schriftgrat(1),
Schriftgroesse(2),
b_Color(TFT_DARKGREY),
Color_Text(TFT_WHITE),
MenuPosType(0),
Focus(false),
Selectable(true),
Edit_State(0),
inputType(0),
ToggleTime(0),
ToggleState(0)
{
    o_height=100;
    o_width=200;
    Name = "MPos";
    Value1 = "Value1";
    Value2 = "Value2";
}
void MenuPos::DrawObject()
{
    if(!visible || !varWindow->WindowActiv)
    return;
    varWindow->tft.fillRect(posX, posY, o_width, o_height, b_Color);
    if(MenuPosType)
        DrawVar2();
    else
        DrawVar1();

}
void MenuPos::DrawVar1()
{
    int Stufen = Focus?32:-32;
    //Zeichne Umriss
    uint16_t TempColor = ChangeBrightness(b_Color, Stufen);
    
    const uint16_t j = 3;
    for (int i = j; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, TempColor);
        varWindow->tft.drawFastVLine(posX + o_width/2+i + CenterOffset, posY, o_height, TempColor);
    }
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.setCursor(posX + 10, posY + o_height/2 - 4*Schriftgroesse);

    if(inputType&0x04)
        varWindow->tft.print(pName);
    else
        varWindow->tft.print(Name);

    varWindow->tft.setCursor(posX + o_width/2 + CenterOffset + 10, posY + (o_height/2) - 4*Schriftgroesse);
    if(inputType&0x01)
        varWindow->tft.print(IntToStr(*pInt[0]));
    else
        varWindow->tft.print(Value1);

}
void MenuPos::DrawVar2()
{
    int Stufen = Focus?32:-32;
    //Zeichne Umriss
    uint16_t TempColor = ChangeBrightness(b_Color, Stufen);
    
    const uint16_t j = 3;
    for (int i = j; i >= 0; i--)
    {
        varWindow->tft.drawRect(posX + i, posY + i, o_width - 2*i, o_height - 2*i, TempColor);
        varWindow->tft.drawFastHLine(posX, posY + o_height/2+i-2 + CenterOffset, o_width, TempColor);
    }
    varWindow->tft.setTextFont(Schriftgrat);
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextColor(Color_Text);
    if(inputType&0x04)
        varWindow->tft.drawCentreString(pName, posX + o_width/2, posY + o_height/4 - 4*Schriftgroesse + 2, Schriftgrat);
    else
        varWindow->tft.drawCentreString(Name, posX + o_width/2, posY + o_height/4 - 4*Schriftgroesse + 2, Schriftgrat);

    varWindow->tft.setCursor(posX + 10, posY + o_height/4*3 - 4*Schriftgroesse);

    if(inputType&0x01)
        varWindow->tft.print(IntToStr(*pInt[0]));
    else
        varWindow->tft.print(Value1);

    varWindow->tft.setCursor(posX + o_width/2 + 10, posY + (o_height/4*3) - 4*Schriftgroesse);

    if(inputType&0x01)
        varWindow->tft.print(IntToStr(*pInt[1]));
    else
        varWindow->tft.print(Value2);
}
void MenuPos::UpdateState()
{   
    if(!visible || !varWindow->WindowActiv)
    return;
    if(Edit_State == 0) //Wenn gerade nichts blinken soll abbrechen
    {
        ToggleTime = 0;
        return;
    }
    if((ToggleTime > millis())&& ToggleTime) //Wenn Zeit noch nicht abgelaufen abbrechen
    return;
    
    ToggleTime= millis()+ 500;
    varWindow->tft.setTextSize(Schriftgroesse);
    varWindow->tft.setTextColor(Color_Text);
    varWindow->tft.setTextFont(Schriftgrat);
                
    if(MenuPosType) //Bei der Zweiten Variante
    {
        if(Edit_State==1) //Wenn erster Wert blinken soll
        {
            varWindow->tft.fillRect(posX+3, posY + o_height/2 + CenterOffset + 2, o_width/2 - 3, o_height/2-6, b_Color);
            if(ToggleState)
            {
                varWindow->tft.setCursor(posX + 10, posY + o_height/4*3 - 4*Schriftgroesse);
                varWindow->tft.print(Value1);
                ToggleState = 0;
            }
            else
            {
                ToggleState = 1;
            }            
        }
        else    //Wenn zweiter Wert blinken soll
        {
            varWindow->tft.fillRect(posX+ o_width/2, posY + o_height/2 + CenterOffset + 2, o_width/2 - 3, o_height/2-6, b_Color);
            if(ToggleState)
            {
                varWindow->tft.setCursor(posX + o_width/2 + 10, posY + (o_height/4*3) - 4*Schriftgroesse);
                varWindow->tft.print(Value2);
                ToggleState = 0;
            }
            else
            {
                ToggleState = 1;
            }
        }        
    }
    else //Bei der ersten Variante
    {
        varWindow->tft.fillRect(posX+o_width/2+3 + CenterOffset, posY+3, o_width/2 - CenterOffset, o_height-6, b_Color);
        if(ToggleState)
        {
            varWindow->tft.setCursor(posX + o_width/2 + CenterOffset + 10, posY + (o_height/2) - 4*Schriftgroesse);
            varWindow->tft.print(Value1);
            ToggleState = 0;
        }
        else
        {
            ToggleState = 1;
        }
    }
}
uint16_t ColorToInt(byte R, byte G, byte B)
{
    uint16_t Temp = R/8;
    Temp = Temp << 5;
    Temp+= G/8;
    Temp = Temp << 6;
    Temp+= B/8;
    return Temp;
}
uint16_t ColorToInt(int * RGB)
{
    uint16_t Temp = RGB[0]/8;
    Temp = Temp << 5;
    Temp+= RGB[1]/8;
    Temp = Temp << 6;
    Temp+= RGB[2]/8;
    return Temp;
}
void IntToColor(uint16_t In ,int * R, int * G, int * B)
{
    *B = In & 0x001F;
    *B *= 8;
    In = In >> 6;
    *G = In & 0x001F;
    *G *= 8;
    In = In >> 5;
    *R = In & 0x001F;
    *R *= 8;    
}
void IntToColor(uint16_t In ,int * RGB)
{
    RGB[2] = In & 0x001F;
    RGB[2] *= 8;
    In = In >> 6;
    RGB[1] = In & 0x001F;
    RGB[1] *= 8;
    In = In >> 5;
    RGB[0] = In & 0x001F;
    RGB[0] *= 8;    
}

uint16_t ChangeBrightness(uint16_t Color, int Steps)
{
    int ColorRGB[3];
    if ((Steps == 0)|| (Steps > 255))
    return Color;

    IntToColor(Color, ColorRGB);
    for (int i = 0; i<3; i++)
    {
        ColorRGB[i] += Steps;
        if (ColorRGB[i] >255)
            ColorRGB[i] = 255;
        else
        if (ColorRGB[i] <0)
            ColorRGB[i] = 0;
    }
    
    return ColorToInt(ColorRGB);
}

String IntToStr(int _var)
{
    char Temp[20];
    sprintf(Temp,"%d",_var);
    return Temp;
}
String IntToStr(char _var)
{
    char Temp[20];
    sprintf(Temp,"%d",_var);
    return Temp;
}
String IntToStr(long int _var)
{
    char Temp[20];
    sprintf(Temp,"%ld",_var);
    return Temp;
}
String IntToStr(uint32_t _var)
{
    char Temp[20];
    sprintf(Temp,"%u",_var);
    return Temp;
}
String IntToStr(float _var)
{
    char Temp[20];
    sprintf(Temp,"%f",_var);
    return Temp;
}
String IntToStrHex(int _var)
{
    char Temp[20];
    sprintf(Temp,"%x",_var);
    return Temp;
}

