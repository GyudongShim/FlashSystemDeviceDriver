#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../DeviceDriver/DeviceDriver.cpp"

using namespace testing;

class MockFlashSubSystem : public FlashMemoryDevice
{
public:
	MOCK_METHOD(unsigned char, read, (long address), (override));
	MOCK_METHOD(void, write, (long address, unsigned char data), (override));
};

class FlashSubSystemTestFixture : public Test
{
public:
	MockFlashSubSystem* pMock;
	DeviceDriver* pDeviceDriver;

	void PreconditionForRead(long address, int intialValue, bool isException)
	{
		// Mocking the 5th read as changed value
		EXPECT_CALL(*pMock, read(_))
			.Times(5)
			.WillOnce(Return((unsigned char)intialValue))
			.WillOnce(Return((unsigned char)intialValue))
			.WillOnce(Return((unsigned char)intialValue))
			.WillOnce(Return((unsigned char)intialValue))
			.WillOnce(Return((unsigned char)intialValue + isException)); // if isException is true, it will return changed value.
	}

	void PreconditionForWrite(long address, int intialValue)
	{
		// Mocking the 5th read as changed value
		EXPECT_CALL(*pMock, read(_))
			.Times(1)
			.WillOnce(Return((unsigned char)intialValue));

		if (intialValue == DeviceDriver::ERASED_VALUE)
			EXPECT_CALL(*pMock, write(_, _));
	}

protected:
	void SetUp() override
	{
		// pMock = new NiceMock<MockFlashSubSystem>();
		pMock = new MockFlashSubSystem();
		pDeviceDriver = new DeviceDriver(pMock);
	}

	void TearDown() override
	{
		delete pDeviceDriver;
		delete pMock;
	}
};


TEST_F(FlashSubSystemTestFixture, ReadBasic)
{
	constexpr long anyAddress = 0x0cafebeefL;

	PreconditionForRead(anyAddress, DeviceDriver::ERASED_VALUE, false);

	try
	{
		auto readValue = pDeviceDriver->read(anyAddress);
		EXPECT_THAT(readValue, Eq(DeviceDriver::ERASED_VALUE));
	} catch (ReadFailException& readException)
	{
		EXPECT_THAT(&readException, NotNull());
		FAIL();
	}
}


TEST_F(FlashSubSystemTestFixture, ReadException)
{
	const long anyAddress = 0x0cafebeefL;
	PreconditionForRead(anyAddress, DeviceDriver::ERASED_VALUE, true);

	try
	{
		auto readValue = pDeviceDriver->read(anyAddress);
		FAIL();
	}
	catch (ReadFailException& readException)
	{
		EXPECT_THAT(&readException, NotNull());
		// Pass! exception is expected
	}
}


TEST_F(FlashSubSystemTestFixture, ErasedPrecondition)
{
	const long anyAddress = 0x0cafebeefL;
	const int initialValue = DeviceDriver::ERASED_VALUE;
	PreconditionForWrite(anyAddress, initialValue);

	try
	{
		constexpr unsigned char writeValue = 0xab;
		pDeviceDriver->write(anyAddress, writeValue);
	}
	catch (WriteFailException& writeException)
	{
		EXPECT_THAT(&writeException, NotNull());
		// Expecting valid write
		FAIL();
	}
}


TEST_F(FlashSubSystemTestFixture, NotErasesPrecondition)
{
	const long anyAddress = 0x0cafebeefL;
	const int notErasedInitialValue = DeviceDriver::ERASED_VALUE + 1;
	PreconditionForWrite(anyAddress, notErasedInitialValue);

	try
	{
		constexpr unsigned char writeValue = 0xab;
		pDeviceDriver->write(anyAddress, writeValue);
		FAIL();
	}
	catch (WriteFailException& writeException)
	{
		EXPECT_THAT(&writeException, NotNull());
		// Expecting exception
	}
}
