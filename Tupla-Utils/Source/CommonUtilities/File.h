#pragma once

#include <string>
#include <assert.h>

namespace CommonUtilities
{
	// Reads the entire contents of the given file into aBuffer.
	// Returns true on success.
	bool ReadWholeFile(const char *aPath, std::string &aBuffer);
}