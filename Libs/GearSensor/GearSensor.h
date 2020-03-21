/*
  GearSensor.cpp - Library for detecting gear changes and displaying curent gear.
  Created by Nemanja Kljaic, January, 18th 2020.
*/
#ifndef GearSensor_h
#define GearSensor_h
#include <Arduino.h>

class GearSensor
{
    //constructor
 public:
    GearSensor(int gearUpPin, int gearDownPin, int neutralPin, bool isNeutralHigh);
    //public fucntions
	//Returns current Gear number/
	// 0 - Neutral
    int GetCurrentGear();
	bool GetGearWarning();
	void ResetGearWarning();
	//Check if there is a signal on gear sensors and 
	//shift the gear
    void IsGearChanging();
 private:
	void ShiftGearUp();
	void ShiftGearDown();
	void ShiftGearToNeutral();
	bool TryToUnlockChanger();
	bool TryToUnlockChanger2();
};
#endif





