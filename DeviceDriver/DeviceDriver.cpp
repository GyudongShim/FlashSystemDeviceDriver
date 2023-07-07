#include "DeviceDriver.h"
#include "Windows.h"
#include <stdexcept>


DeviceDriver::DeviceDriver(FlashMemoryDevice* hardware) : m_hardware(hardware)
{}

int DeviceDriver::read(long address)
{
    int readValue = m_hardware->read(address);
	for (int readCount = 1; readCount < MAX_TRY_COUNT; readCount++)
	{
        Sleep(WATING_TIME_IN_MS);
        if (m_hardware->read(address) == readValue)
            continue;

        throw ReadFailException();
	}

    return readValue;
}

void DeviceDriver::write(long address, int data)
{
    const auto currentValue = m_hardware->read(address);

    if (currentValue != DeviceDriver::ERASED_VALUE)
    {
        throw WriteFailException();
    }

    m_hardware->write(address, (unsigned char)data);
}