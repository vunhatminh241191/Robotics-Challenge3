#pragma config(Sensor, S1,     leftLight,           sensorCOLORFULL)
#pragma config(Sensor, S2,     rightLight,           sensorCOLORFULL)
#pragma config(Sensor, S3,     sonar,               sensorSONAR)

/*    MOTORS & SENSORS:                                                                                   *|
|*    [I/O Port]              [Name]              [Type]              [Description]                       *|
|*    Port C                  motorC              NXT                 Right motor                         *|
|*    Port B                  motorB              NXT                 Left motor                          *|
\*---------------------------------------------------------------------------------------------------4246-*/

#define SAMPLES 50
#define BASESPEED 30
#define WHITE 20 //light sensor white reading value
#define BLACK 7 //light sensor black reading value
#define MAX_DIST 90 //maximum distance for the robot to read
#define MIN_DIST 3 //minimum distance for the robot to read
#define BREAKOUTANGLE 22 //if the robot reads this angle from the gyro, breakout from line

int rSpeed, lSpeed = 0;
int distance;
bool lineFound = false; //flag for priority if line is found
int leftLumenance, rightLumenance; //reading values for light sensors
bool objectFound = false; //flag for priority if object is found

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

void turnAround(){
	lSpeed = 50;
	rSpeed = -50;
	sleep(200);
}

/**
* Once a white triangle line is detected, this method will get on and follow.
* Sway left and right staying in the middle. If the sways become too narrow,
* we are going the wrong way and need to turn around.
* For testing purposes test assuming left sensor detected.
* param: bool left, did left sensor detect.
*/
void followFood(bool left) {
	bool inState = true; //flag for telling system we are following a line
	bool foundFood = false;
	//assuming left side triggered white if (left) {
	//move straight forward until right gets on white
	while(!rightLumenance>=WHITE){ //right is not white
		lSpeed = BASESPEED;
		rSpeed = BASESPEED;
	}
	clearTimer(T1);
	int g = time1[T1];
	//now both on white
	//just keep shifting for testing
	while (inState) {
		clearTimer(T1);
		//shift left until left is not on white
		while(leftLumenance>=WHITE && inState) {
			lSpeed = BASESPEED;
			rSpeed = BASESPEED * 1.5;
			if (time1[T1] > 200) { //been shifting for 200ms, we're on the circle
				playTone(1000, 100);
				foundFood = true;
				inState = false;
			}
		}
		clearTimer(T1);
		if ( !(leftLumenance>=WHITE) && !(rightLumenance>=WHITE)){
			inState = false; //we're off, get off and turn around
			turnAround();
		}
		//shift right until right is not on white
		while(rightLumenance>=WHITE && inState) {
			lSpeed = BASESPEED * 1.5;
			rSpeed = BASESPEED;
			if (time1[T1] > 200) { //been shifting for 200ms, we're on the circle
				playTone(1000, 100);
				foundFood = true;
				inState = false;
			}
		}

	}
	eraseDisplay();
	nxtDisplayTextLine(2, "Found Food: %d", foundFood);
	lSpeed = 0;
	rSpeed = 0;
	while (true){
		wait1Msec(1);
	}
}


/*

void LeftState()
{
	bool inState = true; //flag for telling system we are following a line
	bool firstTurn = false; //flag to check and see if the robot has turned once
	while(inState && !objectFound) //while we are following a line and no object is found
	{
		wait1Msec(100);
		resetGyro(gyro);
		while(leftLumenance<=BLACK && inState && !objectFound) //left is black
		{
			rSpeed = BASESPEED; //turn left
			lSpeed = -1*BASESPEED;
		}
		//Might need inState check here
		resetGyro(gyro);
		while(rightLumenance>=WHITE && inState && !objectFound) //right is white
		{
			lSpeed = BASESPEED; //turn right
			rSpeed = -1*BASESPEED;
			//if angle on turn is greater or less than max breakout angle
			if(getGyroHeading(gyro)>BREAKOUTANGLE ||-1*BREAKOUTANGLE>getGyroHeading(gyro))
			{
				if(firstTurn)
				{
					inState = false; //end of line
				}
				else
				{
					firstTurn = true; //we had a first big turn
					resetGyro(gyro);
				}
			}
		}
		//drive forward
		lSpeed = BASESPEED;
		rSpeed = BASESPEED;
	}
	playTone(1000, 50);
	//drive away from line
	rSpeed = BASESPEED+10;
	lSpeed = BASESPEED+10;
}


void RightState()
{
	bool inState = true; //we found a line
	bool firstTurn = false;
	while(inState && !objectFound) //still in the line and no object found
	{
		wait1Msec(100);
		resetGyro(gyro);
		while(rightLumenance<=BLACK && inState && !objectFound) //while right is on black
		{
			lSpeed = BASESPEED;
			rSpeed = -1*BASESPEED;
		}
		resetGyro(gyro);
		while(leftLumenance>=WHITE && inState && !objectFound) //while left is on white
		{
			rSpeed = BASESPEED;
			lSpeed = -1*BASESPEED;
			//if the angle of the gyro reading is less than or greater than the breakout angle
			if(getGyroHeading(gyro)>BREAKOUTANGLE || -1*BREAKOUTANGLE>getGyroHeading(gyro) )
			{
				if(firstTurn)
				{
					inState = false; //end of line
				}
				else
				{
					firstTurn = true; //we had a first big turn
					resetGyro(gyro);
				}
			}
		}
		//drive forward
		lSpeed = BASESPEED;
		rSpeed = BASESPEED;
	}
	PlayTone(1000, 100);
	//drive away from line
	rSpeed = BASESPEED+10;
	lSpeed = BASESPEED+10;
}
*/

/**
*Task that continually sets the motor speed.
**/
task runMotorsTask()
{
	while(true)
	{
		motor[motorB] = lSpeed; //left motor
		motor[motorC] = rSpeed; //right motor
	}
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

task main()
{
	srand(1023030); //seeds the random number generator
	nVolume = 4; //nVolume is 0 to 4, this sets the max sound volume 4
	StartTask(runMotorsTask);
	StartTask(invertMotorsTask);
	StartTask(lightSensorTask);
	distance = SensorRaw[sonar]; //start with a reading
	//resetGyro(gyro);
	wait1Msec(1000);
	while(true)
	{
		if(leftLumenance<=BLACK-5 && lineFound) //if left light sensor is on the black line
		{
			lSpeed=0;
			rSpeed=0;
			RightState(); //we want to go into this state
			if(!objectFound) //checks to see if we are exiting the object finder task, since it has priority
			{
				lineFound = false;
				wait1Msec(2000);
			}
		}
		else if(rightLumenance<=BLACK-5 && lineFound) //if right light sensor is on the black line
		{
			lSpeed=0;
			rSpeed=0;
			LeftState(); //we want to go into this state
			if(!objectFound)//checks to see if we are exiting the object finder task, since it has priority
			{
				lineFound = false;
				wait1Msec(2000);
			}
		}
	}
}
