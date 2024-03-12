#pragma once
#include <vector>

namespace Tuplitor
{
	enum class Theme : int
	{
		Light,
		Dark,
		Tupla
	};
	enum class Size : int
	{
		Rounded
	};
	enum class Font : int
	{
		Standard,
		Tratex
	};
	class Themes
	{
	public:
		Themes();
		~Themes();

		void SetTheme(Theme aTheme);
		void PopTheme();

		void SetSize(Size aSize);
		void PopSize();

		void SetFont(Font aFont);
		void PopFont();

	private:
		std::vector<Theme> myThemesHistory;
		std::vector<Size> mySizeHistory;
		std::vector<Font> myFontHistory;
	};
}

