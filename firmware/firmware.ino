#include <TinyWireM.h>
#include "ssd1306.h"
#include "ina219.h"
// for some reason sprintf with %f not work , use this custom lib instead
#include "dataConverter.h"

// Devices
SSD1306 oled;
INA219 ina219;
#define THRESHOLD_POT 3
#define FUNC_1 4
#define FUNC_2 1

// Var
unsigned long startTime;
unsigned long lastRead;
unsigned long upTime;

float voltage;
float current;
float power;
float powerConsumption;

float maxVoltage;
float minVoltage;
float maxCurrent;
float minCurrent;

float ignoreCurrent;

int sec;
int min;
int hour;

int mode;
int oldMode;
char str[13];


void update();
void readSensor();
void initData();
//common
void printData();
void drawTemplate();
//for advance mode(1)
void drawAdvanceTemplate();
void printAdvanceData();
//for simple mode(0)
void drawSimpleTemplate();
void printSimpleData();

void setup()
{
  initData();
  pinMode(FUNC_1,INPUT_PULLUP);
  pinMode(FUNC_2,INPUT_PULLUP);
}


void loop()
{
  readSensor();
  update();
  drawTemplate();
  printData();
}


void initData()
{
  startTime = millis();
  upTime = 0;

  voltage = 0;
  current = 0;
  power = 0;
  powerConsumption = 0;

  maxVoltage = 0;
  minVoltage = 0;
  minCurrent = 0;
  maxCurrent = 0;

  mode = 0;
  oldMode = -1;

  ignoreCurrent = 0;

  TinyWireM.begin();
  oled.begin();
  ina219.begin();
}

void readSensor()
{
  voltage = ina219.read_bus_voltage();
  current = ina219.read_current();
  power = ina219.read_power();

  int ignoreVal = analogRead(THRESHOLD_POT);
  ignoreCurrent = 0.5 * ignoreVal / 1024;

  //ignore current value if under threshold
  if (current <= ignoreCurrent)
  {
    current = 0;
    power = 0;
  }

  powerConsumption = powerConsumption + power / 1000 * (millis() - lastRead) / 3600;
  lastRead = millis();

  if(digitalRead(FUNC_1)==LOW)
    mode = 0;
  if(digitalRead(FUNC_2)==LOW)
    mode = 1;
}

void update()
{
  //max voltage and current
  if (voltage > maxVoltage)
    maxVoltage = voltage;
  if (current > maxCurrent)
    maxCurrent = current;

  //min voltage & current
  if (minCurrent == 0)
    minCurrent = current;
  if (minVoltage == 0)
    minVoltage = voltage;
  if ((minCurrent > current) && (current != 0))
    minCurrent = current;
  if (minVoltage > voltage)
    minVoltage = voltage;

  upTime = (lastRead - startTime) / 1000;
  hour = upTime / 3600;
  min = (upTime - hour * 3600)/60;
  sec = upTime%60;
}


void drawTemplate()
{
  if(mode!=oldMode)
  {
    oldMode = mode;
    if(mode)
      drawAdvanceTemplate();
    else
      drawSimpleTemplate();
  }
}


void printData()
{
  if(mode)
    printAdvanceData();
  else
    printSimpleData();
}


void drawSimpleTemplate()
{
  //clear screen
  oled.fill(0x00);

  // line 1
  oled.set_pos(0, 0);
  oled.print("Power monitor       v1.0");
  
  // line 2
  oled.set_pos(0, 1);
  oled.print("------------------------");

  //line 3 
  //blank

  // line 4
  oled.set_pos(40, 3);
  oled.print("V");
  oled.set_pos(96, 3);
  oled.print("mA");
  
  // line 5
  //blank

  // line 6 - power
  oled.set_pos(0, 5);
  oled.print("Power");
  oled.set_pos(106, 5);
  oled.print("mWh");
  
  // line 7
  oled.set_pos(0, 6);
  oled.print("R-load");
  oled.set_pos(106, 6);
  oled.print("Ohm");

  // line 8
  oled.set_pos(0, 7);
  oled.print("Threshold");
  oled.set_pos(106, 7);
  oled.print("mA");
}

void printSimpleData()
{
  // line 4 - voltage
  oled.set_pos(14, 3);
  floatToString(voltage,str,VOLTAGE);
  oled.print(str);

  // line 4 - current
  oled.set_pos(60, 3);
  floatToString(current,str,CURRENT);
  oled.print(str);
  
  // line 6 - power
  oled.set_pos(45, 5);
  floatToString(powerConsumption,str,POWER);
  oled.print(str);

  //line 7 rload
  oled.set_pos(55, 6);
  float rLoad = voltage/(current/1000);
  floatToString(rLoad,str,RLOAD);
  oled.print(str);

  // line 8 - threshold
  oled.set_pos(70, 7);
  floatToString(ignoreCurrent,str,CURRENT);
  oled.print(str);
}


void drawAdvanceTemplate()
{
  //clear screen
  oled.fill(0x00);

  // line 1
  oled.set_pos(0, 0);
  oled.print("Now");
  oled.set_pos(50, 0);
  oled.print("V");
  oled.set_pos(106, 0);
  oled.print("mA");
  
  // line 2
  oled.set_pos(0, 1);
  oled.print("------------------------");


  // line 3
  oled.set_pos(0, 2);
  oled.print("Min");
  oled.set_pos(50, 2);
  oled.print("V");
  oled.set_pos(106, 2);
  oled.print("mA");

  // line 4
  oled.set_pos(0, 3);
  oled.print("Max");
  oled.set_pos(50, 3);
  oled.print("V");
  oled.set_pos(106, 3);
  oled.print("mA");
  
  // line 5
  oled.set_pos(0, 4);
  oled.print("Power");
  oled.set_pos(106, 4);
  oled.print("mWh");

  // line 6
  oled.set_pos(0, 5);
  oled.print("Time");

  
  // line 7
  oled.set_pos(0, 6);
  oled.print("------------------------");

  // line 8
  oled.set_pos(0, 7);
  oled.print("Threshold");
  oled.set_pos(106, 7);
  oled.print("mA");
}

void printAdvanceData()
{
  // line 1 - voltage
  oled.set_pos(24, 0);
  floatToString(voltage,str,VOLTAGE);
  oled.print(str);
  // line 1 - current
  oled.set_pos(70, 0);
  floatToString(current,str,CURRENT);
  oled.print(str);
  
  // line 3 - min voltage
  oled.set_pos(24, 2);
  floatToString(minVoltage,str,VOLTAGE);
  oled.print(str);
  // line 3 - min current
  oled.set_pos(70, 2);
  floatToString(minCurrent,str,CURRENT);
  oled.print(str);

  // line 4 - max voltage
  oled.set_pos(24, 3);
  floatToString(maxVoltage,str,VOLTAGE);
  oled.print(str);
  // line 4 - max current
  oled.set_pos(70, 3);
  floatToString(maxCurrent,str,CURRENT);
  oled.print(str);
  
  // line 5 - power
  oled.set_pos(45, 4);
  floatToString(powerConsumption,str,POWER);
  oled.print(str);

  // line 6 - time
  oled.set_pos(80, 5);
  sprintf(str,"%02d:%02d:%02d",hour,min,sec);
  oled.print(str);

  // line 8 - threshold
  oled.set_pos(70, 7);
  floatToString(ignoreCurrent,str,CURRENT);
  oled.print(str);
}