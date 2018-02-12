/**
 *  VexIQMotor.h -- Header file for VexIQMotor library
 *  Created by Jeremy Desmond, January 6, 2018
 */

#ifndef VexIQ_Motor
#define VexIQ_Motor

#include "Arduino.h"
#include <Wire.h>

#define LIST_SIZE 20
int initBroadcast();
enum enBrakeMode {coast, medium, hold};
int stopAllMotors(enBrakeMode option = hold);

class VexMotor
{
public:
    VexMotor();
    int motorSetup(int enablePin, int newAddress);
    int initVexIQDevice(int enable, int address);
    int servoMode(long deg);
    int resetEncoders();
    long checkEncoders();
    int encoderTarget(float spd, long encTarget, bool stateRelative = true);
    int setMotorSpeed(float spd);
    int holdBrake();
    int mediumBrake();
    int coastBrake();
    bool motorActive();
private:
    int pin;
    int deviceAddress;
};

#endif
