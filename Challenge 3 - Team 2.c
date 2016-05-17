#pragma config(Sensor, S1,     leftLight,           sensorCOLORFULL)
#pragma config(Sensor, S2,     rightLight,           sensorCOLORFULL)
#pragma config(Sensor, S3,     sonar,               sensorSONAR)

/*    MOTORS & SENSORS:                                                                                   *|
|*    [I/O Port]              [Name]              [Type]              [Description]                       *|
|*    Port C                  motorC              NXT                 Right motor                         *|
|*    Port B                  motorB              NXT                 Left motor                          *|
\*---------------------------------------------------------------------------------------------------4246-*/

/*
*Authors: Zachary Martinez, David Anderson, Brandon Scholer
*Due Date: May 6th, 2016
*Course: TCSS437 Mobile Robotics
*Assignment: Challenge #2
*Team Name: Team 9
*/

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

/**
*This function finds the speed proportional to the distance
*Parameters: dist is the distance of the object.
*Returns: speed to distance ratio.
**/
int sonicSpeed(int dist)
{
	if(dist>=MAX_DIST)
		dist = 100;
	if(dist<MIN_DIST)
		dist = 0;
	return dist;
}

/**
*This function is called when object is no longer within range
*Parameters: None
*Returns: boolean based on object's detection
**/
bool lostTarget() //this function will
{
	rSpeed = 0; //stop both motors
	lSpeed = 0;
	wait1Msec(100);
	for(int i = 0;i<100;i++)
	{
		wait1Msec(1); //essentially wait 100ms
		rSpeed = 25; //turn left for 100ms
		lSpeed = -25;
		if(distance <= MAX_DIST) //if object within range set flag to true
			return true;
	}
	rSpeed = 0; //stop motors
	lSpeed = 0;
	wait1Msec(100);
	for(int i = 0;i<200;i++)
	{
		wait1Msec(1); //essentially wait 100ms
		rSpeed = -25; //turn right for 100ms
		lSpeed = 25;
		if(distance <= MAX_DIST) //if object within range set flag to true
			return true;
	}
	PlayTone(10000, 10);
	return false; //we lost the target
}

/**
*Function is called when an object is within range
*Parameters: none
*returns: void
**/
void targetAquired()
{
	int speed = 0;
	while(true)
	{
		if(distance>MAX_DIST)
		{
			if(!lostTarget()) //if we lose the target, check for it
			{
				break; //we lost the target, break out
			}
		}
		else if(distance<=MIN_DIST)
		{
			rSpeed = 0; //stop motors
			lSpeed = 0;
			wait1Msec(5000); //wait 5 seconds
			rSpeed=-50; //reverse motors
			lSpeed=-50;
			wait1Msec(1000); //reverse motors for 1 second
			rSpeed=50; //turn left
			lSpeed=0;
			wait1Msec(1000); //turn left for 1 second
			lSpeed=50;//drive forward
			break;
		} else
		{
			speed = sonicSpeed(distance); //if object is found, find its proportional speed
			rSpeed = speed; //set that speed to motors
			lSpeed = speed;
			PlayTone(speed*50, 5); //debugging
		}
	}
}

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

/*
*Function to check for when left sensor is on left side of line
*/
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
	PlayTone(1000, 50);
	//drive away from line
	rSpeed = BASESPEED+10;
	lSpeed = BASESPEED+10;
}

/*
*Function to check for when right sensor is on right side of line
*/
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
*Task that continually finds the distance between the robot
*and an object. It sets a flag if an object is within 100cm.
**/
task getReadingTask()
{
	int total_dist = 0;
	int cSuccess = 0;
	int i,j;
	while(true)
	{
		cSuccess = 0;
		for(j = 0; j < 10; j++) //we need to ignore false positives
		{
			wait1Msec(10);
			total_dist = 0;
			for(i = 0;i<SAMPLES;i++) //we need to make sure the reading is good
			{
				total_dist += SensorRaw[sonar];
			}
			distance = total_dist/SAMPLES;
			if(distance<=MAX_DIST)
			{
				cSuccess++;
			}
		}
		if(cSuccess>=9) //indicates if we have a 90% success rate with distance
			objectFound = true;
		//showDisplay(distance, 0);
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
	StartTask(getReadingTask);
	StartTask(invertMotorsTask);
	StartTask(lightSensorTask);
	distance = SensorRaw[sonar]; //start with a reading
	resetGyro(gyro);
	wait1Msec(1000);
	while(true)
	{
		if(objectFound) //objectFound is a flag set by the getReadingTask
		{
			targetAquired();
			objectFound = false;
			lineFound = false;
		}

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