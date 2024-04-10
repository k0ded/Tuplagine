#pragma once

inline char MakeLowercase(char c)
{
	return c >= 'A' && c <= 'Z' ? c - ('A' - 'a') : c;
}

inline const char* FindInsensitive(const std::string& hayStack, const std::string& needle)
{
	ASSERT(!needle.empty(), "Finding a needle in a haystack where the needle doesn't exist :(")
	if (needle.size() > hayStack.size()) return nullptr;

	const auto searchEnd = hayStack.end() - needle.size() + 1;
	const char needle0 = MakeLowercase(needle[0]);

	auto c = hayStack.begin();
	while (c != searchEnd)
	{
		if(MakeLowercase(*c) == needle0)
		{
			auto n = needle.begin() + 1;
			auto c2 = c + 1;
			while (n != needle.end() && c2 != hayStack.end()) {
				if (MakeLowercase(*n) != MakeLowercase(*c2)) break;
				++n;
				++c2;
			}
			if (n == needle.end()) return &(*c);
		}
		++c;
	}

	return nullptr;
}

inline std::string WideToString(const std::wstring& aString)
{
	const int length = static_cast<int>(aString.length());

	const int multiByteLength = WideCharToMultiByte(
		CP_UTF8,
		0,
		aString.c_str(), 
		length, 
		nullptr, 
		0, 
		nullptr, 
		nullptr
	);

	std::string result(multiByteLength, L'\0');

	WideCharToMultiByte(
		CP_UTF8,
		0, 
		aString.c_str(),
		-1, 
		result.data(),
		multiByteLength, 
		nullptr, 
		nullptr
	);

	return result;
}

inline std::wstring StringToWide(const std::string& aString)
{
	const int length = static_cast<int>(aString.length());

	const int charLength = MultiByteToWideChar(
		CP_UTF8,
		0,
		aString.c_str(),
		length,
		nullptr,
		0
	);

	std::wstring result(charLength, L'\0');

	MultiByteToWideChar(
		CP_UTF8,
		0,
		aString.c_str(),
		-1,
		result.data(),
		charLength
	);

	return result;
}
