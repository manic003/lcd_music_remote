#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include <MCP3008.h>
#define CS_PIN 15
#define CLOCK_PIN 14
#define MOSI_PIN 13
#define MISO_PIN 12

#define BUTTON_A 5
#define BUTTON_B 4
#define BUTTON_C 3
#define BUTTON_D 2
#define VOLUME_REGLER 0
#define RED_SWITCH 1


MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

int tempValSwitchRight = 0;
int tempValSwitchLeft = 0;
int CURRENT_MODE =0;
boolean backlightON = true;
boolean modeONE;
boolean modeTWO;
boolean modeTHREE;
boolean modeFOUR;

 
int activeMode = 0;

unsigned long previousMillis = 0; // will store last time buttons read
unsigned long previousMillisBacklight = 0; // will store last time buttons read
const long interval = 500;        // interval at which to read buttons
const long intervalBacklight = 15000;
int reglerAlterWert = 0;
const char* SSID = "ADD_SSID";
const char* PSK = "ADD_PASSWORD";
const char* MQTT_BROKER = "192.168.0.241";

// char currentlyDisplayedLine1[21];
// char currentlyDisplayedLine2[21];
// char currentlyDisplayedLine3[21];
// char currentlyDisplayedLine4[21];

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte customCharPLAY[] = {
    B10000,
    B11000,
    B11100,
    B11110,
    B11100,
    B11000,
    B10000,
    B00000};

byte customCharPAUSE[] = {
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B11011,
    B00000};

byte customCharSPEAKER[] = {0x1, 0x3, 0xf, 0xf, 0xf, 0x3, 0x1, 0x0};
byte customCharHaken[] = {0x0, 0x1, 0x3, 0x16, 0x1c, 0x8, 0x0};

byte customCharSTOP[] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B00000};

void checkTurningOffBacklight(unsigned long millisNow){
  if (millisNow - previousMillisBacklight >= intervalBacklight)
  {
     lcd.noBacklight();
     backlightON = false;
  }
}

void printLcd(int line, int tab, String msg, boolean clear = true)
{
  /* 
* The print LCD function clears each line before writing text by default, if parameter clear is false, 
* it just writes/ overwrites text at given cursor position
* 
*/

  if (!backlightON)
  {
    lcd.backlight();
    backlightON = true;
  }
  lcd.setCursor(0, line);
  if (clear)
    lcd.print("                    ");
  lcd.setCursor(tab, line);
  lcd.print(msg);
  previousMillisBacklight = millis();
}



// writes message to line 3, line will be cleared via python script
void printAction(String msg)
{

  if (msg.length() > 12)
    msg = "msg to long!";
  lcd.setCursor(1, 2);

  msg = ">> " + msg + "..";
  lcd.print(msg);
  //delay(200);
  //lcd.print(".");
  //delay(100);
  //lcd.print(".");
  //delay(100);
}

void printSymbol(int line, int tab, int index)
{
  lcd.setCursor(tab, line);
  lcd.write(byte(index));
}

// print in line 3   val=1:   >||   >>  vol+ vol-
//                   val=2:  on/off ch+  vol+ vol-
int printButtons(int val)
{
    lcd.setCursor(0,3);
    lcd.print("                    ");
    Serial.print("delte");
  if (val == 1)
  {
    lcd.setCursor(9, 0);
    lcd.print("modOO:spot.");
    //play pause at A:

    printLcd(3, 0, "      ", false);
    printSymbol(3, 2, 0);
    printSymbol(3, 3, 3);

    //play play (= skip) at B:
    printSymbol(3, 7, 0);
    printSymbol(3, 8, 0);
    printLcd(3, 9, " ", false);

    // //vol + at C:
    // printSymbol(3, 12, 0);
    // printLcd(3, 13, "?", false);

    // // vol - at D:
    // printSymbol(3, 17, 1);
    // printLcd(3, 18, "-", false);

        printSymbol(3, 15, 1);
    printLcd(3, 16, ":", false);

    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(3,18,vol, false);
  }
  else if (val == 2)
  {
    lcd.setCursor(9, 0);
    lcd.print("modO1:radio");
    printLcd(3, 0, "on/off", false);

    printLcd(3, 7, "ch+", false);

    //vol + at C:
    //printSymbol(3, 12, 1);
    //printLcd(3, 13, "+", false);

    // vol - at D:
    //printSymbol(3, 17, 1);
    //printLcd(3, 18, "-", false);


    // vol REGLER - :
    printSymbol(3, 15, 1);
    printLcd(3, 16, ":", false);
    
    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(3,18,vol, false);
  }
    else if (val == 3)
  {
    lcd.setCursor(9, 0);
    lcd.print("mod1O:lists");
    printLcd(3, 1, "a)", false);

    printLcd(3, 6, "b)", false);

    printLcd(3, 11, "c)", false);

    //printLcd(3, 16, "d)", false);


        // vol REGLER - :
    printSymbol(3, 15, 1);
    printLcd(3, 16, ":", false);
    
    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(3,18,vol, false);
  }

      else if (val == 4)
  {
    lcd.setCursor(9, 0);
    lcd.print("mod11: _TV_");
    printLcd(3, 1, "netf", false);

    printLcd(3, 6, "b)", false);

    printLcd(3, 11, "c)", false);

   // printLcd(3, 16, "d)", false);


        // vol REGLER - :
    printSymbol(3, 15, 1);
    printLcd(3, 16, ":", false);
    
    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(3,18,vol, false);
  }
   return val;
}

