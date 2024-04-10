
#include "File.h"

#include <cassert>
#include <fstream>
#include <filesystem>

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

	bool WriteFileBinary(const char* aPath, const std::byte* aData, unsigned aLength, int aOffset)
	{
		CreateDirectoriesForPath(aPath);
		std::ofstream file{ aPath, std::ios::out | std::ios::ate | std::ios::binary };

		if (!file)
		{
			printf("Error opening file: %s!\n", aPath);
			return false;
		}

		const size_t fileSize = static_cast<size_t>(file.tellp()); // Returns cursor position zero-indexed so we add 1 to get the actual size.

		if(fileSize < aOffset)
		{
			printf("Trying to write on an offset outside of the file size!\n");
			return false;
		}

		file.seekp(aOffset);
		file.write((const char*)aData, aLength);
		file.close();

		return true;
	}

	bool RemoveFile(const char* aPath)
	{
		return std::filesystem::remove(aPath);
	}

	void FindAll(const char* aPath, std::vector<std::string>& aPaths, PathType aPathType)
	{
		for (const auto& entry : std::filesystem::directory_iterator(aPath))
		{
			if(aPathType == PathType::File)
			{
				if (entry.is_regular_file())
				{
					aPaths.push_back(entry.path().string());
				}
			}
			else if (aPathType == PathType::Directory)
			{
				if (entry.is_directory())
				{
					aPaths.push_back(entry.path().string());
				}
			}
			else if(aPathType == PathType::Path)
			{
				aPaths.push_back(entry.path().string());
			}

			if (entry.is_directory())
			{
				FindAll(entry.path().string().c_str(), aPaths, aPathType);
			}
		}
	}

	bool CreateDirectoriesForPath(const char* aPath)
	{
		const auto file = std::filesystem::path(aPath);

		if (!file.has_parent_path())
			return true;

		const std::filesystem::path tmp = file.parent_path();
		return std::filesystem::create_directories(tmp);
	}

	bool CreateDirectories(const char* aPath)
	{
		return std::filesystem::create_directories(aPath);
	}

	bool FileExists(const char* aPath)
	{
		return std::filesystem::exists(aPath);
	}

	size_t GetFileTimeStamp(const char* aPath)
	{
		if (!std::filesystem::exists(aPath))
			return 0;
		return std::filesystem::last_write_time(aPath).time_since_epoch().count();
	}

	size_t ReadFileBinary(const char *aPath, std::vector<std::byte>& aData, unsigned int aLength, unsigned int aOffset)
	{
		std::ifstream file { aPath, std::ios::ate | std::ios::binary };

		if(!file.is_open())
		{
			printf("Error opening file: %s!\n", aPath);
			return 0;
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
				return 0;
			}
		}

		aData.resize(aLength);
		file.seekg(aOffset);
		file.read(reinterpret_cast<char*>(aData.data()), aLength);
		file.close();

		return aLength;
	}
}
