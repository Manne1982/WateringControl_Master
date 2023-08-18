#ifndef HTML_Var
#define HTML_Var


#include <pgmspace.h>
#include <Arduino.h>

//Definitionen fuer Datum und Uhrzeit
// enum {clh, clmin}; //clh ist 0 fuer Uhrzeit-Stunden, clmin ist 1 fuer Uhrzeit Minuten
// enum {dtYear, dtMonth, dtDay}; //Tag = 2; Monat = 1; Jahr = 0
const String WeekDays[7]={"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
// //Allgemeine Definitionen
enum {subwl = 27767, subnw = 30574, subcn = 20035, subpd = 17488, subps = 21328, sublf = 17996, sublm = 19788, submq = 29037}; //Zuordnung der Submit-Bereiche einer Ganzzahl
const String Un_Checked[2]{"","Checked"};
const String varSelected[2]{"", " selected=\"\""};
// const String De_Aktiviert[2]{"Deaktiviert","Aktiviert"};
const String Ein_Aus[2]{"Aus","Ein"};
// unsigned long BaudrateRelais = 9600;
// unsigned long BaudrateDebug = 115200;

//Menünamen                 0               1                 2                   3                 4               5               6                7               8          9              10           11          12              13            14            15            16      17            18              19        20            21          22         23         24        25            26
const String MenuName[30]={"Allgemein", "Wasser St l", "Verbr. Ges m3", "Verbr. ml/sek", "Wasser Dif T", "Wasser Dif W", "Wasser Dif M", "Wasser St", "Wasser St Roh", "LED-Config", "Modus", "Speed", "Standard Dauer", "Kanalname", "Offset in %", "Dauer in %", "NW-Config", "Accesspoint", "SSID", "Statische IP", "IP-Adresse", "NW-Name", "Subnetmask", "Gateway", "DNS-Server", "Zeitserver", "Zeitoffset"};
const String LEDMode[9]={"Farbe", "Rot", "Gruen", "Blau", "Weiss", "Flash", "Strobe", "Fade", "Smoth"};


const char html_header[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>Beregnung</title>
  <meta name="viewport" content="width=device-width, initial-scale=1", charset="UTF-8">
</head>
<body bgcolor=\"#BBFFFF\">
Uhrzeit: %s | Datum: %s, %02d.%02d.%d | Status: %s %s | Letzter Reconnect: %s | NanoError: %u / %u
<br />
<hr><h3>
<a href=\>Startseite</a> | 
<a href=\Prog\>Regnerprogramme</a> | 
<a href=\Channel\>Kanal-Einstellungen</a> | 
<a href=\LED-Config\>LED-Einstellungen</a> | 
<a href=\Settings\>Einstellungen</a> | 

</h3><hr>
)rawliteral";
const char html_Start[] PROGMEM = R"rawliteral(
<h1>Beregnung Startseite</h1><hr>
<h2>Informationen</h2><br />
<TABLE border="1">
  <TR>
    <TD WIDTH="250" VALIGN="TOP">
      Fuellstand in Liter<br /></TD>
    <TD WIDTH="100" VALIGN="TOP">
	  &nbsp %u l</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Fuellstand in Prozent<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %.1f %%</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Wasserstand in cm<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %u mm</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Gesamtverbrauch in l<br /></TD>
    <TD VALIGN="TOP">
	  &nbsp %d l</TD>
  </TR>
</TABLE>
<br />


<h2>Zusatzfunktionen</h2><br />
<TABLE> <!-- 'border="1"-->
  <TR>
    <TD WIDTH="220" VALIGN="TOP">
      <a href=/DisplayOn>Display einschalten</a><br /></TD>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DisplayOff>Display ausschalten</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/LastMessages>Kommunikation Nano</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/TouchInit>Touch-Tasten neu initialisieren</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DebugStart>Debug-Modus starten</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DebugEnd>Debug-Modus beenden</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DebugText>Debug-Text anzeigen</a><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      <a href=/DebugToggleWindow>Deburg-Text auf Display</a><br /></TD>
  </TR>
</TABLE>
<br />
</body>
</html>
)rawliteral";


const char html_NWconfig[] PROGMEM = R"rawliteral(
<h1>Beregnung NW-Einstellungen</h1><hr>
<h2>WLAN Einstellungen</h2>
<form method="post" action="/POST">
<TABLE>
  <TR>
    <TD WIDTH="120" VALIGN="TOP">
    Access Poin: </TD>
    <TD WIDTH="300" VALIGN="TOP">
    <input name="wlAP" value="1" type="checkbox" %s> <br /><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      SSID: </TD>
  <TD>
    <input name="wlSSID" type="text" value="%s" minlength="2" maxlength="30" size="15" required="1"><br /><br /></TD>
  <TD>
    <br /><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Passwort: </TD>
  <TD>
    <input name="wlPassword" type="password" value="xxxxxx" minlength="8" maxlength="60" size="35"><br /><br /></TD>
  </TR>
