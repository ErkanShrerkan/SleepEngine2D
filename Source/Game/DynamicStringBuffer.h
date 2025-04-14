#pragma once

class DynamicStringBuffer
{
public:
	DynamicStringBuffer();
	DynamicStringBuffer(const std::string& aString);
	DynamicStringBuffer(uint aSize);
	void SetString(const std::string& aString);
	void SetSize(uint aSize);
	uint GetSize() const;
	uint GetStringSize() const;
	std::string GetString() const;
	char* operator[](uint anIndex);

private:
	std::vector<char> buf;
};

