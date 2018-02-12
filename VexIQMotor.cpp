#include "Arduino.h"
#include "VexIQMotor.h"
/**
 *  VexIQMotor.cpp -- Source file for the VexIQMotor library
 *  Created by Jeremy Desmond, January 6, 2018
 */


byte numMotors = 0;
VexMotor *motorList[LIST_SIZE];

/*
 Initial broadcast to all devices upon turning on.
 This NEEDS to be the first commands sent to all motors
 after the enable pins are set to high and output.
 
 DO NOT RESEND this broadcast after the motors have been
 initialized, as this will reset them back to an address
 of 0x60
 */
int initBroadcast() {
    Wire.beginTransmission(0x00);
    Wire.write(0x4E);
    Wire.write(0xCA);
    Wire.write(0x03);
    int err = Wire.endTransmission();
    delay(5);
    return err;
}

/*
 Stop all the motors with one call. The only parameter is a brake mode
 selection that by default is set to hold. Other options include medium
 and coast.
 */

int stopAllMotors(enBrakeMode option) {
    int error;
    if (option == hold)
    {
        for (int i = 0; i < numMotors; i++)
        {
            error = motorList[i]->holdBrake();
            if (error)
                return error;
        }
    }
    else if (option == coast)
    {
        for (int i = 0; i < numMotors; i++)
        {
            error = motorList[i]->coastBrake();
            if (error)
                return error;
        }
    }
    else
    {
        for (int i = 0; i < numMotors; i++)
        {
            error = motorList[i]->mediumBrake();
            if (error)
                return error;
        }
    }
    return error;
}


/**************************CLASS FUNCTIONS************************/

/*
 Constructor - sets all members to 0
 */
VexMotor::VexMotor()
{
    pin = 0;
    deviceAddress = 0;
}

/*
 Setup for the motor. Real initialization process goes on here.
 The motor is saved to the array of motors for the stop all motors function.
 Then the initVexIQDevice function is called where the motor is given a new address
 */
int VexMotor::motorSetup(int enablePin, int newAddress)
{
    pin = enablePin;
    deviceAddress = newAddress;
    if(numMotors < LIST_SIZE) {
        motorList[numMotors] = this;
        numMotors++;
        return initVexIQDevice(pin, deviceAddress);
    }
    return -1;
}



/*
 Initialization function for an individual Motor.
 USED ONLY IN THE CONSTRUCTOR
 #param enable The enable pin for the motor being initialized.
 #param address This is the new address being assigned to the motor
 #return err The error from the end transmission so you can check
 if the initialization was successful
 */
int VexMotor::initVexIQDevice(int enable, int address) {
    digitalWrite(enable, LOW);
    delayMicroseconds(107);
    digitalWrite(enable, HIGH);
    delay(5);
    int nb;
    do {
        nb = Wire.requestFrom(0x60 >> 1, 1);
    } while (nb != 1);
    //change the motor address to what you want
    Wire.beginTransmission(0x60 >> 1);
    Wire.write(0x4D);
    Wire.write(address);//new motor address
    int err = Wire.endTransmission();
    return err;
}


/*********************ALL FUNCTIONS FOR MOVEMENT AND ENCODER INFO************************/

/*
 For operating as a servo motor.
 #param deg The possition in degrees that the motor will hold
 #return error The error returned from the end transmissions
 
 EXAMPLE: leftMotor.servoMode(45);
 */
int VexMotor::servoMode(long deg) {
    deg = map(deg, 0, 180, 0, 480);
    byte b1 = (deg >> 16) & 0xFF;
    byte b2 = (deg >> 8 ) & 0xFF;
    byte b3 = deg & 0xFF;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(0x3F);//setting the speed
    int error = Wire.endTransmission();
    if (error != 0) return error;
    delay(50);
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2C);
    Wire.write(b1);
    Wire.write(b2);//sending the number of degrees to be turned and held at
    Wire.write(b3);
    error = Wire.endTransmission();
    if (error != 0) return error;
    delay(50);
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x03);//move motor in encoder mode
    error = Wire.endTransmission();
    delay(50);
    return error;
}


/*
 Series of commands that resets the current encoder count on the motor.
 #return error The error from the end transmissions (always check when debugging)
 
 EXAMPLE: leftMotor.resetEncoders();
 */

int VexMotor::resetEncoders()
{
    int error;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x00);
    error = Wire.endTransmission();
    if (error != 0) return error;
    
    delay(100);
    
    //step two(clears everything even what device address was set to)
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x4F);
    Wire.write(0x34);
    error = Wire.endTransmission();
    if (error != 0) return error;
    
    delay(100);
    
    //  //step three
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x00);
    error = Wire.endTransmission();
    if (error != 0) return error;
    
    delay(100);
    
    //step four
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x3E);
    Wire.write(0x74);
    error = Wire.endTransmission();
    if (error != 0) return error;
    
    delay(100);
    
    //step five
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(0x00);
    Wire.write(0x00);
    error = Wire.endTransmission();
    return error;
}


/*
 Returns the motors encoder count.
 #return enc The current encoder count on the motor (960 is equal to one full rotation)
 IMPORTANT: if the return value is 1, 2, 3, or 4 it is likely an error code
 from the end transmission, not the encoder count
 
 EXAMPLE: long leftEncoders = leftMotor.checkEncoders();
 */
