// Copyright © 2015 Daniel Porrey
//
// This file is part of the DHT11 Temperature Sensor project
// on hackster.io.
//
// Dht11_Speed.ino is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Dht11_Speed.ino is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Dht11_Speed.c. If not, see http://www.gnu.org/licenses/.
//

/*
  Dht11_Speed v1.02
  Test the speed at which the DHT11 can be sampled
  by decreasing and increasing the delay time between
  readings until a "steady state" is achieved where
  readings can be taken sequentially without errors

  This sketch was created for the DHT11 project on
  Hackster.io at https://www.hackster.io/porrey/dht11
  to compare th speed at which a readings from the DHT11
  can be taken on a Arduino Uno and compare this to the
  speed at which they can be taken on a Raspberry Pi 2
  using Windows 10 IoT Core.

  Created:        September 19th, 2015
  Last Modified:  September 19th, 2015
  Created By:     Daniel Porrey
  Site:           https://www.hackster.io/porrey

  Requires DHT Library 0.1.13 or 0.1.20 (may not work
  against other versions of this library). Found at
  http://arduino.cc/playground/Main/DHTLib

  Source code on Github at:
  https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib
*/

#include <dht.h>

dht DHT;

#define DHT11_PIN 5              // *** Physical pin on board
#define READING_SAMPLES 5        // *** Fixed count
#define MINIMUM_LOOP_DELAY 0     // *** ms
#define MAXIMUM_LOOP_DELAY 1000  // *** ms
#define LOOP_DELAY_DECREASE .2   // *** Percentage
#define LOOP_DELAY_INCREASE 1    // *** Fixed value

// ***
// *** The current loop delay
// ***
int _loopDelay;

// ***
// *** Number of consecutive successful readings
// ***
int _consecutiveReadings;

// ***
// *** Number of consecutive failed readings
// ***
int _consecutiveErrors;

// ***
// *** True when the lowest delay has been
// *** found; false otherwise.
// ***
bool _foundLowestDelay;

// ***
// ***
// ***
int _counter = 0;

void setup()
{
  // ***
  // *** Initialize
  // ***
  ResetWhen(true);

  // ***
  // *** Display application information
  // ***
  Serial.begin(115200);
  Serial.println("DHT11 SPEED TEST PROGRAM ");
  Serial.print("DHT LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);

  // ***
  // *** Check the library version and display
  // *** a warning if not supported (but
  // *** continue running anyway).
  // ***
  if (DHT_LIB_VERSION != "0.1.13" &&
      DHT_LIB_VERSION != "0.1.20")
  {
    Serial.println("This sketch has not been tested against this version of the DHT library.");
  }

  // ***
  // *** Print a blank line
  // ***
  Serial.println();
}

void loop()
{
  // ***
  // *** Read the sensor data
  // ***
  int chk = DHT.read11(DHT11_PIN);

  // ***
  // *** Increment the counter
  // ***
  _counter++;

  // ***
  // *** Display the counter using
  // *** a fixed width string.
  // ***
  char counterDisplay[7];
  sprintf(counterDisplay, "[%4d]", _counter);
  Serial.print(counterDisplay);

  // ***
  // *** Start the display
  // ***
  Serial.print("[DHT11 ");

  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("- OK   ] ");
      OnSuccess();
      break;
    case DHTLIB_ERROR_CHECKSUM:
    case DHTLIB_ERROR_TIMEOUT:
    default:
      Serial.print("- Error] ");
      OnFail();
      break;
  }

  // ***
  // *** Convert the reading to Fahrenheit
  // ***
  float farenheit = DHT.temperature * (9. / 5.) + 32.;

  // ***
  // *** Display the result
  // ***
  Serial.print("Humidity = ");
  Serial.print(DHT.humidity, 0);
  Serial.print(" %, ");
  Serial.print("Temperature = ");
  Serial.print(DHT.temperature, 0);
  Serial.print(" C (");
  Serial.print(farenheit);
  Serial.print(" F), ");
  Serial.print(" delay = ");
  Serial.print(_loopDelay);
  Serial.println(" ms");

  // ***
  // *** Check if the reading samples are taken too fast
  // *** or too slow and adjust.
  // ***
  if (IsTooFast())
  {
    // ***
    // *** To fast;  increase the delay to slow down.
    // ***
    IncreaseLoopDelay();
  }
  else if (IsTooSlow())
  {
    // ***
    // *** To slow;  decrease the delay to speed up.
    // ***
    DecreaseLoopDelay();
  }

  // ***
  // *** Reset if we start getting a lot of
  // *** consecutive errors. This allows the
  // *** application to "tune" the delay again.
  // ***
  ResetWhen(_consecutiveErrors > READING_SAMPLES);

  // ***
  // *** Delay before taking another reading
  // ***
  delay(_loopDelay);
}

