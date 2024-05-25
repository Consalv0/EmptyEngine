
-- Include directories relative to root folder (solution directory)

IncludeDir = {}
IncludeDir["SDL"] = "External/SDL";
IncludeDir["VulkanSDK"] = "External/VulkanSDK"
IncludeDir["spdlog"] = "External/spdlog";

LibrariesDir = {}
LibrariesDir["SDL"] = "%{prj.location}/Libraries/SDL";
LibrariesDir["VulkanSDK"] = "External/VulkanSDK/Lib"
LibrariesDir["spdlog"] = "%{prj.location}/Libraries/spdlog";