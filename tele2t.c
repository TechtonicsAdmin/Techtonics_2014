#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     motorD,        tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C1_2,     motorE,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     motorF,        tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S1_C4_1,     motorH,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    centerServo,          tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    sideServo,            tServoStandard)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
MotorD = Spinning Test motor
MotorE = Right side drive motors
MotorF = Left side drive motors
MotorG = Flywheel Motors
MotorH = Lift
*/

#include "JoystickDriver.c"

int accelerationInterval = 1;
int threshold = 15;
int outerServoDownAngle = 150;
int outerServoUpAngle = 250;
int innerServoDownAngle = 150;
int innerServoUpAngle = 250;
int max(int a, int b) {
	return (a<b)?b:a;
}
int min(int a, int b) {
	return (a<b)?a:b;
}

int robotSpeedL = 0; // Speed of left wheels
int robotSpeedR = 0; // Speed of right wheels
bool innerServoDown = false;
bool outerServoDown = false;
bool ljb_new = true; // Was the left servo button pushed down last time the code ran
bool rjb_new = true; // Was the left servo button pushed down last time the code ran
int joyVal = 0; // Value of the higher-absolute-value joystick on controller 2 (See line 137 for use)
int flywheelSlowdownCooldown = 0;

task main() {
	while(true) {
		getJoystickSettings(joystick);

    // I. Steering
		// Part 1: Speed
		if(joy1Btn(8) == 1 && joy1Btn(7) == 0) {
			robotSpeedL = min(100, robotSpeedL + accelerationInterval);
			robotSpeedR = min(100, robotSpeedR + accelerationInterval);
		}
		else if(joy1Btn(7) == 1 && joy1Btn(8) == 0) {
			robotSpeedL = max(-100, robotSpeedL - accelerationInterval);
			robotSpeedR = max(-100, robotSpeedR - accelerationInterval);
		}
		else {
			if(robotSpeedL >= accelerationInterval) {
				robotSpeedL -= accelerationInterval;
			}
			else if(robotSpeedL <= -accelerationInterval) {
				robotSpeedL += accelerationInterval;
			}
			else {
				robotSpeedL = 0;
			}
			
			if(robotSpeedR >= accelerationInterval) {
				robotSpeedR -= accelerationInterval;
			}
			else if(robotSpeedR <= -accelerationInterval) {                                                                                                                                                                                                                                                                       
				
				robotSpeedR += accelerationInterval;
			}
			else {
				robotSpeedR = 0;
			}
		}
		// Part 2: Steering
		if(abs(joystick.joy1_x1) >= threshold) {
			if(joystick.joy1_x1 > 0) { // Going right
				robotSpeedR -= ((joystick.joy1_x1 - threshold) / (100-threshold)) * (robotSpeedR * .7);
			}
			else { // Going left
				robotSpeedL -= ((-joystick.joy1_x1 - threshold) / (100-threshold)) * (robotSpeedL * .7);
			}
		}
		// II. Servos
		if(joy1Btn(2) || joy1Btn(3)) { // Left joystick button (for outer latch)
			if(ljb_new) {
				outerServoDown = !outerServoDown;
				ljb_new = false;
			}
		}
		else {
			ljb_new = true;
		}
		
		if(joy1Btn(1) || joy1Btn(4)) { // Right joystick button (for inner latch)
			if(rjb_new) {
				innerServoDown = !innerServoDown;
				rjb_new = false;
			}
		}
		else {
			rjb_new = true;
		}
		// III. Assign values to motors and servos
		// Part 1. Motors
		motor[motorE] = robotSpeedR;
		motor[motorF] = robotSpeedL;
		
		// Part 2. Servos
 	  if(outerServoDown) {
 	  	servo[sideServo] = outerServoDownAngle; //Set servo value for lowered position
 		}
 		else {
 			servo[sideServo] = outerServoUpAngle; //Set servo value for up position
 		}
		
		if(innerServoDown) {
  	  servo[centerServo] = innerServoDownAngle;  //Set Servo value for lowered position
 	 	}
 		else {
 	    servo[centerServo] = innerServoUpAngle;  //Set servo value for up position
 	  }
 	  // IV. 2nd joystick
 	  // Part 1: Flywheel
  	if(joy2Btn(8)) {
  		motor[motorG] = 100; // Sets flywheel to 100% power
  	}
  	else if(motor[motorG] > 0) { // Code below is a bit complex. (Ask Quinn.)
  		if(flywheelSlowdownCooldown > 0) {
  			flywheelSlowdownCooldown -= 1;
  		}
  		else {
  			motor[motorG] -= 1;
  			if(motor[motorG] == 1) {
  				flywheelSlowdownCooldown = 900;
  			}
  			else {
  				flywheelSlowdownCooldown = 1;
  			}
  		}
  	}
  	// Part 2: Ball collector
  	if(joy2Btn(7)) {
  		motor[motorD] = 75; //Sets ball collector to 75% power
	  }
	  else {
	  	motor[motorD] = 0;
		}
		// Part 3: Lift
		if(max(abs(joystick.joy2_y1), abs(joystick.joy2_y2)) > threshold) {
			if(max(abs(joystick.joy2_y1), abs(joystick.joy2_y2)) == abs(joystick.joy2_y1)) {
				joyVal = joystick.joy2_y1;
			}
			else {
				joyVal = joystick.joy2_y2;
			}
			motor[motorH] = joyVal;
		}
		else {
			motor[motorH] = 0;
		}
	}
}
