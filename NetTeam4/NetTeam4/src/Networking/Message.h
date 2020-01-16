#pragma once
#include "BinaryStream.hpp"

class Message
{
public:
	virtual void Read(BinaryStream* stream) = 0;
	virtual int Write(BinaryStream* stream) = 0;
};

