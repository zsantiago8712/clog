workspace("LoggerLibrary")
configurations({ "Debug", "Release" })

project("Logger")
kind("StaticLib")
language("C")
targetdir("bin/%{cfg.buildcfg}")

includedirs("./include")
files("./src/*.c", "./include/*.h")

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")
buildoptions({
	"-Wall",
	"-Werror",
	"-Wextra",
	"-Wshadow",
	"-Wformat=2",
	"-Wconversion",
	"-Wstrict-prototypes",
	"-pedantic",
	"-fsanitize=address",
})
linkoptions({ "-fsanitize=address" })

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")

project("TestBed")
kind("ConsoleApp")
language("C")
targetdir("bin/%{cfg.buildcfg}")

files({ "TestBed/*.c", "TestBed/*.h" })
includedirs({ "./include/" })
links({ "Logger" })

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")
buildoptions({ "-fsanitize=address" })
linkoptions({ "-fsanitize=address" })

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")
