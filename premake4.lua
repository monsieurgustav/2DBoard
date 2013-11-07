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
    files {"include/*.h", "src/*.cpp",
           "simplefilewatcher/includes/*", "simplefilewatcher/source/FileWatcher.cpp"}
    pchheader "include/Labyrinth_Prefix.h"
    pchsource "src/Labyrinth_Prefix.cpp"
    includedirs {"include",
                 "cinder/include",
                 "cinder/boost",
                 "cinder/blocks/FMOD/include",
                 "simplefilewatcher/includes"}
    
    libdirs {"cinder/lib"}
    configuration "windows"
      files "simplefilewatcher/source/FileWatcherWin32.cpp"
      libdirs {"cinder/lib/msw",
               "cinder/blocks/FMOD/lib/msw"}
      links "fmodex_vc"
      buildoptions {"-Zm150", "-FILabyrinth_Prefix.h"}
    configuration "macos"
      files "simplefilewatcher/source/FileWatcherOSX.cpp"
      libdirs "cinder/lib/macos"
    
    configuration "Debug"
      defines {"DEBUG", "_DEBUG"}
      links {"cinder_d"}
      flags {"Symbols"}
    
    configuration "Release"
      defines {"NDEBUG"}
      links {"cinder"}
      flags {"Optimize"}
