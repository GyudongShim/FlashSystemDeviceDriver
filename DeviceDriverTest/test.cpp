#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../DeviceDriver/DeviceDriver.cpp"

using namespace testing;

class MockFlashSubSystem : public FlashMemoryDevice
{
public:
	MOCK_METHOD(unsigned char, read, (long address), (override));
	MOCK_METHOD(void, write, (long address, unsigned char data));
};

TEST(MockFlashSubSystemTest, ReadBasic) {
	MockFlashSubSystem mock;
	const int anyAddress = 0xcafebeef;

	EXPECT_CALL(mock, read(_)).WillRepeatedly(Return(100));
	DeviceDriver deviceDriver{ &mock };

	try
	{
		auto readValue = deviceDriver.read(anyAddress);
		EXPECT_THAT(readValue, Eq(100));
	} catch (ReadFailException e)
	{
		FAIL();
	}
}