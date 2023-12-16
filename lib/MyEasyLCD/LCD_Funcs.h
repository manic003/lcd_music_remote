#include <Arduino.h>

void printLcd(int line, int tab, String msg, boolean clear= true);

void printAction(String msg);

int printButtons(int val);

void checkTurningOffBacklight(unsigned long millisNow);


