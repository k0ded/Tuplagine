#include "Themes.h"
#include <iostream>

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
		ImGui::PushStyleColor(ImGuiCol_Text, Tuplitor::Themes::RGBA(255, 255, 255, 255));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, Tuplitor::Themes::RGBA(128, 128, 128, 255));

		// Background
		ImGui::PushStyleColor(ImGuiCol_WindowBg, Tuplitor::Themes::RGBA(20, 20, 20, 255));
		ImGui::PushStyleColor(ImGuiCol_ChildBg, Tuplitor::Themes::RGBA(0, 0, 0, 128));
		ImGui::PushStyleColor(ImGuiCol_PopupBg, Tuplitor::Themes::RGBA(20, 20, 20, 240));

		// Border
		ImGui::PushStyleColor(ImGuiCol_Border, Tuplitor::Themes::RGBA(200, 200, 200, 25));
		ImGui::PushStyleColor(ImGuiCol_BorderShadow, Tuplitor::Themes::RGBA(0, 0, 0, 0));

		// Frame
		ImGui::PushStyleColor(ImGuiCol_FrameBg, Tuplitor::Themes::RGBA(75, 10, 10, 128));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, Tuplitor::Themes::RGBA(200, 10, 10, 128));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, Tuplitor::Themes::RGBA(40, 10, 10, 128));

		// Title
		ImGui::PushStyleColor(ImGuiCol_TitleBg, Tuplitor::Themes::RGBA(10, 10, 10, 255));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, Tuplitor::Themes::RGBA(10, 200, 200, 100));
		ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, Tuplitor::Themes::RGBA(0, 0, 0, 255));

		// MenuBar
		ImGui::PushStyleColor(ImGuiCol_MenuBarBg, Tuplitor::Themes::RGBA(35, 35, 35, 255));

		// Scrollbar
		ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, Tuplitor::Themes::RGBA(5, 5, 5, 135));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, Tuplitor::Themes::RGBA(25, 25, 25, 255));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, Tuplitor::Themes::RGBA(50, 50, 50, 255));
		ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, Tuplitor::Themes::RGBA(80, 80, 80, 255));

		// Check Mark
		ImGui::PushStyleColor(ImGuiCol_CheckMark, Tuplitor::Themes::RGBA(255, 255, 255, 255));

		// Slider Grab
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, Tuplitor::Themes::RGBA(180, 40, 40, 255));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, Tuplitor::Themes::RGBA(230, 150, 150, 255));

		// Button
		ImGui::PushStyleColor(ImGuiCol_Button, Tuplitor::Themes::RGBA(230, 80, 80, 100));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Tuplitor::Themes::RGBA(255, 130, 130, 100));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, Tuplitor::Themes::RGBA(255, 200, 200, 100));

		// Header
		ImGui::PushStyleColor(ImGuiCol_Header, Tuplitor::Themes::RGBA(115, 5, 5, 80));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Tuplitor::Themes::RGBA(115, 30, 30, 80));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, Tuplitor::Themes::RGBA(115, 60, 60, 80));

		// Separator
		ImGui::PushStyleColor(ImGuiCol_Separator, Tuplitor::Themes::RGBA(110, 110, 128, 128));
		ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, Tuplitor::Themes::RGBA(0, 120, 255, 200));
		ImGui::PushStyleColor(ImGuiCol_SeparatorActive, Tuplitor::Themes::RGBA(25, 100, 190, 255));

		// Resize Grip
		ImGui::PushStyleColor(ImGuiCol_ResizeGrip, Tuplitor::Themes::RGBA(250, 10, 10, 50));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, Tuplitor::Themes::RGBA(250, 30, 30, 170));
		ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, Tuplitor::Themes::RGBA(250, 50, 50, 240));

		// Tab
		ImGui::PushStyleColor(ImGuiCol_Tab, Tuplitor::Themes::RGBA(150, 0, 0, 220));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, Tuplitor::Themes::RGBA(255, 100, 100, 205));
		ImGui::PushStyleColor(ImGuiCol_TabActive, Tuplitor::Themes::RGBA(255, 10, 10, 220));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocused, Tuplitor::Themes::RGBA(150, 0, 0, 220));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Tuplitor::Themes::RGBA(200, 20, 20, 220));

		// Docking
		ImGui::PushStyleColor(ImGuiCol_DockingPreview, Tuplitor::Themes::RGBA(65, 150, 250, 180));
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, Tuplitor::Themes::RGBA(50, 50, 50, 255));

		// Plot
		ImGui::PushStyleColor(ImGuiCol_PlotLines, Tuplitor::Themes::RGBA(155, 155, 155, 255));
		ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered, Tuplitor::Themes::RGBA(255, 110, 90, 255));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Tuplitor::Themes::RGBA(230, 180, 0, 255));
		ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, Tuplitor::Themes::RGBA(255, 150, 0, 255));

		// Tables
		ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, Tuplitor::Themes::RGBA(50, 50, 50, 255));
		ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, Tuplitor::Themes::RGBA(80, 80, 90, 255));
		ImGui::PushStyleColor(ImGuiCol_TableBorderLight, Tuplitor::Themes::RGBA(60, 60, 65, 255));
		ImGui::PushStyleColor(ImGuiCol_TableRowBg, Tuplitor::Themes::RGBA(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, Tuplitor::Themes::RGBA(255, 255, 255, 15));

		// TextSelectBg
		ImGui::PushStyleColor(ImGuiCol_TextSelectedBg, Tuplitor::Themes::RGBA(65, 150, 250, 90));

		// DragDropTarget
		ImGui::PushStyleColor(ImGuiCol_DragDropTarget, Tuplitor::Themes::RGBA(255, 255, 0, 230));

		// Nav
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Tuplitor::Themes::RGBA(65, 150, 250, 255));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingHighlight, Tuplitor::Themes::RGBA(255, 255, 255, 180));
		ImGui::PushStyleColor(ImGuiCol_NavWindowingDimBg, Tuplitor::Themes::RGBA(205, 205, 205, 50));

		// ModalWindow
		ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, Tuplitor::Themes::RGBA(205, 205, 205, 90));

	}
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
#pragma region PopColors

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
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

#pragma endregion
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
		ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize, 10);

#pragma endregion

#pragma region Borders

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1);
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

#pragma region Docking

		ImGui::PushStyleVar(ImGuiStyleVar_DockingSeparatorSize, 3);

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

#pragma region Docking

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

ImVec4 Tuplitor::Themes::RGBA(float aR, float aG, float aB, float aA)
{
	ImVec4 color = { aR / 255, aG / 255, aB / 255, aA / 255 };
	return color;
}