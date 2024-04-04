project "Tupla-Engine"
   kind "StaticLib"
   location "Source"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { 
    "Source/**.h", 
    "Source/**.cpp", 
    "Source/**.hpp"
    }

   includedirs
   {
        "Source",
        "../Tupla-Utils/Source",
		"vendor/External/"
   }

   libdirs {
    "../Dependencies"
   }

   links { 
    "External",
    "Tupla-Utils",
    "glfw3.lib"
    }

    pchheader "tgpch.h"
	pchsource "%{prj.location}/tgpch.cpp"

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")
   defines { "GLSL_VALIDATOR=\"$(VULKAN_SDK)/Bin/glslangValidator\"" }

   filter "system:windows"
       systemversion "latest"
       defines { "GLSL_VALIDATOR=\"$(VULKAN_SDK)/Bin/glslangValidator.exe\"" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       links { "freetype_Debug_x64" }
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       links { "freetype_Release_x64" }
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       runtime "Release"
       links { "freetype_Release_x64" }
       optimize "On"
       symbols "Off"