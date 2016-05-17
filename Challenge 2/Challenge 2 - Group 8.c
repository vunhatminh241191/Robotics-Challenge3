#pragma config(Sensor, S1,     colorLeft,      sensorEV3_Color)
#pragma config(Sensor, S3,     sonar,          sensorEV3_Ultrasonic)
#pragma config(Sensor, S4,     colorRight,     sensorEV3_Color)
#pragma config(Motor,  motorA,          motorLeft,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD,          motorRight,    tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
* TCSS 437 - Challenge 02
* Team #8: Aaron Chau, Abigail Smith, Sean Markus
* Base Code from: Team #12: Aaron Chau, Brandon Scholer
*/
#define MOTOR_SPEED_FAST 45
#define MOTOR_SPEED_NORMAL 30//35
#define MIN_DIR_INTERVAL 750
#define MAX_TURNS 2


int blackCal;
int whiteCal;
int leftColorAvg;
int rightColorAvg;
int sonarAvg;

/**
* Move the robot to the left.
*/
void moveLeft(int * motorLeftSpeed, int * motorRightSpeed) {
    *motorLeftSpeed = MOTOR_SPEED_NORMAL - 5;
    *motorRightSpeed = MOTOR_SPEED_FAST;
}

/**
* Move the robot to the right.
*/
void moveRight(int * motorLeftSpeed, int * motorRightSpeed) {
    *motorLeftSpeed = MOTOR_SPEED_FAST;
    *motorRightSpeed = MOTOR_SPEED_NORMAL - 5;
}

/**
* Return a random time interval between (MIN_DIR_INTERVAL ~ MIN_DIR_INTERVAL+100)
*/
int resetMyTimer() {
    clearTimer(T2);
    return MIN_DIR_INTERVAL + random(100);
}

/**
* Reverse the robot given a duration in milliseconds.
*/
void reverseRobot(int timeToReverse) {
    clearTimer(T1);
    while(time1[T1] < timeToReverse) {
        setMotorSpeed(motorLeft, -1 * MOTOR_SPEED_NORMAL);
        setMotorSpeed(motorRight, -1 * MOTOR_SPEED_NORMAL);
    }
    setMotorSpeed(motorLeft, -1 * 0);
    setMotorSpeed(motorRight, -1 * 0);

}

/**
*   Function call for when the left bumper is hit.
*/
void hardRightTurn()
{
    // Gets a random percentage to multiply to the speed.
    float randomness = random(50);
    float percent = 1 + randomness/100;
    clearTimer(T2);
    while (time1[T2] < 500 + random(250)) {
        setMotorSpeed(motorLeft, MOTOR_SPEED_NORMAL * percent);
        setMotorSpeed(motorRight, MOTOR_SPEED_NORMAL * -1 * percent);
    }
    setMotorSpeed(motorLeft, 0);
    setMotorSpeed(motorRight, 0);
}

/**
*   Function call for when the right bumper is hit.
*/
void hardLeftTurn()
{
    // Gets a random percentage to multiply to the speed.
    float randomness = random(50);
    float percent = 1 + randomness/100;
    clearTimer(T2);
    while (time1[T2] < 500 + random(250)) {
        setMotorSpeed(motorLeft, MOTOR_SPEED_NORMAL * -1 * percent);
        setMotorSpeed(motorRight, MOTOR_SPEED_NORMAL * percent);
    }
    setMotorSpeed(motorLeft, 0);
    setMotorSpeed(motorRight, 0);
}

/**
* follows the edge of a line
* param bool left: are we on the left side of a line?
*/
void followLine(bool left) {
    int runningSpeed = MOTOR_SPEED_NORMAL-10;
    int turnFast = MOTOR_SPEED_FAST-10;
    int turnSlow = 5;

    clearTimer(T3);
    bool online = true;
    int escape = 575;
    while (online) {
        // if black and white then clear
        // else dont clear
        if (leftColorAvg <= blackCal + 5 || rightColorAvg <= blackCal + 5) {
            clearTimer(T3);
            setMotorSpeed(motorLeft, runningSpeed);
            setMotorSpeed(motorRight, runningSpeed);
        }

        while (left ? (blackCal + 5 <= leftColorAvg) : (leftColorAvg <= blackCal + 5)) {	//while left sees white (and right)

            displayCenteredBigTextLine(7, "Time: %d", time1[T3]);

            if ((leftColorAvg >= blackCal + 5 && rightColorAvg >= blackCal + 5) && time1[T3] > escape){
                online = false;
                break;
            }
						//been off the line a while, increase turn
            if ((leftColorAvg >= blackCal + 5 && rightColorAvg >= blackCal + 5) && time1[T3] > escape/4) {
            	setMotorSpeed(motorRight, MOTOR_SPEED_FAST*1.5);		//turn to the left
            	setMotorSpeed(motorLeft, -10);
            } else { //lay off turn
            		setMotorSpeed(motorRight, turnFast);		//turn to the left
            		setMotorSpeed(motorLeft, turnSlow);
          	}

        }
        while (left ? (blackCal + 5 >= rightColorAvg) : (rightColorAvg >= blackCal + 5)) {	//while right sees black

            displayCenteredBigTextLine(7, "Time: %d", time1[T3]);

            if ((leftColorAvg >= blackCal + 5 && rightColorAvg >= blackCal + 5) && time1[T3] > escape){
                online = false;
                break;
            }

            if ((leftColorAvg >= blackCal + 5 && rightColorAvg >= blackCal + 5) && time1[T3] > escape/4) {
            	setMotorSpeed(motorLeft, MOTOR_SPEED_FAST*1.5);		//turn to the left
            	setMotorSpeed(motorRight, -10);
            } else {
        		setMotorSpeed(motorLeft, turnFast);		//turn to the left
        		setMotorSpeed(motorRight, turnSlow);
          	}
        }

        displayCenteredBigTextLine(7, "Time: %d", time1[T3]);
    }
}

