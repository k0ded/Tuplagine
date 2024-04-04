project "Tupla-Editor"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   files { "Source/**.h", "Source/**.cpp", "Source/**.hpp" }

   includedirs
   {
      "Source",

	  -- Include Core
	  "../Tupla-Engine/Source",
	  "../Tupla-Utils/Source",
      "../Tupla-Runtime/Source",
      "../Tupla-Engine/vendor/External/imgui"
   }

   links
   {
      "Tupla-Engine",
      "Tupla-Runtime",
      "Tupla-Utils"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { "WINDOWS" }

   filter "configurations:Debug"
       defines { "DEBUG" }
       runtime "Debug"
       symbols "On"

   filter "configurations:Release"
       defines { "RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

   filter "configurations:Dist"
       defines { "DIST" }
       kind "WindowedApp"
       runtime "Release"
       optimize "On"
       symbols "Off"