</TABLE>
  <br>
  <input value="Submit" type="submit">
</form>
<hr>
<h2>Netzwerk</h2><br />
<form method="post" action="/POST">
<TABLE>
  <TR>
    <TD WIDTH="200" VALIGN="TOP">
      Statische IP-Adresse verwenden: </TD>
    <TD WIDTH="200" VALIGN="TOP">
    <input name="nwSIP" value="" type="checkbox" %s> <br /><br /></TD>
    <TD WIDTH="200" VALIGN="TOP">
    </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      IP-Adresse: </TD>
  <TD>
      <input name="nwIP" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
       </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Netzwerkname: </TD>
  <TD>
    <input name="nwNetzName" type="text" minlength="3" maxlength="15"  value="%s" required="1"> <br /><br /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Subnetmask: </TD>
  <TD>  
    <input name="nwSubnet" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
       </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Gateway: </TD>
  <TD>
    <input name="nwGateway" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
      Wird nur bei einem externen NTP-Server benoetigt!</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      DNS-Server: </TD>
  <TD>
    <input name="nwDNS" type="text" minlength="7" maxlength="15" size="15" value="%s" pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$"><br /><br /></TD>
    <TD VALIGN="TOP">
      Wird nur bei einem externen NTP-Server benoetigt!</TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Zeitserver (z.B. fritz.box): </TD>
  <TD>
    <input name="nwNTPServer" type="text" minlength="3" maxlength="50"  value="%s" required="1"> <br /><br /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
      Zeitoffset (in Stunden): </TD>
    <TD>
      <select name="nwNTPOffset" autofocus="autofocus">
      <option%s>-2</option>
      <option%s>-1</option>
      <option%s>0</option>
      <option%s>+1</option>
      <option%s>+2</option>
      </select> 
    <br /><br /></TD>
    <TD VALIGN="TOP">
      </TD>
  </TR>

</TABLE>
    <br>
  <input value="Submit" type="submit">
  </form>
<hr>
<h2>MQTT</h2><br>
<form method="post" action="/POST">
<table>
  <tbody><tr>
    <td valign="TOP">
      MQTT Server: </td>
  <td>
      <input name="mqServer" type="text" minlength="7" maxlength="15" size="45" value="%s"><br><br></td>
    <td valign="TOP">
       </td>
  </tr>
  <tr>
    <td valign="TOP">
      MQTT Port: </td>
  <td>
    <input name="mqPort" type="number" minlength="3" maxlength="5" size="8" value="%u" required="1" pattern="[0-9]{5}"> <br><br></td>
    <td valign="TOP">
      </td>
  </tr>
  <tr>
    <td valign="TOP">
      Benutzername: </td>
  <td>  
    <input name="mqUser" type="text" minlength="3" maxlength="15" size="19" value="%s"><br><br></td>
    <td valign="TOP">
       </td>
  </tr>
  <tr>
    <td valign="TOP">
      Passwort: </td>
  <td>
    <input name="mqPassword" type="password" minlength="8" maxlength="60" size="35" value="xxxxxx"><br><br></td>
    <td valign="TOP">
  </tr>
  <tr>
    <td valign="TOP">
      MQTT Hauptpfad: </td>
  <td>
    <input name="mqRootpath" type="text" minlength="5" maxlength="100" size="35" value="%s"><br><br></td>
    <td valign="TOP">
  </tr>
  <tr>
    <td valign="TOP">
      Zertifikat: </td>
  <td>
    <input name="mqFPrint" type="password" minlength="8" maxlength="60" size="35" value="xxxxxx"><br><br></td>
    <td valign="TOP">
  </tr>

</tbody></table>
    <br>
  <input value="Submit" type="submit">
  </form>

</body>
</html>
)rawliteral";

const char html_LED_Config[] PROGMEM = R"rawliteral(
<h1>Beregnung LED-Konfiguration</h1><hr>
<h2>Farbauswahl</h2><br />
<form method="post" action="/POST">
Rot: <input name="LF_Rot" type="number" min="0" max="255" step="1" size="8" required="1" value="%u">
Gruen: <input name="LF_Gruen" type="number" min="0" max="255" step="1" size="8" required="1" value="%u">
Blau: <input name="LF_Blau" type="number" min="0" max="255" step="1" size="8" required="1" value="%u">
<input value="Submit" type="submit"><br /><br />
</form>
<br />
<br />
<form method="post" action="/POST">
<TABLE border="1">
  <TR>
    <TD WIDTH="150" VALIGN="TOP">
      <input type="radio" id="LED" name="LM_Prog" value="1" %s>
 	    <label for="LED_Prog">Rot</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="2" %s>
  	  <label for="LED_Prog">Gruen</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="3" %s>
  	  <label for="LED_Prog">Blau</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="4" %s>
  	  <label for="LED_Prog">Weiss</label><br /></TD>
    <TD WIDTH="100" VALIGN="TOP">
      <input type="radio" id="LED" name="LM_Prog" value="5" %s>
 	    <label for="LED_Prog">Flash</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="6" %s>
  	  <label for="LED_Prog">Strobe</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="7" %s>
  	  <label for="LED_Prog">Fade</label><br />
  	  <input type="radio" id="LED" name="LM_Prog" value="8" %s>
  	  <label for="LED_Prog">Smooth</label><br /></TD>
  </TR>
  <TR>
    <TD VALIGN="TOP">
	  <br /><br />
	  Geschwindigkeit: 
	  <input name="LM_Speed" type="number" min="0" max="255" step="1" size="5" required="1" value="%u"><br /><br /></TD>
    <TD VALIGN="TOP">
	  <br /><br />	
	  <input value="Submit" type="submit"></TD>
  </TR>