task wander() {
    // The motor speed
    int motorLeftSpeed, motorRightSpeed;

    // The time when the robot will change directions
    int timeToReset = resetMyTimer();

    // Choose a random direction to move.
    if(random(1))
    {
        //set the motor speed values to turn left
        moveLeft(&motorLeftSpeed, &motorRightSpeed);
    }
    else
    {
        //set the motor speed values to turn right
        moveRight(&motorLeftSpeed, &motorRightSpeed);
    }

    //Have a counter for left/right movement. Negative for left and pos for right.
    int moveCounter = 0;
    while(1) {
        // Get the time in milli when the left/right bumpers are hit.

        // Give motor left/right a random speed
        // Make robot walk forward.
        setMotorSpeed(motorLeft, motorLeftSpeed);
        setMotorSpeed(motorRight, motorRightSpeed);

        // If it is time, reset the direction
        if (time1[T2] > timeToReset)
        {
            // Get a new time.
            timeToReset = resetMyTimer();

            // Choose to go left or right.
            int leftRightRand = random(1);

            // If the robot has moved left/right too many times, force
            // it to go the other direction.
            if (moveCounter <= -1 * MAX_TURNS)
            {
                // move right
                leftRightRand = 0;
            }
            else if (moveCounter >= MAX_TURNS)
            {
                // move left
                leftRightRand = 1;
            }

            if(leftRightRand)
            {
                // Move left
                moveLeft(&motorLeftSpeed, &motorRightSpeed);
                if (moveCounter > 0) moveCounter = 0;
                moveCounter--;
            }
            else
            {
                // Move right
                moveRight(&motorLeftSpeed, &motorRightSpeed);
                if (moveCounter < 0) moveCounter = 0;
                moveCounter++;
            }
        }

        //Loop to monitor value in Sensor debugger window
        sleep(50);

    }
}

/**
 *Looks for a line to follow and calls followLine
 */
task detectLine() {
    bool leftDetect = false;
    bool rightDetect = false;
    while (1) {

        // Check if the left sensor detected the black line.
        if ((0 <= leftColorAvg) &&  (leftColorAvg <= blackCal + 5)) {
            // sleep for a little bit to make sure we actually detected the line.
        	sleep(100);
            if ((0 <= leftColorAvg) &&  (leftColorAvg <= blackCal + 5)) {
                leftDetect = true;
                stopTask(wander);
            }
        // Check if the right sensor detected the line
        } else if ((0 <= rightColorAvg) &&  (rightColorAvg <= blackCal + 5)) {
            // sleep for a little bit to make sure we actually detected the line.
        	sleep(100);
            if ((0 <= rightColorAvg) &&  (rightColorAvg <= blackCal + 5)) {
                rightDetect = true;
                stopTask(wander);
            }
        }
        if (leftDetect || rightDetect) {
            followLine(leftDetect);

            playSound(soundBeepBeep);

            clearTimer(T3);
            while (time1[T3] < 750) {
                // Turn the robot to offset the wide turn from when the robot left followLine
	            if (leftDetect) {
	            	setMotorSpeed(motorLeft, MOTOR_SPEED_FAST*1.5);
	            	setMotorSpeed(motorRight, -5);
	          	} else {
	            	setMotorSpeed(motorRight, MOTOR_SPEED_FAST*1.5);
	            	setMotorSpeed(motorLeft, -5);
	          	}
	          }

            leftDetect = false;
            rightDetect = false;
            startTask(wander);
        }
    }
}

