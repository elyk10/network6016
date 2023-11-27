#include "buffer.h"

namespace network
{
	// Constructor of the Buffer class should initialize all
	// of the variable of the class. We can prepare the vector
	// by setting the size of the vector ahead of time, with
	// default values set. 
	// Check the difference between resize and reserve:
	// Resize: https://cplusplus.com/reference/vector/vector/resize/
	// Resrve: https://cplusplus.com/reference/vector/vector/reserve/
	Buffer::Buffer(uint32 size)
	{
		m_BufferData.resize(size);
		m_WriteIndex = 0;
		m_ReadIndex = 0;
	}

	// Destructor of the buffer class can do some cleanup. It
	// is not necessary to do any cleanup in this class, since
	// we do not have any pointers that need to be deleted, but
	// we can clear the vector anyway.
	//
	// For extra safety and helpful debugging purposes we 
	// can add a custom clearing function that is called when
	// a #define flag is set that sets all of the bytes to a 
	// specific value such as 0xDEADBEEF or something else 
	// that would be obvious as not useable. A pointer may 
	// be set to a position in the buffer data array.
	Buffer::~Buffer()
	{
		m_BufferData.clear();
	}

	void Buffer::WriteUInt32LE(size_t index, uint32 value)
	{
		if (index + sizeof(value) >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + value);
		}

		for (int i = 0; i < sizeof(value); i++)
		{
			m_BufferData[index + i] = value >> 8 * i;
		}
	}

	void Buffer::WriteUInt32LE(uint32 value)
	{
		if (m_WriteIndex + sizeof(value) >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + value);
		}

		for (int i = 0; i < sizeof(value); i++)
		{
			m_BufferData[m_WriteIndex++] = value >> 8 * i;
		}
	}

	void Buffer::WriteUShort16LE(uint16 value)
	{
		if (m_WriteIndex + sizeof(value) >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + value);
		}
		for (int i = 0; i < sizeof(value); i++)
		{
			m_BufferData[m_WriteIndex++] = value >> 8 * i;
		}
	}

	void Buffer::WriteUShort16LE(size_t index, uint16 value)
	{
		if (index + sizeof(value) >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + value);
		}
		for (int i = 0; i < sizeof(value); i++)
		{
			m_BufferData[index + i] = value >> 8 * i;
		}
	}

	void Buffer::WriteString(const std::string& str)
	{
		if (m_WriteIndex + str.length() >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + str.length());
		}
		for (int i = 0; i < str.length(); i++)
		{
			m_BufferData[m_WriteIndex++] = str[i];
		}
	}

	void Buffer::WriteString(size_t index, const std::string& str)
	{
		if (index + str.length() >= m_BufferData.size())
		{
			GrowBuffer(m_BufferData.size() + str.length());
		}
		for (int i = 0; i < str.length(); i++)
		{
			m_BufferData[index + i] = str[i];
		}
	}

	uint32 Buffer::ReadUInt32LE(size_t index)
	{
		uint32_t value = 0;
		value |= m_BufferData[index];
		value |= m_BufferData[index + 1] << 8;
		value |= m_BufferData[index + 2] << 16;
		value |= m_BufferData[index + 3] << 24;

		return value;
	}

	uint32 Buffer::ReadUInt32LE()
	{
		uint32_t value = 0;
		value |= m_BufferData[m_ReadIndex++];
		value |= m_BufferData[m_ReadIndex++] << 8;
		value |= m_BufferData[m_ReadIndex++] << 16;
		value |= m_BufferData[m_ReadIndex++] << 24;

		return value;
	}

	uint16 Buffer::ReadUShort16LE(size_t index)
	{
		uint16_t value = 0;
		value |= m_BufferData[m_ReadIndex++];
		value |= m_BufferData[m_ReadIndex++] << 8;

		return value;

	}

	uint16 Buffer::ReadUShort16LE()
	{
		uint16_t value = 0;
		value |= m_BufferData[m_ReadIndex++];
		value |= m_BufferData[m_ReadIndex++] << 8;

		return value;

	}

	std::string Buffer::ReadString(uint32 length)
	{
		std::string str;
		for (int i = 0; i < length; i++)
		{
			str.push_back(m_BufferData[m_ReadIndex++]);
		}

		return str;
	}

	std::string Buffer::ReadString(size_t index, uint32 length)
	{
		std::string str;
		for (int i = 0; i < length; i++)
		{
			str.push_back(m_BufferData[index + i]);
		}

		return str;
	}

	void Buffer::GrowBuffer(uint32 size)
	{
		m_BufferData.resize(size);
	}
}