#pragma once
#include <string>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace CommonUtilities
{
	// Incredible template work from greatwolf on stackexchange https://codereview.stackexchange.com/questions/1205/c-string-cast-template-function/1466#1466
	template <typename T>
	struct StringTraits;

	template <>
	struct StringTraits<std::string>
	{
		typedef char char_trait;
		static int ByteConvert(const int codepage, LPCSTR data, int data_length, LPWSTR buffer, int buffer_size)
		{
			return ::MultiByteToWideChar(codepage, 0, data, data_length, buffer, buffer_size);
		}
	};

	template <>
	struct StringTraits<std::wstring>
	{
		typedef wchar_t char_trait;
		static int ByteConvert(const int codepage, LPCWSTR data, int data_length, LPSTR buffer, int buffer_size)
		{
			return ::WideCharToMultiByte(codepage, 0, data, data_length, buffer, buffer_size, nullptr, nullptr);
		}
	};

	template <typename T>
	struct StringTypeOf;

	template <>
	struct StringTypeOf<const char*>
	{
		typedef std::string wrap;
	};

	template <>
	struct StringTypeOf<const wchar_t*>
	{
		typedef std::wstring wrap;
	};

	template <typename Td, typename Ts>
	struct StringCastImpl
	{
		static Td cast(const Ts& source)
		{
			const int length = StringTraits<Ts>::ByteConvert(CP_ACP, source.data(), static_cast<int>(source.length()), NULL, 0);

			if(length == 0)
			{
				return Td();
			}

			std::vector<typename StringTraits<Td>::char_trait> buffer(length);
			StringTraits<Ts>::ByteConvert(CP_ACP, source.data(), static_cast<int>(source.length()), buffer.data(), length);
			return Td(buffer.begin(), buffer.end());
		}
	};

	template <typename Td, typename Ts>
	Td string_cast(Ts* source)
	{
		assert(source != nullptr);
		return StringCastImpl<Td, typename StringTypeOf<const Ts*>::wrap>::cast(source);
	}

	template <typename Td, typename Ts>
	Td string_cast(const Ts& source)
	{
		return StringCastImpl<Td, Ts>::cast(source);
	}

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
			if (MakeLowercase(*c) == needle0)
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
}

namespace CU = CommonUtilities;