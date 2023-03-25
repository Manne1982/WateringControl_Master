#ifndef Display
#define Display
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

#define Switch_off 0
#define Switch_on 1
#define Switch_auto 2
#define MaxMenuPos 20

class Window;
class Object;
class Button;
class SubMenu;
class Menu;
class MenuButton;
class MenuPos;

uint16_t ChangeBrightness(uint16_t Color, int Steps);
void IntToColor(uint16_t In ,int * RGB);
void IntToColor(uint16_t In ,int * R, int * G, int * B);
uint16_t ColorToInt(int * RGB);
uint16_t ColorToInt(byte R, byte G, byte B);
String IntToStr(int _var);
String IntToStr(float _var);
String IntToStrHex(int _var);
String IntToStr(uint32_t _var);



class Window
{
    public:
        Window(uint16_t width, uint16_t height);
        ~Window();
        void SetFirstObject(Object * _Set);
        Object * GetFirstObject() const;
        void printnl(const char * Text);
        void print(const char * Text);
        void print(int Zahl);
        void print(uint32_t Zahl);
        void print(float Zahl);
        void print(double Zahl);
        void println(int Zahl);
        void println(uint32_t Zahl);
        void println(float Zahl);
        void println(double Zahl);
        bool WindowActiv;
        static TFT_eSPI tft;
        void setDebug();
        void drawDebug();
        char * getDebugText();
    private:
        uint16_t d_height;
        uint16_t d_width;
        Object * FirstObject;
        char * DebugText;
        bool DebugWindow;

};

class Object
{
    public:
        Object(Window * Fenster);
        Object(Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _height, uint16_t _width);
        virtual ~Object();
        Object * GetLastObject();
        Object * GetObject(uint16_t arrPos);
        uint16_t GetOCount() const;
        virtual void DrawObject() = 0; 
        void DrawAllObjects();
        bool visible;
    protected:
        Window* varWindow;
        Object * o_after;
        Object * o_before;
        uint16_t o_height;
        uint16_t o_width;
        uint16_t posX;
        uint16_t posY;
    private:
        static uint16_t o_count;
};


class Button: public Object
{
    public:
        Button (Window * _Fenster);
        Button (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeW, uint16_t sizeH);
        virtual ~Button() {};
        virtual void DrawObject();
        void UpdateState(uint8_t _all);
        void PushButton();
        uint8_t GetButtonState(){return Button_State;}
        void SetButtonState(uint8_t newState);
        void SetPossibleStates(uint8_t newValue);
        String Text;
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t b_Color;
        uint16_t Text_Color;
    private:
        uint8_t Button_State; //Switch_on, Switch_off, Switch_auto
        uint8_t possibleStates;
        unsigned long ToggleTime;
        uint8_t ToggleState;
};
class Level: public Object
{
    public:
        Level (Window * _Fenster);
        Level (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeH);
        virtual ~Level() {};
        virtual void DrawObject();
        void SetLevelMin(uint16_t _min);
        void SetLevelMax(uint16_t _max);
        void SetLevelIst(uint16_t _ist);
        uint16_t GetWidth(){return o_width;}
        String Text;
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t Color_Text;
        uint16_t Color_Background;
        uint16_t Color_Level_Back;
        uint16_t Color_Level_Front;
    private:    
        uint16_t Level_min; 
        uint16_t Level_max; 
        uint16_t Level_ist; 
};
class Bar: public Object
{
    public:
        Bar (Window * _Fenster);
        Bar (Window * _Fenster, uint16_t _posX, uint16_t _posY, uint16_t _sizeW, uint16_t _sizeH);
        virtual ~Bar() {};
        virtual void DrawObject();
        void UpdateState();
        String Text;
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t Color_Text;
        uint16_t Color_Background;
        uint16_t Color_On;
        uint16_t Color_Off;
        uint8_t Status;
    private:
        uint8_t Button_State; //Switch_on, Switch_off, Switch_auto
        uint8_t possibleStates;
        unsigned long ToggleTime;
        uint8_t ToggleState;
};
class MenuButton: public Object
{
    public:
        MenuButton (Window * _Fenster);
        MenuButton (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeH);
        virtual ~MenuButton() {};
        virtual void DrawObject();
        void UpdateState(uint8_t _all);
        void PushButton();
        uint8_t GetButtonState(){return Button_State;}
        void SetButtonState(uint8_t newState);
        uint16_t b_Color;
    private:
        uint8_t Button_State; //Switch_on, Switch_off, Switch_auto
};
class Menu: public Object
{
    public:
        Menu (Window * _Fenster);
        Menu (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeH, uint16_t sizeW);
        virtual ~Menu() {};
        virtual void DrawObject();
        void CreateMenus(uint8_t _count);
        void setActivSubMenu(uint8_t _SubMenu);
        SubMenu * getActivSubMenu();
        int getActivLayer(){return activLayer;};
        void MenuNavUp();
        void MenuNavDown();
        void MenuNavNext();
        void MenuNavRev();
        void UpdateMenuPos(uint8_t SubM, uint8_t Pos);
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t Color_Text;
        uint16_t Color_Background;
        MenuButton * pointArrayMenuButton[10];
        SubMenu * pointArraySubMenu[10];
    private:  
        uint8_t countSubMenu;
        int activMenu;
        int activLayer; //0=Hauptmenü; 1=Submenü(Navigation der Positionen); 3= Änderung (wird später implementiert)
        bool Focus;          
};

class SubMenu: public Object
{
    public:
        SubMenu (Window * _Fenster);
        SubMenu (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeH, uint16_t sizeW);
        virtual ~SubMenu() {};
        virtual void DrawObject();
        void UpdateMenuPos(uint8_t Pos);
        void createMenuPos(uint8_t _count);
        void deleteAllMenuPos();
        void setActivPos(int _Pos);
        MenuPos * getActivPos();
        int getActivPosInt(){return activPos;};
        uint8_t getCountMenuPos(){return countMenuPos;};
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t Color_Text;
        uint16_t Color_Background;
        uint16_t PosPerPage;
        String MenuTitle;
        MenuPos * pointArrayMenuPos[20];       
        bool Focus;   
    private:  
        uint8_t countMenuPos;
        int activPos; 
        MenuPos * pointerActivPos; 
};
class MenuPos: public Object
{
    public:
        MenuPos (Window * _Fenster);
        MenuPos (Window * _Fenster, uint16_t posX, uint16_t posY, uint16_t sizeW, uint16_t sizeH);
        virtual ~MenuPos() {};
        virtual void DrawObject();
        void UpdateState();
        String Name;
        String Value1;
        String Value2;
        int * pInt[2];
        char * pName; //Pointer für den Namen wenn bei inputType das 3te Bit gesetzt
        int CenterOffset;
        uint16_t Schriftgrat;
        uint16_t Schriftgroesse;
        uint16_t b_Color;
        uint16_t Color_Text;
        uint8_t MenuPosType;
        bool Focus;          
        bool Selectable;
        uint8_t Edit_State; //0 = keines, 1 = Value1, 2 = Value2
        uint8_t inputType; //0 = Text, 1. Bit = PointerInt Val1; 2. Bit = PointerInt Val2; 3. Bit = Pointer Name
    private:
        void DrawVar1();
        void DrawVar2();
        unsigned long ToggleTime;
        uint8_t ToggleState;
};

#include "display.cpp"

#endif
