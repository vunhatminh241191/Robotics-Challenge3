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
bool investigate;
bool feeding;
int energyLevel;
int fearLevel;

//speeds
int avoidLSpeed;
int avoidRSpeed;
int escapeLSpeed;
int escapeRSpeed
int investLSpeed;
int investRSpeed;
int feedLSpeed;
int feedRSpeed;
int wanderLSpeed;
int wanderRSpeed;

task main() {
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

	while(true) {
		if(death) {
			State = DEATH;
		} else if (obstacle) {
			State = AVOID;
		} else if (escape && fearLevel > 0 && energyLevel > DANGER) {
			State = ESCAPE;
		} else if (investigate && energyLevel > DANGER) {
			State = INVESTIGATE;
		} else if (feeding) {
			State = FEEDING;
		} else {
			State = WANDER;
		}
		switch(State) {
			case DEATH:
				motor(leftMotor) = STOP;
				motor(rightMotor) = STOP;
				break;
			case AVOID:
				motor(leftMotor) = avoidLSpeed;
				motor(rightMotor) = avoidRSpeed;
				break;
			case ESCAPE:
				motor(leftMotor) = escapeLSpeed;
				motor(rightMotor) = escapeRSpeed
				break;
			case INVESTIGATE:
				motor(leftMotor) = investLSpeed;
				motor(rightMotor) = investRSpeed;
				break;
			case FEEDING:
				motor(leftMotor) = feedLSpeed;
				motor(rightMotor) = feedRSpeed;
				break;
			case WANDER:
				motor(leftMotor) = wanderLSpeed;
				motor(rightMotor) = wanderRSpeed;
				break;
		}
	}
}
