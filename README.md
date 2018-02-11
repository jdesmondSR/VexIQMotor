# VexIQMotor
Vex IQ motor controls for Arduino usage.


This is a project I did for Storming Robots. In order to learn more about I2C communication and the logic involved, I reverse engineered the Vex IQ motors and used an arduino to replicate them. I used a Saleae Logic Analyzer to see the commands being sent from the Vex IQ to the smart motors. I then used an arduino to replicate the commands and successfully control the vex motors with an arduino.

# How to use for arduino
1) Create a folder that has both the header and .c file in it. 
2) Zip/Compress the folder.
3) In Arduino go to the Sketch menu > Include Library > Add .ZIP library...
4) Select the compressed folder with the .c and .h file inside
5) Include the file into your code and you are ready to go!

# List of functions you can use
VexMotor() - No args constructor. Declares VexMotor object

int motorSetup(int enablePin, int newAddress) - initializes the VexMotor object

int setMotorSpeed(float spd)
