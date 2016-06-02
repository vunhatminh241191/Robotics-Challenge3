#pragma config(Sensor, S1,     ultraSonic,     sensorSONAR)
#pragma config(Sensor, S2,     leftLight,      sensorLightActive)
#pragma config(Sensor, S3,     rightLight,     sensorLightActive)
#pragma config(Motor,  motorA,          leftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,          rightMotor,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
* THIS ROBOT CONTROLS: MOTORS, ULTRASONIC SENSOR, AND BOTH LIGHT SENSORS ON GROUND
*/
#define SAMPLES 50
#define BASESPEED 25
#define WHITE 40 //light sensor white reading value
#define BLACK 7 //light sensor black reading value
#define GREEN 33 //The Green/Gray color of the line pattern carpet floor tiles
#define MAX_DIST 90 //maximum distance for the robot to read
#define MIN_DIST 7 //minimum distance for the robot to read
#define DEATH 0
#define AVOID 1
#define ESCAPE 2
#define INVESTIGATE 3
#define FEEDING 4
#define HUNTING 5
#define WANDER 6
#define FULL 120
#define HUNGRY 60
#define STARVING 30
#define DEAD 0
#define STOP 0
#define HALFSPEED BASESPEED/2
#define BREAKOUT 1200

//flags
//bool foodFound = false;
bool followingFood = false;
bool content = false;
bool hungry = false;
bool starving = false;
bool dead = false;
bool scared = false;
bool bump = false;
bool feeding = false;
bool looking = true;
bool objectFound = false; //flag for priority if object is found
bool lightFlash = false; //flag for when light flashes top sensor
bool leftWhite = false;
bool rightWhite = false;

int WhGreen = (WHITE + GREEN)/2 ; //46, The color that keeps on the line of white and green
int energyLevel;
int fearLevel;
int counter;
int leftLumenance, rightLumenance; //reading values for light sensors
int State;
int distance;

//speeds
int lSpeed, rSpeed = 0;

ubyte data[3];

void backUp()
{
	rSpeed = -1*BASESPEED;
	lSpeed = -1*BASESPEED;
	wait1Msec(500);
	lSpeed = 0;
	rSpeed = 0;
}


void randomTurning()
{
	int randTemp = (random[6]+2)*250;
	if(random[2])
	{
		rSpeed = BASESPEED;
		lSpeed = -1*(BASESPEED-10);
		wait1Msec(randTemp);
	}
	else
	{
		rSpeed = -1*(BASESPEED-10);
		lSpeed = BASESPEED;
		wait1Msec(randTemp);
	}
	lSpeed = 0;
	rSpeed = 0;
}

void runAway()
{
	playTone(1000, 100);
	backUp();
	randomTurning();
	while(bump){}
	for(int i = 0; i<=fearLevel; i++)
	{
		while(bump){}
		rSpeed = BASESPEED+(fearLevel/4+10);
		lSpeed = BASESPEED+(fearLevel/4+10);
		wait1Msec(40);
	}
	rSpeed = 0;
	lSpeed = 0;
}

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
	for(int i = 0;i<10;i++)
	{
		wait1Msec(10); //essentially wait 100ms
		rSpeed = 25; //turn left for 100ms
		lSpeed = -25;
		if(distance <= MAX_DIST || State != INVESTIGATE) //if object within range set flag to true
			return true;
	}
	rSpeed = 0; //stop motors
	lSpeed = 0;
	wait1Msec(100);
	for(int i = 0;i<20;i++)
	{
		wait1Msec(10); //essentially wait 100ms
		rSpeed = -25; //turn right for 100ms
		lSpeed = 25;
		if(distance <= MAX_DIST || State != INVESTIGATE) //if object within range set flag to true
			return true;
	}
	playTone(3000, 10);
	return false; //we lost the target
}

