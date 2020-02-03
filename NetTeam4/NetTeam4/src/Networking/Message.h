#pragma once
#include "BinaryStream.hpp"
#include "../NetworkManager.h"

class Message
{
public:
	virtual ~Message() = default;
	virtual void Read(BinaryStream* stream, NetworkManager& manager) = 0;
	virtual int Write(BinaryStream* stream) = 0;
};	

