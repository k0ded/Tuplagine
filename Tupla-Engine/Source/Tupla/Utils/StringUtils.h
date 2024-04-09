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
