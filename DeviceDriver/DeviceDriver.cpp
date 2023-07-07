#include "DeviceDriver.h"
#include "Windows.h"
#include <stdexcept>


DeviceDriver::DeviceDriver(FlashMemoryDevice* hardware) : m_hardware(hardware)
{}

int DeviceDriver::read(long address)
{
    int readCount = 0;
    int readValue = 0;
    do
    {
        const auto currentValue = m_hardware->read(address);
        if (readCount == 0)
        {
            readValue = currentValue;
        }

        if (currentValue != readValue)
        {
            throw ReadFailException();
        }

        Sleep(WATING_TIME_IN_MS);
    } while (++readCount < MAX_TRY_COUNT);

    return readValue;
}

void DeviceDriver::write(long address, int data)
{
    const auto currentValue = m_hardware->read(address);

    if (currentValue != 0xFF)
    {
        throw WriteFailException();
    }

    m_hardware->write(address, (unsigned char)data);
}