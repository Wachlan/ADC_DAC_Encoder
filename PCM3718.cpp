//Lachlan Chow z5164192
//MTRN3500 Assignment 1

#include "PCM3718.h"

//constructor
EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr)
{
	if (eops->ioperm(base_addr, 16, 1) != 0)
	{
		std::cout << "Permission failed" << std::endl;
	}

	BASE = base_addr; //set base address to the given variable
	setRange(0x0000); //default range should be -5V to 5V
	this->eops = eops;
}

EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint32_t analog_range)
{
	if (eops->ioperm(base_addr, 16, 1) != 0)
	{
		std::cout << "Permission failed" << std::endl;
	}

	BASE = base_addr; //set base address to the given variable
	setRange(analog_range); //set range to the given range code
	this->eops = eops;
}

EmbeddedDevice::PCM3718::~PCM3718()
{
	
}

//read in the high and low byte input, combine them and return the result
uint16_t EmbeddedDevice::PCM3718::digitalInput()
{
	uint16_t result = 0;
	uint16_t lowByte, highByte;

	//read in the low and high bytes
	lowByte = eops->inb(BASE + 0x3);  //base + 3
	highByte = eops->inb(BASE + 0xB); //base + 11

	//combine them by shifting the high byte 8 bits and OR the bytes together
	result = ((highByte << 8) | (lowByte));

	return result;
}

//return the byte as determined by the input argument (1 = high byte, 0 = low byte)
uint8_t EmbeddedDevice::PCM3718::digitalByteInput(bool high_byte)
{
	uint8_t selectedByte = 0;

	//if the argument is 1, read in the high byte and return
	if (high_byte == 1)
	{
		selectedByte = eops->inb(BASE + 0xB); //base + 11
	}
	//if the argument is 0, read in the low byte and return
	else if (high_byte == 0)
	{
		selectedByte = eops->inb(BASE + 0x3);  //base + 3
	}

	return selectedByte;
}

//Return the value of the bit as determined by the input argument (value between 0-15)
//Assumption: there's 2 bytes, bit is the position of the bit of interest, and must determine if that bit is a 1 or 0
bool EmbeddedDevice::PCM3718::digitalBitInput(uint8_t bit)
{
	bool selectedBit;
	uint8_t byte;

	if (bit > 7) //if the byte is the high byte
	{
		byte = eops->inb(BASE + 0xB); //base + 11; read in the high byte
		bit = bit - 8;
		selectedBit = ((byte >> bit) & 0x1); //shift the bit to the LSB and check it
	}
	else
	{
		byte = eops->inb(BASE + 0x3);  //base + 3; read in the low byte
		selectedBit = ((byte >> bit) & 0x1); //shift the bit to the LSB and check it
	}

	return selectedBit;
	
}

//Output the provided value to both low and high channels
void EmbeddedDevice::PCM3718::digitalOutput(uint16_t value)
{
	uint16_t lowByte, highByte;

	highByte = (value >> 8); //obtain the higher byte of value and shift it right 8 places

	lowByte = value & 0x00FF;           //obtain the lower byte of value

	eops->outb(lowByte, BASE + 0x3);    //output the low byte
	eops->outb(highByte, BASE + 0xB);   //output the high byte
}

//Output the provided value to the channel determined by the boolean input (1 = high byte, 0 = low byte)
void EmbeddedDevice::PCM3718::digitalByteOutput(bool high_byte, uint8_t value)
{
	if (high_byte == 1)
	{
		eops->outb(value, BASE + 0xB);
	}
	else if (high_byte == 0)
	{
		eops->outb(value, BASE + 0x3);
	}
}

//Change the range for analog input to be the provided rangeCode (look in manual for range definitions)
void EmbeddedDevice::PCM3718::setRange(uint32_t new_analog_range)
{
	//int32_t polarity, doubling;
	double power;

	//get the range from the input parameter
	rangeCode = new_analog_range;

	//range follows a pattern of powers of 10 according to the 2 LSB of rangecode
	range = 10 / pow(10, (double)(rangeCode & 0x3));

	//if the 2nd MSB is 1, then the range is double the others
	if ((rangeCode & 0x8) != 0)
	{
		range = 2*range;
	}

	//unipolar ranges are between 3 and 8
	if (rangeCode > 3 && rangeCode < 8)
	{
		lowRange = 0.0;
	}
	else //otherwise it's bipolar 
	{
		lowRange = -(range/2);
	}
	
}

//Receive the input in the analog channel provided, convert it to a voltage (determined by the setRange) and return it
double EmbeddedDevice::PCM3718::analogInput(uint8_t channel) const
{
	double data;   //so there are no math computation errors
	double voltage;

	//select the given channel as the start channel in register BASE + 2
	eops->outb((channel << 4) | channel, BASE + 2); //set the high and low nibbles to the stop and start channel number, and out this to the MUX scanning

	//write the range code to bits 0-3 of BASE + 1
	eops->outb(rangeCode, BASE + 1);

	//Sleep to allow time for capacitors on the board
	usleep(50000);

	//trigger the A/D conversion by writing to the A/D low byte register (BASE + 0) with any value
	eops->outb(0x1, BASE);

	//check for the end of the conversion by reading the A/D status register (BASE + 8) INT bit (5th LSB)
	//1 means it's converted, 0 means it's still converting
	while((eops->inb(BASE + 8)) & (1 << 4) != (1 << 4));

	//Clear the INT bit
	eops->outb(0x1, BASE + 8);

	//read data from the A/D converter by reading the A/D data registers (BASE + 0 and BASE + 1)
	//BASE + 1 contains the 8 MSB and the high nibble of BASE + 0 contains the other 4 numbers
	data = ((eops->inb(BASE + 1)) << 4) | ((eops->inb(BASE + 0)) >> 4);

	//convert the binary A/D data to an integer
	voltage = (data/4095)*(range) + lowRange;

	return voltage;
}

//have it output in the following style, with voltages displayed to 2dp.
// "channel 0: <channel 0 voltage>\tchannel 1: <channel 1 voltage>\n"
std::ostream& EmbeddedDevice::operator<<(std::ostream & output, const EmbeddedDevice::PCM3718 & pcm)
{
	double ch0Volt, ch1Volt;

	//set the output to 2 decimal places
	std::cout << std::setprecision(2) << std::fixed;

	//get voltages from channel 0 and channel 1
	ch0Volt = pcm.analogInput(0);
	ch1Volt = pcm.analogInput(1);

	output << "channel 0: " << ch0Volt << "\tchannel 1: " << ch1Volt << "\n";

	return output;
}

