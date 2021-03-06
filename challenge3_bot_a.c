#pragma config(Sensor, S1,     leftBumper,     sensorTouch)
#pragma config(Sensor, S2,     rightBumper,    sensorTouch)
#pragma config(Sensor, S3,     fearLight,      sensorLightActive)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
* THIS ROBOT CONTROLS THE TOP LIGHT SENSOR, AND THE TWO BUMPERS
*/

ubyte data[] = {'n', 'n', 'n'};//{'LeftBumper', 'RightBumper', 'TopLight'}

task bumpChecker()
{
	while(true)
	{
		//if left bumb -> move right
		if (SensorValue(rightBumper)==1)
		{
			data[1] = 'y';
		}
		//if right bump -> move left
		if (SensorValue(leftBumper)==1)
		{
			data[0] = 'y';
		}
	}
}

task lightChecker()
{
	short ambientLight = SensorValue[fearLight];
	short currentLight;
	while(true)
	{
		currentLight = SensorValue[fearLight];
		if(currentLight>=ambientLight+10)
		{
			data[2] = 'y';
		}
		else
			data[2] = 'n';
	}
}

void transmitData()
{
	nxtWriteRawHS(&data[0], 3);
	data[0] = 'n'; data[1] = 'n';
}

task displayValues()
{
	while(1)
	{
		eraseDisplay();
		displayCenteredBigTextLine(3, "%c%c", data[0], data[1]);
	}
}
task main()
{
	nxtEnableHSPort();
	nxtSetHSBaudRate(9600);  // can go as high as 921600 BAUD
	nxtHS_Mode = hsRawMode;
	wait1Msec(2000);
	startTask(bumpChecker);
	startTask(lightChecker);
	startTask(displayValues);
	while(true)
	{
		wait1Msec(200);
		transmitData();
	}
}
