#include "pch.h"
#include "DynamicStringBuffer.h"

DynamicStringBuffer::DynamicStringBuffer()
{
	SetSize(128);
}

DynamicStringBuffer::DynamicStringBuffer(const std::string& aString)
{
	SetSize(128);
	SetString(aString);
}

DynamicStringBuffer::DynamicStringBuffer(uint aSize)
{
	SetSize(aSize);
}

void DynamicStringBuffer::SetString(const std::string& aString)
{
	strncpy_s(&buf[0], buf.size(), aString.data(), buf.size() - 1);
}

void DynamicStringBuffer::SetSize(uint aSize)
{
	buf.resize(aSize);
}

uint DynamicStringBuffer::GetSize()
{
	return (uint)buf.size();
}

std::string DynamicStringBuffer::GetString()
{
	return std::string(&buf[0]);
}

char* DynamicStringBuffer::operator[](uint anIndex)
{
	return &buf[anIndex];
}