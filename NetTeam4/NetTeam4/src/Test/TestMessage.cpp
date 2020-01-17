#include "TestMessage.h"

void TestMessage::Read(BinaryStream* stream)
{
	TestInt = stream->Read<int>();
	TestBool = stream->Read<bool>();

	printf("Test message read: %d\n", TestInt);
}

int TestMessage::Write(BinaryStream* stream)
{
	stream->Write<int>(TestInt);
	stream->Write<bool>(TestBool);
	return sizeof(int) + sizeof(bool);
}
