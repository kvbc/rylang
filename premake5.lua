workspace "rylang"
    configurations { "Debug", "Release" }

project "rylang"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    targetdir "_bin/%{cfg.buildcfg}"
    objdir "_bin/%{cfg.buildcfg}"

    files {"src/**.cpp"}

    filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"