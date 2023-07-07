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
        auto currentValue = m_hardware->read(address);

        if (readCount == 0)
        {
            readValue = currentValue;
        }

        if (currentValue != readValue)
        {
            throw ReadFailException();
        }
    } while (readCount < MAX_TRY_COUNT);

    return (int)(m_hardware->read(address));
}

void DeviceDriver::write(long address, int data)
{
    // TODO: implement this method
    m_hardware->write(address, (unsigned char)data);
}