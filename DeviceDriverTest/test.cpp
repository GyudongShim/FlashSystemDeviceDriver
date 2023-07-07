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

	EXPECT_CALL(mock, read(_)).WillRepeatedly(Return(100));
	DeviceDriver deviceDriver{ &mock };

	try
	{
		const int anyAddress = 0xcafebeef;
		auto readValue = deviceDriver.read(anyAddress);
		EXPECT_THAT(readValue, Eq(100));
	} catch (ReadFailException& e)
	{
		FAIL();
	}
}


TEST(MockFlashSubSystemTest, ReadException) {
	MockFlashSubSystem mock;
	constexpr uint32_t fixedValue = 0xbeefbeef;

	// Mocking the 5th read as changed value
	EXPECT_CALL(mock, read(_))
		.WillOnce(Return(fixedValue))
		.WillOnce(Return(fixedValue))
		.WillOnce(Return(fixedValue))
		.WillOnce(Return(fixedValue))
		.WillOnce(Return(fixedValue + 1));
	DeviceDriver deviceDriver{ &mock };

	try
	{
		constexpr long anyAddress = 0xbeefULL;
		auto readValue = deviceDriver.read(anyAddress);
		FAIL();
	}
	catch (ReadFailException& e)
	{
		// Pass!
	}
}