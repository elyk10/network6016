#pragma once

//#include "common.h"
#include <vector>
#include <string>
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
using namespace std;

namespace network
{
	class Buffer
	{
	public:
		Buffer(uint32 size = 512);
		~Buffer();

		void WriteUInt32LE(size_t index, uint32 value);
		void WriteUInt32LE(uint32 value);
		void WriteUShort16LE(size_t index, uint16 value);
		void WriteUShort16LE(uint16 value);
		void WriteString(size_t index, const std::string& str);
		void WriteString(const std::string& str);
		uint32 ReadUInt32LE(size_t index);
		uint32 ReadUInt32LE();
		uint16 ReadUShort16LE(size_t index);
		uint16 ReadUShort16LE();
		std::string ReadString(size_t index, uint32 length);
		std::string ReadString(uint32 length);


	//private:
		
		// this will grow the buffer when serializing past the write index
		void GrowBuffer(uint32 size);
		
		// this stores all of the data within the buffer
		vector<uint8> m_BufferData;

		// The index to write the next byte of data in the buffer
		uint32 m_WriteIndex;

		// The index to read the next byte of data from the buffer
		uint32 m_ReadIndex;

		
	};
}