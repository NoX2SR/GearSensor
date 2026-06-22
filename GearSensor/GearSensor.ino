#include <Arduino.h>
#include <GearSensor.h>

#define sensorID 'A'
#define separator ':'
#define gearUpPin 2
#define gearDownPin 3
#define gearNeutralPin 4
#define isNeutralHigh 1

#define oilPreasurePin A2
#define oilPreasureLowSignal 1

#define engoneTempSensorPin A0
#define outTempSensorPin A1

#define DEREControlPin 8 //RS-485 Controling Pin

#define BoundRate 115200

GearSensor gearSenzor(gearUpPin, gearDownPin, gearNeutralPin, isNeutralHigh);

int currnetGear = 0;
bool gearwarning = false;

int tempEngine = 0;
int tempOut = 0;

bool oilPreasureLow = true;

char command = '1';

bool tryGetSensorCommand();
void executeCommand();
void ReadSensorsAndRefreshVariables();
void PrintCurrentGear();
void PrintEngineTemp();
void PrintOutTemp();
void PrintOilPreasure();
void serialFlush();

void setup()
{
  Serial.begin(BoundRate);
  pinMode(DEREControlPin, OUTPUT);//DE/RE Controling pin of RS-485
  digitalWrite(DEREControlPin, LOW); //DE/RE=LOW Receive Enabled M1
}

void loop()
{
  ReadSensorsAndRefreshVariables();

  if (tryGetSensorCommand())
  {
    executeCommand();
    gearSenzor.ResetGearWarning();
    command = '1';
  }
  serialFlush();
}

bool tryGetSensorCommand()
{
  byte receivedBytes[2] {0,0};
  int i = 0;
  while (Serial.available() > 0 && i < 2)
  {
    receivedBytes[i] = Serial.read();
    i++;
  }
  if (receivedBytes[0] == 0 && receivedBytes[1] == 0)
  {
    return false;
  }

  if(receivedBytes[0] != sensorID)
  {
    return false;
  }
  
  if ((receivedBytes[1] == 'A') ||
      (receivedBytes[1] == 'B') ||
      (receivedBytes[1] == 'C') ||
      (receivedBytes[1] == 'D') ||
      (receivedBytes[1] == 'E'))
  {
    command = receivedBytes[1];
    return true;
  }
  return false;
}

void executeCommand()
{
  digitalWrite(DEREControlPin, HIGH); //DE/RE=HIGH Transmit enable
  delay(1);
  Serial.print(sensorID);
  Serial.print(separator);
  switch (command)
  {
    case 'A':
      PrintCurrentGear();
      PrintEngineTemp();
      PrintOutTemp();
      PrintOilPreasure();
      break;
    case 'B':
      PrintCurrentGear();
      break;
    case 'C':
      PrintEngineTemp();
      break;
    case 'D':
      PrintOutTemp();
      break;
    case 'E':
      PrintOilPreasure();
      break;
  }
  Serial.print(separator);
  Serial.flush();
  digitalWrite(DEREControlPin, LOW); //DE/RE=LOW Receive Enabled
}

void ReadSensorsAndRefreshVariables()
{
  //Temeperature section
  tempEngine = analogRead(engoneTempSensorPin);
  tempOut = analogRead(outTempSensorPin);
  //Gears section
  gearSenzor.IsGearChanging();
  currnetGear = gearSenzor.GetCurrentGear();
  gearwarning = gearSenzor.GetGearWarning();

  //Oil preasure section
  if (digitalRead(oilPreasurePin) == oilPreasureLowSignal)
  {
    oilPreasureLow = true;
  }
  else
  {
    oilPreasureLow = false;
  }
}

void PrintCurrentGear()
{
  String gear2send = String(currnetGear);
  String warning2send = String(gearwarning);
  Serial.print(gear2send);
  Serial.print(separator);
  Serial.print(warning2send);
  Serial.print(separator);
  //Twice to chack if there is error in received data
  Serial.print(gear2send);
  Serial.print(separator);
  Serial.print(warning2send);
  Serial.print(separator);
  //Serial.print('\n');
}

void PrintEngineTemp()
{
  String toSend = String(tempEngine);
  Serial.print(toSend);
  Serial.print(separator);
  //Twice to chack if there is error in received data
  Serial.print(toSend);
  Serial.print(separator);
}

void PrintOutTemp()
{
  String toSend = String(tempOut);
  Serial.print(toSend);
  Serial.print(separator);
  //Twice to chack if there is error in received data
  Serial.print(toSend);
  Serial.print(separator);
}

void PrintOilPreasure()
{
  String toSend = String(oilPreasureLow);
  Serial.print(toSend);
  Serial.print(separator);
  //Twice to chack if there is error in received data
  Serial.print(toSend);
  Serial.print(separator);
}

void serialFlush(){
  while(Serial.available() > 0) {
    (void) Serial.read();
  }
} 
