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

class FlashSubSystemTestFixture : public Test
{
public:
	MockFlashSubSystem mock{};
	DeviceDriver* deviceDriver; 

	void Precondition(long address, int intialValue, bool isException)
	{
		constexpr uint32_t erasedValue = 0xFF;

		// Mocking the 5th read as changed value
		EXPECT_CALL(mock, read(address))
			.Times(5)
			.WillOnce(Return(intialValue))
			.WillOnce(Return(intialValue))
			.WillOnce(Return(intialValue))
			.WillOnce(Return(intialValue))
			.WillOnce(Return(intialValue + isException)); // if isException is true, it will return changed value.
	}

protected:
	void SetUp() override
	{
		deviceDriver = new DeviceDriver(&mock);
	}
	void TearDown() override
	{
		free(deviceDriver);
	};
};


TEST_F(FlashSubSystemTestFixture, ReadBasic)
{
	constexpr long anyAddress = 0x0cafebeefL;
	const int initialValue = 0xFF;

	Precondition(anyAddress, initialValue, false);

	try
	{
		auto readValue = deviceDriver->read(anyAddress);
		EXPECT_THAT(readValue, Eq(initialValue));
	} catch (ReadFailException& readException)
	{
		EXPECT_THAT(&readException, NotNull());
		FAIL();
	}
}


TEST_F(FlashSubSystemTestFixture, ReadException)
{
	const long anyAddress = 0x0cafebeefL;
	const int initialValue = 0xFF;
	Precondition(anyAddress, 0xFF, true);

	try
	{
		constexpr long anyAddress = 0xbeefULL;
		auto readValue = deviceDriver->read(anyAddress);
		FAIL();
	}
	catch (ReadFailException& readException)
	{
		EXPECT_THAT(&readException, NotNull());
		// Pass! exception is expected
	}
}


TEST_F(FlashSubSystemTestFixture, WriteErasedCondition) {
	MockFlashSubSystem mock;

	try
	{
		constexpr long anyAddress = 0xbeefULL;
		constexpr int writeValue = 0xab;
		deviceDriver->write(anyAddress, writeValue);
	}
	catch (WriteFailException& writeException)
	{
		EXPECT_THAT(&writeException, NotNull());
		// Expecting valid write
		FAIL();
	}
}