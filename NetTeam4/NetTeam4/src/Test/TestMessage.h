#pragma once
#include "../Networking/Message.h"
#include "../Networking/BinaryStream.hpp"

class TestMessage : Message
{
public:
	virtual void Read(BinaryStream* stream);
	virtual int Write(BinaryStream* stream);

	int TestInt = 6;
	bool TestBool = true;

};

