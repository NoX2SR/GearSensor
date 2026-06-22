#include <Arduino.h>
#include <GearSensor.h>

#include <cassert>

namespace {
constexpr int kUpPin = 2;
constexpr int kDownPin = 3;
constexpr int kNeutralPin = 4;

void releaseSensors() {
  fakeDigitalValues[kUpPin] = HIGH;
  fakeDigitalValues[kDownPin] = HIGH;
  fakeDigitalValues[kNeutralPin] = LOW;
}

void releaseChanger(GearSensor& sensor) {
  releaseSensors();
  sensor.IsGearChanging();
}

void pressAndRelease(GearSensor& sensor, int pin) {
  fakeDigitalValues[pin] = LOW;
  sensor.IsGearChanging();
  releaseChanger(sensor);
}
}  // namespace

int main() {
  resetFakeArduino();
  releaseSensors();
  GearSensor sensor(kUpPin, kDownPin, kNeutralPin, true);

  assert(fakePinModes[kUpPin] == INPUT);
  assert(fakePinModes[kDownPin] == INPUT);
  assert(fakePinModes[kNeutralPin] == INPUT);

  sensor.IsGearChanging();
  assert(sensor.GetCurrentGear() == 0);

  fakeDigitalValues[kNeutralPin] = HIGH;
  sensor.IsGearChanging();
  releaseChanger(sensor);

  pressAndRelease(sensor, kDownPin);
  assert(sensor.GetCurrentGear() == 1);

  fakeDigitalValues[kDownPin] = LOW;
  sensor.IsGearChanging();
  sensor.IsGearChanging();
  assert(sensor.GetCurrentGear() == 1);
  releaseChanger(sensor);

  for (int expected = 2; expected <= 5; ++expected) {
    pressAndRelease(sensor, kUpPin);
    assert(sensor.GetCurrentGear() == expected);
  }

  pressAndRelease(sensor, kUpPin);
  assert(sensor.GetCurrentGear() == 5);
  assert(sensor.GetGearWarning());
  sensor.ResetGearWarning();
  assert(!sensor.GetGearWarning());
  sensor.ResetGearWarning();

  pressAndRelease(sensor, kDownPin);
  assert(sensor.GetCurrentGear() == 4);

  fakeDigitalValues[kNeutralPin] = HIGH;
  sensor.IsGearChanging();
  assert(sensor.GetCurrentGear() == 0);
  releaseChanger(sensor);

  pressAndRelease(sensor, kUpPin);
  assert(sensor.GetCurrentGear() == 2);

  GearSensor neutralLowSensor(5, 6, 7, false);
  fakeDigitalValues[5] = HIGH;
  fakeDigitalValues[6] = HIGH;
  fakeDigitalValues[7] = LOW;
  neutralLowSensor.IsGearChanging();
  assert(neutralLowSensor.GetCurrentGear() == 0);
}