// ***
// *** Called when the reading was successful
// ***
void OnSuccess()
{
  // ***
  // *** Increment the _consecutiveReadings counter
  // ***
  _consecutiveReadings++;

  // ***
  // *** Reset the _consecutiveErrors counter
  // ***
  _consecutiveErrors = 0;
}

// ***
// *** Called when the reading failed
// ***
void OnFail()
{
  // ***
  // *** Increment the _consecutiveErrors counter
  // ***
  _consecutiveErrors++;

  // ***
  // *** Reset the _consecutiveReadings counter
  // ***
  _consecutiveReadings = 0;
}

// ***
// *** Increase the loop delay value
// *** to slow the readings down (take
// *** them less often).
// ***
void IncreaseLoopDelay()
{
  // ***
  // *** Never increased beyond the maximum value.
  // ***
  if (_loopDelay < MAXIMUM_LOOP_DELAY)
  {
    // ***
    // *** Increase the loop delay by 1 ms.
    // ***
    _loopDelay += LOOP_DELAY_INCREASE;

    // ***
    // *** Reset the counter since this is a new speed. This
    // *** cal is the result of errors so do not reset the
    // *** error count.
    // ***
    _consecutiveReadings = 0;

    // ***
    // *** Calling IncreaseLoopDelay() means we
    // *** found the lowest delay.
    // ***
    _foundLowestDelay = true;
  }
}

// ***
// *** Decrease the loop delay to take
// *** readings faster (take them more
// *** often).
// ***
void DecreaseLoopDelay()
{
  // ***
  // *** Never let loop delay get below 0.
  // ***
  if (_loopDelay > MINIMUM_LOOP_DELAY)
  {
    // ***
    // *** Reduce the loop delay.
    // ***
    _loopDelay -= (int)(_loopDelay * LOOP_DELAY_DECREASE);

    // ***
    // *** Loop delay must be MINIMUM_LOOP_DELAY or greater.
    // ***
    if (_loopDelay < MINIMUM_LOOP_DELAY) _loopDelay = MINIMUM_LOOP_DELAY;

    // ***
    // *** Reset the counters since this is a new speed. This
    // *** call is not the result of errors so the error count
    // *** is reset too.
    // ***
    _consecutiveReadings = 0;
    _consecutiveErrors = 0;
  }
}

// ***
// *** Determines if the readings are taken too fast
// *** (or too often).
// ***
bool IsTooFast()
{
  bool returnValue = false;

  // ***
  // *** We are too fast if there is at least
  // *** one error.
  // ***
  if (_consecutiveErrors > 0
      && _loopDelay < MAXIMUM_LOOP_DELAY)
  {
    returnValue = true;
  }

  return returnValue;
}

// ***
// *** Determines if the readings are taken too slow.
// ***
bool IsTooSlow()
{
  bool returnValue = false;

  // ***
  // *** Too slow if we have not yet found the
  // *** lowest delay and we have had enough
  // *** consecutive readings.
  // ***
  if (!_foundLowestDelay
      && _consecutiveReadings >= READING_SAMPLES)
  {
    returnValue = true;
  }

  return returnValue;
}

// ***
// *** Resets the application when the given
// *** condition is true.
// ***
void ResetWhen(bool condition)
{
  if (condition)
  {
    // ***
    // *** These values are set back to the
    // *** original values when the application
    // *** started.
    // ***
    _loopDelay = MAXIMUM_LOOP_DELAY;
    _consecutiveReadings = 0;
    _consecutiveErrors = 0;
    _foundLowestDelay = false;
  }
}
