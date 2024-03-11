project "External"
	location ""
	language "C++"
	cppdialect "C++20"

	targetdir ("../../../Lib/")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("../../../Temp/%{prj.name}/%{cfg.buildcfg}")

	--pchheader "stdafx.h"
	--pchsource "Source/%{prj.name}/stdafx.cpp"

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}

	includedirs {
		".",
		"./imgui/",
		"%VULKAN_SDK%/include/"
	}

	defines {"_CONSOLE"}

	filter "configurations:Debug"
		defines {"_DEBUG", "FBXSDK_SHARED"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE",  "FBXSDK_SHARED"}
		runtime "Release"
		optimize "on"

	filter "configurations:Retail"
		defines {"_RETAIL",  "FBXSDK_SHARED"}
		runtime "Release"
		optimize "on"

	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		--warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}

		defines { "_WIN32_WINNT=0x0601" }