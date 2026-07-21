workspace "oNESsan"
    configurations{
        "Debug", 
        "Release"
    }
    architecture "x64"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "oNESsan"
    location "oNESsan"
    language "C++"
    kind "ConsoleApp"

    targetdir ("bin/"..outputdir.."/%{prj.name}")
    objdir ("bin-int/"..outputdir.."/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c",
        "%{prj.name}/include/**.h"
    }

    includedirs
    {
        "%{wks.location}/%{prj.name}/dependencies/include",
        "%{prj.name}/src",
        "%{prj.name}/include"
    }

    libdirs
    {
        "%{wks.location}/%{prj.name}/dependencies/libs"
    }
    
    links 
    {
        "opengl32.lib"
    }

    cppdialect "C++17"
    staticruntime "Off"
    systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        symbols "on"
        linkoptions { "/IGNORE:4099" }

    filter "configurations:Release"
        defines "RELEASE"
        symbols "off"
        optimize "on"
