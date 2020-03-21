/*
  GearSenosr.cpp- Library for detecting gear changes and displaying curent gear.
  Created by Nemanja Kljaic, January, 18th 2020.
*/

#include <Arduino.h>
#include <GearSensor.h>

int currentGearValue = 0; 
int gearUpPin, gearDownPin, neutralPin;
byte neutralIndicator;
byte gearsSensorIndicator = LOW;
byte lockChanger = true; //Lock changer until the neutral sensor is trigered
byte gearChanged = false; // Lock Changer until sensor is deactivated.
byte gearWarning = false; //If driver try to get higer gear than 5th

GearSensor::GearSensor(int _gearUpPin, int _gearDownPin, int _neutralPin, bool _isNeutralHigh)
{
	gearUpPin = _gearUpPin;
	gearDownPin = _gearDownPin;
	neutralPin = _neutralPin;
	
	pinMode(gearUpPin, INPUT);  
	pinMode(gearDownPin, INPUT);
	pinMode(neutralPin, INPUT);
	
	neutralIndicator = LOW;
	if(_isNeutralHigh)
	{
		neutralIndicator = HIGH;
	}
	
	
}

bool GearSensor::GetGearWarning(void)
{
	return gearWarning;
}

void GearSensor::ResetGearWarning(void)
{
	if(gearWarning)
	{
		gearWarning = !gearWarning;
	}
}

int GearSensor::GetCurrentGear(void)
{
	return currentGearValue;
}

void GearSensor::IsGearChanging(void)
{
	if(!TryToUnlockChanger())
	{
		return;
	}
	
	if(gearChanged)
	{
		if(!TryToUnlockChanger2())
		{
			return;
		}
	}
	
	if(digitalRead(gearUpPin) == gearsSensorIndicator)
	{
		ShiftGearUp();
		gearChanged = true;
	}	
	else if(digitalRead(gearDownPin) == gearsSensorIndicator)
	{
		ShiftGearDown();
		gearChanged = true;
	}		
	else if(digitalRead(neutralPin) == neutralIndicator)
	{
		ShiftGearToNeutral();
		gearChanged = true;
	}
}
//Returns False if sensor is still activated.
//Unlock changer and retun true if all sensors are releaced.
bool GearSensor::TryToUnlockChanger2(void)
{
	if(digitalRead(gearUpPin) != gearsSensorIndicator &&
	digitalRead(gearDownPin) != gearsSensorIndicator &&
	digitalRead(neutralPin) != neutralIndicator)
	{
		gearChanged = false;
		return true;
	}
	return false;
}
//Unlock Changer if Neutral sensor is trigered.
//Othervise, keep it locked and return false
bool GearSensor::TryToUnlockChanger(void)
{
	if(!lockChanger)
	{
		return true;
	}
	else if(digitalRead(neutralPin) == neutralIndicator)
	{
		lockChanger = false;
		return true;
	}
	
	return false;
}

void GearSensor::ShiftGearUp()
{
	if(currentGearValue == 5)
	{
		gearWarning = true;
		return;
	}
	if(currentGearValue == 0)
	{
		currentGearValue = 2;
		return;
	}
	
	currentGearValue++;
}

void GearSensor::ShiftGearDown()
{
	if(currentGearValue == 0)
	{
		currentGearValue = 1;
	}
	if(currentGearValue == 1)
	{
		return;
	}
	currentGearValue--;
}

void GearSensor::ShiftGearToNeutral()
{
	currentGearValue = 0;
}
