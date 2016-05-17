#pragma config(Sensor, S1, LightL, sensorEV3_Color, modeEV3Color_Reflected)
#pragma config(Sensor, S2, LightR, sensorEV3_Color, modeEV3Color_Reflected)
#pragma config(Sensor, S3, Sonar, sensorEV3_Ultrasonic)
#pragma config(Sensor, S4, gyro, sensorEV3_Gyro)
#pragma config(Motor, motorB, leftMotor, tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor, motorC, rightMotor, tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)

/* Gabrielle Glynn, Taylor Rowland, Michael Hedges
 * TCSS 437 - Spring 2016
 * Robot Challenge 2
 *
 * This program implements three behaviors: wandering (or drunk walk), trail
 * following (or line follow), and object detection/approach/investigation.
 * When wandering, the robot will explore the mat area with a biased "walk."
 * When the robot senses a line on the mat area it will override the wander
 * behavior and follow the line. At the end of the of the line it should beep
 * and then resume the wander behavior. Lines are detected using light sensors.
 * Finally, if the robot senses an object within 3 feet, it should abandon
 * the previous two behaviors and approach at a speed proportional to the
 * distance (slower as it gets closer to the object) and stop as close to the
 * object as possible without touching it. Then it should back up, turn, and
 * resume the wander.
 *
 * NOTE: Line follow behavior is not fully working.
 */

/*=======================
 *      Definitions
 *=======================*/
#define DEFAULT_SPEED 25
#define DEFAULT_CORNER 32
#define LF_DEF_SPEED 25
#define MIN_SPEED 5
#define MAX_DIFF 20
#define MAX_CHANGE 15
#define MIN_CHANGE 3

#define DETECTRANGE 100 //the range in mm at which the sonar will signal the bot to chase an obsticle
#define ALPHA 0.15 //The porportional value in our weighted average equation
#define ALPHA_LIGHT 0.35
#define OFFSET 4 //The distance between the sensor and the nose of the robot
#define STOPTOLLERANCE 1 //If the ultrasound reads less than this,
//the bot will stop, beleiveing it to have reached the obsticle

#define MAX_DEGREE 45
#define BLACK_RANGE 6
#define WHITE_RANGE 6

/*=========================
 *     Global Variables
 *=========================*/
// values for drunk walk
int cornerSpeed;
int driveSpeed;
int LSpeed;
int RSpeed;

bool foundObj;	// flag indicating if the sonar has found an obsticle
bool lineFound; // flag indicating a black line has been found on the ground

int whiteCal;	// value of white surface at time of program running
int blackCal;	// value of black sureface at time of program running
int polledLLight; // value of current left light sensor (0 to 100)
int polledRLight; // value of current right light sensor (0 to 100)
int side;	// keeps track of where line in in relation to robot

// speed Values
int sonarSpeedL;
int sonarSpeedR;
int drunkSpeedL;
int drunkSpeedR;
int lineSpeedL;
int lineSpeedR;

/*=========================
 *    Prototypes
 *=========================*/
task objectDetect();
task pollLightSensors();
task lineFollow();
task drunkWalk();

void sonarForward(int lMot, int rMot);
void pollLightSensorsForCalibration();
void calibrateLight();
void setLineSide();
void displayCalibratedWhiteAndBlackValues();
void displayWhiteAndBlackValues();
void drunkTurn();
int getSpeedChange();
int setNewSpeed(int speed, int change);

task main() {
	// initialization
	foundObj = false;
	lineFound = false;
	int State = 0;

	// calibrates light sensors
	calibrateLight();

	// begin regular operation
	startTask(drunkWalk);
	startTask(pollLightSensors);
	startTask(lineFollow);
	startTask(objectDetect);

	while(true) {
		if (foundObj) { //if an obsticle is seen, chase the obstacle
			State = 1;
		} else if(lineFound){ //if a line is found, follow the line
				State = 2;
		} else { 		//otherwise, wander
			State = 0;
		}
		switch(State) {
			case 0: // wander
				drunkSpeedL = LSpeed;
				drunkSpeedR = RSpeed;
				setMotorSpeed(leftMotor, drunkSpeedL);
				setMotorSpeed(rightMotor, drunkSpeedR);
				break;
			case 1: // seek object
				setMotorSpeed(leftMotor, sonarSpeedL);
				setMotorSpeed(rightMotor, sonarSpeedR);
				break;
			case 2: // follow lines
				setMotorSpeed(leftMotor, lineSpeedL);
				setMotorSpeed(rightMotor, lineSpeedR);
				break;
		}
	}
}

