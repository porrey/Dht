#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TIME 85
#define DHT11PIN 3

int data[5] = { 0,0,0,0,0 };

void dht11_read_val()
{
	uint8_t lststate = HIGH;
	uint8_t counter = 0;
	uint8_t j = 0, i;
	float farenheit;

	for (i = 0; i < 5; i++)
	{
		data[i] = 0;
	}

	pinMode(DHT11PIN, OUTPUT);
	
	digitalWrite(DHT11PIN, LOW);
	delay(18);
	
	digitalWrite(DHT11PIN, HIGH);
	delayMicroseconds(40);
	
	pinMode(DHT11PIN, INPUT);

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
		// ***  First 3 transitions are ignored
		// *** 
		if ((i >= 4) && (i % 2 == 0)) 
		{
			printf("Counter = %d\n", counter);

			data[j / 8] <<= 1;

			printf("counter = %d\n", counter);
			
			if (counter > 16)
			{
				data[j / 8] |= 1;
			}

			j++;
		}
	}

	// *** 
	// *** Verify checksum and print the verified data
	// *** 
	if ((j >= 40) && (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)))
	{
		farenheit = data[2] * 9. / 5. + 32;
		printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n", data[0], data[1], data[2], data[3], farenheit);
	}
	else
	{
		printf("Invalid Data!!\n");
	}
}

int main(void)
{
	printf("Interfacing Temperature and Humidity Sensor (DHT11) With Raspberry Pi\n");

	if (wiringPiSetup() == -1)
	{
		exit(1);
	}

	while (1)
	{
		dht11_read_val();
		delay(3000);
	}

	return 0;
}