void setup_wifi()
{
  delay(10);
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(SSID);

  WiFi.begin(SSID, PSK);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("wifi verbunden.");
  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}

// via raspberry :mosquitto_pub  -t /home/LCD/inputESP -m "Hallo000" (Schreibt 'Hallo' aufs LCD  bei line=0, tab=0, mit clear_line=false)
void callback(char *topic, byte *payload, unsigned int length)
{
  //Serial.print("Received message [");
  //Serial.print(topic);
  //Serial.print("] ");
  char msg[length];
  String nachricht = "";

  for (int i = 0; i < length; i++)
  {

    msg[i] = (char)payload[i];
    nachricht += msg[i];
  }

  // control backlight via mqtt:
  if (nachricht.charAt(0) == '#' && nachricht.charAt(1) == '&')
  {
    Serial.print("Befehl empfangen:");
    Serial.println(nachricht);

    if (nachricht.charAt(2) == 'b')
    {
      if (nachricht.charAt(3) == '0')
      {
        lcd.noBacklight();
        backlightON = false;
        //Serial.println("Backlight switch off (via mqtt)");
      }
      else
      {
        lcd.backlight();
        backlightON = true;
        //Serial.println("Backlight switched on (via mqtt)");
      }
    }
  }
  // print on LCD via mqtt:
  else if (nachricht.length() > 3)
  { // if no command send (message string starts not with #&) and at least 4 chars
    // (last 3 are in use for tab, line, clear(true/false))

    Serial.print("nachricht empfangen:");
    Serial.println(nachricht);

    // get line and tab for setting  the cursor (always last two characters of the String)
    int line = ((int)nachricht.charAt(nachricht.length() - 2)) - ((int)'0');
    int tab = ((int)nachricht.charAt(nachricht.length() - 1)) - ((int)'0');
    bool clearLine = (nachricht.charAt(nachricht.length() - 3) == '1');

    // $ write custom char if message string starts with $
    if (nachricht.charAt(0) == '$')
    {
      lcd.setCursor(tab, line);
      //Serial.println(((int)nachricht.charAt(1)) - ((int)'0'));
      lcd.write(byte(((int)nachricht.charAt(1)) - ((int)'0'))); // $000 write custom char 0 at line 0 tab 0..
    }

    else
    {

      // remove last 3 characters of message (clear,line,tabs)
      nachricht = nachricht.substring(0, nachricht.length() - 3);
      printLcd(line, tab, nachricht, clearLine);
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Versuche MQTT zuverbinden...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("MQTT connected");
      // Once connected, publish an announcement...
      client.publish("/home/LCD/outputToRaspy", "LCD Online");
      // ... and resubscribe
      client.subscribe("/home/LCD/inputESP");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

boolean switchOpen(int val)
{
  if (val <= 900)
    return true;
  return false;
}

boolean buttonPressed(int indexButton)
{
  int k = adc.readADC(indexButton);
  if (k >= 900){
    lcd.backlight();
    backlightON = true;
    previousMillisBacklight = millis();
    return true;
  }
  return false;
}

void setup()
{
  Serial.begin(115200);

  setup_wifi();

  client.setServer(MQTT_BROKER, 1883);
  client.setCallback(callback);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("booting");
  printLcd(1, 0, "setup wifi..");
  printLcd(2, 0, "connected!");

  lcd.clear();

  lcd.createChar(0, customCharPLAY);
  lcd.createChar(1, customCharSPEAKER);
  lcd.createChar(2, customCharSTOP);
  lcd.createChar(3, customCharPAUSE);
  lcd.createChar(4, customCharHaken);
  reglerAlterWert = adc.readADC(7) / 51;;

  // ModeOne = 0 0
  // ModeTwo = 0 1
  // ModeThree = 1 0 
  // ModeFour = 1 1 
  //tempValSwitchRight = switchOpen(adc.readADC(0));
  
  
  //tempValSwitchLeft = switchOpen(adc.readADC(RED_SWITCH));


  modeONE = true;
  modeTWO = modeTHREE = modeFOUR = false;

  lcd.print("ready!");
  if (modeONE) activeMode = printButtons(1);
  else if (modeTWO) activeMode =  printButtons(2);
  else if (modeTHREE) activeMode =  printButtons(3);
  else if (modeFOUR) activeMode =  printButtons(4);
  
}

void loop()
{

  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // delay for reading buttons
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time taster gelesen
    previousMillis = currentMillis;
    checkTurningOffBacklight(currentMillis);

    //tempValSwitchRight = switchOpen(adc.readADC(0));
    tempValSwitchLeft = buttonPressed(BUTTON_D);
    if (tempValSwitchLeft){
      CURRENT_MODE = (++CURRENT_MODE)%4;
    }

    modeONE = CURRENT_MODE == 0;
    modeTWO = CURRENT_MODE == 1;
    modeTHREE = CURRENT_MODE == 2;
    modeFOUR = CURRENT_MODE == 3;


    if (reglerAlterWert != adc.readADC(VOLUME_REGLER) / 51){
      reglerAlterWert = adc.readADC(VOLUME_REGLER) / 51;

      char msg[3];
      msg[2] = '\0';
      sprintf(msg, "%02d", reglerAlterWert);


      Serial.printf("Regler hat VolumeWert: %s\n ", msg);

   
       printLcd(3,18,msg, false);
      client.publish("/home/LCD/outputToRaspy", msg);



    }


    if (modeONE && activeMode != 1)
    {
      activeMode = printButtons(1);
      client.publish("/home/LCD/outputToRaspy", "mode1");
      Serial.println("mode1 set");
      
    }
    else if (modeTWO && activeMode != 2)
    {
      activeMode =  printButtons(2);
      client.publish("/home/LCD/outputToRaspy", "mode2");
      Serial.println("mode2 set");
    }

    else if (modeTHREE && activeMode != 3){
      activeMode = printButtons(3);
      client.publish("/home/LCD/outputToRaspy", "mode3");
      Serial.println("mode3 set");

    }
    else if (modeFOUR && activeMode != 4){
      activeMode = printButtons(4);
      client.publish("/home/LCD/outputToRaspy", "mode4");
      Serial.println("mode4 set");
    }

    // if (buttonPressed(BUTTON_D))
    // {

    //   if (modeONE)
    //   {
    //     client.publish("/home/LCD/outputToRaspy", "4a");
    //     Serial.println("button4a pressed");
    //     printAction("  vol down  ");
    //   }
    //   else if (modeTWO)
    //   {
    //     client.publish("/home/LCD/outputToRaspy", "4b");
    //     printAction("  vol down  ");
    //   }
    //   else if (modeTHREE)
    //   {
    //     client.publish("/home/LCD/outputToRaspy", "4c");
    //   }
    //   else if (modeFOUR)
    //   {
    //     client.publish("/home/LCD/outputToRaspy", "4d");
    //   }

    // }

    if (buttonPressed(BUTTON_C))
    {
      if (modeONE)
      {
        Serial.println("button3a pressed");
        client.publish("/home/LCD/outputToRaspy", "3a");
        printAction("   vol up   ");
      }
      else if (modeTWO)
      {
        Serial.println("button3b pressed");
        client.publish("/home/LCD/outputToRaspy", "3b");
        printAction("   vol up   ");
      }
      else if (modeTHREE)
      {
        client.publish("/home/LCD/outputToRaspy", "3c");
      }
      else if (modeFOUR)
      {
        client.publish("/home/LCD/outputToRaspy", "3d");
      }

    }

    else if (buttonPressed(BUTTON_B))
    {
      if (modeONE)
      {
        client.publish("/home/LCD/outputToRaspy", "2a");
        Serial.println("button2 pressed");
        printAction("  skipping  ");
      }
      else if (modeTWO)
      {
        client.publish("/home/LCD/outputToRaspy", "2b");
        printAction("skip channel");
      }
      
      else if (modeTHREE)
      {
        client.publish("/home/LCD/outputToRaspy", "2c");
      }
      else if (modeFOUR)
      {
        client.publish("/home/LCD/outputToRaspy", "2d");
      }

    }

    else if (buttonPressed(BUTTON_A))
    {

      if (modeONE)
      {
        client.publish("/home/LCD/outputToRaspy", "1a");
        Serial.println("button1a pressed");
        printAction(" play/pause ");
      }
      else if (modeTWO)
      {
        client.publish("/home/LCD/outputToRaspy", "1b");
        printAction(" radio pl/ps");
      }
      else if (modeTHREE)
      {
        client.publish("/home/LCD/outputToRaspy", "1c");
      }
      else if (modeFOUR)
      {
        client.publish("/home/LCD/outputToRaspy", "1d");
          printAction("netflix!");
      }
    }
  }
}
