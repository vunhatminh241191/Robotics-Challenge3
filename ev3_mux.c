#pragma config(Sensor, S3,     ,               sensorEV3_GenericI2C)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-ev3smux.h"

/**
 * Credits:
 * - Big thanks to Mindsensors for providing me with the hardware necessary to write and test this.
 *
 * License: You may use this code as you wish, provided you give credit where its due.
 *
 * THIS CODE WILL ONLY WORK WITH ROBOTC VERSION 4.10 AND HIGHER
 * \author Xander Soldaat (xander_at_botbench.com)
 * \date 14 December 2014
 */

// The SMUX can have up to 3 sensors attached to it.
tMSEV3 muxedSensor[3];

// Configure your sensor type here.  The following are available:
// colorReflectedLight
// colorAmbientLight
// colorMeasureColor
// gyroAngle
// gyroRate
// infraRedProximity
// infraRedBeacon
// infraRedRemote
// sonarCM
// sonarInches
// sonarPresence
// touchStateBump

tEV3SensorTypeMode typeMode[3] = {touchStateBump, colorReflectedLight, colorReflectedLight};

task main()
{
  displayCenteredTextLine(0, "Mindsensors");
  displayCenteredBigTextLine(1, "EV3 SMUX");
  displayCenteredTextLine(3, "Test 1");
  sleep(2000);
  eraseDisplay();
  if (!initSensor(&muxedSensor[0], msensor_S3_1, typeMode[0]))
     writeDebugStreamLine("initSensor() failed! for msensor_S3_1");

  if (!initSensor(&muxedSensor[1], msensor_S3_2, typeMode[1]))
     writeDebugStreamLine("initSensor() failed! for msensor_S3_2");

  if (!initSensor(&muxedSensor[2], msensor_S3_3, typeMode[2]))
     writeDebugStreamLine("initSensor() failed! for msensor_S3_3");

  while (true)
  {
       for (int i = 0; i < 3; i++)
       {
         if (!readSensor(&muxedSensor[i]))
            writeDebugStreamLine("readSensor() failed! for %d", i);

          switch(muxedSensor[i].typeMode)
          {
             case touchStateBump:
               displayTextLine(i*3, "Chan[%d]: Touch", i+1);
               displayTextLine(i*3 + 1, "Touch: %s, Bumps: %d", muxedSensor[i].touch ? "yes" : "no ", muxedSensor[i].bumpCount);
                break;

             case colorReflectedLight:
               displayTextLine(i*3, "Chan[%d]: Ref Light", i+1);
               displayTextLine(i*3 + 1, "Value: %d", muxedSensor[i].light);
                break;

             case colorAmbientLight:
               displayTextLine(i*3, "Chan[%d]: Amb Light", i+1);
               displayTextLine(i*3 + 1, "Value: %d", muxedSensor[i].light);
                break;

             case colorMeasureColor:
               displayTextLine(i*2, "Chan[%d]: Color", i+1);
               displayTextLine(i*2 + 1, "Color: %d", muxedSensor[i].color);
                break;

           case gyroAngle:
               displayTextLine(i*3, "Chan[%d]: Gyro", i+1);
               displayTextLine(i*3 + 1, "Angle: %d", muxedSensor[i].angle);
              break;

           case gyroRate:
               displayTextLine(i*3, "Chan[%d]: Gyro", i+1);
               displayTextLine(i*3 + 1, "Rate: %d", muxedSensor[i].rate);
              break;

           case infraRedProximity:
               displayTextLine(i*3, "Chan[%d]: IR Prox", i+1);
               displayTextLine(i*3 + 1, "Distance: %d", muxedSensor[i].distance);
              break;

           // Only beacon data (proximity and heading) for channel 1 is displayed
           case infraRedBeacon:
               displayTextLine(i*3, "Chan[%d]: IR Beac", i+1);
               displayTextLine(i*3 + 1, "Prox: %d, Head: %d", muxedSensor[i].beaconProx[0], muxedSensor[i].beaconHeading[0]);
              break;

           case infraRedRemote:
               displayTextLine(i*3, "Chan[%d]: IR Rem ", i+1);
               displayTextLine(i*3 + 1, "1: %02d, 2: %02d, 3: %02d, 1: %02d", muxedSensor[i].touch ? "yes" : "no ", muxedSensor[i].bumpCount);
              break;

           case sonarCM:
               displayTextLine(i*3, "Chan[%d]: US CM", i+1);
               displayTextLine(i*3 + 1, "Distance: %d", muxedSensor[i].distance);
               break;

            case sonarInches:
               displayTextLine(i*3, "Chan[%d]: US Inch", i+1);
               displayTextLine(i*3 + 1, "Distance: %d", muxedSensor[i].distance);
              break;

           case sonarPresence:
               displayTextLine(i*3, "Chan[%d]: US Pres", i+1);
               displayTextLine(i*3 + 1, "Present: %s", muxedSensor[i].presence ? "yes" : "no ");
              break;
        }
        displayTextLine(i*3 +2, " ");
       }
     sleep(100);
  }
}