#pragma once

#include <string>
#include <vector>

namespace CommonUtilities
{
	enum class PathType
	{
		Directory,
		File,
		Path
	};

	// Reads the entire contents of the given file into aBuffer.
	// Returns true on success.
	bool ReadFile(const char *aPath, std::string &aBuffer, int aOffset = 0, int aLength = -1);

	size_t ReadFileBinary(const char *aPath, std::vector<std::byte>& aData, unsigned int aLength = ~0u, unsigned int aOffset = 0);
	bool WriteFileBinary(const char* aPath, const std::byte* aData, unsigned int aLength, int aOffset = 0);

	void FindAll(const char* aPath, std::vector<std::string>& aPaths, PathType aPathType = PathType::File);

	bool CreateDirectoriesForPath(const char* aPath);
	bool CreateDirectories(const char* aPath);

	bool FileExists(const char* aPath);

	size_t GetFileTimeStamp(const char* aPath);
}