/**
* Uses a weighted moving average to keep track
* of color and sensor values
*/
task calculateAverages() {

    leftColorAvg = 0;
    rightColorAvg = 0;
    sonarAvg = 0;
    int sonarCount = 0;
    int leftCount = 0;
    int rightCount = 0;

    int i = 0;
    for (i = 0; i < 10; i ++) {
        leftColorAvg += getColorReflected(colorLeft);
        rightColorAvg += getColorReflected(colorRight);
        sonarAvg += getUSDistance(S3);
    }
    leftColorAvg /= 10;
    rightColorAvg /= 10;
    sonarAvg /= 10;

    float alpha = 0.9;
    while (1) {
        int currLeft = getColorReflected(colorLeft);
        int currRight = getColorReflected(colorRight);
        int currSonar = getUSDistance(S3);

        // We use a counter to make sure that we aren't getting weird noise. We have a threshold of 3 readings. 
        // If we think we have noise, increment the counter. If the counter counts up to 3, we probably have a 
        // valid reading from the sensors. 
        if (leftColorAvg - currLeft < 20 || leftCount > 3) {
        	leftColorAvg =  currLeft + alpha * (leftColorAvg - currLeft);
        	leftCount = 0;
        } else {
        	leftCount++;
      	}

        if (rightColorAvg - currRight < 20 || rightCount > 3) {
        	rightColorAvg =  currRight + alpha * (rightColorAvg - currRight);
        	rightCount = 0;
        } else {
        	rightCount++;
      	}

        if (sonarAvg - currSonar < 20 || sonarCount > 3) {
	        sonarAvg =  currSonar + alpha * (sonarAvg - currSonar);
	        sonarCount = 0;
        } else {
	      	sonarCount++;
    	}
    }
}


/**
* The task that looks for an object. This task is never interrupted. 
*/
task detectObject() {
    bool running = true;
    while (1) {
        // Check if the sonar is 5 cm away.
        if (sonarAvg <= 5.0) {
            // Sleep for a little to double check the reading. 
        	sleep(250);
            if (sonarAvg <= 5.0) {
                // Stop the other lower tier tasks.
	            stopTask(wander);
	            stopTask(detectLine);
	            stopAllMotors();

                // Wait
	            sleep(2000);

                // Reverse the robot for 3 seconds
	            reverseRobot(3000);

                // Make a random turn left/right
	            if (random(1)) {
	                hardRightTurn();
	            } else {
	                hardLeftTurn();
	            }
	            setLEDColor(ledGreen);
	            running = false;
	          }
        } else if (sonarAvg <= 92.0) {
            // Make sure that the sonar reading is actually under 92 cm.
            sleep(250);
            if (sonarAvg <= 92.0) {

            	stopTask(wander);
            	stopTask(detectLine);
            	displayCenteredBigTextLine(3, "%d", sonarAvg);
            	setLEDColor(ledOrangePulse);

            	running = false;
            	// An exponential equation that uses the sonar reading to decrease the 
                // speed as the robot gets closer to an object. 
            	int motorSpeed = -1 * ((.1 * sonarAvg - 10)*(.1 * sonarAvg - 10)) + 100;
            	setMotorSpeed(motorLeft, motorSpeed);
            	setMotorSpeed(motorRight, motorSpeed);
            }
        } else {
            if (!running) {
                // If the robot is done detecting an object then start the wander and line detection again.
                startTask(wander);
                startTask(detectLine);
                setLEDColor(ledGreen);
                running = true;
            }
        }

    }
}



/**
* The main function that contains the driver code.
*/
task main()
{

    whiteCal = 0;
    blackCal = 0;
    int sampleSize = 10;
    bool hasCalibrated = false;
    while (/*!whiteCal || !blackCal*/!hasCalibrated) {
        int i = 0;
        // Code to calibrate the white reading. However, we ended up only using the black calibration color. 
        /*if (getButtonPress(buttonAny) && !whiteCal) {
        setMotorSpeed(motorLeft, 10);
        setMotorSpeed(motorRight, 10);
        for (i = 0; i < sampleSize; i++) {
        sleep(300);
        whiteCal += (getColorReflected(colorLeft) + getColorReflected(colorRight))/2;
        }
        } else */
        // Press the up button to use a predefined black calibration color.
        if (getButtonPress(buttonUp) && !blackCal) {
            blackCal = 0;
            hasCalibrated = true;
        }

        // Press the middle button to let the robot calibrate the black color.
        if (getButtonPress(buttonEnter) && !blackCal) {
            setMotorSpeed(motorLeft, 10);
            setMotorSpeed(motorRight, 10);
            for (i = 0; i < sampleSize; i++) {
                sleep(300);
                blackCal += (getColorReflected(colorLeft) + getColorReflected(colorRight))/2;
            }
            blackCal /= sampleSize;
            hasCalibrated = true;
        }
        setMotorSpeed(motorLeft, 0);
        setMotorSpeed(motorRight, 0);
        displayCenteredBigTextLine(4, "%d",blackCal);
    }


    // Wait for the user to press a button to start the robot.
    while (!getButtonPress(buttonAny)){ sleep(100); }

    // Start calculating our weighted average readings.
    startTask(calculateAverages);
    sleep(1000);

    // Start our tasks.
    startTask(wander);
    startTask(detectLine);
    startTask(detectObject);

    while(1) {
        wait(10);
    }
}