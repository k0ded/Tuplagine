-- premake5.lua
workspace "TuplaGE"
   architecture "x64"
   startproject "Tupla-Editor"
   configurations { "Debug", "Release", "Dist" }

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
   include "Tupla-Engine/vendor/External/Build-External.lua"
   include "Tupla-Utils/Build-Utils.lua"
group ""

group "Core"
	include "Tupla-Engine/Build-Engine.lua"
group ""

group "Tools"
include "Tupla-Editor/Build-Editor.lua"
group ""

group "Game"
include "Tupla-Runtime/Build-Runtime.lua"
group ""