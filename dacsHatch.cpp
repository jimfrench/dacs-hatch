/*
 *  dacsHatch.cpp
 *
 *  6 - way Remote control volume based on DACS Eightch
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts
 *  Controls motorised pot via DF Robot shield model L298
 *  Reads resistance of pot back in to allow for manual control
 *  Uses this feedback value to in turn vary a digital pot to give control voltage to DACS hardware
 *  Control SPI display TBD
 *
 *  Copyright (C) 2022 Jim French
 *  jim@tophat.eu.com
 *
 *  DACS Hatch is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/gpl.html>.
 *
 */

#include <Arduino.h>
#include "TinyIRReceiver.hpp"
#include "dacsHatch.hpp"

struct MotorPotDataStruct sMotorPotData;

volatile struct TinyIRReceiverCallbackDataStruct sCallbackData;

void setup() {
	Serial.begin(115200);
	delay(4000);

	Serial.println(F("START " __FILE__ " from " __DATE__));

	initPCIInterruptForTinyReceiver();

	pinMode(MOTOR_DIRECTION_PIN, OUTPUT);
	pinMode(MOTOR_SPEED_PIN, OUTPUT);

	Serial.println(
			F("Ready to receive NEC IR signals at pin " STR(IR_INPUT_PIN)));
}

void loop() {

	static unsigned long timer;
	static unsigned long readTimer;

	if (sCallbackData.justWritten) {
		sCallbackData.justWritten = false;

		if (sCallbackData.Address == IR_ADDRESS) {

			switch (sCallbackData.Command) {
			case IR_CMD_VOL_UP:
				volumeMotorDrive(true);

				if (sCallbackData.isRepeat)
					Serial.print("IR: Volume continuing up");
				else
					Serial.print("IR: Volume up");
				Serial.println();
				break;

			case IR_CMD_VOL_DOWN:
				volumeMotorDrive(false);

				if (sCallbackData.isRepeat)
					Serial.print("IR: Volume continuing down");
				else
					Serial.print("IR: Volume down");
				Serial.println();
				break;

			default:
				Serial.print(F("IR: Address=0x"));
				Serial.print(sCallbackData.Address, HEX);

				Serial.print(F(" Command=0x"));
				Serial.print(sCallbackData.Command, HEX);

				if (sCallbackData.isRepeat)
					Serial.print(" repeating");
				Serial.println();
				break;
			}
		}

	}

	else {
		if (sMotorPotData.isOn) {

			if (sMotorPotData.timer >= MOTOR_PULSE_DURATION) {

				volumeMotorStop();

				sMotorPotData.timer = 0;
			}

			else {
				if (millis() - timer >= MOTOR_PULSE_DIVIDER) {
					timer = millis();
					sMotorPotData.timer ++;

				}

			}
		}
	}

	if (millis() - readTimer >= POT_READ_INTERVAL) {
		readTimer = millis();

		sMotorPotData.rawValue = analogRead(MOTOR_POT_READ_PIN);

		Serial.print("POT READ: ");
		Serial.print(sMotorPotData.rawValue);
		Serial.println();
	}

}

void volumeMotorStop() {
	sMotorPotData.isOn = false;

	analogWrite(MOTOR_SPEED_PIN, 0);

	Serial.print("MOTORPOT: Stopped after ");
	Serial.print(sMotorPotData.timer * MOTOR_PULSE_DIVIDER);
	Serial.print(" mS delay");
	Serial.println();
}

void volumeMotorDrive(bool isUp) {
	sMotorPotData.isUp = isUp;
	sMotorPotData.isOn = true;
	sMotorPotData.timer = 0;

	digitalWrite(MOTOR_DIRECTION_PIN, isUp);

	analogWrite(MOTOR_SPEED_PIN, MOTOR_SPEED);

	Serial.print("MOTORPOT: Driving");
	Serial.println();

}

void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand,
		bool isRepeat) {
	sCallbackData.Address = aAddress;
	sCallbackData.Command = aCommand;
	sCallbackData.isRepeat = isRepeat;
	sCallbackData.justWritten = true;
}
