/*
 *  dacsHatch.hpp
 *
 *  6 - way Remote control volume based on DACS Eightch
 *
 *  Receives IR protocol data of NEC protocol using pin change interrupts
 *  Controls motorised pot via DF Robot shield model L298
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

#ifndef _DACS_HATCH_HPP
#define _DACS_HATCH_HPP

//#define DEBUG
//#define TRACE

#define IR_INPUT_PIN    		2		//digital input

#define MOTOR_POT_READ_PIN		A0		//analogue input motor pot

#define MOTOR_DIRECTION_PIN		4		//digital output
#define MOTOR_SPEED_PIN			5		//analogue output

#define MOTOR_SPEED				255		// maximum = 255
#define MOTOR_PULSE_DIVIDER		10		// milliseconds divided by to get sane counter
#define MOTOR_PULSE_DURATION	25		// total duration in ms = MOTOR_PULSE_DURATION * MOTOR_PULSE_DIVIDER

#define POT_READ_INTERVAL		50

#define IR_ADDRESS				0x38

#define IR_CMD_VOL_UP			0xC
#define IR_CMD_VOL_DOWN			0x10

#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif

struct TinyIRReceiverCallbackDataStruct {
	uint16_t Address;
	uint8_t Command;
	bool isRepeat;
	bool justWritten;
};

struct MotorPotDataStruct {
	unsigned long timer;
	bool isOn;
	bool isUp;
	int rawValue;
	float dbValue;
};

void volumeMotorDrive(bool isUp);
void volumeMotorStop();

#endif // _DACS_HATCH_HPP
#pragma once
