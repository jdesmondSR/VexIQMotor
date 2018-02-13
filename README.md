# VexIQMotor
Vex IQ motor controls for Arduino usage.This library had prewriten I2C commands for easy use. If you want to write all the I2C commands on your own you can see my Vex IQ motor Doc here. 

*****DISCLAIMER*****
This library and the documentation is not official Vex products. There was a lack of official documentation about the Vex motors, so I set out to learn as much as I could on my own. I managed to discover a lot of interesting things along the way and wish to share them with anyone who is interested. This is WITHOUT ANY WARRANTY and NOT BY ANY MEANS AN OFFICIAL DOCUMENT. I simply wish to share what I have learned. 

This is a project I in order to learn more about I2C communication and the logic involved, I reverse engineered the Vex IQ motor commands and used an arduino to replicate them. I used a Saleae Logic Analyzer to see the commands being sent from the Vex IQ to the smart motors. I then used an arduino to replicate the commands and successfully control the vex motors with an arduino.


# How to use for arduino
1) Create a folder that has both the header and .c file in it. 
2) Zip/Compress the folder.
3) In Arduino go to the Sketch menu > Include Library > Add .ZIP library...
4) Select the compressed folder with the .c and .h file inside
5) Include the file into your code and you are ready to go!

API for the following commands: [a relative link](VexIQMotor API.pdf)

# List of functions you can use
## Startup
VexMotor() - No args constructor. Declares VexMotor object

int motorSetup(int enablePin, int newAddress) - initializes the VexMotor object

## Simple Movement
int setMotorSpeed(float spd) - Move the motor

int coastBrake() - Coast to a stop

int mediumBrake() - Brake and hold possition with medium power

int holdBrake() - Brake and hold the possition

int stopAllMotors() -Stop all motors with a hold brake. Pass (coast) or (medium) as arguments to change the brake mode

## Encoder Movement
long checkEncoders() - Returns the current encoder count

int resetEncoders() - Resets the current encoder count

int encoderTarget(float spd, long encTarget) - Travel to a specific encoder value 

bool motorActive() - Check if the motor is still moving to its target

## Servo Movement
int servoMode(long deg) - Operate the motor as a servo
