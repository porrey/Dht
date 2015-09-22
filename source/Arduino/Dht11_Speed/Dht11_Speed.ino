/*
  Dht11_Speed
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

#define DHT11_PIN 5
#define READING_SAMPLES 5
#define MAXIMUM_LOOP_DELAY 2000
#define LOOP_DELAY_DECREASE .2
#define LOOP_DELAY_INCREASE 1

int _loopDelay = MAXIMUM_LOOP_DELAY;
int _previousLoopDelay = MAXIMUM_LOOP_DELAY;
int _consecutiveReadings = 0;
int _errors = 0;
bool _steadyState = false;

void setup()
{
  // ***
  // *** Display application information
  // ***
  Serial.begin(115200);
  Serial.println("DHT11 SPEED TEST PROGRAM ");
  Serial.print("DHT LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  
  if (DHT_LIB_VERSION != "0.1.13" &&
      DHT_LIB_VERSION != "0.1.20")
  {
    Serial.println("This sketch has not been tested against this version of the DHT library.");
  }

  Serial.println();

  // ***
  // *** Display the current loop delay in ms
  // ***
  DisplayLoopDelay();
}

void loop()
{
  // ***
  // *** Read Data
  // ***
  Serial.print("DHT11 = ");
  int chk = DHT.read11(DHT11_PIN);
  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("OK, ");
      OnSuccess();
      break;
    case DHTLIB_ERROR_CHECKSUM:
      Serial.print("Checksum Error, ");
      OnFail();
      break;
    case DHTLIB_ERROR_TIMEOUT:
      Serial.print("Time Out, ");
      OnFail();
      break;
    default:
      Serial.print("Unknown Error, ");
      OnFail();
      break;
  }

  // ***
  // *** Display Sensor Reading
  // ***
  Serial.print("Humidity = ");
  Serial.print(DHT.humidity, 0);
  Serial.print(" %, ");
  Serial.print("Temperature = ");
  Serial.print(DHT.temperature, 0);
  Serial.println(" C");

  // ***
  // *** Check if the reading sample are taken too fast
  // *** and increase the delay if necessary
  // ***
  if (IsTooFast())
  {    
    IncreaseLoopDelay();
    DisplayLoopDelay();
  }
  else if (IsTooSlow())
  {
    _consecutiveReadings = 0;
    _steadyState = false;

    DecreaseLoopDelay();
    DisplayLoopDelay();
  }
  else
  {
    // ***
    // *** Determine if steady state has 
    // *** been achieved and set the application
    // *** at steady if true.
    // *** 
    DetermineIfAtSteadyStateAndSet();
    
    // ***
    // *** Steady must be reset after it is achieved 
    // *** and one or more errors occur. This causes
    // *** the application to "tune" the delay again.
    // ***
    ResetWhen(_steadyState && _errors > 0);
  }
  
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
  // *** Increment the counter. Looking to get a
  // *** all successful samples sequentially
  // ***
  _consecutiveReadings++;
}

// ***
// *** Called when the reading failed
// ***
void OnFail()
{
  // ***
  // *** Increment the error count
  // ***
  _errors++;
  
  // ***
  // *** Reset the number of consecutive readings
  // ***
  _consecutiveReadings = 0;
}

// ***
// *** Displays the current loop
// *** delay value.
// ***
void DisplayLoopDelay()
{
  // ***
  // *** Determine if the loop delay has increased,
  // *** decreased or statyed the same
  // ***
  if (_loopDelay == _previousLoopDelay)
  {
    // ***
    // *** Loop delay is the same.
    // ***
    Serial.print("Loop delay is ");
  }
  else if (_loopDelay < _previousLoopDelay)
  {
    // ***
    // *** Loop delay decreased.
    // ***
    Serial.print("Too slow! Loop delay decreased to ");
  }
  else
  {
    // ***
    // *** Loop delay increased.
    // ***
    Serial.print("Too fast! Loop delay increased to ");
  }

  Serial.print(_loopDelay);
  Serial.println(" ms");
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
    _previousLoopDelay = _loopDelay;
    _loopDelay += LOOP_DELAY_INCREASE;
  }
}

// ***
// *** Decrease the loop delay to take
// *** readings fatser (take them more 
// *** often).
// ***
void DecreaseLoopDelay()
{
  // ***
  // *** Never let loop delay get below 0.
  // ***
  if (_loopDelay > 0)
  {
    // ***
    // *** Reduce the loop delay.
    // ***
    _previousLoopDelay = _loopDelay;
    _loopDelay -= (int)(_loopDelay * LOOP_DELAY_DECREASE);
    
    // ***
    // *** Loop delay must be 0 or greater.
    // ***
    if (_loopDelay < 0) _loopDelay = 0;
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
  // *** We are too fast if there are errors,
  // *** we have not taken enough consecutive
  // *** readings (one more than required) and 
  // *** loop delay is some value less the 
  // *** maximum (we are already the slowest 
  // *** we can get).
  // ***
  if (_errors > 0
      && _consecutiveReadings <= READING_SAMPLES
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
  // *** Too slow if we are not at steady state, there
  // *** are not errors and we have taken enough consecutive
  // *** readings.
  // ***
  if (!_steadyState 
      && _errors == 0
      && _consecutiveReadings >= READING_SAMPLES)
  {
    returnValue = true;
  }

  return returnValue;
}

// ***
// *** Determine if steady state has 
// *** been achieved and set the application
// *** at steady if true.
// ***
void DetermineIfAtSteadyStateAndSet()
{
  // ***
  // *** Set steady state when we are not 
  // *** already at it and we have read enough
  // *** consecutive readings from the sensor.
  // ***
  if (!_steadyState && _consecutiveReadings >= READING_SAMPLES)
  {
    _errors = 0;
    _steadyState = true;
  } 
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
    _steadyState = false;
    _loopDelay = MAXIMUM_LOOP_DELAY;
    _previousLoopDelay = MAXIMUM_LOOP_DELAY;
    _consecutiveReadings = 0;
    _errors = 0;    
  }
}
