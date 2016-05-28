#pragma config(Sensor, S1,     leftLight,      sensorLightActive)
#pragma config(Sensor, S3,     rightLight,     sensorLightActive)
#pragma config(Sensor, S2,     sonar,               sensorSONAR)
#pragma config(Motor,  motorA,          leftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,          rightMotor,    tmotorNXT, PIDControl, driveRight, encoder)

/*    MOTORS & SENSORS:                                                                                   *|
|*    [I/O Port]              [Name]              [Type]              [Description]                       *|
|*    Port C                  motorC              NXT                 Right motor                         *|
|*    Port B                  motorB              NXT                 Left motor                          *|
\*---------------------------------------------------------------------------------------------------4246-*/

#define SAMPLES 50
#define BASESPEED 25
#define WHITE 40 //corresponding to higher color sensors
#define BLACK 7 //light sensor black reading value
#define MAX_DIST 90 //maximum distance for the robot to read
#define MIN_DIST 3 //minimum distance for the robot to read
#define BREAKOUTANGLE 22 //if the robot reads this angle from the gyro, breakout from line
#define FULL 120

int GREEN = 33; //The Green/Gray color of the line pattern carpet floor tiles

int rSpeed, lSpeed = 0;
int distance;
bool lineFound = false; //flag for priority if line is found
int leftLumenance, rightLumenance; //reading values for light sensors
bool objectFound = false; //flag for priority if object is found
bool looking = true;
bool feeding = false;
bool dead = false;
int energyLevel;
int WhGreen = (WHITE + GREEN)/2 ; //46, The color that keeps on the line of white and green

/*
*Function used to show the user what values are being given
*Parameters: two integers
*returns: nothing
*/
void showDisplay(int x, int y)
{
	eraseDisplay(); //reset display
	nxtDisplayTextLine(0, "Gyro = %d", x); //what are you trying to do with this?
	nxtDisplayTextLine(1, "%d", y);
}

task energyRate()
{
	while(true)
	{
		if(dead)
			break;
		else
		{
			while(FEEDING && energyLevel<=FULL)
			{
				energyLevel++;
				wait1Msec(1000);
			}
			while(!FEEDING && !dead)
			{
				energyLevel--;
				wait1Msec(2000);
			}
		}
	}
}

/*
*Function to make the robot do a drunk-sailor walk
*Parameters: none
*returns: none
*/
void drunkTurn()
{
	if(objectFound)
	{
		return; //we want to break out if an object is found
	}
	//get direction and duration
	int Dir = random[2];
	//time is for turning duration, time2 is for the forward duration.
	int turnDuration = (random[5]+1) * 350;
	int forwardDuration = (random[3]+1) * 100;

	//initial turn off-course
	if(Dir==1) {
		rSpeed = BASESPEED+15;
		lSpeed = BASESPEED;
	} else
	{
		lSpeed = BASESPEED;
		rSpeed = BASESPEED+15;
	}

	//move straight
	wait1Msec(turnDuration);
	lSpeed = BASESPEED;
	rSpeed = BASESPEED;
	wait1Msec(forwardDuration);

	//Turn back to the forward position
	if(Dir==1) {
		lSpeed = BASESPEED;
		rSpeed = BASESPEED+15;
	} else
	{
		lSpeed = BASESPEED+15;
		rSpeed = BASESPEED;
	}
	wait1Msec(turnDuration);
	//set mototrs back to normal speed, straight ahead.
	lSpeed = BASESPEED;
	rSpeed = BASESPEED;
}

/**
*Task that runs continuously and calls drunkTurn function every few seconds.
**/
task invertMotorsTask()
{
	while(true)
	{
		while(!lineFound && !objectFound)
		{
			wait1Msec((random[6]+3)*250); //wait 1 to 3 seconds (250mS resolution)
			drunkTurn();
		}
	}
}

/**
*Rotates right until it gets back on the triangle
*/
void turnAround(){
	while (!(leftLumenance >= WHITE)){
		lSpeed = 50;
		rSpeed = -50;
	}
}

/**
*Rotates right about 180*
*/
void oneEighty(){
	lSpeed = 50;
	rSpeed = -50;
	sleep(1000);
}

/**robot moves around the patch,
* staying on the patch until full or task ends
*/
void feed(){
	startTask(invertMotorsTask);
	while (energyLevel < FULL) {
		nxtDisplayTextLine(0, "Food: %d", energyLevel);
		if (leftLumenance <= WhGreen || rightLumenance <= WhGreen) {
			stopTask(invertMotorsTask);
			oneEighty();
			lSpeed = BASESPEED;
			rSpeed = BASESPEED;
			sleep(400); //go forward a bit
			startTask(invertMotorsTask);
		}
	}
	playTone(10000, 10);
	feeding = false;
	looking = true;
}

int BREAKOUT = 1200;

