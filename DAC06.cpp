//Lachlan Chow z5164192
//MTRN3500 Assignment 1

#include "DAC06.h"

//constructor
EmbeddedDevice::DAC06::DAC06(EmbeddedOperations *eops, uint32_t base_addr)
{
	if (eops->ioperm(base_addr, 11, 1) != 0)
	{
		std::cout << "Permission failed" << std::endl;
	}

	BASE = base_addr;
	this->eops = eops;
}

//destructor
EmbeddedDevice::DAC06::~DAC06()
{

}

//Output the raw value provided to the specified channel in the DAC
//write to any channel 0-5
void EmbeddedDevice::DAC06::analogOutputRaw(uint8_t channel, uint16_t value)
{
	uint8_t startingAddress, highByte, lowByte;

	//Get the lower 8 bits of value
	lowByte = value & (0xFF);
	//Get the higher 8 bits of value, shift it right 8 places, AND this to get the lower nibble
	highByte = (((value & (0xFF00)) >> 8) & 0xF);

	startingAddress = channel*2;

	//out the low byte first according to the manual
	eops->outb(lowByte, BASE + startingAddress);
	eops->outb(highByte, BASE + startingAddress + 1);
}

//Turn a desired voltage from -5V to 5V into a 12 bit value (from 0 to 4096) and output into any channel 0-5
void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage)
{
	//formula to scale the -5 to 5V input into a number between 0 and 4095
	uint16_t output = (uint16_t)((409.6*desired_voltage) + 2048);

	//reuse the analogOutputRaw funtion to output the value
	analogOutputRaw(channel, output);
}
