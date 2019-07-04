//Lachlan Chow z5164192
//MTRN3500 Assignment 1

#include "MSIP404.h"

EmbeddedDevice::MSIP404::MSIP404(EmbeddedOperations *eops, uint32_t base_addr) 
{
	if (eops->ioperm(base_addr, 15, 1) != 0)
	{
		std::cout << "Permission failed" << std::endl;
	}

	//obtain permission for reading the index pulses
	if (eops->ioperm(0x800, 2, 1) != 0)
	{
		std::cout << "Permission failed" << std::endl;
	}

	BASE = base_addr;
	this->eops = eops; 
}

EmbeddedDevice::MSIP404::~MSIP404()
{

}


//reset the channel determined by the input value
void EmbeddedDevice::MSIP404::resetChannel(uint8_t channel)  //assuming the channel is the given reset channel address
{
	//The counts contained in all registers (high to low bytes) can be reset to zero by performing 
	//a write of any value to the reset addresses
	eops->outb(0x01, BASE + channel);
}

//read the channel determined by the input value
//read the MSB first as this changes the least and will still be accurate once it has finished reading
int32_t EmbeddedDevice::MSIP404::readChannel(uint8_t channel)
{
	int32_t count = 0;
	int32_t byte1, byte2, byte3, byte4;
	int8_t startingAddress = 0;

	//the count is stored in 4 8-bit registers, and the address of the lowest register is 4 times the channel number + BASE
	startingAddress = channel * 4;
	
	byte4 = eops->inb(BASE + startingAddress + 3);
	byte3 = eops->inb(BASE + startingAddress + 2);
	byte2 = eops->inb(BASE + startingAddress + 1);
	byte1 = eops->inb(BASE + startingAddress + 0);

	//combine all the bytes to form the counter
	count = (byte4 << 24) | (byte3 << 16) | (byte2 << 8) | (byte1);

	return count;
}

//read the index bit for the channel determined by the input value
//to read the index pulses for channels 0-2 ONLY
//assume the encoder index pulse base address is always 0x800
bool EmbeddedDevice::MSIP404::readIndex(uint8_t channel)
{
	uint8_t indexByte;
	bool index;

	//read in the MSB if the channel is 0
	if (channel == 0)
	{
		indexByte = eops->inb(0x800);
		if ((indexByte & (1 << 7)) == (1 << 7))
		{
			index = 1;
		}
		else
		{
			index = 0;
		}
	}

	//read the 3rd MSB if the channel is 1
	if (channel == 1)
	{
		indexByte = eops->inb(0x800);
		if ((indexByte & (1 << 5)) == (1 << 5))
		{
			index = 1;
		}
		else
		{
			index = 0;
		}
	}

	//read the MSB if the channel is 2
	if (channel == 2)
	{
		indexByte = eops->inb(0x801);
		if ((indexByte & (1 << 7)) == (1 << 7))
		{
			index = 1;
		}
		else
		{
			index = 0;
		}
	}

	return index;
}

//Reset all 8 channels
bool EmbeddedDevice::MSIP404::operator!()
{
	//write a value to all the reset addresses
	for (uint8_t i = 0; i < 8; i++)
	{
		eops->outb(0x1, BASE + i);
	}

	return true;
}


