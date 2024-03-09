
#include "File.h"

namespace CommonUtilities
{
	bool ReadWholeFile(const char *aPath, std::string &aBuffer)
	{
		FILE *file;
		errno_t error = fopen_s(&file, aPath, "r");
		bool opened = error == 0;
		if (opened)
		{
			fseek(file, 0, SEEK_END);
			int length = ftell(file);
			fseek(file, 0, SEEK_SET);
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
}