/**
* Once a white triangle line is detected, this method will get on and follow.
* Sensor1 that detected line will move onto the white until the Sensor2
* is on the edge of white and green where it will follow.
* If Sensor1 detects green, we are going down skinny end and need to turn around.
* If Sensor2 detects a lot of white, we have reached the food circle.
* param: bool left, did left sensor detect.
*/
void followFood(bool left) {
    //int runningSpeed = MOTOR_SPEED_NORMAL-5;
    clearTimer(T3);
    bool online = true;
    bool foundFood = false;
    int smallerTurnCount = 0;
    int lastTurn = 0;
    float offset = 3.0;
    while (rightLumenance < WHITE) { //move until right gets on white
    	lSpeed = BASESPEED; //will need a way to break out eventuallu
    	rSpeed = BASESPEED;
    }
    doOver: //saving time here

    // quickly align right before beginning
    while (rightLumenance > WhGreen - offset) {
    	lSpeed = BASESPEED * 2;
      rSpeed = BASESPEED * 0.3;
    }

    while (online) {
    	// -1 * ((.1 * sonarAvg - 10)*(.1 * sonarAvg - 10)) + 100;
	  	displayCenteredBigTextLine(7, "current: %d", leftLumenance);
				clearTimer(T1); //wrong way timer
				clearTimer(T2); //on food patch timer
        while (rightLumenance < WhGreen - offset) {	// off right, too much green
        	lSpeed = BASESPEED * 0.3;
          rSpeed = BASESPEED;
          if (leftLumenance < WHITE) { //left is off white?
          	if (time1(T2) > 200) { //went off the wrong way
          		turnAround();
          		goto doOver;
          		//online = false; TODO: Are we stuck in a loop?
          		//break;
          	}
          }
          else {
          	clearTimer(T2);
          }

        }
        clearTimer(T1);
				clearTimer(T2);
        while (rightLumenance > WhGreen + offset) {	// off left, too much white
            lSpeed = BASESPEED;
          	rSpeed = BASESPEED * 0.3;
            if (leftLumenance < WHITE) { //left is off white?
          		if (time1(T2) > 200) { //went off the wrong way
          			turnAround();
          			goto doOver;
          			//break;
          		}
          	}
          	else {
          		clearTimer(T2);
          	}
            if (time1(T1) > 800){ //been on white a while, probably on food patch
            	foundFood = true;
      				online = false;
      				break;
      			}
        }
    }
  	eraseDisplay();
		nxtDisplayTextLine(2, "Found Food: %d", foundFood);
		if (foundFood) { //rotate left about 90 degrees to face the circle hopefully
			lSpeed = -50;
			rSpeed = 50;
			sleep(300);
			playTone(1000, 10);
			lSpeed = BASESPEED;
			rSpeed = BASESPEED;
			sleep(400); //go forward a bit
			feeding = true;
			feed();
		}
		else {
			looking = true;
		}

}

/**
*Task that continually sets the motor speed.
**/
task runMotorsTask()
{
	while(true){
		motor[leftMotor] = lSpeed;
		motor[rightMotor] = rSpeed;
	}
}



/**
*Task that continually samples the light sensors to find when
*either one is on black or white or neither.
**/
task lightSensorTask()
{
	int leftLums, rightLums = 0;
	int lSuccess= 0;
	int i,j;
	while(true)
	{
		lSuccess = 0;
		for(j = 0; j < 4; j++) //we need to ignore false positives
		{
			leftLums = 0;
			rightLums = 0;
			for(i = 0;i<SAMPLES;i++) //we need to find a good reading
			{
				leftLums += SensorValue[leftLight];
				rightLums += SensorValue[rightLight];
			}
			leftLumenance = leftLums/SAMPLES;
			rightLumenance = rightLums/SAMPLES;
			if(leftLumenance<=BLACK-5 || rightLumenance<=BLACK-5)
				lSuccess++;
		}
		if(lSuccess>=3)//if the reading has a 75% success rate
			lineFound = true;
	}
}

task displayColors() {
	while (true){
		nxtDisplayTextLine(0, "Left = %d",leftLumenance);
		nxtDisplayTextLine(1, "Right = %d",rightLumenance);
	}
}

task main()
{
	srand(1023030); //seeds the random number generator
	nVolume = 4; //nVolume is 0 to 4, this sets the max sound volume 4
	energyLevel = FULL / 2; //begin hungry for test
	startTask(runMotorsTask);
	startTask(invertMotorsTask);
	startTask(lightSensorTask);
	startTask(displayColors);
	startTask(energyRate);
	distance = SensorRaw[sonar]; //start with a reading
	//resetGyro(gyro);
	wait1Msec(1000);
	while(true)
	{

		if (leftLumenance>=WHITE && looking && energyLevel < 90) {
			sleep(50);
			if (leftLumenance>=WHITE) {
				looking = false; //be sure to set looking back to true if followFood tasks are interrupted
				stopTask(displayColors); //leftLumanence calculations spiking up to over a thousand at random times
				stopTask(invertMotorsTask);//every few seconds, need to double check to throw this out.

				followFood(true);
			}
		}
	}
}