/**
Handles the investigate object behavior (FSM state 1)
suggests setting motor speeds to sonarSpeedL and sonarSpeedR
*/
task objectDetect() {
	//initialize
	float distCur = 0; // Xt; The current sensor reading
	float distPri = 0; // Xt-1; the average of our previous readings
	float distNew = 0; /* Yt; what we are reporting as our
							  curent reading after processing */
	//run
	while(true){
		//check to see if object found
		distCur = SensorValue[S3] - OFFSET;
		distNew = distCur + ALPHA * (distPri - distCur);
		distPri = distNew;

		if (distNew < DETECTRANGE) {
			foundObj = true;
		} else {
			foundObj = false;
		}

		//if object found, approach at porportional speed
		if(foundObj){
			eraseDisplay();
			displayBigTextLine(3, "%d cm", distNew);
			if(distNew > 80){
				sonarForward(100,100); /* if object is far,
										  approach at max speed */
			}else if (distNew > STOPTOLLERANCE){
				sonarForward(distNew/2, distNew/2);
			} else {
				sonarForward(0,0);//found it!
			}
			sleep(30);
		}
		// if object not found, look for object
		else {
			sonarForward(25, 25);
			sleep(30);
		}
	}
}

/**
Handles the task of continually polling the left and right light
sensors in order to get an updated reading of whether the robot
is on a black or white surface.
*/
task pollLightSensors() {

	float lLightCur = 0; // Xt; The current sensor reading
	float lLightPri = 0; // Xt-1; the average of our previous readings
	float lLightNew = 0; /* Yt; what we are reporting as our
							curent reading after processing */

	float rLightCur = 0; // Xt
	float rLightPri = 0; // Xt-1
	float rLightNew = 0; // Yt

	while(true) {
		lLightCur = SensorValue[S1];
		lLightNew = lLightCur + ALPHA_LIGHT * (lLightPri - lLightCur);
		lLightPri = lLightCur;

		rLightCur = SensorValue[S2];
		rLightNew = rLightCur + ALPHA_LIGHT * (rLightPri - rLightCur);
		rLightPri = rLightCur;

		polledLLight = lLightNew;
		polledRLight = rLightNew;
	}
	return;
}


/**
Handles the line or "trail" following behavior (FSM state 2)
suggests setting motor speeds to lineSpeedL and lineSpeedR
*/
task lineFollow() {
	// initialization
	int blackMax;
	int whiteMin;
	int enterDegree;
	blackMax = blackCal + BLACK_RANGE;
	whiteMin = whiteCal - WHITE_RANGE;

	while (true) {
		// checks if either light sensor senses black
		if (polledLLight <= blackMax || polledRLight <= blackMax) {
			lineFound = true;
			setLineSide(); /* sets side to 0 (on robot's left)
							  or 1 (on robot's right) */
		}
		while(lineFound) {
			while ((side == 0 && polledLLight <= blackMax && polledRLight >= whiteMin)
				|| (side == 1 && polledLLight >= whiteMin && polledRLight <= blackMax)) {
				lineSpeedL = LF_DEF_SPEED;
				lineSpeedR = LF_DEF_SPEED;
			}
			resetGyro(S4);
			enterDegree = getGyroHeading(S4);
			while (((side == 0 && polledLLight > blackMax)
				  || (side == 1 && polledLLight < whiteMin))
				  && lineFound == true) {
				// turn left
				lineSpeedL = -1;
				lineSpeedR =  10;
				if((abs(enterDegree - getGyroHeading(S4))) > MAX_DEGREE) {
					lineFound = false;
					playSound(soundBlip);
				}
			}
			while (((side == 0 && polledRLight < whiteMin)
				  || (side == 1 && polledRLight > blackMax))
			      && lineFound == true) {
				// turn right
				lineSpeedL =  10;
				lineSpeedR = -1;
				if(abs(enterDegree - getGyroHeading(S4)) > MAX_DEGREE) {
					lineFound = false;
					playSound(soundBlip);
				}
			}
		}
	}
}

/**
Handles the drunken walk behavior (FSM state 0)
suggests setting motors to drunkSpeedL and drunkSpeedR
*/
task drunkWalk() {
	//initialization
	driveSpeed = DEFAULT_SPEED;
	cornerSpeed = DEFAULT_CORNER;
	drunkSpeedL = 30;
	drunkSpeedR = 30;
	int speed_change;
	bool is_drive = true;

    while(true) {
    // get randomized number to change speed of a motor
		speed_change = getSpeedChange();
		//display if active
		if (!foundObj && !lineFound) {
			eraseDisplay();
			displayBigTextLine(3,"I'm drunk");
		}
		// alternates changing speed of cornering and driving
		if (is_drive == true){
			is_drive = false;
			driveSpeed = setNewSpeed(driveSpeed, speed_change);
		} else {
			is_drive = true;
			cornerSpeed = setNewSpeed(cornerSpeed, speed_change);
		}
		if (abs(cornerSpeed - driveSpeed) > MAX_DIFF) {
			cornerSpeed = DEFAULT_CORNER;
			driveSpeed = DEFAULT_SPEED;
		}
		sleep(300);
		drunkTurn();
    }
}


/**
A helper method for quickly setting the speed of both motors
lMot = the speed of the left motor. Expect an int from -100 to 100
where 0 is stopped, -100 is full reverse, and 100 is ahead full.
rMot = the speed of the right motor. Expect an int from -100 to 100
where 0 is stopped, -100 is full reverse, and 100 is ahead full.
*/
void sonarForward(int lMot, int rMot) {
		sonarSpeedL = lMot;
    sonarSpeedR = rMot;
}

