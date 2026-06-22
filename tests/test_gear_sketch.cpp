#include <Arduino.h>

#include <cassert>
#include <string>

void setup();
void loop();
bool tryGetSensorCommand();
void executeCommand();
void ReadSensorsAndRefreshVariables();
void PrintCurrentGear();
void PrintEngineTemp();
void PrintOutTemp();
void PrintOilPreasure();
void serialFlush();

#include "../GearSensor/GearSensor.ino"

int main() {
  resetFakeArduino();
  setup();
  assert(Serial.baudRate == 115200UL);
  assert(fakePinModes[DEREControlPin] == OUTPUT);
  assert(fakeDigitalValues[DEREControlPin] == LOW);

  assert(!tryGetSensorCommand());
  Serial.pushInput({0, 0});
  assert(!tryGetSensorCommand());
  Serial.pushInput({'X', 'A'});
  assert(!tryGetSensorCommand());
  Serial.pushInput({'A', 'Z'});
  assert(!tryGetSensorCommand());

  for (char validCommand : {'A', 'B', 'C', 'D', 'E'}) {
    Serial.pushInput({'A', static_cast<byte>(validCommand)});
    assert(tryGetSensorCommand());
    assert(command == validCommand);
  }

  currnetGear = 3;
  gearwarning = true;
  tempEngine = 321;
  tempOut = 123;
  oilPreasureLow = false;
  command = 'A';
  Serial.output.clear();
  executeCommand();
  assert(Serial.outputString() == "A:3:1:3:1:321:321:123:123:0:0::");
  assert(fakeDigitalValues[DEREControlPin] == LOW);

  for (char requestedCommand : {'B', 'C', 'D', 'E'}) {
    command = requestedCommand;
    Serial.output.clear();
    executeCommand();
    assert(Serial.outputString().front() == 'A');
    assert(Serial.outputString().back() == ':');
  }

  fakeAnalogValues[engoneTempSensorPin] = 456;
  fakeAnalogValues[outTempSensorPin] = 654;
  fakeDigitalValues[oilPreasurePin] = oilPreasureLowSignal;
  ReadSensorsAndRefreshVariables();
  assert(tempEngine == 456);
  assert(tempOut == 654);
  assert(oilPreasureLow);
  fakeDigitalValues[oilPreasurePin] = !oilPreasureLowSignal;
  ReadSensorsAndRefreshVariables();
  assert(!oilPreasureLow);

  Serial.pushInput({'x', 'y'});
  serialFlush();
  assert(Serial.available() == 0);

  Serial.pushInput({'A', 'B'});
  loop();
  assert(command == '1');
}
