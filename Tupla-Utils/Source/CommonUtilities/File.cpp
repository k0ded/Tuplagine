
#include "File.h"

#include <fstream>
#include <stdexcept>

namespace CommonUtilities
{
	bool ReadFile(const char *aPath, std::string &aBuffer, int aOffset, int aLength)
	{
		FILE *file;
		errno_t error = fopen_s(&file, aPath, "r");
		bool opened = error == 0;
		if (opened)
		{
			fseek(file, 0, SEEK_END);
			int length = ftell(file) - aOffset;

			if(aLength >= 0)
			{
				assert(length >= aLength && "Cannot read more than the file length!");
				length = aLength;
			}
			
			fseek(file, aOffset, SEEK_SET);
			aBuffer.resize(length);
			fread(aBuffer.data(), sizeof(char), length, file);
			fclose(file);
		}
		else
		{
			// Max length is 94 characters according to https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/strerror-s-strerror-s-wcserror-s-wcserror-s?view=msvc-170
			// Worst case it truncates anyway
			char buf[95]; 
			strerror_s(buf, error);
			printf("Error opening file: %s!\n", buf);
			//assert(false && "Unable to open file!");
		}

		return opened;
	}
	
	bool ReadFileBinary(const char *aPath, std::vector<char>& aBuffer, unsigned int aOffset, unsigned int aLength)
	{
		std::ifstream file { aPath, std::ios::ate | std::ios::binary };

		if(!file.is_open())
		{
			printf("Error opening file: %s!\n", aPath);
			return false;
		}

		const size_t fileSize = file.tellg();

		if(fileSize < aOffset + aLength)
		{
			if (aLength == (~((unsigned int)0)))
			{
				aLength = (unsigned)fileSize - aOffset;
			}
			else 
			{
				printf("Offset and length is large than filesize!\n");
				return false;
			}
		}
		
		aBuffer.resize(aLength);
		file.seekg(aOffset);
		file.read(aBuffer.data(), aLength);
		file.close();

		return true;
	}
}
