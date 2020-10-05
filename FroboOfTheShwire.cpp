/*
 * author: ochoa
 * Frobo's spicy code
 */

// required libraries
#include <Arduino.h>
#include <PS2X_lib.h>
#include <Adafruit_PWMServoDriver.h>

// functions
void autoRoutine();
void goForward(int x);
void goBackward(int x);
void liftArm(int x);
void lowerArm(int x);
void closeClaw(int x);
void openClaw(int x);


// drivetrain motor pins
byte leftMotorBck = 2; // bytes store values 0 to 255--perfect!
byte leftMotorFwd = 3;
byte leftPWM = 4;
byte rightMotorFwd = 5;
byte rightMotorBck = 6;
byte rightPWM = 7;

int motorPower = 0; // doesn't matter what it's initially set to

// arm motor pins
byte leftArmUp = 8;
byte leftArmDown = 9;
byte leftArmPWM = 10;
byte rightArmUp = 11;
byte rightArmDown = 12;
byte rightArmPWM = 13;

// claw servo slot
byte servo = 0;

Adafruit_PWMServoDriver servoDriverBoard;	// creating an Adafruit servo driver object

// PS2 controller pins
const byte PS2_DAT = 51;// assigning pin numbers to variables
const byte PS2_CMD = 50;
const byte PS2_ATT = 53;
const byte PS2_CLK = 52;

bool pressures = false;	// set pressures to false
bool rumble = false;	// set rumble to false

PS2X controller;		// creating a PS2X object

void setup() {

	// give the receiver time to boot up
	delay(300);

	// setting up the PS2 controller
	controller.config_gamepad(PS2_CLK, PS2_CMD, PS2_ATT, PS2_DAT, pressures, rumble);

	// configuring drivetrain motor pins
	pinMode(leftMotorBck, OUTPUT);
	pinMode(leftMotorFwd, OUTPUT);
	pinMode(leftPWM, OUTPUT);
	pinMode(rightMotorBck, OUTPUT);
	pinMode(rightMotorFwd, OUTPUT);
	pinMode(rightPWM, OUTPUT);

	// configuring arm motor pins
	pinMode(leftArmUp, OUTPUT);
	pinMode(leftArmDown, OUTPUT);
	pinMode(leftArmPWM, OUTPUT);
	pinMode(rightArmUp, OUTPUT);
	pinMode(rightArmDown, OUTPUT);
	pinMode(rightArmPWM, OUTPUT);

	// get the servo driver board rolling
	servoDriverBoard.begin();
	// analog servos run at ~60 Hz updates
	servoDriverBoard.setPWMFreq(60);

	delay(10);
	autoRoutine(); // time to call auto!!
	delay(500);

}

void loop() {

	// check for joystick positions + button pressings
	controller.read_gamepad();

	//IF the left joystick is pushed forward
	if(controller.Analog(PSS_LY) >= 0 && controller.Analog(PSS_LY) <= 120){
		// GO FORWARD in coast mode
		motorPower = map(controller.Analog(PSS_LY), 0, 120, 255, 0);
		digitalWrite(leftPWM, HIGH);
		analogWrite(leftMotorFwd, motorPower);

	// OTHERWISE if the left joystick is pulled backwards
	}else if(controller.Analog(PSS_LY) >= 140 && controller.Analog(PSS_LY) <= 255){
		// GO BACKWARD in coast mode
		motorPower = map(controller.Analog(PSS_LY), 140, 255, 0, 255);
		digitalWrite(leftPWM, HIGH);
		analogWrite(leftMotorBck, motorPower);

	// when the left joystick is in the d e a d z o n e
	}else{
		// STOP MOVIN
		analogWrite(leftMotorFwd, 0);
		analogWrite(leftMotorBck, 0);
	}

	//IF the right joystick is pushed forward
	if(controller.Analog(PSS_RY) >= 0 && controller.Analog(PSS_RY) <= 120){
		// GO FORWARD in coast mode
		motorPower = map(controller.Analog(PSS_RY), 0, 120, 255, 0);
		digitalWrite(rightPWM, HIGH);
		analogWrite(rightMotorFwd, motorPower);

	// OTHERWISE if the right joystick is pulled backwards
	}else if(controller.Analog(PSS_RY) >= 140 && controller.Analog(PSS_RY) <= 255){
		// GO BACKWARD in coast mode
		motorPower = map(controller.Analog(PSS_RY), 140, 255, 0, 255);
		digitalWrite(rightPWM, HIGH);
		analogWrite(rightMotorBck, motorPower);

	// when the right joystick is in the d e a d z o n e
	}else{
		// STOP MOVIN
		analogWrite(rightMotorFwd, 0);
		analogWrite(rightMotorBck, 0);
	}

	// if the upper d-pad button is pressed...
	if(controller.Button(PSB_PAD_UP)) {

		// ...raise it up!
		digitalWrite(leftArmUp, HIGH);
		digitalWrite(rightArmUp, HIGH);
		digitalWrite(leftArmDown, LOW);
		digitalWrite(rightArmDown, LOW);
		analogWrite(leftArmPWM, 200);
		analogWrite(rightArmPWM, 200);

	// otherwise, if the lower d-pad button is pressed...
	} else if(controller.Button(PSB_PAD_DOWN)) {

		// ...lower that sucker!
		digitalWrite(leftArmUp, LOW);
		digitalWrite(rightArmUp, LOW);
		digitalWrite(leftArmDown, HIGH);
		digitalWrite(rightArmDown, HIGH);
		analogWrite(leftArmPWM, 110);
		analogWrite(rightArmPWM, 110);

	// if neither d-pad button is pressed...
	} else {

		// ...give it juuust enough power to
		//    hold its position. ;)
		digitalWrite(leftArmUp, HIGH);
		digitalWrite(rightArmUp, HIGH);
		digitalWrite(leftArmDown, LOW);
		digitalWrite(rightArmDown, LOW);
		analogWrite(leftArmPWM, 30);
		analogWrite(rightArmPWM, 30);

	}

	// if the left bumper is pressed...
	if(controller.Button(PSB_L1)) {

		// ...close claw
		servoDriverBoard.setPWM(servo, 0, 400);

	// otherwise, if the right bumper is pressed...
	} else if(controller.Button(PSB_R1)) {

		// ...open claw
		servoDriverBoard.setPWM(servo, 0, 300);

	// otherwise...
	} else {

		// ...stay still, lil bud.
		servoDriverBoard.setPWM(servo, 0, 0);
	}

	// small delay
	delay(50);
}

