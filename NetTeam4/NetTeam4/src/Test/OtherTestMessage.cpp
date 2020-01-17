#include "OtherTestMessage.h"

void OtherTestMessage::Read(BinaryStream* stream)
{
	Value = stream->Read<float>();
	printf("Other test message %f\n", Value);
}

int OtherTestMessage::Write(BinaryStream* stream){

	stream->Write<float>(Value);

	return sizeof(float);
}
