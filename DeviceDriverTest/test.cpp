#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "../DeviceDriver/DeviceDriver.cpp"
#include "../DeviceDriver/FlashApplication.cpp"


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
		EXPECT_CALL(*pMock, read(address))
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
		EXPECT_CALL(*pMock, read(address))
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

TEST_F(FlashSubSystemTestFixture, FlashApplicationReadAndPrint)
{
	FlashApplication application{pDeviceDriver};

	long startingAddress = 100;
	long endAddress = 120;

	unsigned char initialData[] = { 0xaa, 0xab, 0xac, 0xad, 0xae };

	for (int address = startingAddress, offset = 0; address < endAddress; address+= 4, offset++)
	{
		PreconditionForRead(address, initialData[offset], false);
	}

	try
	{
		string result = application.ReadAndPrint(100, 120);
		cout << result << endl;

		EXPECT_THAT(result, StrEq("Address: 100 Value: 170\nAddress: 104 Value: 171\nAddress: 108 Value: 172\nAddress: 112 Value: 173\nAddress: 116 Value: 174\n"));
	}
	catch (WriteFailException& writeException)
	{
		FAIL();
	}
}


TEST_F(FlashSubSystemTestFixture, WriteAll)
{
	FlashApplication application{pDeviceDriver};
	long baseAddress = 0;
	PreconditionForWrite(baseAddress, DeviceDriver::ERASED_VALUE);
	unsigned char writeValue = 99;
	try
	{
		application.WriteAll(writeValue);
	}
	catch (WriteFailException& writeException)
	{
		FAIL();
	}

	PreconditionForRead(baseAddress, writeValue, false);
	try
	{
		auto result = application.ReadAndPrint(0, 4);
		cout << result << endl;
	}
	catch (ReadFailException& readException)
	{
		FAIL();
	}
}