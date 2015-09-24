// Copyright © 2015 Daniel Porrey
//
// This file is part of the DHT11 Temperature Sensor project
// on hackster.io (based on the code posted by Posted by Rahul Kar
// found at http://www.rpiblog.com/2012/11/interfacing-temperature-and-humidity.html)
// 
// Dht11_Speed.c is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Dht11_Speed.c is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Dht11_Speed.c. If not, see http://www.gnu.org/licenses/.
//
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TIME 85
#define DHT11PIN 7
#define DELAY_TIME 500

int dht11_val[5] = { 0,0,0,0,0 };

int dht11_read_val()
{
	int returnValue = 0;

	uint8_t lststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i = 0;
	float farenheit = 0;
	
	// ***
	// *** Initialize the values
	// ***
	for (i = 0; i < 5; i++)
	{
		dht11_val[i] = 0;
	}

	// ***
	// *** Signal the sensor to send data
	// ***
	pinMode(DHT11PIN, OUTPUT);
	digitalWrite(DHT11PIN, LOW);

	// ***
	// *** Datasheet states that we should wait 18ms
	// ***
	delay(18);

	// ***
	// *** Set the pin to high and switch to input mode
	// ***
	digitalWrite(DHT11PIN, HIGH);
	delayMicroseconds(40);
	pinMode(DHT11PIN, INPUT);

	// ***
	// *** Get the bits
	// ***
	for (i = 0; i < MAX_TIME; i++)
	{
		counter = 0;
		while (digitalRead(DHT11PIN) == lststate)
		{
			counter++;
			delayMicroseconds(1);

			if (counter == 255)
			{
				break;
			}
		}

		lststate = digitalRead(DHT11PIN);

		if (counter == 255)
		{
			break;
		}

		// ***
		// *** Top 3 transitions are ignored
		// ***
		if ((i >= 4) && (i % 2 == 0))
		{
			dht11_val[j / 8] <<= 1;
			
			if (counter > 16)
			{
				dht11_val[j / 8] |= 1;
			}

			j++;
		}
	}

	// ***
	// *** Verify checksum and print the verified data
	// ***
	if ((j >= 40) && (dht11_val[4] == ((dht11_val[0] + dht11_val[1] + dht11_val[2] + dht11_val[3]) & 0xFF)))
	{
		farenheit = dht11_val[2] * 9. / 5. + 32;
		printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", dht11_val[0], dht11_val[1], dht11_val[2], dht11_val[3], farenheit);
		returnValue = 1;
	}
	else
	{
		printf("Invalid Data!!\n");
		returnValue = 0;
	}
	
	return returnValue;
}

int usage (const char *progName)
{
  fprintf (stderr, "Usage: %s delayTime [in ms] sampleCount\n", progName) ;
  return EXIT_FAILURE;
}

int main(int argc, char *argv[])
{
	if(argc == 3)
	{
		printf("Interfacing Temperature and Humidity Sensor (DHT11) With Raspberry Pi\n");

		// ***
		// *** Ensure we have wiringPi
		// ***
		if (wiringPiSetup() == -1)
		{
			exit(1);
		}
		
		int successCount = 0;
		int delayTime = atoi(argv[1]);
		int sampleCount = atoi(argv[2]);
		
		// ***
		// *** Make sure delayTime is 0 or greater
		// *** (Default is 500)
		// ***
		if (delayTime < 0)
		{
			delayTime = 500;			
		}
		
		// ***
		// *** Make sure sampleCount is greater 
		// *** than 0 (default is 10)
		// ***
		if (sampleCount < 1)
		{
			sampleCount = 10;
		}
		
		printf("Taking %d samples at %d ms intervals.\n\n", sampleCount, delayTime);

		for(int i = 0; i < sampleCount; ++i)
		{
			// ***
			// *** Read the sensor
			// ***
			int result = dht11_read_val();

			if (result == 1)
			{
				successCount++;
			}

			// ***
			// *** Delay between readings
			// ***
			delay(delayTime);
		}
		
		printf("\n%d of %d successful (%d %).\n", successCount, sampleCount, 100 * successCount / sampleCount);
	}
	else
	{
		return usage(argv[0]);
	}

	return 0;
}
