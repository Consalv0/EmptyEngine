
-- Include directories relative to root folder (solution directory)

IncludeDir = {}
IncludeDir["SDL"] = "External/SDL";
IncludeDir["VulkanSDK"] = "External/VulkanSDK"
IncludeDir["spdlog"] = "External/spdlog";

LibrariesDir = {}
LibrariesDir["SDL"] = "%{IncludeDir.SDL}";
LibrariesDir["VulkanSDK"] = "%{IncludeDir.VulkanSDK}/Lib"
LibrariesDir["spdlog"] = "%{prj.location}/Libraries/spdlog";