// the autonomous routine
void autoRoutine() {

	delay(10);

	// close claw on ring
	closeClaw(800);

	delay(10);

	// lift up the arm!
	liftArm(870);

	delay(10);

	// to the volcano!!
	goForward(2700);

	delay(10);

	// RELEASE THE RING!!!
	openClaw(300);

	delay(10);

	// back up...
	goBackward(1100);

	delay(10);

	// and, finally, lower the arm.
	lowerArm(900);

	delay(1000);

}

// function for going forward for a certain amount of time
void goForward(int x) {

	digitalWrite(leftMotorFwd, HIGH);
	digitalWrite(rightMotorFwd, HIGH);
	digitalWrite(leftMotorBck, LOW);
	digitalWrite(rightMotorBck, LOW);
	analogWrite(leftPWM, 180);
	analogWrite(rightPWM, 180);
	delay(x);
	analogWrite(leftPWM, 0);
	analogWrite(rightPWM, 0);

}

// function for going backward for a certain amount of time
void goBackward(int x) {

	digitalWrite(leftMotorFwd, LOW);
	digitalWrite(rightMotorFwd, LOW);
	digitalWrite(leftMotorBck, HIGH);
	digitalWrite(rightMotorBck, HIGH);
	analogWrite(leftPWM, 180);
	analogWrite(rightPWM, 180);
	delay(x);
	analogWrite(leftPWM, 0);
	analogWrite(rightPWM, 0);

}

// function for lifting arm for a certain amount of time
void liftArm(int x) {

	digitalWrite(leftArmUp, HIGH);
	digitalWrite(rightArmUp, HIGH);
	digitalWrite(leftArmDown, LOW);
	digitalWrite(rightArmDown, LOW);
	analogWrite(leftArmPWM, 255);
	analogWrite(rightArmPWM, 255);
	delay(x);
	analogWrite(leftArmPWM, 30);
	analogWrite(rightArmPWM, 30);

}

// function for lowering arm for a certain amount of time
void lowerArm(int x) {

	digitalWrite(leftArmUp, LOW);
	digitalWrite(rightArmUp, LOW);
	digitalWrite(leftArmDown, HIGH);
	digitalWrite(rightArmDown, HIGH);
	analogWrite(leftArmPWM, 110);
	analogWrite(rightArmPWM, 110);
	delay(x);
	analogWrite(leftArmPWM, 0);
	analogWrite(rightArmPWM, 0);
}

// function for closing claw for a certain amount of time
void closeClaw(int x) {

	servoDriverBoard.setPWM(servo, 0, 400);
	delay(x);
	servoDriverBoard.setPWM(servo, 0, 0);
}

// function for opening claw for a certain amount of time
void openClaw(int x) {

	servoDriverBoard.setPWM(servo, 0, 300);
	delay(x);
	servoDriverBoard.setPWM(servo, 0, 0);
}
