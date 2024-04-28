#pragma once
#include "WindowsWindow.h"
#include "Commands/RenderCommand.h"
#include "CommonUtilities/Time/StopWatch.h"
#include "Primitives/Texture.h"
#include "RHI/PipelineStateObject.h"
#include "Tupla/Utils/TemplateArray.h"

namespace Tupla
{
	class Renderer
	{
	public:
		void StartWindow(const WindowProps& props);
		bool Init();
		void Shutdown();

		// Requires syncing!!!
		void BeginFrame();

		// Does not require syncing with other threads
		void ProcessCommands();
		void EndFrame();


		template<typename T> requires std::is_base_of_v<RenderCommand, T>
		void SendCommand(const T& aCommand)
		{
			myQueueBuffer->GetVector<T>().GetData().push_back(aCommand);
		}
		
		WindowsWindow* GetWindow() const { return myWindow.get(); }
	private:
		void SwapBuffers() { std::swap(myQueueBuffer, myRenderBuffer); myQueueBuffer->Clear(); }
		void CreateDefaultPSO();
		void ResetToDefault();

		TemplatedVectorHolder myBuffer1 {};
		TemplatedVectorHolder myBuffer2 {};
		TemplatedVectorHolder* myQueueBuffer = &myBuffer1;
		TemplatedVectorHolder* myRenderBuffer = &myBuffer2;

		PipelineStateObject myDefaultPSO;
		Scope<WindowsWindow> myWindow;

		Ref<Shader> myDefaultVertexShader;
		Ref<Shader> myDefaultPixelShader;

		CU::Vector2ui myRenderingSize;

		CommonUtilities::StopWatch myFrameWatch{};
		std::vector<float> myPrevTimes{};
	};
}
