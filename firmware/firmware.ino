#include <TinyWireM.h>
#include "ssd1306.h"
#include "ina219.h"
#include "dataConverter.h"

// Devices
SSD1306 oled;
INA219 ina219;
#define THRESHOLD_POT 3
#define FUNC_1 4
#define FUNC_2 1
#define OUTPUT_INTERVAL 500
#define DEBOUNCE_TIME 50

// Var
float voltage;
float current;
float power;
float rLoad;

float maxVoltage;
float minVoltage;
float maxCurrent;
float minCurrent;

float ignoreCurrent;

int mode;
int oldMode;
char str[13];

unsigned long lastOutput;

void flashScreen()
{
  oled.fill(0x00);
  /*
  Tiny Power Monitor(38)
   */
  oled.set_pos(6,0);
  oled.print("Tiny Power Monitor v1.2");
  oled.set_pos(0, 1);
  oled.print("-------------------------");
  oled.set_pos(12, 2);
  oled.print("* ATTINY85");
  oled.set_pos(12, 3);
  oled.print("* INA219");
  oled.set_pos(12, 4);
  oled.print("* SSD1306");
  oled.set_pos(12, 5);
  oled.print("* XCL102D333CR-G");

  oled.set_pos(0, 6);
  oled.print("-------------------------");
  oled.set_pos(2,7);
  oled.print("ChipCE             GPL v3");
  delay(1000);
}

void update();
void readSensor();
void initData();
//common - call this function will auto draw 2 of the selected modes bellow
void printData();
void drawTemplate();
//for advance mode(1)
void drawDetailModeTemplate();
void printDetailData();
//for graph mode(1)
void drawGraphModeTemplate();
void printGraphData();
int graphPos;
int graphCurrentRange[] = {0,100,500,1000,2000,3000,4000};
int graphUpperCurrent;
int graphVoltageRange[] = {0,4,7,15,27,35};
int graphUpperVoltage;

void setup()
{
  initData();
  pinMode(FUNC_1,INPUT_PULLUP);
  pinMode(FUNC_2,INPUT_PULLUP);
  flashScreen();
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
  voltage = 0;
  current = 0;
  power = 0;
  rLoad = 0;

  maxVoltage = 0;
  minVoltage = 0;
  minCurrent = 0;
  maxCurrent = 0;

  mode = 0;
  oldMode = -1;

  ignoreCurrent = 0;
  lastOutput = 0;

  graphPos = 0;
  graphUpperCurrent = 100;
  graphUpperVoltage = 4;

  TinyWireM.begin();
  oled.begin();
  delay(100);
  ina219.begin();
}

void readSensor()
{
  voltage = ina219.read_bus_voltage();
  current = ina219.read_current();
  power = ina219.read_power();
  rLoad = voltage/(current/1000);

  int ignoreVal = analogRead(THRESHOLD_POT);
  ignoreCurrent = 0.5 * ignoreVal / 1024;

  //ignore current value if under threshold
  if (current <= ignoreCurrent)
  {
    current = 0;
    power = 0;
  }

  if(digitalRead(FUNC_1)==LOW)
    mode = 0;
  if(digitalRead(FUNC_2)==LOW)
  {
    if(mode == 5)
      mode = 1;
    else
      mode += 1;
    delay(DEBOUNCE_TIME);
  }
}

void update()
{
  //max voltage and current
  if (voltage > maxVoltage)
    maxVoltage = voltage;
  if (current > maxCurrent)
    maxCurrent = current;

  //min voltage & current - non-zero
  if (minCurrent == 0)
    minCurrent = current;
  if (minVoltage == 0)
    minVoltage = voltage;

  //min voltage & current
  if ((minCurrent > current) && (current > 0))
    minCurrent = current;
  if ((minVoltage > voltage) && (voltage > 0))
    minVoltage = voltage;
}


