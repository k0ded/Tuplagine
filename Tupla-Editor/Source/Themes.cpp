#include "Themes.h"
#include <imgui.h>

Tuplitor::Themes::Themes()
{
}
Tuplitor::Themes::~Themes()
{
	PopTheme();
	PopSize();
	PopFont();
}

void Tuplitor::Themes::SetTheme(Theme aTheme)
{
	myThemesHistory.push_back(aTheme);

	switch (aTheme)
	{
	case Tuplitor::Theme::Light:
		ImGui::StyleColorsLight();
		break;
	case Tuplitor::Theme::Dark:
		ImGui::StyleColorsDark();
		break;
	case Tuplitor::Theme::Tupla:
	{
		// Text
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.5f, 0.5f, 0.5f, 1));

		// Background
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.01f, 0.01f, 0.01f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.07f, 0.07f, 0.07f, 0.95f));

		// Border
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 0.1f));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));

		// Frame
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.3f, 0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.25f, 0.6f, 0.95f, 0.4f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.6f, 0.95f, 0.65f));

		// Title

		// MenuBar

		// Scrollbar







	}
		//ImGui::PushStyleColor(ImGuiCol_);
		break;
	default:
		break;
	}
}
void Tuplitor::Themes::PopTheme()
{
	for (int i = myThemesHistory.size() - 1; i >= 0; i--)
	{
		switch (myThemesHistory.back())
		{
		case Tuplitor::Theme::Light:
			break;
		case Tuplitor::Theme::Dark:
			break;
		case Tuplitor::Theme::Tupla:
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			break;
		default:
			break;
		}
		myThemesHistory.pop_back();
	}
}

void Tuplitor::Themes::SetSize(Size aSize)
{
	mySizeHistory.push_back(aSize);

	switch (aSize)
	{
	case Tuplitor::Size::Rounded:
#pragma region Size

#pragma region Main

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(7, 4));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(5, 4));

		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 16);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 7);


#pragma endregion

#pragma region Borders

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 1);

#pragma endregion

#pragma region Rounding

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 5);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 12);
		ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 12);
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7);

#pragma endregion

#pragma region Tables

		/*ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4, 2));
		ImGui::PushStyleVar(ImGuiTableColumnFlags_AngledHeader, 35);*/

#pragma endregion

#pragma region Widgets

		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.00f, 0.50f));

		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextAlign, ImVec2(0.00f, 0.00f));
		ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextPadding, ImVec2(20, 2));


#pragma endregion

#pragma endregion
		break;
	default:
		break;
	}
}
void Tuplitor::Themes::PopSize()
{
	for (int i = mySizeHistory.size() - 1; i >= 0; i--)
	{
		switch (mySizeHistory.back())
		{
		case Tuplitor::Size::Rounded:
#pragma region Size

#pragma region Main

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

#pragma endregion

#pragma region Borders

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

#pragma endregion

#pragma region Rounding

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

#pragma endregion

#pragma region Tables

			/*ImGui::PopStyleVar();
			ImGui::PopStyleVar();*/

#pragma endregion

#pragma region Widgets

			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

#pragma endregion

#pragma endregion
			break;
		default:
			break;
		}
		mySizeHistory.pop_back();
	}


}

void Tuplitor::Themes::SetFont(Font aFont)
{
	myFontHistory.push_back(aFont);

	switch (aFont)
	{
	case Tuplitor::Font::Standard:
		break;
	case Tuplitor::Font::Tratex:
		break;
	default:
		break;
	}
}
void Tuplitor::Themes::PopFont()
{
	while (!myFontHistory.empty())
	{
		myFontHistory.pop_back();
	}
}


