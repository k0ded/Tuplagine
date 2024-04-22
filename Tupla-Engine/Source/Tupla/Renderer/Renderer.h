#pragma once
#include "WindowsWindow.h"
#include "CommonUtilities/Time/StopWatch.h"
#include "RHI/RHI.h"
#include "Primitives/Texture.h"

namespace Tupla
{
	class Renderer
	{
	public:
		void StartWindow(const WindowProps& props);
		bool Init();
		void Shutdown();

		void BeginFrame();
		void EndFrame();
		
		WindowsWindow* GetWindow() const { return myWindow.get(); }
	private:
		Scope<WindowsWindow> myWindow;

		Scope<Texture> myBackBuffer;
		Scope<Texture> myDepthBuffer;

		CU::Vector2ui myRenderingSize;

		CommonUtilities::StopWatch myFrameWatch{};
		std::vector<float> myPrevTimes{};
	};
}