/**
Method to set the max and min light readings of surface.
Instructions are displayed on robot screen.
*/
void calibrateLight() {
	eraseDisplay();
	displayCenteredBigTextLine(2, "TO CALIBRATE");
	displayCenteredBigTextLine(5, "LINE ON RIGHT");
	waitForButtonPress();

	displayCenteredBigTextLine(2, "CALIBRATING...");
	sleep(20);
	pollLightSensorsForCalibration();
	sleep(500);
	eraseDisplay();
	flushButtonMessages();

	displayCenteredBigTextLine(2, "PRESS ENTER TO");
	displayCenteredBigTextLine(5, "CONTINUE");
	waitForButtonPress();

	eraseDisplay();
	flushButtonMessages();

	return;
}

/**
Method to average a high number of light sensor values for
when the light sensors are in one place in order to get an accurate
reading for calibration of surface values.
*/
void pollLightSensorsForCalibration() {

	float lLightCur = 0; // Xt; The current sensor reading
	float lLightPri = 0; // Xt-1; the average of our previous readings
	float lLightNew = 0; /* Yt; what we are reporting as our
						    curent reading after processing */

	float rLightCur = 0; // Xt
	float rLightPri = 0; // Xt-1
	float rLightNew = 0; // Yt
	int i;
	int numPoll = 500;

	for(i = 0; i < numPoll; i++) {
		lLightCur = SensorValue[S1];
		lLightNew = lLightCur + ALPHA_LIGHT * (lLightPri - lLightCur);
		lLightPri = lLightCur;

		rLightCur = SensorValue[S2];
		rLightNew = rLightCur + ALPHA_LIGHT * (rLightPri - rLightCur);
		rLightPri = rLightCur;
	}

	whiteCal = lLightNew;
	blackCal = rLightNew;

	return;
}

/**
Helper method that sets global variable side (0 for line on left and
1 for line on right of robot) when the robot encounters readings within
the black threshold.
*/
void setLineSide() {
	if (polledLLight < polledRLight) {
		side = 0;
	} else if (polledRLight < polledLLight) {
		side = 1;
	} else {
		if(random(100)%2 == 0) {
			resetGyro(S4);
			while(getGyroHeading(S4) < 90) {
				lineSpeedL = -25;
				lineSpeedR = 25;
			}
		} else {
			resetGyro(S4);
			while(getGyroHeading(S4) < 90) {
				lineSpeedL = 25;
				lineSpeedR = -25;
			}
		}
		lineFound = false;
	}
	return;
}

/**
Method used to display calibrated light sensor values for debugging.
*/
void displayCalibratedWhiteAndBlackValues() {
	displayCenteredBigTextLine(3, "whiteValue: %d", whiteCal);
	displayCenteredBigTextLine(6, "blackValue: %d", blackCal);
	sleep(20);

	return;
}

/**
Method used to display current light sensor values for debugging.
*/
void displayWhiteAndBlackValues() {
	eraseDisplay();
	displayBigTextLine(8, "Left: %d", polledLLight);
	displayBigTextLine(10, "Right: %d", polledRLight);
	sleep(20);

	return;
}

/**
A helper method for the drunken walk behavior.
turn off course some arbitrary random amount, then go straight, then
turn back to the original orientation. An "S" bend gets performed, effectively.
*/
void drunkTurn() {
	//get direction and duration
	int Dir = random(100) % 2;

	//time is for turning duration, time2 is for the forward duration.
	int turnDuration = (random(100)%5+3) * 350;
	int forwardDuration = (random(100)%5+5) * 100;

	//initial turn off-course
	if(Dir) {
		LSpeed = cornerSpeed;
		RSpeed = driveSpeed;
		} else {
		LSpeed = driveSpeed;
		RSpeed = cornerSpeed;
	}
	setLEDColor(ledRed);

	//move straight
	wait1Msec(turnDuration);
	LSpeed = driveSpeed;
	RSpeed = driveSpeed;
	setLEDColor(ledOrange);
	wait1Msec(forwardDuration);

	//Turn back to the forward position
	if(Dir) {
		LSpeed = driveSpeed;
		RSpeed = cornerSpeed;
		} else {
		LSpeed = cornerSpeed;
		RSpeed = driveSpeed;
	}
	setLEDColor(ledGreen);
	wait1Msec(turnDuration);
	//set mototrs back to normal speed, straight ahead.
	LSpeed = driveSpeed;
	RSpeed = driveSpeed;
	setLEDColor(ledOff);
}

/**
Returns an amount that is randomly between MIN_CHANGE and
MAX_CHANGE and is positive or negative (with 50% chance of either).
*/
int getSpeedChange() {
	int speed_change = MAX_CHANGE - abs(random(MIN_CHANGE));
	if(abs(random(100))%2){
		speed_change = speed_change * -1;
	}
	return speed_change;
}

/**
Changes the current speed of one motor by the variable change.
If the speed goes below a certain value it is reset to the MIN_SPEED.
*/
int setNewSpeed(int speed, int change) {
	speed += change;
	if(speed < MIN_SPEED)
		speed = MIN_SPEED;
	return speed;
}