</TABLE>
</form>
<br />
</body>
</html>
)rawliteral";
const char html_Prog1[] PROGMEM = R"rawliteral(
<h1>Beregnung Kanalbenennung</h1><hr>
<form method="post" action="/POST">
<TABLE> <!-- 'border="1"-->
  <TR>
    <TD WIDTH="100" VALIGN="TOP">
      <b>Kanal</b><br /><br /> </TD>
    <TD WIDTH="100" VALIGN="TOP">
	  <b>Name</b><br /><br /> </TD>
  </TR>
)rawliteral";

const char html_Prog2[] PROGMEM = R"rawliteral(
%s
  <TR>
    <TD VALIGN="TOP">
	  Kanal %u<br /><br /></TD>
    <TD VALIGN="TOP">
	  <input name="CN_%u" type="text" minlength="2" maxlength="9" size="14" value="%s" required="1"><br /><br /></TD></TR>
)rawliteral";
const char html_Prog2_1[] PROGMEM = R"rawliteral(
  <TR>
    <TD VALIGN="TOP">
	  <br /></TD>
    <TD VALIGN="TOP">
	  <input value="Submit" type="submit"></TD></TR>
</TABLE>
</form>
</body>
</html>
)rawliteral";
const char html_Prog3[] PROGMEM = R"rawliteral(
%s
<h1>Beregnung Programme</h1><hr>
Erklaerung:<br>
Verzoegerung und Dauer werden in %% angegeben.<br>
Beispiel:<br>
Das Programm laeuft 100 Minuten.<br>
Verzoegerung wurde fuer Kanal 1 auf 10 und Dauer auf 20 eingestellt.<br>
Dann Schaltet Kanal 1 ab Minute 10 ein fuer 20 Minuten.<br>
Wenn das Programm 50 Minuten laeuft,<br> schaltet Kanal ein ab Minute 5 ein fuer 10 Minuten.<br>
<hr>
)rawliteral";
const char html_Prog4[] PROGMEM = R"rawliteral(
%s
<h2>Programm %u</h2><br />
<form method="post" action="/POST">
Programmname: <input name="PD_Prog_Name_%u" type="text" minlength="2" maxlength="6" size="15" value="%s" required="1"><br />
Standarddauer in Minuten: <input name="PD_Prog_Dauer_%u" type="number" min="10" max="300" step="1" size="8" required="1" value="%u">
<input value="Submit" type="submit"><br /><br />
</form>
<form method="post" action="/POST">
<TABLE> <!-- 'border="1"-->
  <TR>
    <TD WIDTH="100" VALIGN="TOP">
      Verzoegerung <br /></TD>
    <TD WIDTH="100" VALIGN="TOP">
	  Dauer</TD>
    <TD WIDTH="100" VALIGN="TOP">
	  Kanal</TD>
    <TD WIDTH="200" VALIGN="TOP">
	  </TD>
  </TR>
%s
  <TR>
    <TD VALIGN="TOP">
	  <input name="PS_Verz_%u" type="number" min="0" max="99" step="1" size="8" required="1"><br /><br /></TD>
    <TD VALIGN="TOP">
	  <input name="PS_Dauer_%u" type="number" min="1" max="100" step="1" size="8" required="1"><br /><br /></TD>
    <TD VALIGN="TOP">
	  <input name="PS_Kanal_%u" type="number" min="1" max="8" step="1" size="8" required="1"><br /><br /></TD>
    <TD VALIGN="TOP">
	  <input value="Submit" type="submit"></TD>
  </TR>
</TABLE>
</form>
<br />
<hr>
)rawliteral";
const char html_Prog4_1[] PROGMEM = R"rawliteral(
%s
  <TR>
    <TD VALIGN="TOP">
	  %u</TD>
    <TD VALIGN="TOP">
	  %u</TD>
    <TD VALIGN="TOP">
	  %u</TD>
    <TD VALIGN="TOP">
	  <a href=\DelProg\%u\%u>Loeschen</a></TD>
  </TR>
)rawliteral";
const char html_Prog5[] PROGMEM = R"rawliteral(
%s
</body>
</html>
)rawliteral";


#endif