/**
*Function is called when an object is within range
*Parameters: none
*returns: void
**/
task targetAquired()
{
	while(true)
	{
		int speed = 0;
		while(State == INVESTIGATE)
		{
			if(distance>MAX_DIST)
			{
				if(!lostTarget()) //if we lose the target, check for it
				{
					objectFound = false;
					break;
				}
			}
			else if(distance<=MIN_DIST)
			{
				rSpeed = 0; //stop motors
				lSpeed = 0;
				for(int x = 0; x < 60; x++)
				{
					wait1Msec(50); //wait 3 seconds
					if (State != INVESTIGATE) break;
				}
				if(State == INVESTIGATE) {
					objectFound = false;
					backUp();
					randomTurning();
				}
			} else
			{
				speed = sonicSpeed(distance); //if object is found, find its proportional speed
				rSpeed = speed; //set that speed to motors
				lSpeed = speed;
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
	//get direction and duration
	if(State!=WANDER)
		return;
	else
	{
		int Dir = random[2];
		//time is for turning duration, time2 is for the forward duration.
		int turnDuration = (random[5]+1) * 3;
		int forwardDuration = (random[3]+1);

		for(int j = 0; j < turnDuration; j++)
		{
			if(Dir) {
				rSpeed = BASESPEED+15;
				lSpeed = BASESPEED;
			} else
			{
				lSpeed = BASESPEED;
				rSpeed = BASESPEED+15;
			}
			wait1Msec(100);
			if(State!=WANDER)
				return;
		}
		for(int k = 0; k < forwardDuration; k++)
		{
			lSpeed = BASESPEED;
			rSpeed = BASESPEED;
			wait1Msec(100);
			if(State!=WANDER)
				return;
		}

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
}

//This function is called when both bumpers have read a value. the robot
//beeps, backs up, then waits for 2 seconds. Then it chooses a random direction
//and turns in that direction for a random amount of time.
void bothD()
{
	playTone(2400, 100);
	backUp();
	wait1Msec(2000);
	randomTurning();
}

//When a left bumb is detected wait 100ms.
//If in this 100ms we detect the right bumper as well, then we stop both motors
//and proceed with the bothD() function. Otherwise, we stop both motors, reverse
//for a little, and then turn a random amount to the right.
void leftD()
{
	wait1Msec(100); //wait .1 sec
	if(data[0] == 'y') //check to see if perpendicular
	{
		lSpeed = 0;
		rSpeed = 0;
		bothD(); //if perpendicular
	}
	else //if not perpendicular, follow normal procedure
	{
		lSpeed = 0;
		rSpeed = 0;
		wait1Msec(50);
		backUp();
		rSpeed = BASESPEED; //turn left
		lSpeed = -1*BASESPEED;
		wait1Msec((random[4]+3)*100); //500
	}
}

//When a right bumb is detected wait 100ms.
//If in this 100ms we detect the left bumper as well, then we stop both motors
//and proceed with the bothD() function. Otherwise, we stop both motors, reverse
//for a little, and then turn a random amount to the left.
void rightD()
{
	wait1Msec(100); //wait .1 sec
	if(data[1]=='y') //check to see if perpendicular
	{
		lSpeed = 0;
		rSpeed = 0;
		bothD(); //if perpendicular
	}
	else //if not perpendicular, follow normal procedure
	{
		lSpeed = 0;
		rSpeed = 0;
		wait1Msec(50);
		backUp();
		rSpeed = -1*BASESPEED;
		lSpeed = BASESPEED;
		wait1Msec((random[4]+3)*100); //500
	}
}

void obstacle()
{
	if(data[1]=='y')
		leftD();
	else
		rightD();
}

task commTask()
{
	nxtEnableHSPort();
	nxtSetHSBaudRate(9600);  // can go as high as 921600 BAUD
	nxtHS_Mode = hsRawMode;
	while(true)
	{
		while (nxtGetAvailHSBytes() < 4) EndTimeSlice(); // wait for the two bytes to come in.
		{
			nxtReadRawHS(&data[0], 3*sizeof(ubyte));
		}
		if(data[2]=='y') {
			lightFlash = true;
		}
		if(data[0]=='y' || data[1]=='y')
			bump = true;
	}
}

/**
*Task that runs continuously and calls drunkTurn function every few seconds.
**/
task invertMotorsTask()
{
	while(true)
	{
		if(State==WANDER && !feeding)
		{
			wait1Msec((random[6]+3)*250); //wait 1 to 3 seconds (250mS resolution)
			if(State==WANDER && !feeding)
				drunkTurn();
		}
	}
}

/**
*Rotates right until it gets back on the triangle
*/
void turnAroundLeft(){
	while (!(leftWhite) && (State==FEEDING || State==HUNTING)){
		lSpeed = 50;
		rSpeed = -50;
	}
}

/**
*Rotates left until it gets back on the triangle
*/
void turnAroundRight(){
	while (!(rightWhite) && (State==FEEDING || State==HUNTING)){
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
	while((leftWhite && rightWhite) && (State==FEEDING || State==HUNTING)) {
		//playTone(3000, 200);
		lSpeed = 50;
		rSpeed = -50;
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
		while(State==FEEDING)
		{
			int Dir = random[2];
			//time is for turning duration, time2 is for the forward duration.
			int turnDuration = (random[5]+1) * 350;

			//initial turn off-course
			if(Dir) {
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
				if(State!=FEEDING)
				{
					feeding=false;
					break;
				}
			}

			//Turn back to the forward position
			if(Dir) {
				lSpeed = HALFSPEED / 2 ;
				rSpeed = HALFSPEED * 2;
			} else
			{
				lSpeed = HALFSPEED * 2;
				rSpeed = HALFSPEED / 2;
			}
			for (i = 0; i < turnDuration; i += 10) {
				sleep(10); //sleep in tiny increments so task can be stopped
				if(State!=FEEDING)
				{
					feeding=false;
					break;
				}
			}
		}
	}
}

/**Robot moves around the patch,
* staying on the patch until full or task is stopped
*/
task feed(){
	while(true)
	{
		if(State==FEEDING)
		{
			while (energyLevel < FULL && State==FEEDING) {
				//nxtDisplayTextLine(0, "Food: %d", energyLevel);
				if (leftWhite || rightWhite) {
					//stopTask(feedingInvertMotorsTask);
					oneEighty();
					if(State==FEEDING)
					{
						lSpeed = HALFSPEED;
						rSpeed = HALFSPEED;
					}
					else
					{
						feeding=false;
						break;
					}
					sleep(50); //go forward a bit
					//startTask(feedingInvertMotorsTask);
				}
			}
		}
	}
	if(energyLevel==FULL)
	{
		playTone(1000, 10);
		feeding = false;
		looking = true;
	}
}

/**
* Once a white triangle line is detected, this method will get on and follow.
* Sensor1 that detected line will move onto the white until the Right sensor
* is on the edge of white and green where it will follow.
* If Left Sensor detects green, we are going down skinny end and need to turn around.
* If Right detects a lot of white, we have reached the food circle.
*/
task followFoodLeft() {
	followingFood = true;
	bool online = true;
	bool foundFood = false;
	bool doOver = true;
	float offset = 3.0;
	int wrongWayCount = 0;
	while(true)
	{
		while(State==HUNTING && !followingFood)
		{
			while (!rightWhite && State==HUNTING) { //move until right gets on white
				lSpeed = BASESPEED; //will need a way to break out eventuallu
				rSpeed = BASESPEED;
			}
			while (doOver  && State==HUNTING) {
				online = true;

				// quickly align right before beginning
				while (rightLumenance > WhGreen - offset && State==HUNTING) {
					lSpeed = BASESPEED * 2;
					rSpeed = BASESPEED * 0.3;
				}

				while (online && State==HUNTING) {
					//displayCenteredBigTextLine(7, "current: %d", leftLumenance);
					clearTimer(T1); //wrong way timer
					clearTimer(T2); //on food patch timer
					while (rightLumenance < WhGreen - offset && State==HUNTING) {	// off right, too much green
						//playTone(1000, 1);
						lSpeed = BASESPEED * 0.3;
						rSpeed = BASESPEED;
						if (!leftWhite && State==HUNTING) { //left is off white?
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
					while (rightLumenance > WhGreen + offset && State==HUNTING) {	// off left, too much white
						lSpeed = BASESPEED;
						rSpeed = BASESPEED * 0.3;
						if (!leftWhite) { //left is off white?
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
			if (foundFood) { //rotate left about 90 degrees to face the circle hopefully
				lSpeed = -50;
				rSpeed = 50;
				sleep(400); //is this enough turn? Extremely insensative for some reason
				if(State!=HUNTING)
				{
					looking=true;
					break;
				}
				else
				{
					lSpeed = BASESPEED;
					rSpeed = BASESPEED;
					sleep(300); //go forward a bit
					feeding = true;
				}
			}
			else {
				looking = true;
			}
		}
		followingFood = false;
	}
}

task followFoodRight() {
	followingFood = true;
	bool online = true;
	bool foundFood = false;
	bool doOver = true;
	float offset = 3.0;
	int wrongWayCount = 0;
	bool check = true;
	while(true)
	{
		while(State==HUNTING && !followingFood)
		{
			while (check && State==HUNTING) { //move until left gets on white
				lSpeed = BASESPEED; //TODO: will need a way to break out eventualluy
				rSpeed = BASESPEED;
				if (leftWhite) { //check that leftLumenance spike
					sleep(20);
					if (leftWhite) {
						check = false;
					}
				}

			}
			while (doOver && State==HUNTING) {
				online = true;

				// quickly align left before beginning
				while (leftLumenance > WhGreen - offset && State==HUNTING) {
					lSpeed = BASESPEED * 0.3;
					rSpeed = BASESPEED * 2;
				}

				while (online && State==HUNTING) {
					clearTimer(T1); //wrong way timer
					clearTimer(T2); //on food patch timer
					while (leftLumenance < WhGreen - offset && State==HUNTING) {	// off left, too much green
						//playTone(1000, 1);
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
					while (leftLumenance > WhGreen + offset && State==HUNTING) {	// off right, too much white
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
			if (foundFood) { //rotate right about 90 degrees to face the circle hopefully
				lSpeed = 30;
				rSpeed = -30;
				sleep(400); //is this enough turn? Extremely insensative for some reason
				//playTone(1000, 10);
				if(State!=HUNTING)
					break;
				else
				{
					lSpeed = BASESPEED;
					rSpeed = BASESPEED;
					sleep(300); //go forward a bit
					feeding = true;
				}
			}
			else {
				looking = true;
			}
		}
		followingFood = false;
	}
}

task watchForGradient()
{
	while(true)
	{
		while (looking && !content) {
			if (leftWhite) {
				sleep(50);
				if (leftWhite) {
					looking = false; //be sure to set looking back to true if followFood tasks are interrupted
					/*leftLumanence calculations spiking up to over a thousand at random times
					every few seconds, need to double check to throw it out.*/
				}
			}
			else if (rightWhite) {
				sleep(50);
				if (rightWhite) {
					looking = false; //be sure to set looking back to true if followFood tasks are interrupted
					/*rightLumenance didn't look like it was spiking but I'm double checking anyway*/
				}
			}
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
		else
			objectFound = false;
	}
}

/**
*Task that continually samples the light sensors to find when
*either one is on black or white or neither.
**/
task lightSensorTask()
{
	short leftLums, rightLums;
	int lSuccess = 0;
	int rSuccess = 0;
	int i,j;
	while(true)
	{
		lSuccess = 0;
		rSuccess = 0;
		for(j = 0; j < 4; j++) //we need to ignore false positives
		{
			leftLums = 0;
			rightLums = 0;
			for(i = 0;i<SAMPLES;i++) //we need to find a good reading
			{
				leftLums += sensorValue[leftLight];
				rightLums += sensorValue(rightLight);
			}
			leftLumenance = leftLums/SAMPLES;
			rightLumenance = rightLums/SAMPLES;
			if(leftLumenance>=WHITE)
				lSuccess++;
			if(rightLumenance>=WHITE)
				rSuccess++;
		}
		if(lSuccess>=3)//if the reading has a 75% success rate
			leftWhite = true;
		else
			leftWhite = false;
		if(rSuccess>=3)
			rightWhite = true;
		else
			rightWhite = false;
	}
}

task fearCounter()
{
	lightFlash = false;
	counter = 0;
	while(true)
	{
		while(counter < 60 && !lightFlash)
		{
			counter++;
			wait1Msec(1000);
		}
	}
}

task fearState()
{
	fearLevel = 100;
	scared = false;
	while(!dead)
	{
		if(counter>=60)
		{
			scared = false;
		}
		while(fearLevel<100 && !scared && !lightFlash)
		{
			fearLevel++;
			wait1Msec(600);
			if(lightFlash)
				break;
			wait1Msec(600);
			if(lightFlash)
				break;
			wait1Msec(600);
			if(lightFlash)
				break;
			wait1Msec(600);
			if(lightFlash)
				break;
		}
		if(lightFlash)
		{
			counter = 0;
			if(!starving)
			{
				if(fearLevel-25>0)
					runAway();
				if(scared)
				{
					if(fearLevel<25)
						fearLevel = 0;
					else
						fearLevel -= 25;
				}
				else if(!scared)
					scared = true;
			}
			lightFlash = false;
		}
	}
}

task energyRate()
{
	while(!dead)
	{
		if(State==FEEDING && energyLevel<FULL)
		{
			energyLevel++;
		}
		else
		{
			energyLevel--;
			wait1Msec(1000);
		}
		wait1Msec(1000);
		if(energyLevel==FULL)
		{
			playTone(10000, 200);
			feeding = false;
		}
	}
}


task energyState()
{
	energyLevel = 40;
	while(1)
	{
		if(energyLevel>HUNGRY)
		{
			content = true;
			hungry = false;
			starving = false;
			dead = false;
		}
		else if(energyLevel>STARVING && energyLevel<=HUNGRY)
		{
			content = false;
			hungry = true;
			starving = false;
			dead = false;
		}
		else if(energyLevel<=STARVING && energyLevel>DEATH)
		{
			content = false;
			hungry = false;
			starving = true;
			dead = false;
		}
		else
		{
			content = false;
			hungry = false;
			starving = false;
			dead = true;
		}
	}
}

task displayValues()
{
	while(1)
	{
		eraseDisplay();
		displayCenteredBigTextLine(1, "E=%d", energyLevel);
		displayCenteredBigTextLine(3, "S=%d", State);
		if(leftWhite)
			displayCenteredBigTextLine(5, "LW");
		else
			displayCenteredBigTextLine(5, "!LW");
	}
}

task main()
{
	clearTimer(T1);
	clearTimer(T2);

	startTask(runMotors);
	startTask(fearState);
	startTask(energyState);
	startTask(getReadingTask);
	startTask(invertMotorsTask);
	startTask(energyRate);
	startTask(commTask);
	startTask(displayValues);
	startTask(targetAquired);
	startTask(watchForGradient);
	startTask(feedingInvertMotorsTask);
	startTask(feed);
	startTask(followFoodLeft);
	startTask(followFoodRight);
	startTask(lightSensorTask);
	startTask(fearCounter);

	distance = SensorValue[ultraSonic];

	while(true)
	{
		if(dead) { //if we are dead
			State = DEATH;
			}	else if (bump) { //bumper
			State = AVOID;
			} else if (lightFlash && !starving && fearLevel > 0) {//!starving) {
			State = ESCAPE;
			} else if (objectFound && !starving) { //found an object
			State = INVESTIGATE;
			} else if (feeding && energyLevel < FULL) { //found food
			State = FEEDING; //foodFound !replaces feeding
			} else if ((rightWhite || leftWhite) && !content) { //found a scent
			State = HUNTING; //got rid of looking
			} else {
			State = WANDER; //otherwise just wander around
		}
		switch(State) {
		case DEATH: //are we dead?
			lSpeed = 100;
			rSpeed = -100;
			scared = false;
			bump = false;
			feeding = false;
			lightFlash = false;
			//escape = false;
			objectFound = false;
			looking = false;
			playTone(3000, 200);
			wait1Msec(1000);
			lSpeed = 0;
			rSpeed = 0;
			wait1Msec(200);
			stopAllTasks();
			break;
		case AVOID: //did we hit a bumper?
			feeding = false;
			objectFound = false;
			looking = true;
			obstacle();
			bump = false;
			break;
		case ESCAPE: //are we afraid of light?
			feeding = false;
			objectFound = false;
			looking = true;
			break;
		case INVESTIGATE: //did we find an object?
			feeding = false;
			//objectFound = true;
			looking = true;
			break;
		case FEEDING: //are we eating?
			break;
		case HUNTING:
			break;
		case WANDER: //nothing is happen
			break;
		}
	}
}