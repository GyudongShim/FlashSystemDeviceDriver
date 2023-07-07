#pragma once

#include "FlashMemoryDevice.h"
#include <stdexcept>

using namespace std;

class ReadFailException : public exception
{
};

class WriteFailException : public exception
{
};

class DeviceDriver
{
public:
    DeviceDriver(FlashMemoryDevice* hardware);
    int read(long address);
    void write(long address, int data);

protected:
    static constexpr int WATING_TIME_IN_MS = 200;
    static constexpr int MAX_TRY_COUNT = 5;
    FlashMemoryDevice* m_hardware;
};