long VexMotor::checkEncoders() {
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x32);
    int err = Wire.endTransmission();
    if (err != 0 ) return err;
    Wire.requestFrom(deviceAddress >> 1, 3);
    long high = Wire.read();//the first byte is the upper part of the number
    long middle = Wire.read();
    long low = Wire.read();
    if(high & 128)
    {
        long negByte = 255;
        long enc = (negByte << 24) + (high << 16) + (middle << 8) + low;
        return enc;
    }
    else
    {
        long enc = (high << 16) + (middle << 8) + low;
        return enc;
    }
}



/*
 Moves a given number of encoders at a specified speed.
 #param spd Speed of the motor (can be both possitive and negative)
 #param encTarget Target number of encoders the motor will turn
 #return err Error code from the end transmission
 
 EXAMPLE: leftMotor.encoderTarget(50, 960); //one rotation at 50% speed
          or
          leftMotor.encoderTarget(-50, -480); //half a rotation backwards at -50% speed
 
 */
int VexMotor::encoderTarget(float spd, long encTarget, bool stateRelative) {
    if (spd < 0 && encTarget > 0)
        encTarget *= -1;
    if (stateRelative)
        encTarget += this->checkEncoders();
    unsigned char hexValSpeed;
    byte b1 = (encTarget >> 16) & 0xFF;
    byte b2 = (encTarget >> 8 ) & 0xFF;
    byte b3 = encTarget & 0xFF;
    spd = constrain(spd, -100, 100);
    hexValSpeed = 126 * (spd / 100);
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(hexValSpeed);
    int err = Wire.endTransmission();
    if (err != 0) return err;
    delay(10);
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2C);
    Wire.write(b1);//(b1);
    Wire.write(b2);//(b2);
    Wire.write(b3);//(b3);
    err = Wire.endTransmission();
    if (err != 0) return err;
    delay(10);
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x04);
    err = Wire.endTransmission();
    delay(75);
    return err;
}

/*
 Checks the current movement status of the motor is. Best used
 in conjunction with the encoder target movement function.
 #return True if the motor is still active. False if the motor
 has come to a stop or finished the previous movment command
 
 EXAMPLE: leftMotor.encoderTarget(75, 5000);
 while(leftMotor.motorActive())
 {
 delay(5);
 //anything else you would like to do while moving
 //to the target encoder count
 }
 */
bool VexMotor::motorActive() {
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x32);
    int err = Wire.endTransmission();
    if (err != 0 ) return err;
    Wire.requestFrom(deviceAddress >> 1, 4);
    Wire.read();//dont care about these first three bytes
    Wire.read();//since they are just encoder counts
    Wire.read();
    byte motorStatus = Wire.read();//fourth byte here is an active status indicator
    delay(5);
    if (motorStatus == 0)
        return false;
    else
        return true;
}

/***********SIMPLE MOVEMENT FUNCTION**************/

/*
 Set the motor speed. Range is -100 to 100
 #param spd Speed of the motor
 #return err The error returned from the end transmission
 
 EXAMPLE: leftMotor.setMotorSpeed(100);
 rightMotor.setMotorSpeed(-35);
 delay(2000);
 */
int VexMotor::setMotorSpeed(float spd) {//sets the speed but the motor needs to be stoped still
    unsigned char hexVal;
    spd = constrain(spd, -100, 100);
    Wire.beginTransmission(deviceAddress >> 1);
    hexVal = 126 * (spd / 100);
    Wire.write(0x2A);
    Wire.write(hexVal);
    int err = Wire.endTransmission();
    if (err != 0) return err;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x05);
    err = Wire.endTransmission();
    return err;
}


/*
 Brake the motor by stopping in place and holding the position (hard brake)
 #return err The error status from the end transmission
 
 EXAMPLE: leftMotor.setMotorSpeed(50);
 delay(3000);
 leftMotor.holdBrake();
 delay(1000);
 
 */
int VexMotor::holdBrake() {
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x02);//brake mode(hold)
    int err = Wire.endTransmission();
    if (err != 0) return err;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(0x00);
    err = Wire.endTransmission();
    return err;
}


/*
 Applies a medium brake the motor (in between coast and hold)
 #return error The error status from end transmission
 
 EXAMPLE: leftMotor.setMotorSpeed(-45);
 delay(4000);
 leftMotor.mediumBrake();
 delay(1000);
 */
int VexMotor::mediumBrake() {
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x01);//brake mode(medium)
    int error = Wire.endTransmission();
    if (error != 0) return error;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(0x00);
    error = Wire.endTransmission();
    return error;
}


/*
 Cuts power to the motor letting it coast to a stop.
 #return error The error status of the end transmission
 
 EXAMPLE: leftMotor.setMotorSpeed(100);
 delay(1000);
 leftMotor.coastBrake();
 delay(500);
 */
int VexMotor::coastBrake() {
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x28);
    Wire.write(0x00);//brake mode(coast)
    int error = Wire.endTransmission();
    if (error != 0) return error;
    Wire.beginTransmission(deviceAddress >> 1);
    Wire.write(0x2A);
    Wire.write(0x00);
    error = Wire.endTransmission();
    return error;
}

