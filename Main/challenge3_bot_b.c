#pragma config(Sensor, S1,     ultraSonic,     sensorSONAR)
#pragma config(Sensor, S2,     leftLight,      sensorLightActive)
#pragma config(Sensor, S3,     rightLight,     sensorLightActive)
#pragma config(Motor,  motorA,          leftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,          rightMotor,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
#define SAMPLES 50
#define BASESPEED 40
#define WHITE 20 //light sensor white reading value
#define BLACK 7 //light sensor black reading value
#define MAX_DIST 90 //maximum distance for the robot to read
#define MIN_DIST 3 //minimum distance for the robot to read
#define DEATH 0
#define AVOID 1
#define ESCAPE 2
#define INVESTIGATE 3
#define FEEDING 4
#define WANDER 5
#define FULL 100
#define HUNGRY 50
#define DANGER 25
#define DEAD 0
#define STOP 0

//flags
bool death;
bool obstacle;
bool escape;
bool feeding;
int energyLevel;
int fearLevel;
bool objectFound = false; //flag for priority if object is found


//speeds
int avoidLSpeed;
int avoidRSpeed;
int escapeLSpeed;
int escapeRSpeed;
int investLSpeed;
int investRSpeed;
int feedLSpeed;
int feedRSpeed;
int wanderLSpeed;
int wanderRSpeed;
int lSpeed, rSpeed = 0;

int distance;

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
	playTone(10000, 10);
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
			playTone(speed*50, 5); //debugging
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
		while(!objectFound)
		{
			wait1Msec((random[6]+3)*250); //wait 1 to 3 seconds (250mS resolution)
			drunkTurn();
		}
	}
}

task runMotors()
{
	while(true){
		motor[leftMotor] = lSpeed;
		motor[rightMotor] = rSpeed;
	}
}

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
				total_dist += SensorValue[ultraSonic];
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

task main()
{
	startTask(runMotors);
	startTask(getReadingTask);
	startTask(invertMotorsTask);
	int State;

	energyLevel = FULL;

	avoidLSpeed = 0;
	avoidRSpeed = 0;
	escapeLSpeed = 0;
	escapeRSpeed = 0;
	investLSpeed = 0;
	investRSpeed = 0;
	feedLSpeed = 0;
	feedRSpeed = 0;
	wanderLSpeed = 0;
	wanderRSpeed = 0;
	//nxtEnableHSPort();
	//nxtSetHSBaudRate(9600);  // can go as high as 921600 BAUD
	//nxtHS_Mode = hsRawMode;
	//ubyte reply[2];
	//while (nxtGetAvailHSBytes() < 2) EndTimeSlice(); // wait for the two bytes to come in.
	//	nxtReadRawHS(reply, 2);
	while(true)
	{
		distance = SensorValue[ultraSonic];
		if(death) {
			State = DEATH;
			} else if (obstacle) {
			State = AVOID;
			} else if (escape && fearLevel > 0 && energyLevel > DANGER) {
			State = ESCAPE;
			} else if (objectFound && energyLevel > DANGER) {
			State = INVESTIGATE;
			} else if (feeding) {
			State = FEEDING;
			} else {
			State = WANDER;
		}
		switch(State) {
		case DEATH:
			lSpeed = STOP;
			rSpeed = STOP;
			break;
		case AVOID:
			lSpeed = avoidLSpeed;
			rSpeed = avoidRSpeed;
			break;
		case ESCAPE:
			lSpeed = escapeLSpeed;
			rSpeed = escapeRSpeed;
			break;
		case INVESTIGATE:
			targetAquired();
			break;
		case FEEDING:
			//eatingfunction
			lSpeed = feedLSpeed;
			rSpeed = feedRSpeed;
			break;
		case WANDER:
			lSpeed = wanderLSpeed;
			rSpeed = wanderRSpeed;
			break;
		}
	}
}
