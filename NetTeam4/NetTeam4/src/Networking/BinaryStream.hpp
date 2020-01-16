#pragma once
#include <vector>
typedef unsigned char byte;

class BinaryStream
{
public:
	BinaryStream() = default;

	template<typename T>
	void Write(const T& value);

	void Write(const void* const ptr, const size_t size);

	template<typename T>
	T Read();

	template<typename T>
	T Peek() const;

	template<typename T>
	BinaryStream& operator<<(T& value);

	template<typename T>
	BinaryStream& operator>>(T& output);

	const byte* Begin() const 
	{
		if (Buffer.size() == 0) 
			return nullptr; return &Buffer[0];
	};

	unsigned int Size() const
	{ 
		return Buffer.size();
	};

	void Clear();

	void Reset();

	bool EndOfStream() const 
	{ 
		return Cursor >= Buffer.size();
	}

	size_t GetCursor() const 
	{ 
		return Cursor;
	}

	void SetCursor(size_t pos);

	std::vector<byte> Buffer;
	size_t Cursor = 0;


};

template<typename T> inline void BinaryStream::Write(const T& value)
{ 
	Write(&value, sizeof(T)); 
}
inline void BinaryStream::Write(const void* const ptr, const size_t dataSize)
{
	Buffer.resize(Size() + dataSize);

	// Copy data from pointer onto the buffer
	memcpy(&Buffer[Size() - dataSize], ptr, dataSize);
}

template<typename T> inline T BinaryStream::Read()
{
	// Get value at cursor and advance
	T value = Peek<T>();
	Cursor += sizeof(T);

	return value;
}

template<typename T> inline T BinaryStream::Peek() const 
{
	// Check if buffer is too small
	if (Cursor + sizeof(T) > Size())
		throw std::exception("Tried to read beyond stream size");

	T value;
	// Copy memory into value from the buffer
	memcpy(&value, &Buffer[Cursor], sizeof(T));

	return value;
}

template<typename T> inline BinaryStream& BinaryStream::operator<<(T& value)
{
	Write(value);
	return *this;
}

template<typename T> inline BinaryStream & BinaryStream::operator>>(T & output) {
	// Read value
	T value = Read<T>();

	// Copy memory
	memcpy(&output, &value, sizeof(T));

	return *this;
}

inline void BinaryStream::Clear()
{
	Buffer.clear();
	Reset();
}

inline void BinaryStream::Reset() 
{ 
	SetCursor(0); 
}

inline void BinaryStream::SetCursor(size_t pos) {
	if (pos > Size())
		throw std::exception("Tried to set cursor position beyond stream size");

	Cursor = pos;
}
