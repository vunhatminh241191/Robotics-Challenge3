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

task watchForGradient();

#define SAMPLES 50
#define BASESPEED 25
#define WHITE 40 //corresponding to higher color sensors
#define BLACK 7 //light sensor black reading value
#define MAX_DIST 90 //maximum distance for the robot to read
#define MIN_DIST 3 //minimum distance for the robot to read
#define BREAKOUTANGLE 22 //if the robot reads this angle from the gyro, breakout from line
#define FULL 120
#define HALFSPEED BASESPEED/2

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

task energyRate()
{
	while(true)
	{
		if(dead)
			break;
		else
		{
			while(feeding && energyLevel<=FULL)
			{
				energyLevel++;
				wait1Msec(1000);
			}
			while(!feeding && !dead)
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
*Called during feeding, like invertMotorsTask but sways much more
*Moved the drunkTurn method into here, cannot cancel it like a task,
*which is needed when going off the patch
**/
task feedingInvertMotorsTask()
{
	while(true)
	{
		int Dir = random[2];
		//time is for turning duration, time2 is for the forward duration.
		int turnDuration = (random[5]+1) * 350;

		//initial turn off-course
		if(Dir==1) {
			rSpeed = HALFSPEED * 2;
			lSpeed = HALFSPEED / 2;
		} else
		{
			lSpeed = HALFSPEED / 2;
			rSpeed = HALFSPEED * 2;
		}
		int i;
		for (i = 0; i < turnDuration; i += 10) {
			sleep(10); //sleep in tiny increments so task can be stopped
		}

		//Turn back to the forward position
		if(Dir==1) {
			lSpeed = HALFSPEED / 2 ;
			rSpeed = HALFSPEED * 2;
		} else
		{
			lSpeed = HALFSPEED * 2;
			rSpeed = HALFSPEED / 2;
		}
		for (i = 0; i < turnDuration; i += 10) {
			sleep(10); //sleep in tiny increments so task can be stopped
		}
	}
}

/**
*Rotates right until it gets back on the triangle
*/
void turnAroundLeft(){
	while (!(leftLumenance >= WHITE)){
		lSpeed = 50;
		rSpeed = -50;
	}
}

/**
*Rotates left until it gets back on the triangle
*/
void turnAroundRight(){
	while (!(rightLumenance >= WHITE)){
		lSpeed = -50;
		rSpeed = 50;
	}
}

/**
*Rotates right about 180*
*Might only turn about 30*, turning is highely random
*then keeps turning until both are on white
*/
void oneEighty(){
	lSpeed = 50;
	rSpeed = -50;
	sleep(1000);
	while(leftLumenance < WHITE || rightLumenance < WHITE) {
		lSpeed = 50;
		rSpeed = -50;
	}
}

/**Robot moves around the patch,
* staying on the patch until full or task is stopped
*/
task feed(){
	startTask(feedingInvertMotorsTask);
	while (energyLevel < FULL) {
		nxtDisplayTextLine(0, "Food: %d", energyLevel);
		if (leftLumenance < WHITE || rightLumenance < WHITE) {
			stopTask(feedingInvertMotorsTask);
			oneEighty();
			lSpeed = HALFSPEED;
			rSpeed = HALFSPEED;
			sleep(50); //go forward a bit
			startTask(feedingInvertMotorsTask);
		}
	}
	playTone(1000, 10);
	feeding = false;
	looking = true;
	stopTask(feedingInvertMotorsTask);
	startTask(watchForGradient);
	startTask(invertMotorsTask); //finished with gradient following, start this task and return to normal
}

int BREAKOUT = 1200;

/**
* Once a white triangle line is detected, this method will get on and follow.
* Sensor1 that detected line will move onto the white until the Right sensor
* is on the edge of white and green where it will follow.
* If Left Sensor detects green, we are going down skinny end and need to turn around.
* If Right detects a lot of white, we have reached the food circle.
*/
task followFoodLeft() {
    //int runningSpeed = MOTOR_SPEED_NORMAL-5;
		stopTask(invertMotorsTask);
    bool online = true;
    bool foundFood = false;
    bool doOver = true;
    float offset = 3.0;
    int wrongWayCount = 0;
    while (rightLumenance < WHITE) { //move until right gets on white
    	lSpeed = BASESPEED; //will need a way to break out eventually
    	rSpeed = BASESPEED;
    }
    while (doOver) {
			online = true;

			// quickly align right before beginning
    	while (rightLumenance > WhGreen - offset) {
    		lSpeed = BASESPEED * 2;
      	rSpeed = BASESPEED * 0.3;
    	}

    	while (online) {
		  	displayCenteredBigTextLine(7, "current: %d", leftLumenance);
				clearTimer(T1); //wrong way timer
				clearTimer(T2); //on food patch timer
     	  while (rightLumenance < WhGreen - offset) {	// off right, too much green
       		lSpeed = BASESPEED * 0.3;
         	rSpeed = BASESPEED;
          if (leftLumenance < WHITE) { //left is off white?
         		if (time1(T2) > 200) { //went off the wrong way
         			turnAroundLeft();
         			online = false;
         			wrongWayCount++;
         			break;
         		}
         	}
         	else {
         		clearTimer(T2);
         	}

        }
        if (!online) {
        	break;
        }
       	clearTimer(T1);
				clearTimer(T2);
       	while (rightLumenance > WhGreen + offset) {	// off left, too much white
        	lSpeed = BASESPEED;
         	rSpeed = BASESPEED * 0.3;
          if (leftLumenance < WHITE) { //left is off white?
        		if (time1(T2) > 200) { //went off the wrong way
          		turnAroundLeft();
          		online = false;
          		wrongWayCount++;
          		break;
         		}
         	}
         	else {
         		clearTimer(T2);
         	}
           if (time1(T1) > 800){ //been on white a while, probably on food patch
           	foundFood = true;
      			online = false;
      			doOver = false; //exit loop
      			break;
     			}
       	}
   		}
   		if (wrongWayCount >= 20) {
   			doOver = false; //something has gone wrong, exit
   			break;
   		}
 		}
  	eraseDisplay();
		nxtDisplayTextLine(2, "Found Food: %d", foundFood);
		if (foundFood) { //rotate left about 90 degrees to face the circle hopefully
			lSpeed = -50;
			rSpeed = 50;
			sleep(400); //is this enough turn? Extremely insensative for some reason
			playTone(1000, 10);
			lSpeed = BASESPEED;
			rSpeed = BASESPEED;
			sleep(300); //go forward a bit
			feeding = true;
			startTask(feed);
		}
		else {
			looking = true;
			startTask(invertMotorsTask);
			startTask(watchForGradient);
		}

}

task followFoodRight() {
		stopTask(invertMotorsTask);
    bool online = true;
    bool foundFood = false;
    bool doOver = true;
    float offset = 3.0;
    int wrongWayCount = 0;
    bool check = true;
    while (check) { //move until left gets on white
    	lSpeed = BASESPEED; //TODO: will need a way to break out eventualluy
    	rSpeed = BASESPEED;
    	if (leftLumenance >= WHITE) { //check that leftLumenance spike
    		sleep(20);
    		if (leftLumenance >= WHITE) {
    			check = false;
    		}
    	}

    }
    while (doOver) {
			online = true;

			// quickly align left before beginning
    	while (leftLumenance > WhGreen - offset) {
    		lSpeed = BASESPEED * 0.3;
      	rSpeed = BASESPEED * 2;
    	}

    	while (online) {
		  	displayCenteredBigTextLine(7, "current: %d", rightLumenance);
				clearTimer(T1); //wrong way timer
				clearTimer(T2); //on food patch timer
     	  while (leftLumenance < WhGreen - offset) {	// off left, too much green
       		lSpeed = BASESPEED;
         	rSpeed = BASESPEED * 0.3;
          if (rightLumenance < WHITE) { //right is off white?
         		if (time1(T2) > 200) { //went off the wrong way
         			turnAroundRight();
         			online = false;
         			wrongWayCount++;
         			break;
         		}
         	}
         	else {
         		clearTimer(T2);
         	}

        }
        if (!online) {
        	break;
        }
       	clearTimer(T1);
				clearTimer(T2);
       	while (leftLumenance > WhGreen + offset) {	// off right, too much white
        	lSpeed = BASESPEED * 0.3;
         	rSpeed = BASESPEED;
          if (rightLumenance < WHITE) { //right is off white?
        		if (time1(T2) > 200) { //went off the wrong way
          		turnAroundRight();
          		online = false;
          		wrongWayCount++;
          		break;
         		}
         	}
         	else {
         		clearTimer(T2);
         	}
           if (time1(T1) > 800){ //been on white a while, probably on food patch
           	foundFood = true;
      			online = false;
      			doOver = false; //exit loop
      			break;
     			}
       	}
   		}
   		if (wrongWayCount >= 20) {
   			doOver = false;
   			break;
   		}
 		}
  	eraseDisplay();
		nxtDisplayTextLine(2, "Found Food: %d", foundFood);
		if (foundFood) { //rotate right about 90 degrees to face the circle hopefully
			lSpeed = 50;
			rSpeed = -50;
			sleep(400); //is this enough turn? Extremely insensative for some reason
			playTone(1000, 10);
			lSpeed = BASESPEED;
			rSpeed = BASESPEED;
			sleep(300); //go forward a bit
			feeding = true;
			startTask(feed);
		}
		else {
			looking = true;
			startTask(invertMotorsTask);
			startTask(watchForGradient);
		}

}

task watchForGradient(){
	while (looking) {
	if (leftLumenance>=WHITE && looking && energyLevel < 90) {
		sleep(50);
		if (leftLumenance>=WHITE) {
			looking = false; //be sure to set looking back to true if followFood tasks are interrupted
			 /*leftLumanence calculations spiking up to over a thousand at random times
			every few seconds, need to double check to throw it out.*/
			startTask(followFoodLeft);
			}
		}
		if (rightLumenance>=WHITE && looking && energyLevel < 90) {
			sleep(50);
			if (rightLumenance>=WHITE) {
				looking = false; //be sure to set looking back to true if followFood tasks are interrupted
				/*rightLumenance didn't look like it was spiking but I'm double checking anyway*/
				startTask(followFoodRight);
			}
		}
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
	startTask(watchForGradient);
	distance = SensorRaw[sonar]; //start with a reading
	//resetGyro(gyro);
	wait1Msec(1000);
	while(true)
	{
		sleep(10);
	}
}
