
-- Include directories relative to root folder (solution directory)

IncludeDir = {}
IncludeDir["SDL"] = "External/SDL";
IncludeDir["VulkanSDK"] = "External/VulkanSDK"
IncludeDir["VMA"] = "External/VulkanMemoryAllocator"
IncludeDir["spdlog"] = "External/spdlog";

LibrariesDir = {}
LibrariesDir["SDL"] = "%{IncludeDir.SDL}";
LibrariesDir["VulkanSDK"] = "%{IncludeDir.VulkanSDK}/Lib"
LibrariesDir["VMA"] = "%{IncludeDir.VMA}"
LibrariesDir["spdlog"] = "%{prj.location}/Libraries/spdlog";