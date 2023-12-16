#include "LCD_Funcs.h"
#include <string.h>
#include <LiquidCrystal_I2C.h>
#include <config.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);
extern LiquidCrystal_I2C lcd;
extern boolean backlightON;
extern int activeMode;
extern unsigned long previousMillis;          // will store last time buttons read
extern unsigned long previousMillisBacklight; // will store last time buttons read
extern const long interval;                 // interval at which to read buttons
extern const long intervalBacklight;
extern int reglerAlterWert;

void printLcd(int line, int tab, String msg, boolean clear /*= true*/ )
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
    lcd.print("                   ");
  lcd.setCursor(tab, line);
  lcd.print(msg);
  previousMillisBacklight = millis();
}

void printSymbol(int line, int tab, int index)
{
  lcd.setCursor(tab, line);
  lcd.write(byte(index));
}

// writes message to line 3, line will be cleared via python script
void printAction(String msg)
{

  if (msg.length() > 12)
    msg = "msg to long!";
  lcd.setCursor(1, 2);

  msg = ">> " + msg + "..";
  lcd.print(msg);
}


      //                      01 23 45 67 89 10  11  12 13 14 15
// print in line 1   val=1:   >||   >>     ...     v: 12  [v= volume symbol]
//                   val=2:  on/off ch+  vol+ vol-
int printButtons(int val)
{
    lcd.setCursor(0,1);
    lcd.print("                ");
    Serial.print("delte line 1");
  if (val == 1)
  {
    // lcd.setCursor(9, 0);
    // lcd.print("modOO:spot.");
    //play pause at line 1 tab 0 and tab 1:
    
    printSymbol(1, 0, 0);
    printSymbol(1, 1, 3);
    printLcd(1, 2, "  ", false); // at index 2 and 3 spaces

    //play play (= skip) at B:
    printSymbol(3, 4, 0);
    printSymbol(3, 5, 0);
    printLcd(1, 6, "  ", false); // at index 6 and 7 spaces // todo print das alles in einem mit dem naechsten!


    // menu as ' ... '
    //printSymbol(1, 12, 1);
    printLcd(1, 8, "... ", false); // 8-10 dots at 11 space 

    // vol - :
    printSymbol(1, 12, 1);
    printLcd(1, 13, ":", false);
    
    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(1,14,vol, false);

  }
  else if (val == 2)
  {
    printLcd(1, 0,"<<  >>  ok ");
   
    printSymbol(1, 12, 1);
    printLcd(1, 13, ":", false);
    
    char vol[3];
    vol[2] = '\0';
    sprintf(vol, "%02d", reglerAlterWert);
    printLcd(1,14,vol, false);



  }
     return val;
}

void checkTurningOffBacklight(unsigned long millisNow)
{
  if (millisNow - previousMillisBacklight >= intervalBacklight)
  {
    lcd.noBacklight();
    backlightON = false;
  }
}
