
-- Include directories relative to root folder (solution directory)

IncludeDir = {}
IncludeDir["SDL"] = "External/SDL";
IncludeDir["spdlog"] = "External/spdlog";

LibrariesDir = {}
LibrariesDir["SDL"] = "%{prj.location}/Libraries/SDL";
LibrariesDir["spdlog"] = "%{prj.location}/Libraries/spdlog";