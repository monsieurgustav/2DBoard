solution "LabyrinthBoard"
  configurations {"Debug", "Release"}
  
  location (_ACTION)
  flags {"Unicode"}
  
  configuration "windows"
    defines {"WIN32", "_WINDOWS", "NOMINMAX"}
    flags {"StaticRuntime", "WinMain"}
  
  project "Labyrinth"
    kind "WindowedApp"
    language "C++"
    files {"include/*.h", "src/*.cpp"}
    includedirs {"include",
                 "cinder/include",
                 "cinder/boost"}
    
    libdirs {"cinder/lib"}
    configuration "windows"
      libdirs {"cinder/lib/msw"}
    configuration "macos"
      libdirs {"cinder/lib/macos"}
    
    configuration "Debug"
      defines {"DEBUG", "_DEBUG"}
      links {"cinder_d"}
      flags {"Symbols"}
    
    configuration "Release"
      defines {"NDEBUG"}
      links {"cinder"}
      flags {"Optimize"}
