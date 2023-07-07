#pragma once

#include <string>
#include <iostream>
#include "DeviceDriver.h"

using namespace std;

class FlashApplication
{
public:

	FlashApplication(DeviceDriver* pDeviceDriver)
		: m_pDeviceDriver(pDeviceDriver)
	{
	}

	string ReadAndPrint(long startAddress, long endAddress)
	{
		if (startAddress >= endAddress)
			throw invalid_argument("Not allowed");

		if (startAddress % 4 != 0)
		{
			throw invalid_argument("start address should be aligned");
		}

		if (endAddress % 4 != 0)
		{
			throw invalid_argument("end address should be aligned");
		}

		string result = "";
		for (long address = startAddress; address < endAddress; address+= 4)
		{
			auto data = m_pDeviceDriver->read(address);
			result += "Address: " +  std::to_string(address);
			result += " Value: " + std::to_string(data);
			result += "\n";
		}

		return result;
	}

	void WriteAll(unsigned char value)
	{
		long address = 0;
		m_pDeviceDriver->write(address, value);
	}

private:
	DeviceDriver* m_pDeviceDriver{};
};