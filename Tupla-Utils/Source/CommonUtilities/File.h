#pragma once

#include <string>
#include <assert.h>
#include <vector>

namespace CommonUtilities
{
	// Reads the entire contents of the given file into aBuffer.
	// Returns true on success.
	bool ReadFile(const char *aPath, std::string &aBuffer, int aOffset = 0, int aLength = -1);

	bool ReadFileBinary(const char *aPath, std::vector<char>& aBuffer, int aOffset = 0, int aLength = -1);
}