void drawTemplate()
{
  if(mode!=oldMode)
  {
    oldMode = mode;
    if(mode == 0)
      drawDetailModeTemplate();
    else
      drawGraphModeTemplate();
  }
}


void printData()
{
  if(mode != oldMode)
  {
    if(mode == 0)
      printDetailData();
    else
      printGraphData();
  }
  else
  {
    if(millis() - lastOutput > OUTPUT_INTERVAL)
    {
      lastOutput = millis();
      if(mode == 0)
        printDetailData();
      else
        printGraphData();
    }
  }
}


void drawGraphModeTemplate()
{
  //clear screen
  oled.fill(0x00);
  graphPos = 0;
  //calc graphUpperCurrent
  int pos;
  for(pos=0;pos<7;pos++)
    if(maxCurrent < graphCurrentRange[pos])
    {
      graphUpperCurrent = graphCurrentRange[pos];
      break;
    }
  //calc graphUpperVoltage
  for(pos=0;pos<6;pos++)
    if(maxVoltage < graphVoltageRange[pos])
    {
      graphUpperVoltage = graphVoltageRange[pos];
      break;
    }
  //draw template
  oled.set_pos(117,1);
  oled.print("mA");
  oled.set_pos(122,5);
  oled.print("V");

}

void printGraphData()
{
  oled.set_pos(93, 2);
  oled.print_float(current,7);

  oled.set_pos(103, 6);
  oled.print_float(voltage,5);

  oled.plot_area(graphPos,0,3,current,0,graphUpperCurrent);
  oled.plot_area(graphPos,4,3,voltage,0,graphUpperVoltage);
  graphPos ++;

  if(graphPos == 90)
    drawGraphModeTemplate();
}


void drawDetailModeTemplate()
{
  //clear screen
  oled.fill(0x00);

  // line 1
  oled.set_pos(0, 0);
  oled.print("Now");
  oled.set_pos(60, 0);
  oled.print("V");
  oled.set_pos(116, 0);
  oled.print("mA");
  
  // line 2
  oled.set_pos(0, 1);
  oled.print("-------------------------");


  // line 3
  oled.set_pos(0, 2);
  oled.print("Min");
  oled.set_pos(60, 2);
  oled.print("V");
  oled.set_pos(116, 2);
  oled.print("mA");

  // line 4
  oled.set_pos(0, 3);
  oled.print("Max");
  oled.set_pos(60, 3);
  oled.print("V");
  oled.set_pos(116, 3);
  oled.print("mA");
  
  // line 5
  oled.set_pos(0, 4);
  oled.print("Power");
  oled.set_pos(116, 4);
  oled.print("mW");

  // line 6
  oled.set_pos(0, 5);
  oled.print("R-load");
  oled.set_pos(121, 5);
  oled.print("R");

  
  // line 7
  oled.set_pos(0, 6);
  oled.print("-------------------------");

  // line 8
  oled.set_pos(0, 7);
  oled.print("Threshold");
  oled.set_pos(116, 7);
  oled.print("mA");
}

void printDetailData()
{
  // line 1 - voltage
  oled.set_pos(34, 0);
  oled.print_float(voltage,5);
  // line 1 - current
  oled.set_pos(80, 0);
  oled.print_float(current,7);
  
  // line 3 - min voltage
  oled.set_pos(34, 2);
  oled.print_float(minVoltage,5);
  // line 3 - min current
  oled.set_pos(80, 2);
  oled.print_float(minCurrent,7);

  // line 4 - max voltage
  oled.set_pos(34, 3);
  oled.print_float(maxVoltage,5);
  // line 4 - max current
  oled.set_pos(80, 3);
  oled.print_float(maxCurrent,7);
  
  // line 5 - power
  oled.set_pos(75, 4);
  oled.print_float(power,8);

  //line 6 rload
  oled.set_pos(65, 5);
  oled.print_float(rLoad,10);

  // line 8 - threshold
  oled.set_pos(80, 7);
  oled.print_float(ignoreCurrent,7);
}