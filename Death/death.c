#pragma config(Motor, motorB, leftMotor, tmotorEV3_Large, PIDControl, reversed, driveLeft, encoder)
#pragma config(Motor, motorC, rightMotor, tmotorEV3_Large, PIDControl, reversed, driveRight, encoder)

#define DEATH 0;


#define STOP 0;

int death;

task main() {
	int State;

	startTask(Death, 255);

	while(true) {
		if (death) {
			State = DEATH;
		}
		switch(State) {
			case DEATH:
				setMotorSpeed(leftMotor, STOP);
				setMotorSpeed(rightMotor, STOP);
				break;
		}
	}
}

task Death() {
	while (true) {
		if (death == 1) {
			//playSound(deathKnell);
			//poweroff?
		}
		sleep(1000);
	}
}
