#define FULL 100
#define HUNGRY 50
#define STARVING 25
#define DEAD 0

int energyLevel;

bool feeding;
bool death;

task EnergyLevel() {

	while (death == false) {
		if (feeding && energyLevel < FULL) {
			energyLevel += 1;
		} else if (energyLevel > 0) {
			energyLevel -= 1;
		}
		sleep(1200);
		if (energyLevel <= 0) {
			death = true;
		}
	}
}
