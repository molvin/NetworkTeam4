#pragma once
#include "../Networking/Message.h"

class OtherTestMessage : Message
{
public:

	float Value;

	virtual void Read(BinaryStream* stream) override;

	virtual int Write(BinaryStream* stream) override;

};

