/*

rapidobj - Fast Wavefront .obj file loader

Licensed under the MIT License <http://opensource.org/licenses/MIT>
SPDX-License-Identifier: MIT
Copyright (c) 2024 Slobodan Pavlic

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef RAPID_OBJ_INCLUDE_HEADER_GUARD
#define RAPID_OBJ_INCLUDE_HEADER_GUARD

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cfloat>
#include <charconv>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <optional>
#include <string>
#include <system_error>
#include <thread>
#include <utility>
#include <variant>
#include <vector>

#ifdef __linux__

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#elif _WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#elif __APPLE__

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#endif

#define RAPIDOBJ_VERSION_MAJOR 1
#define RAPIDOBJ_VERSION_MINOR 1
#define RAPIDOBJ_VERSION_PATCH 0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Public API Begins
//

namespace rapidobj {

static constexpr struct {
    int Major;
    int Minor;
    int Patch;
} Version = { RAPIDOBJ_VERSION_MAJOR, RAPIDOBJ_VERSION_MINOR, RAPIDOBJ_VERSION_PATCH };

enum class Load { Mandatory, Optional };

struct MaterialLibrary final {
    static MaterialLibrary Default() { return MaterialLibrary(); }
    static MaterialLibrary Default(Load policy) { return MaterialLibrary(policy); }

    static MaterialLibrary SearchPath(std::filesystem::path path, Load policy = Load::Mandatory)
    {
        return MaterialLibrary({ std::move(path) }, policy);
    }

    static MaterialLibrary SearchPaths(std::vector<std::filesystem::path> paths, Load policy = Load::Mandatory)
    {
        return MaterialLibrary(std::move(paths), policy);
    }

    static MaterialLibrary String(std::string_view text) { return MaterialLibrary(text); }

    static MaterialLibrary Ignore() { return MaterialLibrary(nullptr); }

    const auto& Value() const noexcept { return m_value; }
    const auto& Policy() const noexcept { return m_policy; }

  private:
    MaterialLibrary() noexcept = default;
    MaterialLibrary(Load policy) noexcept : m_policy(policy) {}
    MaterialLibrary(std::vector<std::filesystem::path>&& paths, Load policy) noexcept
        : m_value(std::move(paths)), m_policy(policy)
    {}
    MaterialLibrary(std::string_view text) noexcept : m_value(text) {}
    MaterialLibrary(std::nullptr_t) noexcept : m_value(nullptr) {}

    using Variant = std::variant<std::monostate, std::nullptr_t, std::vector<std::filesystem::path>, std::string_view>;

    Variant             m_value{};
    std::optional<Load> m_policy{};
};

template <typename T>
class Array final {
  public:
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using iterator        = pointer;
    using const_iterator  = const_pointer;

    Array() noexcept {}
    Array(size_type size) : m_state{ size ? std::unique_ptr<T[]>(new T[size]) : nullptr, size } {}

    template <typename Iter>
    Array(Iter begin, Iter end)
    {
        static_assert(std::is_same_v<std::random_access_iterator_tag, typename Iter::iterator_category>);
        static_assert(std::is_same_v<value_type, typename Iter::value_type>);

        const auto size = static_cast<size_type>(end - begin);
        m_state.data.reset(new value_type[size]);
        memcpy(&*begin, m_state.data.get(), size * sizeof(value_type));
        m_state.size = size;
    }

    Array(const Array&)            = delete;
    Array& operator=(const Array&) = delete;

    Array(Array&& other) noexcept { m_state = std::exchange(other.m_state, {}); }
    Array& operator=(Array&& other) noexcept
    {
        if (this != &other) {
            m_state = std::exchange(other.m_state, {});
        }
        return *this;
    }

    constexpr reference       operator[](size_type index) noexcept { return m_state.data[index]; }
    constexpr const_reference operator[](size_type index) const noexcept { return m_state.data[index]; }
    constexpr reference       front() noexcept { return m_state.data[0]; }
    constexpr const_reference front() const noexcept { return m_state.data[0]; }
    constexpr reference       back() noexcept { return m_state.data[m_state.size - 1]; }
    constexpr const_reference back() const noexcept { return m_state.data[m_state.size - 1]; }
    constexpr pointer         data() noexcept { return m_state.data.get(); }
    constexpr const_pointer   data() const noexcept { return m_state.data.get(); }
    constexpr iterator        begin() noexcept { return m_state.data.get(); }
    constexpr const_iterator  begin() const noexcept { return m_state.data.get(); }
    constexpr const_iterator  cbegin() const noexcept { return m_state.data.get(); }
    constexpr iterator        end() noexcept { return m_state.data.get() + m_state.size; }
    constexpr const_iterator  end() const noexcept { return m_state.data.get() + m_state.size; }
    constexpr const_iterator  cend() const noexcept { return m_state.data.get() + m_state.size; }
    constexpr bool            empty() const noexcept { return m_state.size == 0; }
    constexpr size_type       size() const noexcept { return m_state.size; }
    void                      swap(Array& other) noexcept { std::swap(m_state.state, other.m_state.state); }

  private:
    struct final {
        std::unique_ptr<T[]> data;
        size_type            size;
    } m_state{};

    static_assert(
        std::is_trivially_constructible_v<value_type> && std::is_trivially_destructible_v<value_type> &&
        std::is_trivially_copyable_v<value_type>);
};

template <typename Iter>
Array(Iter b, Iter e) -> Array<typename std::iterator_traits<Iter>::value_type>;

template <typename T>
bool operator==(const Array<T>& lhs, const Array<T>& rhs)
{
    return (lhs.size() == rhs.size()) && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
bool operator!=(const Array<T>& lhs, const Array<T>& rhs)
{
    return !operator==(lhs, rhs);
}

struct Attributes final {
    Array<float> positions; // 'v'  (xyz)
    Array<float> texcoords; // 'vt' (uv)
    Array<float> normals;   // 'vn' (xyz)
    Array<float> colors;    //  vertex color extension (see http://paulbourke.net/dataformats/obj/colour.html)
};

struct Index final {
    int position_index;
    int texcoord_index;
    int normal_index;
};

struct Mesh final {
    Array<Index>    indices;             // Position/Texture/Normal indices
    Array<uint8_t>  num_face_vertices;   // Number of vertices per face: 3 (triangle), 4 (quad), ... , 255
    Array<int32_t>  material_ids;        // Material ID per face
    Array<uint32_t> smoothing_group_ids; // Smoothing group ID per face (group id 0 means off)
};

struct Lines final {
    Array<Index>   indices;           // Polyline indices
    Array<int32_t> num_line_vertices; // Number of vertices per polyline
};

struct Points final {
    Array<Index> indices; // Points indices
};

struct Shape final {
    std::string name;
    Mesh        mesh;
    Lines       lines;
    Points      points;
};

using Shapes = std::vector<Shape>;

enum class TextureType { None, Sphere, CubeTop, CubeBottom, CubeFront, CubeBack, CubeLeft, CubeRight };

using Float3 = std::array<float, 3>;

// see https://en.wikipedia.org/wiki/Wavefront_.obj_file#Texture_options
struct TextureOption final {
    TextureType type               = TextureType::None; // -type
    float       sharpness          = 1.0f;              // -boost
    float       brightness         = 0.0f;              // -mm
    float       contrast           = 1.0f;              // -mm
    Float3      origin_offset      = { 0, 0, 0 };       // -o
    Float3      scale              = { 1, 1, 1 };       // -s
    Float3      turbulence         = { 0, 0, 0 };       // -t
    int         texture_resolution = -1;                // -texres
    bool        clamp              = false;             // -clamp
    char        imfchan            = 'm';               // -imfchan
    bool        blendu             = true;              // -blendu
    bool        blendv             = true;              // -blendv
    float       bump_multiplier    = 1.0f;              // -bm
};

struct Material final {
    std::string name;

    // Material parameters (see http://www.fileformat.info/format/material/)
    Float3 ambient       = { 0, 0, 0 }; // Ka
    Float3 diffuse       = { 0, 0, 0 }; // Kd
    Float3 specular      = { 0, 0, 0 }; // Ks
    Float3 transmittance = { 0, 0, 0 }; // Kt
    Float3 emission      = { 0, 0, 0 }; // Ke
    float  shininess     = 1.0f;        // Ns
    float  ior           = 1.0f;        // Ni
    float  dissolve      = 1.0f;        // d
    int    illum         = 0;           // illum

    std::string ambient_texname;            // map_Ka
    std::string diffuse_texname;            // map_Kd
    std::string specular_texname;           // map_Ks
    std::string specular_highlight_texname; // map_Ns
    std::string bump_texname;               // map_bump, map_Bump, bump
    std::string displacement_texname;       // disp
    std::string alpha_texname;              // map_d
    std::string reflection_texname;         // refl

    TextureOption ambient_texopt;
    TextureOption diffuse_texopt;
    TextureOption specular_texopt;
    TextureOption specular_highlight_texopt;
    TextureOption bump_texopt;
    TextureOption displacement_texopt;
    TextureOption alpha_texopt;
    TextureOption reflection_texopt;

    // PBR extension (see http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr)
    float roughness           = 0.0f; // Pr
    float metallic            = 0.0f; // Pm
    float sheen               = 0.0f; // Ps
    float clearcoat_thickness = 0.0f; // Pc
    float clearcoat_roughness = 0.0f; // Pcr
    float anisotropy          = 0.0f; // aniso
    float anisotropy_rotation = 0.0f; // anisor

    std::string roughness_texname; // map_Pr
    std::string metallic_texname;  // map_Pm
    std::string sheen_texname;     // map_Ps
    std::string emissive_texname;  // map_Ke
    std::string normal_texname;    // norm

    TextureOption roughness_texopt;
    TextureOption metallic_texopt;
    TextureOption sheen_texopt;
    TextureOption emissive_texopt;
    TextureOption normal_texopt;
};

using Materials = std::vector<Material>;

struct Error final {
    explicit operator bool() const noexcept { return code.value(); }

    std::error_code code{};
    std::string     line{};
    size_t          line_num{};
};

struct [[nodiscard]] Result final {
    Attributes attributes;
    Shapes     shapes;
    Materials  materials;
    Error      error;
};

inline Result ParseFile(
    const std::filesystem::path& obj_filepath,
    const MaterialLibrary&       mtl_library = MaterialLibrary::Default());

inline Result ParseStream(std::istream& obj_stream, const MaterialLibrary& mtl_library = MaterialLibrary::Default());

inline bool Triangulate(Result& result);

} // namespace rapidobj

//
// Public API Ends
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Third-party includes Begin
//

// clang-format off

#include "earcut.inl"

#include "fast_float.inl"

// clang-format on

//
// Third-party includes End
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rapidobj {
enum class rapidobj_errc {
    Success,
    ParseError,
    MaterialFileError,
    MaterialParseError,
    MaterialNotFoundError,
    MaterialRelativePathError,
    AmbiguousMaterialLibraryError,
    LineTooLongError,
    IndexOutOfBoundsError,
    InvalidArgumentsError,
    TooFewIndicesError,
    TooManyIndicesError,
    TriangulationError,
    InternalError
};
} // namespace rapidobj

namespace std {
template <>
struct is_error_code_enum<rapidobj::rapidobj_errc> : true_type {};
} // namespace std

namespace rapidobj {

struct rapidobj_error_category : std::error_category {
    const char* name() const noexcept override { return "rapidobj"; }
    std::string message(int err) const override
    {
        switch (static_cast<rapidobj_errc>(err)) {
        case rapidobj_errc::Success: return "No error.";
        case rapidobj_errc::ParseError: return "Object file parse error.";
        case rapidobj_errc::MaterialFileError: return "Material file not found.";
        case rapidobj_errc::MaterialParseError: return "Material file parse error.";
        case rapidobj_errc::MaterialNotFoundError: return "Material not found.";
        case rapidobj_errc::MaterialRelativePathError: return "Material file path is relative.";
        case rapidobj_errc::AmbiguousMaterialLibraryError: return "Ambiguous material library.";
        case rapidobj_errc::LineTooLongError: return "Line too long.";
        case rapidobj_errc::IndexOutOfBoundsError: return "Index out of bounds.";
        case rapidobj_errc::InvalidArgumentsError: return "Invalid Arguments.";
        case rapidobj_errc::TooFewIndicesError: return "Polygon has too few indices.";
        case rapidobj_errc::TooManyIndicesError: return "Polygon has too many indices.";
        case rapidobj_errc::TriangulationError: return "Triangulation errror.";
        case rapidobj_errc::InternalError: return "Internal error.";
        }
        return "Unrecognised error.";
    }
};

inline std::error_code make_error_code(rapidobj_errc err)
{
    static const rapidobj_error_category category{};
    return { static_cast<int>(err), category };
}

namespace detail {

constexpr std::size_t operator"" _GiB(unsigned long long int gibibytes) noexcept
{
    return static_cast<std::size_t>(1024 * 1024 * 1024 * gibibytes);
}

constexpr std::size_t operator"" _MiB(unsigned long long int mebibytes) noexcept
{
    return static_cast<std::size_t>(1024 * 1024 * mebibytes);
}

constexpr std::size_t operator"" _KiB(unsigned long long int kibibytes) noexcept
{
    return static_cast<std::size_t>(1024 * kibibytes);
}

static constexpr auto kMaxLineLength      = 4_KiB;
static constexpr auto kBlockSize          = 256_KiB;
static constexpr auto kMinVerticesInFace  = 3;
static constexpr auto kMaxVerticesInFace  = 255;
static constexpr auto kMinVerticesInLine  = 2;
static constexpr auto kMaxVerticesInLine  = 1000;
static constexpr auto kMinVerticesInPoint = 1;
static constexpr auto kMaxVerticesInPoint = 1000;
static constexpr auto kSingleThreadCutoff = 1_MiB;

static constexpr auto kMergeCopyByteCost  = 12;
static constexpr auto kMergeCopyIntCost   = 31;
static constexpr auto kMergeFillIdCost    = 32;
static constexpr auto kMergeCopyIndexCost = 186;
static constexpr auto kMergeSubdivideCost = 50000000;

static constexpr auto kTriangulateTriangleCost  = 55;
static constexpr auto kTriangulateQuadCost      = 140;
static constexpr auto kTriangulatePerIndexCost  = 46;
static constexpr auto kTriangulateSubdivideCost = 5000000;

static constexpr auto kMemoryRecyclingSize = 25_MiB;

static_assert(kMaxLineLength < kBlockSize);
static_assert(kBlockSize % 4_KiB == 0);

static constexpr auto kSuccess = std::errc();

inline constexpr bool StartsWith(std::string_view text, std::string_view sv) noexcept
{
    if (text.length() < sv.length()) {
        return false;
    }
    for (size_t i = 0; i != sv.length(); ++i) {
        if (text[i] != sv[i]) {
            return false;
        }
    }
    return true;
}

inline constexpr bool EndsWith(std::string_view text, char c) noexcept
{
    return text.length() > 0 && text.back() == c;
}

inline void TrimLeft(std::string& text) noexcept
{
    size_t index = 0;
    while (index < text.size() && (text[index] == ' ' || text[index] == '\t')) {
        ++index;
    }
    text.erase(0, index);
}

inline void TrimRight(std::string& text) noexcept
{
    auto index = text.size();
    while (index && (text[index - 1] == ' ' || text[index - 1] == '\t')) {
        --index;
    }
    text.erase(index);
}

inline void Trim(std::string& text) noexcept
{
    TrimLeft(text);
    TrimRight(text);
}

inline constexpr void TrimLeft(std::string_view& text) noexcept
{
    size_t index = 0;
    while (index < text.size() && (text[index] == ' ' || text[index] == '\t')) {
        ++index;
    }
    text.remove_prefix(index);
}

inline constexpr void TrimRight(std::string_view& text) noexcept
{
    auto index = text.size();
    while (index && (text[index - 1] == ' ' || text[index - 1] == '\t')) {
        --index;
    }
    text = text.substr(0, index);
}

inline constexpr void Trim(std::string_view& text) noexcept
{
    TrimLeft(text);
    TrimRight(text);
}

enum class ApplyOffset : uint8_t { None = 0, Position = 1, Texcoord = 2, Normal = 4, All = 7 };

using OffsetFlags = std::underlying_type_t<ApplyOffset>;

inline bool operator&(OffsetFlags lhs, ApplyOffset rhs) noexcept
{
    return lhs & static_cast<OffsetFlags>(rhs);
}

inline OffsetFlags operator|(OffsetFlags lhs, ApplyOffset rhs) noexcept
{
    return lhs | static_cast<OffsetFlags>(rhs);
}

inline OffsetFlags operator|(ApplyOffset lhs, ApplyOffset rhs) noexcept
{
    return static_cast<OffsetFlags>(lhs) | static_cast<OffsetFlags>(rhs);
}

template <typename T>
struct Buffer final {
    Buffer() noexcept = default;
    Buffer(size_t size) noexcept : m_size(size), m_room(0), m_data(new T[size]) {}

    size_t   size() const noexcept { return m_size; }
    const T* data() const noexcept { return m_data.get(); }
    T*       data() noexcept { return m_data.get(); }
    T&       back() noexcept { return m_data[m_size - 1]; }

    void push_back(T value) noexcept
    {
        m_data[m_size] = value;
        ++m_size, --m_room;
    }

    T pop_back() noexcept
    {
        --m_size, ++m_room;
        return m_data.get()[m_size];
    }

    void fill_n(size_t n, T value)
    {
        std::fill_n(m_data.get() + m_size, n, value);
        m_size += n;
        m_room -= n;
    }

    void ensure_enough_room_for(size_t size)
    {
        if (size > m_room) {
            auto cap = std::max(kInitialSize, 2 * (m_size + size));
            auto src = std::unique_ptr<T[]>(std::move(m_data));
            m_data.reset(new T[cap]);
            if (src) {
                memcpy(m_data.get(), src.get(), m_size * sizeof(T));
            }
            m_room = cap - m_size;
        }
    }

  private:
    static constexpr size_t kInitialSize = 4096;

    size_t               m_size{};
    size_t               m_room{};
    std::unique_ptr<T[]> m_data{};

    static_assert(std::is_trivially_copyable_v<T>);
};

struct ShapeRecord final {
    struct Mesh final {
        size_t index_buffer_start{};
        size_t face_buffer_start{};
    };
    struct Lines final {
        size_t index_buffer_start{};
        size_t segment_buffer_start{};
    };
    struct Points final {
        size_t index_buffer_start{};
    };
    std::string name{};
    Mesh        mesh{};
    Lines       lines{};
    Points      points{};
    size_t      chunk_index{};
};

struct MaterialRecord final {
    std::string name;
    std::string line{};
    size_t      line_num{};
    size_t      face_buffer_start{};
};

using MaterialMap = std::map<std::string, int>;

struct ParseMaterialsResult final {
    MaterialMap material_map;
    Materials   materials;
    Error       error;
};

struct SmoothingRecord final {
    unsigned int group_id{};
    size_t       face_buffer_start{};
};

struct SharedContext final {
    struct Thread final {
        size_t concurrency{};
    } thread;

    struct Stats final {
        size_t num_positions{};
        size_t num_texcoords{};
        size_t num_normals{};
    } stats;

    struct Material final {
        const MaterialLibrary*            library{};
        std::filesystem::path             basepath{};
        std::string                       library_name{};
        std::future<ParseMaterialsResult> parse_result{};
        std::mutex                        mutex{}; // protects library_name and parse_result
    } material;

    struct Parsing final {
        std::atomic_size_t thread_count{};
        std::promise<void> completed{};
    } parsing;

    struct Merging final {
        std::atomic_size_t task_index{};
        std::atomic_size_t thread_count{};
        std::promise<void> completed{};
        rapidobj_errc      error{};
        std::mutex         mutex{}; // protects error
    } merging;

    struct Debug final {
        struct IO final {
            std::vector<int>                      num_requests{};
            std::vector<size_t>                   num_bytes_read{};
            std::vector<std::chrono::nanoseconds> submit_time;
            std::vector<std::chrono::nanoseconds> wait_time;
        } io;
        struct Parse final {
            std::vector<std::chrono::nanoseconds> time;
            std::chrono::nanoseconds              total_time;
        } parse;
        struct Merge final {
            std::chrono::nanoseconds total_time;
        } merge;
    } debug;
};

struct Chunk final {
    struct Text final {
        size_t line_count{};
    };
    struct Positions final {
        Buffer<float> buffer{};
        size_t        count{};
    };
    struct Texcoords final {
        Buffer<float> buffer{};
        size_t        count{};
    };
    struct Normals final {
        Buffer<float> buffer{};
        size_t        count{};
    };
    struct Colors final {
        Buffer<float> buffer{};
        size_t        count{};
    };
    struct Mesh final {
        struct Indices final {
            Buffer<Index>       buffer;
            Buffer<OffsetFlags> flags;
        } indices{};
        struct Faces final {
            Buffer<unsigned char> buffer;
            size_t                count;
        } faces{};
    };
    struct Lines final {
        struct Indices final {
            Buffer<Index>       buffer;
            Buffer<OffsetFlags> flags;
        } indices{};
        struct Segments final {
            Buffer<int> buffer;
            size_t      count;
        } segments{};
    };
    struct Points final {
        struct Indices final {
            Buffer<Index>       buffer;
            Buffer<OffsetFlags> flags;
        } indices{};
    };
    struct Shapes final {
        std::vector<ShapeRecord> list;
    };
    struct Materials final {
        std::vector<MaterialRecord> list;
    };
    struct Smoothing final {
        std::vector<SmoothingRecord> list;
    };

    Text      text;
    Positions positions;
    Texcoords texcoords;
    Normals   normals;
    Colors    colors;
    Mesh      mesh;
    Lines     lines;
    Points    points;
    Shapes    shapes;
    Materials materials;
    Smoothing smoothing;
    Error     error;
};

inline size_t SizeInBytes(const Chunk& chunk) noexcept
{
    auto size = size_t{ 0 };

    size += chunk.positions.buffer.size() * sizeof(float);
    size += chunk.texcoords.buffer.size() * sizeof(float);
    size += chunk.normals.buffer.size() * sizeof(float);
    size += chunk.colors.buffer.size() * sizeof(float);
    size += chunk.mesh.indices.buffer.size() * sizeof(Index);
    size += chunk.mesh.indices.flags.size() * sizeof(OffsetFlags);
    size += chunk.mesh.faces.buffer.size() * sizeof(unsigned char);
    size += chunk.lines.indices.buffer.size() * sizeof(Index);
    size += chunk.lines.indices.flags.size() * sizeof(OffsetFlags);
    size += chunk.lines.segments.buffer.size() * sizeof(int);
    size += chunk.points.indices.buffer.size() * sizeof(Index);
    size += chunk.points.indices.flags.size() * sizeof(OffsetFlags);

    return size;
}

inline size_t SizeInBytes(const Mesh& mesh) noexcept
{
    auto size = size_t{ 0 };

    size += mesh.indices.size() * sizeof(Index);
    size += mesh.num_face_vertices.size() * sizeof(uint8_t);
    size += mesh.material_ids.size() * sizeof(int32_t);
    size += mesh.smoothing_group_ids.size() * sizeof(uint32_t);

    return size;
}

template <typename>
struct CopyElements;

template <typename>
struct FillElements;

template <typename>
struct FillIds;

template <typename T>
struct FillSrc final {
    T      id{};
    size_t offset{};
};

struct AttributeInfo final {
    size_t position{};
    size_t texcoord{};
    size_t normal{};

    AttributeInfo& operator+=(const AttributeInfo& rhs) noexcept
    {
        position += rhs.position;
        texcoord += rhs.texcoord;
        normal += rhs.normal;
        return *this;
    }
};

struct CopyIndices;

using CopyBytes  = CopyElements<uint8_t>;
using CopyInts   = CopyElements<int32_t>;
using CopyFloats = CopyElements<float>;

using FillFloats = FillElements<float>;

using FillMaterialIds       = FillIds<int32_t>;
using FillSmoothingGroupIds = FillIds<uint32_t>;

template <typename T>
struct CopyElements final {
    CopyElements(T* dst, const T* src, size_t size) noexcept : m_dst(dst), m_src(src), m_size(size) {}

    auto Cost() const noexcept
    {
        constexpr auto cost = sizeof(T) == 1 ? kMergeCopyByteCost : kMergeCopyIntCost;
        return cost * m_size;
    }

    auto Execute() const noexcept
    {
        memcpy(m_dst, m_src, m_size * sizeof(T));
        return rapidobj_errc::Success;
    }

    inline auto Subdivide(size_t num) const noexcept;

  private:
    T*       m_dst{};
    const T* m_src;
    size_t   m_size;
};

template <typename T>
struct FillElements final {
    FillElements(T* dst, T value, size_t size) noexcept : m_dst(dst), m_value(value), m_size(size) {}

    auto Cost() const noexcept
    {
        constexpr auto cost = sizeof(T) == 1 ? kMergeCopyByteCost : kMergeCopyIntCost;
        return cost * m_size;
    }

    auto Execute() const noexcept
    {
        std::fill_n(m_dst, m_size, m_value);
        return rapidobj_errc::Success;
    }

    inline auto Subdivide(size_t num) const noexcept;

  private:
    T*      m_dst{};
    const T m_value;
    size_t  m_size;
};

template <typename T>
struct FillIds final {
    FillIds(T* dst, const std::vector<FillSrc<T>>& src, size_t size, size_t start) noexcept
        : m_dst(dst), m_src(&src), m_size(size), m_start(start)
    {}

    auto Cost() const noexcept { return kMergeFillIdCost * m_size; }

    auto Execute() const noexcept
    {
        auto comp = [](auto src, size_t offset) { return src.offset < offset; };
        auto it   = std::lower_bound(m_src->begin(), m_src->end(), m_start, comp);
        if (it == m_src->end()) {
            return rapidobj_errc::InternalError;
        }

        auto index = static_cast<size_t>(it - m_src->begin());
        if (m_src->at(index).offset != m_start) {
            --index;
        }

        auto dst   = m_dst;
        auto size  = m_size;
        auto start = m_start;

        while (size > 0 && index < m_src->size() - 1) {
            auto count = std::min(size, m_src->at(index + 1).offset - start);
            std::fill_n(dst, count, m_src->at(index).id);
            start += count;
            dst += count;
            size -= count;
            ++index;
        }

        return rapidobj_errc::Success;
    }

    inline auto Subdivide(size_t num) const noexcept;

  private:
    T*                             m_dst{};
    const std::vector<FillSrc<T>>* m_src{};
    size_t                         m_size{};
    size_t                         m_start{};
};

struct CopyIndices final {
    CopyIndices(
        Index*             dst,
        const Index*       src,
        const OffsetFlags* offset_flags,
        size_t             size,
        AttributeInfo      offset,
        AttributeInfo      count) noexcept
        : m_dst(dst), m_src(src), m_offset_flags(offset_flags), m_size(size), m_offset(offset), m_count(count)
    {}

    auto Cost() const noexcept { return kMergeCopyIndexCost * m_size; }

    auto Execute() const noexcept
    {
        for (size_t i = 0; i != m_size; ++i) {
            auto position_index = m_src[i].position_index;
            auto texcoord_index = m_src[i].texcoord_index;
            auto normal_index   = m_src[i].normal_index;
            auto offset_flags   = m_offset_flags[i];

            bool is_out_of_bounds = false;

            if (offset_flags & ApplyOffset::Position) {
                position_index += static_cast<int>(m_offset.position);
            }
            is_out_of_bounds |= position_index < 0 || position_index >= static_cast<int>(m_count.position);

            if (offset_flags & ApplyOffset::Texcoord) {
                texcoord_index += static_cast<int>(m_offset.texcoord);
                is_out_of_bounds |= texcoord_index < 0;
            } else {
                is_out_of_bounds |= texcoord_index < -1;
            }
            is_out_of_bounds |= texcoord_index >= static_cast<int>(m_count.texcoord);

            if (offset_flags & ApplyOffset::Normal) {
                normal_index += static_cast<int>(m_offset.normal);
                is_out_of_bounds |= normal_index < 0;
            } else {
                is_out_of_bounds |= normal_index < -1;
            }
            is_out_of_bounds |= normal_index >= static_cast<int>(m_count.normal);

            if (is_out_of_bounds) {
                return rapidobj_errc::IndexOutOfBoundsError;
            }

            m_dst[i].position_index = position_index;
            m_dst[i].texcoord_index = texcoord_index;
            m_dst[i].normal_index   = normal_index;
        }

        return rapidobj_errc::Success;
    }

    inline auto Subdivide(size_t num) const noexcept;

  private:
    Index*             m_dst{};
    const Index*       m_src{};
    const OffsetFlags* m_offset_flags{};
    size_t             m_size{};
    AttributeInfo      m_offset{};
    AttributeInfo      m_count{};
};

using MergeTask =
    std::variant<CopyBytes, CopyInts, CopyFloats, CopyIndices, FillFloats, FillMaterialIds, FillSmoothingGroupIds>;
using MergeTasks = std::vector<MergeTask>;

template <typename T>
auto CopyElements<T>::Subdivide(size_t num) const noexcept
{
    auto begin = size_t{ 0 };
    auto tasks = MergeTasks();
    tasks.reserve(num);
    for (size_t i = 0; i != num; ++i) {
        auto end = (1 + i) * m_size / num;
        tasks.push_back(CopyElements(m_dst + begin, m_src + begin, end - begin));
        begin = end;
    }
    return tasks;
}

template <typename T>
auto FillElements<T>::Subdivide(size_t num) const noexcept
{
    auto begin = size_t{ 0 };
    auto tasks = MergeTasks();
    tasks.reserve(num);
    for (size_t i = 0; i != num; ++i) {
        auto end = (1 + i) * m_size / num;
        tasks.push_back(FillElements(m_dst + begin, m_value, end - begin));
        begin = end;
    }
    return tasks;
}

template <typename T>
auto FillIds<T>::Subdivide(size_t num) const noexcept
{
    auto begin = size_t{ 0 };
    auto tasks = MergeTasks();
    tasks.reserve(num);
    for (size_t i = 0; i != num; ++i) {
        auto end = (1 + i) * m_size / num;
        tasks.push_back(FillIds(m_dst + begin, *m_src, end - begin, m_start + begin));
        begin = end;
    }
    return tasks;
}

auto CopyIndices::Subdivide(size_t num) const noexcept
{
    auto begin = size_t{ 0 };
    auto tasks = MergeTasks();
    tasks.reserve(num);
    for (size_t i = 0; i != num; ++i) {
        auto end = (1 + i) * m_size / num;
        tasks.push_back(CopyIndices(m_dst + begin, m_src + begin, m_offset_flags, end - begin, m_offset, m_count));
        begin = end;
    }
    return tasks;
}

struct Reader {
    struct ReadResult final {
        std::size_t     bytes_read;
        std::error_code error_code;
    };

    virtual ~Reader() noexcept = default;

    virtual std::error_code ReadBlock(size_t offset, size_t size, char* buffer) = 0;
    virtual ReadResult      WaitForResult()                                     = 0;

    auto NumRequests() const noexcept { return m_num_requests; }
    auto SubmitTime() const noexcept { return m_submit_time; }
    auto WaitTime() const noexcept { return m_wait_time; }
    auto BytesRead() const noexcept { return m_bytes_read; }
    auto Error() const noexcept { return m_error; }

  protected:
    int                      m_num_requests{};
    std::chrono::nanoseconds m_submit_time{};
    std::chrono::nanoseconds m_wait_time{};
    size_t                   m_bytes_read{};
    std::error_code          m_error{};
};

namespace sys {

#ifdef __linux__

inline auto AlignedAllocate(size_t size, size_t alignment)
{
    return static_cast<char*>(aligned_alloc(alignment, size));
}

struct AlignedDeleter final {
    void operator()(void* ptr) const { free(ptr); }
};

class File final {
  public:
    File(const std::filesystem::path& filepath)
    {
        auto filepath_string = filepath.string();

        if (-1 == stat(filepath_string.c_str(), &m_info)) {
            m_error = std::error_code(errno, std::system_category());
            return;
        }

        m_fd = open(filepath_string.c_str(), O_RDONLY);

        if (-1 == m_fd) {
            m_error = std::error_code(errno, std::system_category());
        }
    }
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
    File(File&&)                 = delete;
    File& operator=(File&&)      = delete;
    ~File() noexcept
    {
        if (m_fd != -1) {
            close(m_fd);
        }
    }

    explicit operator bool() const noexcept { return m_fd != -1; }
    auto     handle() const noexcept { return m_fd; }
    auto     size() const noexcept { return static_cast<size_t>(m_info.st_size); }
    auto     error() const noexcept { return m_error; }

  private:
    int             m_fd = -1;
    struct stat     m_info {};
    std::error_code m_error{};
};

struct FileReader : Reader {
    FileReader(const File& file) noexcept : m_fd{ file.handle() } {}

    std::error_code ReadBlock(size_t offset, size_t size, char* buffer) override
    {
        assert(buffer);
        assert(m_fd != -1);
        assert(std::uintptr_t(buffer) % 4096 == 0);

        ++m_num_requests;

        auto t1 = std::chrono::steady_clock::now();

        m_offset = offset;
        m_size   = size;
        m_buffer = buffer;

        readahead(m_fd, offset, size);

        auto t2 = std::chrono::steady_clock::now();

        m_submit_time += t2 - t1;

        return {};
    }

    ReadResult WaitForResult() override
    {
        auto t1 = std::chrono::steady_clock::now();

        auto result = pread64(m_fd, m_buffer, m_size, m_offset);

        auto t2 = std::chrono::steady_clock::now();

        m_wait_time += t2 - t1;

        if (result < 0) {
            auto error = std::error_code(errno, std::system_category());
            return ReadResult{ 0, error };
        }

        auto bytes_read = static_cast<size_t>(result);

        m_bytes_read += bytes_read;

        return ReadResult{ bytes_read, std::error_code() };
    }

  private:
    int    m_fd = -1;
    size_t m_offset{};
    size_t m_size{};
    char*  m_buffer{};
};

#elif _WIN32

inline auto AlignedAllocate(size_t size, size_t alignment)
{
    return static_cast<char*>(_aligned_malloc(size, alignment));
}

struct AlignedDeleter final {
    void operator()(void* ptr) const { _aligned_free(ptr); }
};

class File final {
  public:
    File(const std::filesystem::path& filepath)
    {
        auto filepath_string = filepath.string();

        m_handle = CreateFileA(
            filepath_string.c_str(),
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING | FILE_FLAG_OVERLAPPED,
            nullptr);

        if (m_handle == INVALID_HANDLE_VALUE) {
            m_error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
            return;
        }

        auto size = LARGE_INTEGER{};
        if (!GetFileSizeEx(m_handle, &size)) {
            m_error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
            Close();
            return;
        }

        m_size = static_cast<size_t>(size.QuadPart);
    }
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
    File(File&&)                 = delete;
    File& operator=(File&&)      = delete;
    ~File() noexcept { Close(); }

    explicit operator bool() const noexcept { return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE; }
    auto     handle() const noexcept { return m_handle; }
    auto     size() const noexcept { return m_size; }
    auto     error() const noexcept { return m_error; }

  private:
    void Close() noexcept
    {
        if (m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);
            m_handle = INVALID_HANDLE_VALUE;
        }
    }

    HANDLE          m_handle{};
    size_t          m_size{};
    std::error_code m_error{};
};

struct FileReader : Reader {
    FileReader(const File& file)
    {
        m_handle = CreateEventA(nullptr, FALSE, FALSE, nullptr);

        if (m_handle == INVALID_HANDLE_VALUE) {
            m_error = std::error_code(static_cast<int>(GetLastError()), std::system_category());
        }

        m_file = file.handle();
    }

    ~FileReader() noexcept override
    {
        if (m_handle != INVALID_HANDLE_VALUE) {
            CloseHandle(m_handle);
        }
    }

    std::error_code ReadBlock(size_t offset, size_t size, char* buffer) override
    {
        assert(m_handle && m_handle != INVALID_HANDLE_VALUE);
        assert(m_file && m_file != INVALID_HANDLE_VALUE);

        ++m_num_requests;

        auto t1 = std::chrono::steady_clock::now();

        m_overlapped.hEvent     = m_handle;
        m_overlapped.Offset     = static_cast<DWORD>(offset);
        m_overlapped.OffsetHigh = static_cast<DWORD>(offset >> 32);

        auto error   = std::error_code();
        bool success = ReadFile(m_file, buffer, static_cast<DWORD>(size), nullptr, &m_overlapped);

        if (!success) {
            if (auto ec = GetLastError(); ec != ERROR_IO_PENDING) {
                error = std::error_code(ec, std::system_category());
            }
        }

        auto t2 = std::chrono::steady_clock::now();

        m_submit_time += t2 - t1;

        return error;
    }

    ReadResult WaitForResult() override
    {
        assert(m_handle && m_handle != INVALID_HANDLE_VALUE);
        assert(m_file && m_file != INVALID_HANDLE_VALUE);

        auto t1 = std::chrono::steady_clock::now();

        auto bytes_read = DWORD{};
        bool success    = GetOverlappedResult(m_handle, &m_overlapped, &bytes_read, TRUE);

        auto t2 = std::chrono::steady_clock::now();

        m_wait_time += t2 - t1;

        if (!success) {
            if (auto ec = GetLastError(); ec != ERROR_HANDLE_EOF) {
                auto error = std::error_code(ec, std::system_category());
                return ReadResult{ 0, error };
            }
        }

        m_bytes_read += bytes_read;

        return { bytes_read, std::error_code() };
    }

  private:
    HANDLE     m_handle{};
    HANDLE     m_file{};
    OVERLAPPED m_overlapped{};
};

#elif __APPLE__

inline auto AlignedAllocate(size_t size, size_t alignment)
{
    return static_cast<char*>(aligned_alloc(alignment, size));
}

struct AlignedDeleter final {
    void operator()(void* ptr) const { free(ptr); }
};

class File final {
  public:
    File(const std::filesystem::path& filepath)
    {
        auto filepath_string = filepath.string();

        if (-1 == stat(filepath_string.c_str(), &m_info)) {
            m_error = std::error_code(errno, std::system_category());
            return;
        }

        m_fd = open(filepath_string.c_str(), O_RDONLY);

        if (-1 == m_fd) {
            m_error = std::error_code(errno, std::system_category());
        }
    }
    File(const File&)            = delete;
    File& operator=(const File&) = delete;
    File(File&&)                 = delete;
    File& operator=(File&&)      = delete;
    ~File() noexcept
    {
        if (m_fd != -1) {
            close(m_fd);
        }
    }

    explicit operator bool() const noexcept { return m_fd != -1; }
    auto     handle() const noexcept { return m_fd; }
    auto     size() const noexcept { return static_cast<size_t>(m_info.st_size); }
    auto     error() const noexcept { return m_error; }

  private:
    int             m_fd = -1;
    struct stat     m_info {};
    std::error_code m_error{};
};

struct FileReader : Reader {
    FileReader(const File& file) noexcept : m_fd{ file.handle() }
    {
        assert(m_fd != -1);

        fcntl(m_fd, F_NOCACHE, 1);
    }

    std::error_code ReadBlock(size_t offset, size_t size, char* buffer) override
    {
        assert(buffer);
        assert(std::uintptr_t(buffer) % 4096 == 0);

        ++m_num_requests;

        auto t1 = std::chrono::steady_clock::now();

        auto error = std::error_code();

        m_offset = offset;
        m_size   = size;
        m_buffer = buffer;

        radvisory args{};
        args.ra_offset = static_cast<off_t>(offset);
        args.ra_count  = static_cast<int>(size);

        auto result = fcntl(m_fd, F_RDADVISE, &args);

        if (result == -1) {
            error = std::error_code(errno, std::system_category());
        }

        auto t2 = std::chrono::steady_clock::now();

        m_submit_time += t2 - t1;

        return error;
    }

    ReadResult WaitForResult() override
    {
        auto t1 = std::chrono::steady_clock::now();

        auto result = pread(m_fd, m_buffer, m_size, m_offset);

        auto t2 = std::chrono::steady_clock::now();

        m_wait_time += t2 - t1;

        if (result < 0) {
            auto error = std::error_code(errno, std::system_category());
            return ReadResult{ 0, error };
        }

        auto bytes_read = static_cast<size_t>(result);

        m_bytes_read += bytes_read;

        return ReadResult{ bytes_read, std::error_code() };
    }

  private:
    int    m_fd = -1;
    off_t  m_offset{};
    size_t m_size{};
    char*  m_buffer{};
};

#endif

} // namespace sys

using DataSource = std::variant<sys::File*, std::istream*>;

struct StreamReader : Reader {
    StreamReader(std::istream* is) noexcept : m_stream(is)
    {
        assert(is);
        if (!is->good()) {
            m_error = std::make_error_code(std::io_errc::stream);
        }
    }

    std::error_code ReadBlock(size_t offset, size_t size, char* buffer) override
    {
        assert(buffer);

        ++m_num_requests;

        m_offset = offset;
        m_size   = size;
        m_buffer = buffer;

        return std::error_code();
    }

    ReadResult WaitForResult() override
    {
        ReadResult result{};

        auto t1 = std::chrono::steady_clock::now();

        if (m_stream->read(m_buffer, m_size) || m_stream->eof()) {
            result = { static_cast<size_t>(m_stream->gcount()), std::error_code() };
        } else {
            result = { size_t{}, std::make_error_code(std::io_errc::stream) };
        }

        auto t2 = std::chrono::steady_clock::now();

        m_wait_time += t2 - t1;

        m_bytes_read += result.bytes_read;

        return result;
    }

  private:
    std::istream* m_stream{};
    size_t        m_offset{};
    size_t        m_size{};
    char*         m_buffer{};
};

inline std::unique_ptr<Reader> CreateReader(sys::File* file)
{
    return std::make_unique<sys::FileReader>(*file);
}
inline std::unique_ptr<Reader> CreateReader(std::istream* is)
{
    return std::make_unique<StreamReader>(is);
}
inline std::unique_ptr<Reader> CreateReader(DataSource source)
{
    return std::visit([](auto arg) { return CreateReader(arg); }, source);
}

inline std::string ToString(std::chrono::nanoseconds time, int width = 0)
{
    auto text = std::string();
    if (time.count() > 19'000'000'000) {
        text = std::to_string(time.count() / 1'000'000'000);
        text.append(" s");
    } else if (time.count() > 19'000'000) {
        text = std::to_string(time.count() / 1'000'000);
        text.append(" ms");
    } else if (time.count() > 19'000) {
        text = std::to_string(time.count() / 1'000);
        text.append(" us");
    } else {
        text = std::to_string(time.count());
        text.append(" ns");
    }
    auto n = width - static_cast<int>(text.size());
    if (n > 0) {
        text.insert(0, n, ' ');
    }
    return text;
}

inline std::string ToString(size_t i, int width = 0)
{
    auto text = std::to_string(i);
    auto n    = width - static_cast<int>(text.size());
    if (n > 0) {
        text.insert(0, n, ' ');
    }
    return text;
}

inline std::string RateToString(double bytes_per_second, int width = 0)
{
    const char* unit = " B/s";

    if (bytes_per_second > 99_GiB) {
        bytes_per_second /= 1_GiB;
        unit = " GB/s";
    } else if (bytes_per_second > 99_MiB) {
        bytes_per_second /= 1_MiB;
        unit = " MB/s";
    } else if (bytes_per_second > 99_KiB) {
        bytes_per_second /= 1_KiB;
        unit = " KB/s";
    }

    auto text = std::to_string(static_cast<int>(bytes_per_second));
    text.append(unit);
    auto n = width - static_cast<int>(text.size());
    if (n > 0) {
        text.insert(0, n, ' ');
    }
    return text;
}

inline std::string ComputeDebugStats(const std::vector<std::chrono::nanoseconds>& population) noexcept
{
    auto text = std::string();

    auto min_time  = std::chrono::nanoseconds{ std::chrono::nanoseconds::max() };
    auto max_time  = std::chrono::nanoseconds{ std::chrono::nanoseconds::min() };
    auto min_index = size_t{};
    auto max_index = size_t{};
    auto avg_time  = std::chrono::nanoseconds{};

    for (size_t i = 0; i != population.size(); ++i) {
        auto sample = population[i];
        if (sample < min_time) {
            min_time  = sample;
            min_index = i;
        }
        if (sample > max_time) {
            max_time  = sample;
            max_index = i;
        }
        avg_time += sample;
    }

    avg_time /= population.size();

    int64_t sq_sum{};

    for (size_t i = 0; i != population.size(); ++i) {
        auto sample = population[i];
        auto diff   = (sample - avg_time).count();
        sq_sum += diff * diff;
    }

    auto std_dev = std::chrono::nanoseconds(static_cast<int64_t>(std::sqrt(sq_sum / population.size())));

    text.append("min: ").append(ToString(min_time, 10));
    text.append(" [#").append(std::to_string(min_index + 1)).append("]\n");
    text.append("max: ").append(ToString(max_time, 10));
    text.append(" [#").append(std::to_string(max_index + 1)).append("]\n");
    text.append("avg: ").append(ToString(avg_time, 10)).append("\n");
    text.append("std: ").append(ToString(std_dev, 10)).append("\n");

    return text;
}

inline std::string DumpDebug(const SharedContext& context)
{
    auto text = std::string();

    auto population = std::vector<std::chrono::nanoseconds>(context.debug.io.num_requests.size());

    auto num_bytes_read = size_t{};

    for (size_t i = 0; i != context.debug.io.num_requests.size(); ++i) {
        auto n          = context.debug.io.num_requests[i];
        auto submit_ns  = context.debug.io.submit_time[i];
        auto wait_ns    = context.debug.io.wait_time[i];
        auto total_ns   = submit_ns.count() + wait_ns.count();
        auto average_ns = total_ns / n;
        auto thread     = ToString(i + 1, 3);
        auto requests   = ToString(n, 5);
        auto submit     = ToString(std::chrono::nanoseconds(submit_ns), 9);
        auto wait       = ToString(std::chrono::nanoseconds(wait_ns), 9);
        auto total      = ToString(std::chrono::nanoseconds(total_ns), 9);
        auto average    = ToString(std::chrono::nanoseconds(average_ns), 9);

        population[i] = std::chrono::nanoseconds{ total_ns };

        num_bytes_read += context.debug.io.num_bytes_read[i];

        text.append(thread);
        text.append(": blk").append(requests);
        text.append("    read").append(submit);
        text.append("    wait").append(wait);
        text.append("    sum").append(total);
        text.append("    avg").append(average);
        text.push_back('\n');
    }

    text.push_back('\n');
    text.append(ComputeDebugStats(population));
    text.push_back('\n');

    for (size_t i = 0; i != context.debug.io.num_requests.size(); ++i) {
        auto parse_ns      = context.debug.parse.time[i];
        auto io_ns         = context.debug.io.submit_time[i] + context.debug.io.wait_time[i];
        auto io_percentage = static_cast<int>(0.5f + 100.0f * io_ns.count() / parse_ns.count());
        auto thread        = ToString(i + 1, 3);
        auto parse         = ToString(parse_ns, 9);

        population[i] = parse_ns;

        text.append(thread).append(": parse").append(parse);
        text.append("  (").append(std::to_string(io_percentage)).append("% io)\n");
    }

    text.push_back('\n');
    text.append(ComputeDebugStats(population));
    text.push_back('\n');

    auto parse_total      = context.debug.parse.total_time;
    auto merge_total      = context.debug.merge.total_time;
    auto total            = parse_total + merge_total;
    auto parse_percentage = static_cast<int>(0.5f + 100.0f * parse_total.count() / total.count());
    auto merge_percentage = 100 - parse_percentage;
    auto bytes_per_second = num_bytes_read / (parse_total.count() / 1000'000'000.0);

    text.append("Parse Time: ");
    text.append(ToString(parse_total, 10));
    text.append(" (").append(std::to_string(parse_percentage)).append("%)\n");
    text.append("Merge Time: ");
    text.append(ToString(merge_total, 10));
    text.append(" (").append(std::to_string(merge_percentage)).append("%)\n");
    text.append("Parse Rate: ").append(RateToString(bytes_per_second, 12)).append("\n");

    return text;
}

inline auto ParseXReals(std::string_view line, size_t max_count, float* out)
{
    size_t count = 0;
    while (!line.empty() && count < max_count) {
        TrimLeft(line);
        auto [ptr, rc] = fast_float::from_chars(line.data(), line.data() + line.size(), *out);
        if (rc != kSuccess) {
            return std::make_pair(count, line);
        }
        auto num_parsed = static_cast<size_t>(ptr - line.data());
        line.remove_prefix(num_parsed);
        ++count;
        ++out;
    }
    return std::make_pair(count, line);
}

inline auto ParseXReals(std::string_view line, size_t max_count, Buffer<float>* out)
{
    size_t count = 0;
    out->ensure_enough_room_for(max_count);
    while (!line.empty() && count < max_count) {
        TrimLeft(line);
        auto value     = float();
        auto [ptr, rc] = fast_float::from_chars(line.data(), line.data() + line.size(), value);
        if (rc != kSuccess) {
            return std::make_pair(count, line);
        }
        auto num_parsed = static_cast<size_t>(ptr - line.data());
        out->push_back(value);
        line.remove_prefix(num_parsed);
        ++count;
    }
    return std::make_pair(count, line);
}

inline auto ParseReal(std::string_view line, float* out1)
{
    return ParseXReals(line, 1, out1);
}

inline auto ParseReals(std::string_view line, float* out1, float* out2)
{
    float temp[2];
    auto  result = ParseXReals(line, 2, temp);
    *out1        = result.first > 0 ? temp[0] : *out1;
    *out2        = result.first > 1 ? temp[1] : *out2;
    return result;
}

inline auto ParseReals(std::string_view line, float* out1, float* out2, float* out3)
{
    float temp[3];
    auto  result = ParseXReals(line, 3, temp);
    *out1        = result.first > 0 ? temp[0] : *out1;
    *out2        = result.first > 1 ? temp[1] : *out2;
    *out3        = result.first > 2 ? temp[2] : *out3;
    return result;
}

inline size_t ParseReals(std::string_view text, size_t max_count, float* out)
{
    auto count = size_t{};

    TrimLeft(text);

    while (!text.empty() && count < max_count) {
        auto [ptr, rc] = fast_float::from_chars(text.data(), text.data() + text.size(), *out);
        if (rc != kSuccess) {
            return 0;
        }
        auto num_parsed = static_cast<size_t>(ptr - text.data());
        text.remove_prefix(num_parsed);

        TrimLeft(text);

        ++count;
        ++out;
    }

    TrimLeft(text);

    return text.empty() ? count : 0;
}

inline size_t ParseReals(std::string_view text, size_t max_count, Float3* out)
{
    return ParseReals(text, max_count, out->data());
}

inline size_t ParseReals(std::string_view text, size_t max_count, Buffer<float>* out)
{
    auto count = size_t{};

    out->ensure_enough_room_for(max_count);

    TrimLeft(text);

    while (!text.empty() && count < max_count) {
        auto value     = float();
        auto [ptr, rc] = fast_float::from_chars(text.data(), text.data() + text.size(), value);
        if (rc != kSuccess) {
            return 0;
        }
        auto num_parsed = static_cast<size_t>(ptr - text.data());
        out->push_back(value);
        text.remove_prefix(num_parsed);

        TrimLeft(text);

        ++count;
    }

    TrimLeft(text);

    return text.empty() ? count : 0;
}

inline auto ParseFace(
    std::string_view     text,
    size_t               position_count,
    size_t               texcoord_count,
    size_t               normal_count,
    size_t               min_count,
    size_t               max_count,
    OffsetFlags          permitted_flags,
    Buffer<Index>*       indices,
    Buffer<OffsetFlags>* offset_flags)
{
    using std::make_pair;

    auto count = size_t{};
    auto value = 0;

    indices->ensure_enough_room_for(max_count);
    offset_flags->ensure_enough_room_for(max_count);

    while (count <= max_count) {
        TrimLeft(text);

        // exit if there is nothing left to process
        if (text.empty()) {
            break;
        }

        // parse position index
        {
            auto [ptr, rc] = std::from_chars(text.data(), text.data() + text.size(), value);
            if (rc != kSuccess) {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }
            auto num_parsed = static_cast<size_t>(ptr - text.data());
            text.remove_prefix(num_parsed);

            offset_flags->push_back(static_cast<OffsetFlags>(ApplyOffset::None));
            if (value > 0) {
                --value;
            } else if (value < 0) {
                value += static_cast<int>(position_count);
                offset_flags->back() |= static_cast<OffsetFlags>(ApplyOffset::Position);
            } else {
                return make_pair(size_t{ 0 }, rapidobj_errc::IndexOutOfBoundsError);
            }
            if (permitted_flags & ApplyOffset::Position) {
                indices->push_back({ value, -1, -1 });
                ++count;
            } else {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }
        }

        // exit if there is nothing left to process
        if (text.empty()) {
            break;
        }

        // Parse UV
        if (text.front() == '/') {
            text.remove_prefix(1);
            if (text.empty()) {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }
            if (text.front() != '/') {
                // parse texcoord index
                auto [ptr, rc] = std::from_chars(text.data(), text.data() + text.size(), value);
                if (rc != kSuccess) {
                    return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
                }
                auto num_parsed = static_cast<size_t>(ptr - text.data());
                text.remove_prefix(num_parsed);

                if (value > 0) {
                    --value;
                } else if (value < 0) {
                    value += static_cast<int>(texcoord_count);
                    offset_flags->back() |= static_cast<OffsetFlags>(ApplyOffset::Texcoord);
                } else {
                    return make_pair(size_t{ 0 }, rapidobj_errc::IndexOutOfBoundsError);
                }
                if (permitted_flags & ApplyOffset::Texcoord) {
                    indices->back().texcoord_index = value;
                } else {
                    return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
                }
            }
        }

        // exit if there is nothing left to process
        if (text.empty()) {
            break;
        }

        // Parse Normal
        if (text.front() == '/') {
            text.remove_prefix(1);
            if (text.empty()) {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }

            // parse normal index
            auto [ptr, rc] = std::from_chars(text.data(), text.data() + text.size(), value);
            if (rc != kSuccess) {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }
            auto num_parsed = static_cast<size_t>(ptr - text.data());
            text.remove_prefix(num_parsed);

            if (value > 0) {
                --value;
            } else if (value < 0) {
                value += static_cast<int>(normal_count);
                offset_flags->back() |= static_cast<OffsetFlags>(ApplyOffset::Normal);
            } else {
                return make_pair(size_t{ 0 }, rapidobj_errc::IndexOutOfBoundsError);
            }
            if (permitted_flags & ApplyOffset::Normal) {
                indices->back().normal_index = value;
            } else {
                return make_pair(size_t{ 0 }, rapidobj_errc::ParseError);
            }
        }
    }

    if (count < min_count) {
        return make_pair(size_t{ 0 }, rapidobj_errc::TooFewIndicesError);
    }
    if (count > max_count) {
        return make_pair(size_t{ 0 }, rapidobj_errc::TooManyIndicesError);
    }

    return make_pair(count, rapidobj_errc::Success);
}

inline auto ParseTextureOption(std::string_view line, TextureOption* texture_option)
{
    assert(texture_option);

    const auto fail = std::make_pair(std::string_view(), false);

    TrimRight(line);

    while (true) {
        TrimLeft(line);
        if (line.empty()) {
            break;
        }
        if (line.front() != '-') {
            return std::make_pair(line, true);
        } else {
            line.remove_prefix(1);
            if (StartsWith(line, "blendu ") || StartsWith(line, "blendu\t")) {
                line.remove_prefix(7);
                TrimLeft(line);
                if (StartsWith(line, "on")) {
                    line.remove_prefix(2);
                    texture_option->blendu = true;
                } else if (StartsWith(line, "off")) {
                    line.remove_prefix(3);
                    texture_option->blendu = false;
                } else {
                    return fail;
                }
            } else if (StartsWith(line, "blendv ") || StartsWith(line, "blendv\t")) {
                line.remove_prefix(7);
                TrimLeft(line);
                if (StartsWith(line, "on")) {
                    line.remove_prefix(2);
                    texture_option->blendv = true;
                } else if (StartsWith(line, "off")) {
                    line.remove_prefix(3);
                    texture_option->blendv = false;
                } else {
                    return fail;
                }
            } else if (StartsWith(line, "boost ") || StartsWith(line, "boost\t")) {
                line.remove_prefix(6);
                auto [count, remainder] = ParseReal(line, &texture_option->sharpness);
                if (count != 1) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "mm ") || StartsWith(line, "mm\t")) {
                line.remove_prefix(3);
                auto [count, remainder] = ParseReals(line, &texture_option->brightness, &texture_option->contrast);
                if (count != 2) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "o ") || StartsWith(line, "o\t")) {
                line.remove_prefix(2);
                auto [count, remainder] = ParseReals(
                    line,
                    &texture_option->origin_offset[0],
                    &texture_option->origin_offset[1],
                    &texture_option->origin_offset[2]);
                if (count < 1) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "s ") || StartsWith(line, "s\t")) {
                line.remove_prefix(2);
                auto [count, remainder] =
                    ParseReals(line, &texture_option->scale[0], &texture_option->scale[1], &texture_option->scale[2]);
                if (count < 1) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "t ") || StartsWith(line, "t\t")) {
                line.remove_prefix(2);
                auto [count, remainder] = ParseReals(
                    line,
                    &texture_option->turbulence[0],
                    &texture_option->turbulence[1],
                    &texture_option->turbulence[2]);
                if (count < 1) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "texres ") || StartsWith(line, "texres\t")) {
                line.remove_prefix(7);
                TrimLeft(line);
                auto [ptr, rc] =
                    std::from_chars(line.data(), line.data() + line.size(), texture_option->texture_resolution);
                if (rc != kSuccess) {
                    return fail;
                }
                auto num_parsed = static_cast<size_t>(ptr - line.data());
                line.remove_prefix(num_parsed);
            } else if (StartsWith(line, "clamp ") || StartsWith(line, "clamp\t")) {
                line.remove_prefix(6);
                TrimLeft(line);
                if (StartsWith(line, "on")) {
                    line.remove_prefix(2);
                    texture_option->clamp = true;
                } else if (StartsWith(line, "off")) {
                    line.remove_prefix(3);
                    texture_option->clamp = false;
                } else {
                    return fail;
                }
            } else if (StartsWith(line, "bm ") || StartsWith(line, "bm\t")) {
                line.remove_prefix(3);
                auto [count, remainder] = ParseReal(line, &texture_option->bump_multiplier);
                if (count < 1) {
                    return fail;
                }
                line = remainder;
            } else if (StartsWith(line, "imfchan ") || StartsWith(line, "imfchan\t")) {
                line.remove_prefix(8);
                char c = line.empty() ? '?' : line.front();
                if (c == 'r' || c == 'g' || c == 'b' || c == 'm' || c == 'l' || c == 'z') {
                    texture_option->imfchan = c;
                    line.remove_prefix(1);
                } else {
                    return fail;
                }
            } else if (StartsWith(line, "type ") || StartsWith(line, "type\t")) {
                line.remove_prefix(5);
                if (StartsWith(line, "sphere")) {
                    line.remove_prefix(6);
                    texture_option->type = TextureType::Sphere;
                } else if (StartsWith(line, "cube_top")) {
                    line.remove_prefix(8);
                    texture_option->type = TextureType::CubeTop;
                } else if (StartsWith(line, "cube_bottom")) {
                    line.remove_prefix(11);
                    texture_option->type = TextureType::CubeBottom;
                } else if (StartsWith(line, "cube_front")) {
                    line.remove_prefix(10);
                    texture_option->type = TextureType::CubeFront;
                } else if (StartsWith(line, "cube_back")) {
                    line.remove_prefix(9);
                    texture_option->type = TextureType::CubeBack;
                } else if (StartsWith(line, "cube_left")) {
                    line.remove_prefix(9);
                    texture_option->type = TextureType::CubeLeft;
                } else if (StartsWith(line, "cube_right")) {
                    line.remove_prefix(10);
                    texture_option->type = TextureType::CubeRight;
                } else {
                    return fail;
                }
            } else {
                return fail;
            }
            if (!line.empty() && line.front() != ' ' && line.front() != '\t') {
                return fail;
            }
        }
    }

    return std::make_pair(line, true);
}

inline ParseMaterialsResult ParseMaterials(std::string_view text)
{
    auto material_map = MaterialMap{};
    auto material_id  = 0;
    auto materials    = Materials{};
    auto material     = Material{};

    material.bump_texopt.imfchan = 'l';

    auto line_num = size_t{};

    bool has_dissolve = false;

    while (text.empty() == false) {
        auto line_parsed = false;
        auto line        = std::string_view();
        {
            auto ptr = static_cast<const char*>(memchr(text.data(), '\n', text.size()));
            auto len = static_cast<size_t>(ptr - text.data());
            bool eol = ptr;
            line     = eol ? text.substr(0, len) : text;
            text.remove_prefix(line.size() + eol);
            if (EndsWith(line, '\r')) {
                line.remove_suffix(1);
            }
            ++line_num;
        }
        auto line_clone = line;
        Trim(line);
        if (line.empty()) {
            continue;
        }
        switch (line.front()) {
        case 'n': {
            if (StartsWith(line, "newmtl ")) {
                line.remove_prefix(7);
                Trim(line);
                if (!material.name.empty()) {
                    materials.push_back(std::move(material));
                    material = {};
                }
                material.name                = line;
                material.bump_texopt.imfchan = 'l';
                material_map.try_emplace(std::string(line), material_id++);
                has_dissolve = false;
                line_parsed  = true;
            } else if (StartsWith(line, "norm ") || StartsWith(line, "norm\t")) {
                line.remove_prefix(5);
                auto [remainder, success] = ParseTextureOption(line, &material.normal_texopt);
                material.normal_texname   = remainder;
                line_parsed               = success;
            }
            break;
        }
        case 'K': {
            if (StartsWith(line, "Ka ") || StartsWith(line, "Ka\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.ambient);
            } else if (StartsWith(line, "Kd ") || StartsWith(line, "Kd\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.diffuse);
            } else if (StartsWith(line, "Ks ") || StartsWith(line, "Ks\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.specular);
            } else if (StartsWith(line, "Kt ") || StartsWith(line, "Kt\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.transmittance);
            } else if (StartsWith(line, "Ke ") || StartsWith(line, "Ke\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.emission);
            } else if (StartsWith(line, "Km ") || StartsWith(line, "Km\t")) {
                // ignore
                line_parsed = true;
            }
            break;
        }
        case 'T': {
            if (StartsWith(line, "Tf ") || StartsWith(line, "Tf\t")) {
                line.remove_prefix(3);
                line_parsed = 3 == ParseReals(line, 3, &material.transmittance);
            } else if (StartsWith(line, "Tr ") || StartsWith(line, "Tr\t")) {
                line.remove_prefix(3);
                auto transparency = 0.0f;
                line_parsed       = 1 == ParseReals(line, 1, &transparency);
                if (!has_dissolve) {
                    material.dissolve = 1.0f - transparency;
                }
            }
            break;
        }
        case 'd': {
            if (StartsWith(line, "disp ") || StartsWith(line, "disp\t")) {
                line.remove_prefix(5);
                auto [remainder, success]     = ParseTextureOption(line, &material.displacement_texopt);
                material.displacement_texname = remainder;
                line_parsed                   = success;
            } else if (StartsWith(line, "d ") || StartsWith(line, "d\t")) {
                line.remove_prefix(2);
                line_parsed  = 1 == ParseReals(line, 1, &material.dissolve);
                has_dissolve = true;
            }
            break;
        }
        case 'a': {
            if (StartsWith(line, "aniso ") || StartsWith(line, "aniso\t")) {
                line.remove_prefix(6);
                line_parsed = 1 == ParseReals(line, 1, &material.anisotropy);
            } else if (StartsWith(line, "anisor ") || StartsWith(line, "anisor\t")) {
                line.remove_prefix(7);
                line_parsed = 1 == ParseReals(line, 1, &material.anisotropy_rotation);
            }
            break;
        }
        case 'i': {
            if (StartsWith(line, "illum ") || StartsWith(line, "illum\t")) {
                line.remove_prefix(6);
                TrimLeft(line);
                auto [ptr, rc] = std::from_chars(line.data(), line.data() + line.size(), material.illum);
                if (rc == kSuccess) {
                    auto num_parsed = static_cast<size_t>(ptr - line.data());
                    line.remove_prefix(num_parsed);
                    Trim(line);
                    line_parsed = line.empty();
                }
            }
            break;
        }
        case 'b': {
            if (StartsWith(line, "bump ") || StartsWith(line, "bump\t")) {
                line.remove_prefix(5);
                auto [remainder, success] = ParseTextureOption(line, &material.bump_texopt);
                material.bump_texname     = remainder;
                line_parsed               = success;
            }
            break;
        }
        case 'r': {
            if (StartsWith(line, "refl ") || StartsWith(line, "refl\t")) {
                line.remove_prefix(5);
                auto [remainder, success]   = ParseTextureOption(line, &material.reflection_texopt);
                material.reflection_texname = remainder;
                line_parsed                 = success;
            }
            break;
        }
        case 'm': {
            if (StartsWith(line, "map_Ka ") || StartsWith(line, "map_Ka\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.ambient_texopt);
                material.ambient_texname  = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Kd ") || StartsWith(line, "map_Kd\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.diffuse_texopt);
                material.diffuse_texname  = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Ks ") || StartsWith(line, "map_Ks\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.specular_texopt);
                material.specular_texname = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Ns ") || StartsWith(line, "map_Ns\t")) {
                line.remove_prefix(7);
                auto [remainder, success]           = ParseTextureOption(line, &material.specular_highlight_texopt);
                material.specular_highlight_texname = remainder;
                line_parsed                         = success;
            } else if (
                StartsWith(line, "map_bump ") || StartsWith(line, "map_bump\t") || StartsWith(line, "map_Bump ") ||
                StartsWith(line, "map_Bump\t")) {
                line.remove_prefix(9);
                auto [remainder, success] = ParseTextureOption(line, &material.bump_texopt);
                material.bump_texname     = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_d ") || StartsWith(line, "map_d\t")) {
                line.remove_prefix(6);
                auto [remainder, success] = ParseTextureOption(line, &material.alpha_texopt);
                material.alpha_texname    = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Pr ") || StartsWith(line, "map_Pr\t")) {
                line.remove_prefix(7);
                auto [remainder, success]  = ParseTextureOption(line, &material.roughness_texopt);
                material.roughness_texname = remainder;
                line_parsed                = success;
            } else if (StartsWith(line, "map_Pm ") || StartsWith(line, "map_Pm\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.metallic_texopt);
                material.metallic_texname = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Ps ") || StartsWith(line, "map_Ps\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.sheen_texopt);
                material.sheen_texname    = remainder;
                line_parsed               = success;
            } else if (StartsWith(line, "map_Ke ") || StartsWith(line, "map_Ke\t")) {
                line.remove_prefix(7);
                auto [remainder, success] = ParseTextureOption(line, &material.emissive_texopt);
                material.emissive_texname = remainder;
                line_parsed               = success;
            }
            break;
        }
        case 'N': {
            if (StartsWith(line, "Ns ") || StartsWith(line, "Ns\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.shininess);
            } else if (StartsWith(line, "Ni ") || StartsWith(line, "Ni\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.ior);
            }
            break;
        }
        case 'P': {
            if (StartsWith(line, "Pr ") || StartsWith(line, "Pr\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.roughness);
            } else if (StartsWith(line, "Pm ") || StartsWith(line, "Pm\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.metallic);
            } else if (StartsWith(line, "Ps ") || StartsWith(line, "Ps\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.sheen);
            } else if (StartsWith(line, "Pc ") || StartsWith(line, "Pc\t")) {
                line.remove_prefix(3);
                line_parsed = 1 == ParseReals(line, 1, &material.clearcoat_thickness);
            } else if (StartsWith(line, "Pcr ") || StartsWith(line, "Pcr\t")) {
                line.remove_prefix(4);
                line_parsed = 1 == ParseReals(line, 1, &material.clearcoat_roughness);
            }
            break;
        }
        case '#': {
            line_parsed = true;
            break;
        }
        } // end switch
        if (!line_parsed && line_num > 1) {
            return { MaterialMap{},
                     Materials{},
                     Error{ make_error_code(rapidobj_errc::MaterialParseError), std::string(line_clone), line_num } };
        }
    } // end loop

    if (!material.name.empty()) {
        materials.push_back(std::move(material));
    }

    return { std::move(material_map), std::move(materials), Error{} };
}

inline auto FindBestPath(SharedContext* context)
{
    const auto& basepath = context->material.basepath;
    const auto& paths    = std::get<std::vector<std::filesystem::path>>(context->material.library->Value());

    for (const auto& path : paths) {
        auto bestpath = path.is_absolute() ? path : (basepath / path);
        if (std::filesystem::is_directory(bestpath)) {
            bestpath /= context->material.library_name;
        }
        if (std::filesystem::exists(bestpath) && std::filesystem::is_regular_file(bestpath)) {
            return bestpath;
        }
    }

    return std::filesystem::path();
}

inline auto ParseMaterialLibrary(SharedContext* context)
{
    if (std::holds_alternative<std::string_view>(context->material.library->Value())) {
        return ParseMaterials(std::get<std::string_view>(context->material.library->Value()));
    }

    auto filepath = FindBestPath(context);

    if (filepath.empty()) {
        return ParseMaterialsResult{ {}, {}, Error{ make_error_code(rapidobj_errc::MaterialFileError) } };
    }

    auto file = std::ifstream(filepath, std::ios::in | std::ios::binary | std::ios::ate);

    auto filesize = file.tellg();
    if (filesize <= 0) {
        return ParseMaterialsResult{ {}, {}, Error{ std::make_error_code(std::io_errc::stream) } };
    }
    file.seekg(0, std::ios::beg);

    auto filedata = Buffer<char>(static_cast<size_t>(filesize));
    if (file.is_open()) {
        file.read(filedata.data(), filesize);
    } else {
        return ParseMaterialsResult{ {}, {}, Error{ std::make_error_code(std::io_errc::stream) } };
    }
    file.close();

    return ParseMaterials(std::string_view(filedata.data(), static_cast<size_t>(filesize)));
}

inline void DispatchMergeTasks(const MergeTasks& tasks, std::shared_ptr<SharedContext> context)
{
    while (true) {
        auto fetched_index = std::atomic_fetch_add(&context->merging.task_index, size_t(1));

        if (fetched_index >= tasks.size()) {
            break;
        }

        const auto& merge_task = tasks[fetched_index];

        if (auto rc = std::visit([](const auto& task) { return task.Execute(); }, merge_task);
            rc != rapidobj_errc::Success) {
            std::lock_guard lock(context->merging.mutex);
            if (context->merging.error == rapidobj_errc::Success) {
                context->merging.error = rc;
            }
            break;
        }
    }

    if (1 == std::atomic_fetch_sub(&context->merging.thread_count, size_t(1))) {
        context->merging.completed.set_value();
    }
}

inline void MergeSequential(MergeTasks* tasks, std::shared_ptr<SharedContext> context)
{
    DispatchMergeTasks(std::vector<MergeTask>(tasks->begin(), tasks->end()), context);
}

inline void MergeParallel(MergeTasks* merge_tasks, std::shared_ptr<SharedContext> context)
{
    auto tasks = MergeTasks();
    tasks.reserve(merge_tasks->size());

    for (auto& merge_task : *merge_tasks) {
        auto cost = std::visit([](const auto& task) { return task.Cost(); }, merge_task);

        if (cost >= kMergeSubdivideCost) {
            auto divisor  = std::max(size_t(2), cost / kMergeSubdivideCost);
            auto subtasks = std::visit([divisor](const auto& task) { return task.Subdivide(divisor); }, merge_task);
            for (auto& subtask : subtasks) {
                tasks.push_back(std::move(subtask));
            }
        } else {
            tasks.push_back(std::move(merge_task));
        }
    }

    auto threads = std::vector<std::thread>{};
    threads.reserve(context->thread.concurrency);

    context->merging.thread_count = context->thread.concurrency;

    for (size_t i = 0; i != context->thread.concurrency; ++i) {
        threads.emplace_back(DispatchMergeTasks, tasks, context);
        threads.back().detach();
    }

    // wait for merging to finish
    context->merging.completed.get_future().wait();
}

// Merge function helper structs
struct ListInfo final {
    size_t face_buffers_size{};
    size_t shape_records_size{};
    size_t material_offsets_size{};
    size_t smoothing_offsets_size{};

    ListInfo& operator+=(const ListInfo& rhs) noexcept
    {
        face_buffers_size += rhs.face_buffers_size;
        shape_records_size += rhs.shape_records_size;
        material_offsets_size += rhs.material_offsets_size;
        smoothing_offsets_size += rhs.smoothing_offsets_size;
        return *this;
    }
};

struct ShapeInfo final {
    struct Mesh final {
        size_t index_array_size{};
        size_t faces_array_size{};
    };
    struct Line final {
        size_t index_array_size{};
        size_t segment_array_size{};
    };
    struct Point final {
        size_t index_array_size{};
    };
    Mesh  mesh;
    Line  line;
    Point point;
};

struct Offset final {
    size_t position{};
    size_t texcoord{};
    size_t normal{};
    size_t face{};

    Offset& operator+=(const Offset& rhs) noexcept
    {
        position += rhs.position;
        texcoord += rhs.texcoord;
        normal += rhs.normal;
        face += rhs.face;
        return *this;
    }
};

inline Result Merge(const std::vector<Chunk>& chunks, std::shared_ptr<SharedContext> context)
{
    // compute overall sizes of lists
    auto list_info = ListInfo{};
    for (const Chunk& chunk : chunks) {
        list_info += { chunk.mesh.faces.buffer.size(),
                       chunk.shapes.list.size(),
                       chunk.materials.list.size(),
                       chunk.smoothing.list.size() };
    }

    // compute offsets for each chunk and total attribute count
    auto offsets = std::vector<Offset>();
    auto count   = AttributeInfo{};
    {
        offsets.reserve(chunks.size());
        auto running = Offset{};
        for (const Chunk& chunk : chunks) {
            offsets.push_back(running);
            running += { chunk.positions.count, chunk.texcoords.count, chunk.normals.count, chunk.mesh.faces.count };
        }
        count          = { running.position, running.texcoord, running.normal };
        context->stats = { running.position, running.texcoord, running.normal };
    }

    // coalesce per-chunk shape records into a single shape records list
    auto shape_records = std::vector<ShapeRecord>();
    {
        shape_records.reserve(list_info.shape_records_size + 2);
        shape_records.push_back({});
        for (size_t i = 0; i != chunks.size(); ++i) {
            for (const ShapeRecord& record : chunks[i].shapes.list) {
                shape_records.push_back(record);
                shape_records.back().chunk_index = i;
            }
        }
        shape_records.push_back({});
        shape_records.back().mesh.index_buffer_start    = chunks.back().mesh.indices.buffer.size();
        shape_records.back().mesh.face_buffer_start     = chunks.back().mesh.faces.buffer.size();
        shape_records.back().lines.index_buffer_start   = chunks.back().lines.indices.buffer.size();
        shape_records.back().lines.segment_buffer_start = chunks.back().lines.segments.buffer.size();
        shape_records.back().points.index_buffer_start  = chunks.back().points.indices.buffer.size();
        shape_records.back().chunk_index                = chunks.size() - 1;
    }

    // prepare smoothing-source list from per-chunk smoothing-record lists
    auto smoothing_src = std::vector<FillSrc<uint32_t>>();
    {
        smoothing_src.reserve(list_info.smoothing_offsets_size + 2);
        smoothing_src.push_back({ 0, 0 });
        for (size_t i = 0; i != chunks.size(); ++i) {
            for (const SmoothingRecord& record : chunks[i].smoothing.list) {
                if (!smoothing_src.empty()) {
                    if (record.face_buffer_start + offsets[i].face == smoothing_src.back().offset) {
                        smoothing_src.pop_back();
                    }
                }
                smoothing_src.push_back({ record.group_id, record.face_buffer_start + offsets[i].face });
            }
        }
        smoothing_src.push_back({ 0, list_info.face_buffers_size });
    }

    auto parsed_materials = ParseMaterialsResult{};

    if (context->material.parse_result.valid()) {
        parsed_materials = context->material.parse_result.get();
        if (parsed_materials.error.code) {
            if (context->material.library->Policy() == Load::Optional) {
                parsed_materials = {};
                int id           = 0;
                for (const Chunk& chunk : chunks) {
                    for (const MaterialRecord& record : chunk.materials.list) {
                        auto [it, emplaced] = parsed_materials.material_map.try_emplace(std::string(record.name), id);
                        id += emplaced ? 1 : 0;
                    }
                }
            } else {
                return Result{ Attributes{}, Shapes{}, Materials{}, { parsed_materials.error.code } };
            }
        }
    }

    // prepare material-source list from per-chunk material-record lists
    auto material_src = std::vector<FillSrc<int32_t>>();
    if (context->material.library) {
        material_src.reserve(list_info.material_offsets_size + 2);
        material_src.push_back({ -1, 0 });
        for (size_t i = 0; i != chunks.size(); ++i) {
            for (const MaterialRecord& record : chunks[i].materials.list) {
                if (!material_src.empty()) {
                    if (record.face_buffer_start + offsets[i].face == material_src.back().offset) {
                        material_src.pop_back();
                    }
                }
                auto it = parsed_materials.material_map.find(record.name);
                if (it != parsed_materials.material_map.end()) {
                    material_src.push_back({ it->second, record.face_buffer_start + offsets[i].face });
                } else {
                    auto line_num = record.line_num;
                    for (size_t j = 0; j != i; ++j) {
                        line_num += chunks[j].text.line_count;
                    }
                    auto error = Error{ rapidobj_errc::MaterialNotFoundError, record.line, line_num };
                    return Result{ Attributes{}, Shapes{}, Materials{}, std::move(error) };
                }
            }
        }
        material_src.push_back({ -1, list_info.face_buffers_size });
    }

    auto shapes = Shapes();
    auto tasks  = MergeTasks();

    shapes.reserve(shape_records.size());

    // this big loop allocates shapes and computes tasks that construct them
    for (size_t i = 0; i != shape_records.size() - 1; ++i) {
        auto& shape = shape_records[i];
        auto& next  = shape_records[i + 1];

        // compute shape info
        auto shape_info = ShapeInfo{};

        for (size_t j = shape.chunk_index; j <= next.chunk_index; ++j) {
            auto mesh_index_buffer_size    = chunks[j].mesh.indices.buffer.size();
            auto mesh_faces_buffer_size    = chunks[j].mesh.faces.buffer.size();
            auto lines_index_buffer_size   = chunks[j].lines.indices.buffer.size();
            auto lines_segment_buffer_size = chunks[j].lines.segments.buffer.size();
            auto points_index_buffer_size  = chunks[j].points.indices.buffer.size();
            if (shape.chunk_index == next.chunk_index) {
                shape_info.mesh.index_array_size   = next.mesh.index_buffer_start - shape.mesh.index_buffer_start;
                shape_info.mesh.faces_array_size   = next.mesh.face_buffer_start - shape.mesh.face_buffer_start;
                shape_info.line.index_array_size   = next.lines.index_buffer_start - shape.lines.index_buffer_start;
                shape_info.line.segment_array_size = next.lines.segment_buffer_start - shape.lines.segment_buffer_start;
                shape_info.point.index_array_size  = next.points.index_buffer_start - shape.points.index_buffer_start;
            } else if (j == shape.chunk_index) {
                shape_info.mesh.index_array_size   = mesh_index_buffer_size - shape.mesh.index_buffer_start;
                shape_info.mesh.faces_array_size   = mesh_faces_buffer_size - shape.mesh.face_buffer_start;
                shape_info.line.index_array_size   = lines_index_buffer_size - shape.lines.index_buffer_start;
                shape_info.line.segment_array_size = lines_segment_buffer_size - shape.lines.segment_buffer_start;
                shape_info.point.index_array_size  = points_index_buffer_size - shape.points.index_buffer_start;
            } else if (j == next.chunk_index) {
                shape_info.mesh.index_array_size += next.mesh.index_buffer_start;
                shape_info.mesh.faces_array_size += next.mesh.face_buffer_start;
                shape_info.line.index_array_size += next.lines.index_buffer_start;
                shape_info.line.segment_array_size += next.lines.segment_buffer_start;
                shape_info.point.index_array_size += next.points.index_buffer_start;
            } else {
                shape_info.mesh.index_array_size += mesh_index_buffer_size;
                shape_info.mesh.faces_array_size += mesh_faces_buffer_size;
                shape_info.line.index_array_size += lines_index_buffer_size;
                shape_info.line.segment_array_size += lines_segment_buffer_size;
                shape_info.point.index_array_size += points_index_buffer_size;
            }
        }

        // skip empty shape
        if (!shape_info.mesh.index_array_size && !shape_info.line.index_array_size &&
            !shape_info.point.index_array_size) {
            continue;
        }

        auto num_indices       = shape_info.mesh.index_array_size;
        auto num_faces         = shape_info.mesh.faces_array_size;
        auto num_material_ids  = context->material.library ? num_faces : 0;
        auto num_smoothing_ids = num_faces;

        // allocate Shape
        shapes.push_back(Shape{
            std::move(shape.name),
            Mesh{ Array<Index>(num_indices),
                  Array<uint8_t>(num_faces),
                  Array<int32_t>(num_material_ids),
                  Array<uint32_t>(num_smoothing_ids) },
            Lines{ Array<Index>(shape_info.line.index_array_size), Array<int32_t>(shape_info.line.segment_array_size) },
            Points{ Array<Index>(shape_info.point.index_array_size) } });

        // compute tasks to construct shape mesh
        if (shape_info.mesh.index_array_size) {
            // compute mesh mem copies
            {
                auto index_dst = shapes.back().mesh.indices.begin();
                auto nface_dst = shapes.back().mesh.num_face_vertices.begin();

                for (size_t j = shape.chunk_index; j <= next.chunk_index; ++j) {
                    auto index_src   = chunks[j].mesh.indices.buffer.data();
                    auto index_flags = chunks[j].mesh.indices.flags.data();
                    auto index_size  = chunks[j].mesh.indices.buffer.size();
                    auto nface_src   = chunks[j].mesh.faces.buffer.data();
                    auto nface_size  = chunks[j].mesh.faces.buffer.size();
                    if (shape.chunk_index == next.chunk_index) {
                        index_src   = index_src + shape.mesh.index_buffer_start;
                        index_flags = index_flags + shape.mesh.index_buffer_start;
                        index_size  = next.mesh.index_buffer_start - shape.mesh.index_buffer_start;
                        nface_src   = nface_src + shape.mesh.face_buffer_start;
                        nface_size  = next.mesh.face_buffer_start - shape.mesh.face_buffer_start;
                    } else if (j == shape.chunk_index) {
                        index_src   = index_src + shape.mesh.index_buffer_start;
                        index_flags = index_flags + shape.mesh.index_buffer_start;
                        index_size  = index_size - shape.mesh.index_buffer_start;
                        nface_src   = nface_src + shape.mesh.face_buffer_start;
                        nface_size  = nface_size - shape.mesh.face_buffer_start;
                    } else if (j == next.chunk_index) {
                        index_size = next.mesh.index_buffer_start;
                        nface_size = next.mesh.face_buffer_start;
                    }
                    if (index_size) {
                        auto offset = AttributeInfo{ offsets[j].position, offsets[j].texcoord, offsets[j].normal };
                        tasks.push_back(CopyIndices(index_dst, index_src, index_flags, index_size, offset, count));
                        index_dst += index_size;
                    }
                    if (nface_size) {
                        tasks.push_back(CopyBytes(nface_dst, nface_src, nface_size));
                        nface_dst += nface_size;
                    }
                }
            }
            // compute material id fills
            if (!material_src.empty()) {
                auto dst   = shapes.back().mesh.material_ids.begin();
                auto size  = shape_info.mesh.faces_array_size;
                auto start = shape.mesh.face_buffer_start + offsets[shape.chunk_index].face;
                tasks.push_back(FillMaterialIds(dst, material_src, size, start));
            }
            // compute smoothing group id fills
            {
                auto dst   = shapes.back().mesh.smoothing_group_ids.begin();
                auto size  = shape_info.mesh.faces_array_size;
                auto start = shape.mesh.face_buffer_start + offsets[shape.chunk_index].face;
                tasks.push_back(FillSmoothingGroupIds(dst, smoothing_src, size, start));
            }
        }

        // compute tasks to construct shape lines
        if (shape_info.line.index_array_size) {
            auto index_dst = shapes.back().lines.indices.begin();
            auto nline_dst = shapes.back().lines.num_line_vertices.begin();

            for (size_t j = shape.chunk_index; j <= next.chunk_index; ++j) {
                auto index_src   = chunks[j].lines.indices.buffer.data();
                auto index_flags = chunks[j].lines.indices.flags.data();
                auto index_size  = chunks[j].lines.indices.buffer.size();
                auto nline_src   = chunks[j].lines.segments.buffer.data();
                auto nline_size  = chunks[j].lines.segments.buffer.size();
                if (shape.chunk_index == next.chunk_index) {
                    index_src   = index_src + shape.lines.index_buffer_start;
                    index_flags = index_flags + shape.lines.index_buffer_start;
                    index_size  = next.lines.index_buffer_start - shape.lines.index_buffer_start;
                    nline_src   = nline_src + shape.lines.segment_buffer_start;
                    nline_size  = next.lines.segment_buffer_start - shape.lines.segment_buffer_start;
                } else if (j == shape.chunk_index) {
                    index_src   = index_src + shape.lines.index_buffer_start;
                    index_flags = index_flags + shape.lines.index_buffer_start;
                    index_size  = index_size - shape.lines.index_buffer_start;
                    nline_src   = nline_src + shape.lines.segment_buffer_start;
                    nline_size  = nline_size - shape.lines.segment_buffer_start;
                } else if (j == next.chunk_index) {
                    index_size = next.lines.index_buffer_start;
                    nline_size = next.lines.segment_buffer_start;
                }
                if (index_size) {
                    auto offset = AttributeInfo{ offsets[j].position, offsets[j].texcoord, offsets[j].normal };
                    tasks.push_back(CopyIndices(index_dst, index_src, index_flags, index_size, offset, count));
                    index_dst += index_size;
                }
                if (nline_size) {
                    tasks.push_back(CopyInts(nline_dst, nline_src, nline_size));
                    nline_dst += nline_size;
                }
            }
        }

        // compute tasks to construct shape points
        if (shape_info.point.index_array_size) {
            auto index_dst = shapes.back().points.indices.begin();

            for (size_t j = shape.chunk_index; j <= next.chunk_index; ++j) {
                auto index_src   = chunks[j].points.indices.buffer.data();
                auto index_flags = chunks[j].points.indices.flags.data();
                auto index_size  = chunks[j].points.indices.buffer.size();
                if (shape.chunk_index == next.chunk_index) {
                    index_src   = index_src + shape.points.index_buffer_start;
                    index_flags = index_flags + shape.points.index_buffer_start;
                    index_size  = next.points.index_buffer_start - shape.points.index_buffer_start;
                } else if (j == shape.chunk_index) {
                    index_src   = index_src + shape.points.index_buffer_start;
                    index_flags = index_flags + shape.points.index_buffer_start;
                    index_size  = index_size - shape.points.index_buffer_start;
                } else if (j == next.chunk_index) {
                    index_size = next.points.index_buffer_start;
                }
                if (index_size) {
                    auto offset = AttributeInfo{ offsets[j].position, offsets[j].texcoord, offsets[j].normal };
                    tasks.push_back(CopyIndices(index_dst, index_src, index_flags, index_size, offset, count));
                    index_dst += index_size;
                }
            }
        }
    }

    // compute overall attribute array sizes
    auto attribute_size    = AttributeInfo{};
    bool has_vertex_colors = false;

    for (const Chunk& chunk : chunks) {
        attribute_size += { chunk.positions.buffer.size(), chunk.texcoords.buffer.size(), chunk.normals.buffer.size() };
        has_vertex_colors |= chunk.colors.count ? true : false;
    }

    auto attribute_size_color = has_vertex_colors ? attribute_size.position : size_t{};

    // allocate attribute arrays
    auto attributes = Attributes{ { attribute_size.position },
                                  { attribute_size.texcoord },
                                  { attribute_size.normal },
                                  { attribute_size_color } };

    // compute tasks to construct attribute arrays
    auto positions_destination = attributes.positions.data();
    auto texcoords_destination = attributes.texcoords.data();
    auto normals_destination   = attributes.normals.data();
    auto colors_destination    = attributes.colors.data();

    for (const Chunk& chunk : chunks) {
        if (chunk.positions.buffer.size()) {
            auto dst  = positions_destination;
            auto src  = chunk.positions.buffer.data();
            auto size = chunk.positions.buffer.size();
            tasks.push_back(CopyFloats(dst, src, size));
            positions_destination += size;
        }
        if (chunk.texcoords.buffer.size()) {
            auto dst  = texcoords_destination;
            auto src  = chunk.texcoords.buffer.data();
            auto size = chunk.texcoords.buffer.size();
            tasks.push_back(CopyFloats(dst, src, size));
            texcoords_destination += size;
        }
        if (chunk.normals.buffer.size()) {
            auto dst  = normals_destination;
            auto src  = chunk.normals.buffer.data();
            auto size = chunk.normals.buffer.size();
            tasks.push_back(CopyFloats(dst, src, size));
            normals_destination += size;
        }
        if (chunk.colors.buffer.size()) {
            auto dst  = colors_destination;
            auto src  = chunk.colors.buffer.data();
            auto size = chunk.colors.buffer.size();
            tasks.push_back(CopyFloats(dst, src, size));
            colors_destination += size;
        } else if (has_vertex_colors) {
            auto dst  = colors_destination;
            auto val  = 1.0f;
            auto size = chunk.positions.buffer.size();
            tasks.push_back(FillFloats(dst, val, size));
            colors_destination += size;
        }
    }

    // perform merge
    if (context->thread.concurrency > 1) {
        MergeParallel(&tasks, context);
    } else {
        MergeSequential(&tasks, context);
    }

    if (context->merging.error != rapidobj_errc::Success) {
        auto error = make_error_code(context->merging.error);
        return Result{ Attributes{}, Shapes{}, Materials{}, Error{ error } };
    }

    return Result{ std::move(attributes), std::move(shapes), std::move(parsed_materials.materials), Error{} };
}

inline auto ParsePosition(std::string_view line, Chunk* chunk)
{
    auto [count, remainder] = ParseXReals(line, 3, &chunk->positions.buffer);
    if (count < 3) {
        return rapidobj_errc::ParseError;
    }
    ++chunk->positions.count;
    auto [count2, remainder2] = ParseXReals(remainder, 3, &chunk->colors.buffer);
    if (count2 == 0) {
        if (chunk->colors.buffer.size()) {
            chunk->colors.buffer.ensure_enough_room_for(3);
            chunk->colors.buffer.fill_n(3, 1.0f);
            ++chunk->colors.count;
        }
    } else if (count2 == 3) {
        if ((chunk->colors.buffer.size() == 3) && (chunk->positions.buffer.size() > 3)) {
            size_t n = chunk->positions.buffer.size();
            float  b = chunk->colors.buffer.pop_back();
            float  g = chunk->colors.buffer.pop_back();
            float  r = chunk->colors.buffer.pop_back();
            chunk->colors.buffer.ensure_enough_room_for(n);
            chunk->colors.buffer.fill_n(n - 3, 1.0f);
            chunk->colors.buffer.push_back(r);
            chunk->colors.buffer.push_back(g);
            chunk->colors.buffer.push_back(b);
        }
        ++chunk->colors.count;
    } else {
        return rapidobj_errc::ParseError;
    }
    TrimLeft(remainder2);
    if (!remainder2.empty()) {
        return rapidobj_errc::ParseError;
    }
    return rapidobj_errc::Success;
}

inline rapidobj_errc ProcessLine(std::string_view line, Chunk* chunk, SharedContext* context)
{
    const auto text = line;

    TrimLeft(line);

    // skip empty lines
    if (line.empty()) {
        return rapidobj_errc::Success;
    }

    // process token
    switch (line.front()) {
    case 'v': {
        if (StartsWith(line, "v ") || StartsWith(line, "v\t")) {
            line.remove_prefix(2);
            if (auto rc = ParsePosition(line, chunk); rc != rapidobj_errc::Success) {
                return rc;
            }
        } else if (StartsWith(line, "vt ") || StartsWith(line, "vt\t")) {
            line.remove_prefix(3);
            auto count = ParseReals(line, 3, &chunk->texcoords.buffer);
            if (count < 2) {
                return rapidobj_errc::ParseError;
            } else if (count == 3) {
                chunk->texcoords.buffer.pop_back();
            }
            ++chunk->texcoords.count;
        } else if (StartsWith(line, "vn ") || StartsWith(line, "vn\t")) {
            line.remove_prefix(3);
            auto count = ParseReals(line, 3, &chunk->normals.buffer);
            if (count < 3) {
                return rapidobj_errc::ParseError;
            }
            ++chunk->normals.count;
        } else {
            return rapidobj_errc::ParseError;
        }
        break;
    }
    case 'f': {
        if (StartsWith(line, "f ") || StartsWith(line, "f\t")) {
            line.remove_prefix(2);
            auto [count, rc] = ParseFace(
                line,
                chunk->positions.count,
                chunk->texcoords.count,
                chunk->normals.count,
                kMinVerticesInFace,
                kMaxVerticesInFace,
                static_cast<OffsetFlags>(ApplyOffset::All),
                &chunk->mesh.indices.buffer,
                &chunk->mesh.indices.flags);
            if (rc != rapidobj_errc::Success) {
                return rc;
            }
            chunk->mesh.faces.buffer.ensure_enough_room_for(1);
            chunk->mesh.faces.buffer.push_back(static_cast<unsigned char>(count));
            ++chunk->mesh.faces.count;
        }
        break;
    }
    case '#': break; // ignore comments
    case 'g':
    case 'o': {
        if (StartsWith(line, "g ") || StartsWith(line, "g\t") || StartsWith(line, "o ") || StartsWith(line, "o\t")) {
            line.remove_prefix(2);
            auto name = std::string(line);
            Trim(name);
            chunk->shapes.list.push_back({});
            chunk->shapes.list.back().name                       = std::move(name);
            chunk->shapes.list.back().mesh.index_buffer_start    = chunk->mesh.indices.buffer.size();
            chunk->shapes.list.back().mesh.face_buffer_start     = chunk->mesh.faces.buffer.size();
            chunk->shapes.list.back().lines.index_buffer_start   = chunk->lines.indices.buffer.size();
            chunk->shapes.list.back().lines.segment_buffer_start = chunk->lines.segments.buffer.size();
            chunk->shapes.list.back().points.index_buffer_start  = chunk->points.indices.buffer.size();
        } else {
            return rapidobj_errc::ParseError;
        }
        break;
    }
    case 'm': {
        if (StartsWith(line, "mtllib ") || StartsWith(line, "mtllib\t")) {
            if (context->material.library) {
                line.remove_prefix(7);
                Trim(line);
                bool ambiguous_material_library = false;
                if (context->material.library_name.empty()) {
                    std::lock_guard lock(context->material.mutex);
                    if (context->material.library_name.empty()) {
                        context->material.library_name = line;
                        context->material.parse_result = std::async(std::launch::async, ParseMaterialLibrary, context);
                    } else if (context->material.library_name != line) {
                        ambiguous_material_library = true;
                    }
                } else if (context->material.library_name != line) {
                    ambiguous_material_library = true;
                }
                if (ambiguous_material_library) {
                    return rapidobj_errc::AmbiguousMaterialLibraryError;
                }
            }
        } else {
            return rapidobj_errc::ParseError;
        }
        break;
    }
    case 'u': {
        if (StartsWith(line, "usemtl ") || StartsWith(line, "usemtl\t")) {
            if (context->material.library) {
                line.remove_prefix(7);
                chunk->materials.list.push_back(
                    { std::string(line), std::string(text), chunk->text.line_count, chunk->mesh.faces.buffer.size() });
            }
        } else {
            return rapidobj_errc::ParseError;
        }
        break;
    }
    case 's': {
        if (StartsWith(line, "s ") || StartsWith(line, "s\t")) {
            line.remove_prefix(2);
            Trim(line);
            if (line.empty()) {
                return rapidobj_errc::ParseError;
            } else if (line == "off") {
                chunk->smoothing.list.push_back({ 0, chunk->mesh.faces.buffer.size() });
            } else {
                auto value = 0U;
                auto data  = &line[0];
                if (auto [p, e] = std::from_chars(data, data + line.size(), value); e != kSuccess) {
                    return rapidobj_errc::ParseError;
                }
                chunk->smoothing.list.push_back({ value, chunk->mesh.faces.buffer.size() });
            }
        } else {
            return rapidobj_errc::ParseError;
        }
        break;
    }
    case 'l': {
        if (StartsWith(line, "l ") || StartsWith(line, "l\t")) {
            line.remove_prefix(2);
            auto [count, rc] = ParseFace(
                line,
                chunk->positions.count,
                chunk->texcoords.count,
                chunk->normals.count,
                kMinVerticesInLine,
                kMaxVerticesInLine,
                static_cast<OffsetFlags>(ApplyOffset::Position | ApplyOffset::Texcoord),
                &chunk->lines.indices.buffer,
                &chunk->lines.indices.flags);
            if (rc != rapidobj_errc::Success) {
                return rc;
            }
            chunk->lines.segments.buffer.ensure_enough_room_for(1);
            chunk->lines.segments.buffer.push_back(static_cast<unsigned char>(count));
            ++chunk->lines.segments.count;
        }
        break;
    }
    case 'p': {
        if (StartsWith(line, "p ") || StartsWith(line, "p\t")) {
            line.remove_prefix(2);
            auto [count, rc] = ParseFace(
                line,
                chunk->positions.count,
                chunk->texcoords.count,
                chunk->normals.count,
                kMinVerticesInPoint,
                kMaxVerticesInPoint,
                static_cast<OffsetFlags>(ApplyOffset::Position),
                &chunk->points.indices.buffer,
                &chunk->points.indices.flags);
            if (rc != rapidobj_errc::Success) {
                return rc;
            }
        }
        break;
    }
    default: {
        return rapidobj_errc::ParseError;
    }
    }

    return rapidobj_errc::Success;
}

inline void ProcessBlocksImpl(
    Reader*        reader,
    size_t         block_begin,
    size_t         block_end,
    bool           stop_parsing_after_eol,
    Chunk*         chunk,
    SharedContext* context)
{
    assert(reader);

    bool begin_parsing_after_eol = block_begin > 0;
    bool reached_eof             = false;

    auto buffer_size = kMaxLineLength + kBlockSize;
    auto buffer1     = std::unique_ptr<char, sys::AlignedDeleter>(sys::AlignedAllocate(buffer_size, 4_KiB));
    auto buffer2     = std::unique_ptr<char, sys::AlignedDeleter>(sys::AlignedAllocate(buffer_size, 4_KiB));

    auto front_buffer = buffer1.get();
    auto back_buffer  = buffer2.get();

    auto file_offset = block_begin * kBlockSize;

    auto line = std::string_view();
    auto text = std::string_view();

    if (auto ec = reader->ReadBlock(file_offset, kBlockSize, front_buffer + kMaxLineLength)) {
        chunk->error = Error{ ec };
        return;
    }

    if (auto [bytes_read, ec] = reader->WaitForResult(); ec) {
        chunk->error = Error{ ec };
        return;
    } else {
        reached_eof = bytes_read < kBlockSize;
        text        = std::string_view(front_buffer + kMaxLineLength, bytes_read);
    }

    if (begin_parsing_after_eol) {
        if (auto ptr = static_cast<const char*>(memchr(text.data(), '\n', kMaxLineLength))) {
            auto pos = static_cast<size_t>(ptr - text.data());
            text.remove_prefix(pos + 1);
        } else {
            ++chunk->text.line_count;
            auto ec      = make_error_code(rapidobj_errc::LineTooLongError);
            chunk->error = Error{ ec, std::string(text, 0, kMaxLineLength), chunk->text.line_count };
            return;
        }
    }

    for (size_t i = block_begin; i != block_end; ++i) {
        auto remainder = size_t{};

        bool last_block = (i + 1 == block_end) || reached_eof;

        if (!last_block) {
            file_offset = (i + 1) * kBlockSize;

            if (auto ec = reader->ReadBlock(file_offset, kBlockSize, back_buffer + kMaxLineLength)) {
                chunk->error = Error{ ec };
                return;
            }

        } else if (stop_parsing_after_eol) {
            if (auto ptr = static_cast<const char*>(memchr(text.data(), '\n', kMaxLineLength))) {
                auto pos = static_cast<size_t>(ptr - text.data());
                line     = text.substr(0, pos);
                if (EndsWith(line, '\r')) {
                    line.remove_suffix(1);
                }
                ++chunk->text.line_count;
                if (auto rc = ProcessLine(line, chunk, context); rc != rapidobj_errc::Success) {
                    chunk->error = Error{ make_error_code(rc), std::string(line), chunk->text.line_count };
                }
            } else {
                ++chunk->text.line_count;
                auto ec      = make_error_code(rapidobj_errc::LineTooLongError);
                chunk->error = Error{ ec, std::string(text, 0, kMaxLineLength), chunk->text.line_count };
            }
            return;
        }

        while (!text.empty()) {
            if (auto ptr = static_cast<const char*>(memchr(text.data(), '\n', text.size()))) {
                auto pos = static_cast<size_t>(ptr - text.data());
                ++chunk->text.line_count;
                if (pos > kMaxLineLength) {
                    auto ec      = make_error_code(rapidobj_errc::LineTooLongError);
                    chunk->error = Error{ ec, std::string(text, 0, kMaxLineLength), chunk->text.line_count };
                    return;
                }
                line = text.substr(0, pos);
                if (EndsWith(line, '\r')) {
                    line.remove_suffix(1);
                }
                text.remove_prefix(pos + 1);
            } else {
                if (text.size() > kMaxLineLength) {
                    ++chunk->text.line_count;
                    auto ec      = make_error_code(rapidobj_errc::LineTooLongError);
                    chunk->error = Error{ ec, std::string(text, 0, kMaxLineLength), chunk->text.line_count };
                    return;
                }
                if (last_block) {
                    line = text;
                    ++chunk->text.line_count;
                    if (auto rc = ProcessLine(line, chunk, context); rc != rapidobj_errc::Success) {
                        chunk->error = Error{ make_error_code(rc), std::string(line), chunk->text.line_count };
                        return;
                    }
                } else {
                    remainder = text.size();
                    memcpy(back_buffer + kMaxLineLength - remainder, text.data(), remainder);
                }
                text = {};
                break;
            }

            if (auto rc = ProcessLine(line, chunk, context); rc != rapidobj_errc::Success) {
                chunk->error = Error{ make_error_code(rc), std::string(line), chunk->text.line_count };
                return;
            }
        }

        if (!last_block) {
            auto [bytes_read, ec] = reader->WaitForResult();
            if (ec) {
                chunk->error = Error{ ec };
                return;
            }
            reached_eof = bytes_read < kBlockSize;
            std::swap(front_buffer, back_buffer);
            text = std::string_view(front_buffer + kMaxLineLength - remainder, bytes_read + remainder);
        } else if (reached_eof) {
            break;
        }
    }
}

inline void ProcessBlocks(
    DataSource                     source,
    size_t                         thread_index,
    size_t                         block_begin,
    size_t                         block_end,
    bool                           stop_parsing_after_eol,
    Chunk*                         chunk,
    std::shared_ptr<SharedContext> context)
{
    assert(chunk);
    assert(context);
    assert(block_begin < block_end);

    auto t1 = std::chrono::steady_clock::now();

    auto reader = CreateReader(source);

    if (reader->Error()) {
        chunk->error = Error{ reader->Error() };
    } else {
        ProcessBlocksImpl(reader.get(), block_begin, block_end, stop_parsing_after_eol, chunk, context.get());
    }

    if (1 == std::atomic_fetch_sub(&context->parsing.thread_count, size_t(1))) {
        context->parsing.completed.set_value();
    }

    auto t2 = std::chrono::steady_clock::now();

    auto parse_time = t2 - t1;

    context->debug.io.num_requests[thread_index]  = reader->NumRequests();
    context->debug.io.num_bytes_read[thread_index] = reader->BytesRead();
    context->debug.io.submit_time[thread_index] = reader->SubmitTime();
    context->debug.io.wait_time[thread_index]   = reader->WaitTime();
    context->debug.parse.time[thread_index]     = parse_time;
}

inline void ParseFileSequential(sys::File* file, std::vector<Chunk>* chunks, std::shared_ptr<SharedContext> context)
{
    context->thread.concurrency   = 1;
    context->parsing.thread_count = 1;

    chunks->resize(1);

    context->debug.io.num_requests.resize(1);
    context->debug.io.num_bytes_read.resize(1);
    context->debug.io.submit_time.resize(1);
    context->debug.io.wait_time.resize(1);
    context->debug.parse.time.resize(1);

    auto source = DataSource(file);

    auto num_blocks             = file->size() / kBlockSize + (file->size() % kBlockSize != 0);
    auto stop_parsing_after_eol = false;
    auto chunk                  = &chunks->front();

    ProcessBlocks(source, 0, 0, num_blocks, stop_parsing_after_eol, chunk, context);
}

inline void ParseFileParallel(sys::File* file, std::vector<Chunk>* chunks, std::shared_ptr<SharedContext> context)
{
    auto source                = DataSource(file);
    auto num_blocks            = file->size() / kBlockSize + (file->size() % kBlockSize != 0);
    auto num_threads           = std::thread::hardware_concurrency();
    auto num_blocks_per_thread = num_blocks / num_threads;
    auto num_remainder_blocks  = num_blocks - (num_blocks_per_thread * num_threads);

    auto tasks = std::vector<size_t>();

    tasks.reserve(num_threads);

    // allocate blocks to tasks
    {
        auto block = size_t{};

        while (block < num_blocks) {
            tasks.push_back(block);
            block += num_blocks_per_thread;
            if (num_remainder_blocks > 0) {
                ++block;
                --num_remainder_blocks;
            }
        }
    }

    auto num_tasks = tasks.size();
    auto threads   = std::vector<std::thread>{};

    chunks->resize(num_tasks);

    context->thread.concurrency   = num_threads;
    context->parsing.thread_count = num_tasks;

    context->debug.io.num_requests.resize(num_threads);
    context->debug.io.num_bytes_read.resize(num_threads);
    context->debug.io.submit_time.resize(num_threads);
    context->debug.io.wait_time.resize(num_threads);
    context->debug.parse.time.resize(num_threads);

    threads.reserve(num_tasks);

    // allocate tasks to threads
    for (size_t i = 0; i != tasks.size(); ++i) {
        bool is_last                = i + 1 == tasks.size();
        auto begin                  = tasks[i];
        auto end                    = is_last ? num_blocks : (tasks[i + 1] + 1);
        bool stop_parsing_after_eol = !is_last;
        auto chunk                  = &(*chunks)[i];

        threads.emplace_back(ProcessBlocks, source, i, begin, end, stop_parsing_after_eol, chunk, context);
        threads.back().detach();
    }

    // wait for parsing to finish
    context->parsing.completed.get_future().wait();
}

inline Result ParseFile(const std::filesystem::path& filepath, const MaterialLibrary& material_library)
{
    if (filepath.empty()) {
        auto error = std::make_error_code(std::errc::invalid_argument);
        return Result{ Attributes{}, Shapes{}, Materials{}, Error{ error } };
    }

    auto file = sys::File(filepath);

    if (!file) {
        return Result{ Attributes{}, Shapes{}, Materials{}, Error{ file.error() } };
    }

    const auto material_library_value   = &material_library.Value();
    const auto default_loading_policy   = material_library.Policy().value_or(Load::Mandatory);
    const auto default_material_library = MaterialLibrary::SearchPath(".", default_loading_policy);

    auto context = std::make_shared<SharedContext>();

    context->material.basepath = filepath.parent_path();

    if (std::get_if<std::nullptr_t>(material_library_value) != nullptr) {
        context->material.library = nullptr;
    } else if (std::get_if<std::monostate>(material_library_value) != nullptr) {
        context->material.library = &default_material_library;
    } else if (auto* paths = std::get_if<std::vector<std::filesystem::path>>(material_library_value)) {
        if (paths->empty()) {
            return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::InvalidArgumentsError } };
        }
        context->material.library = &material_library;
    } else if (std::get_if<std::string_view>(material_library_value) != nullptr) {
        context->material.library = &material_library;
    } else {
        return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::InternalError } };
    }

    auto chunks = std::vector<Chunk>();

    auto t1 = std::chrono::steady_clock::now();

    if (file.size() <= kSingleThreadCutoff) {
        ParseFileSequential(&file, &chunks, context);
    } else {
        ParseFileParallel(&file, &chunks, context);
    }

    auto t2 = std::chrono::steady_clock::now();

    context->debug.parse.total_time = t2 - t1;

    // check if an error occured
    size_t running_line_num = size_t{};
    for (auto& chunk : chunks) {
        if (chunk.error.code) {
            chunk.error.line_num += running_line_num;
            return Result{ Attributes{}, Shapes{}, Materials{}, chunk.error };
        }
        running_line_num += chunk.text.line_count;
    }

    t1 = std::chrono::steady_clock::now();

    auto result = Merge(chunks, context);

    t2 = std::chrono::steady_clock::now();

    context->debug.merge.total_time = t2 - t1;

    // std::cout << DumpDebug(*context);

    auto memory = size_t{ 0 };

    for (const auto& chunk : chunks) {
        memory += SizeInBytes(chunk);
    }

    // Free memory in a different thread
    if (memory > kMemoryRecyclingSize) {
        auto recycle = std::thread([](std::vector<Chunk>&&) {}, std::move(chunks));
        recycle.detach();
    }

    return result;
}

inline Result ParseStream(std::istream& is, const MaterialLibrary& material_library)
{
    auto context                  = std::make_shared<SharedContext>();
    auto chunks                   = std::vector<Chunk>(1);
    auto source                   = DataSource(&is);
    auto num_blocks               = std::numeric_limits<size_t>::max();
    auto stop_parsing_after_eol   = false;
    auto chunk                    = &chunks.front();
    auto material_library_value   = &material_library.Value();
    auto default_material_library = MaterialLibrary::SearchPaths({}, Load::Optional);

    if (std::get_if<std::nullptr_t>(material_library_value) != nullptr) {
        context->material.library = nullptr;
    } else if (std::get_if<std::monostate>(material_library_value) != nullptr) {
        if (material_library.Policy()) {
            return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::InvalidArgumentsError } };
        }
        context->material.library = &default_material_library;
    } else if (auto* paths = std::get_if<std::vector<std::filesystem::path>>(material_library_value)) {
        if (paths->empty()) {
            return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::InvalidArgumentsError } };
        }
        if (std::any_of(paths->begin(), paths->end(), [](auto& path) { return path.is_relative(); })) {
            return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::MaterialRelativePathError } };
        }
        context->material.library = &material_library;
    } else if (std::get_if<std::string_view>(material_library_value) != nullptr) {
        context->material.library = &material_library;
    } else {
        return Result{ Attributes{}, Shapes{}, Materials{}, Error{ rapidobj_errc::InternalError } };
    }

    context->thread.concurrency   = 1;
    context->parsing.thread_count = 1;

    context->debug.io.num_requests.resize(1);
    context->debug.io.num_bytes_read.resize(1);
    context->debug.io.submit_time.resize(1);
    context->debug.io.wait_time.resize(1);
    context->debug.parse.time.resize(1);

    auto t1 = std::chrono::steady_clock::now();

    ProcessBlocks(source, 0, 0, num_blocks, stop_parsing_after_eol, chunk, context);

    auto t2 = std::chrono::steady_clock::now();

    context->debug.parse.total_time = t2 - t1;

    if (chunks.front().error.code) {
        return Result{ Attributes{}, Shapes{}, Materials{}, chunks.front().error };
    }

    t1 = std::chrono::steady_clock::now();

    auto result = Merge(chunks, context);

    t2 = std::chrono::steady_clock::now();

    context->debug.merge.total_time = t2 - t1;

    // std::cout << DumpDebug(*context);

    // Free memory in a different thread
    if (SizeInBytes(chunks.front()) > kMemoryRecyclingSize) {
        auto recycle = std::thread([](std::vector<Chunk>&&) {}, std::move(chunks));
        recycle.detach();
    }

    return result;
}

struct TriangulateTask final {
    TriangulateTask(
        const Mesh* src_,
        Mesh*       dst_,
        size_t      cost_,
        size_t      isrc_,
        size_t      idst_,
        size_t      fsrc_,
        size_t      fdst_,
        size_t      size_) noexcept
        : src(src_), dst(dst_), cost(cost_), isrc(isrc_), idst(idst_), fsrc(fsrc_), fdst(fdst_), size(size_)
    {}

    const Mesh* src{};
    Mesh*       dst{};
    size_t      cost{};
    size_t      isrc{};
    size_t      idst{};
    size_t      fsrc{};
    size_t      fdst{};
    size_t      size{};
};

inline auto CalculatePolygonArea(float* x, float* y, size_t size) noexcept
{
    auto area = 0.0f;

    for (size_t i = 1; i != size; ++i) {
        auto avg_height = (y[i - 1] + y[i]) / 2;
        auto width      = x[i] - x[i - 1];
        area += width * avg_height;
    }

    auto avg_height = (y[0] + y[size - 1]) / 2;
    auto width      = x[0] - x[size - 1];
    area += width * avg_height;

    return std::abs(area);
}

enum class ProjectionPlane { X, Y, Z };

inline bool TriangulateSingleTask(const Array<float>& positions, const TriangulateTask& task)
{
    auto [src, dst, cost, isrc, idst, fsrc, fdst, size] = task;

    auto  complex   = std::array<std::vector<std::array<float, 2>>, 1>();
    auto& polygon   = complex.front();
    auto  index_map = std::vector<size_t>();

    for (size_t i = 0; i != size; ++i) {
        auto num_vertices = src->num_face_vertices[fsrc + i];
        auto material_id  = src->material_ids[fsrc + i];
        auto smoothing_id = src->smoothing_group_ids[fsrc + i];

        if (num_vertices == 3) {
            dst->indices[idst + 0] = src->indices[isrc + 0];
            dst->indices[idst + 1] = src->indices[isrc + 1];
            dst->indices[idst + 2] = src->indices[isrc + 2];

            isrc += 3;
            idst += 3;

            dst->num_face_vertices[fdst]   = 3;
            dst->material_ids[fdst]        = material_id;
            dst->smoothing_group_ids[fdst] = smoothing_id;

            fdst++;

        } else if (num_vertices == 4) {
            const auto& index0 = src->indices[isrc + 0];
            const auto& index1 = src->indices[isrc + 1];
            const auto& index2 = src->indices[isrc + 2];
            const auto& index3 = src->indices[isrc + 3];

            isrc += 4;

            bool d02_is_less{};
            {
                auto position_index0 = 3 * static_cast<size_t>(index0.position_index);
                auto position_index1 = 3 * static_cast<size_t>(index1.position_index);
                auto position_index2 = 3 * static_cast<size_t>(index2.position_index);
                auto position_index3 = 3 * static_cast<size_t>(index3.position_index);

                auto pos0_x = positions[position_index0 + 0];
                auto pos0_y = positions[position_index0 + 1];
                auto pos0_z = positions[position_index0 + 2];

                auto pos1_x = positions[position_index1 + 0];
                auto pos1_y = positions[position_index1 + 1];
                auto pos1_z = positions[position_index1 + 2];

                auto pos2_x = positions[position_index2 + 0];
                auto pos2_y = positions[position_index2 + 1];
                auto pos2_z = positions[position_index2 + 2];

                auto pos3_x = positions[position_index3 + 0];
                auto pos3_y = positions[position_index3 + 1];
                auto pos3_z = positions[position_index3 + 2];

                auto e02_x = pos0_x - pos2_x;
                auto e02_y = pos0_y - pos2_y;
                auto e02_z = pos0_z - pos2_z;

                auto e13_x = pos1_x - pos3_x;
                auto e13_y = pos1_y - pos3_y;
                auto e13_z = pos1_z - pos3_z;

                auto d02 = e02_x * e02_x + e02_y * e02_y + e02_z * e02_z;
                auto d13 = e13_x * e13_x + e13_y * e13_y + e13_z * e13_z;

                d02_is_less = d02 < d13;
            }

            dst->indices[idst + 0] = index0;
            dst->indices[idst + 1] = index1;
            dst->indices[idst + 2] = d02_is_less ? index2 : index3;
            dst->indices[idst + 3] = d02_is_less ? index0 : index1;
            dst->indices[idst + 4] = index2;
            dst->indices[idst + 5] = index3;

            idst += 6;

            dst->num_face_vertices[fdst + 0]   = 3;
            dst->num_face_vertices[fdst + 1]   = 3;
            dst->material_ids[fdst + 0]        = material_id;
            dst->material_ids[fdst + 1]        = material_id;
            dst->smoothing_group_ids[fdst + 0] = smoothing_id;
            dst->smoothing_group_ids[fdst + 1] = smoothing_id;

            fdst += 2;
        } else {
            auto xs = std::array<float, kMaxVerticesInFace>();
            auto ys = std::array<float, kMaxVerticesInFace>();
            auto zs = std::array<float, kMaxVerticesInFace>();

            polygon.clear();
            index_map.clear();

            for (size_t k = 0; k != num_vertices; ++k) {
                auto position_index = 3 * static_cast<size_t>(src->indices[isrc + k].position_index);
                index_map.push_back(isrc + k);
                xs[k] = positions[position_index + 0];
                ys[k] = positions[position_index + 1];
                zs[k] = positions[position_index + 2];
            }

            isrc += num_vertices;

            auto area_x = CalculatePolygonArea(ys.data(), zs.data(), num_vertices);
            auto area_y = CalculatePolygonArea(xs.data(), zs.data(), num_vertices);
            auto area_z = CalculatePolygonArea(xs.data(), ys.data(), num_vertices);

            if (FLT_MIN > std::max({ area_x, area_y, area_z })) {
                return false;
            }

            auto proj_plane = ProjectionPlane{};

            if (area_x > area_y) {
                proj_plane = area_x > area_z ? ProjectionPlane::X : ProjectionPlane::Z;
            } else {
                proj_plane = area_y > area_z ? ProjectionPlane::Y : ProjectionPlane::Z;
            }

            if (proj_plane == ProjectionPlane::X) {
                for (size_t k = 0; k != num_vertices; ++k) {
                    polygon.push_back({ ys[k], zs[k] });
                }
            } else if (proj_plane == ProjectionPlane::Y) {
                for (size_t k = 0; k != num_vertices; ++k) {
                    polygon.push_back({ xs[k], zs[k] });
                }
            } else {
                for (size_t k = 0; k != num_vertices; ++k) {
                    polygon.push_back({ xs[k], ys[k] });
                }
            }

            auto result = mapbox::earcut(complex);

            if (result.empty() || result.size() % 3 != 0) {
                return false;
            }

            for (size_t k = 0; k < result.size(); k += 3) {
                std::swap(result[k], result[k + 1]);
            }

            for (size_t k = 0; k != result.size(); ++k) {
                dst->indices[idst + k] = src->indices[index_map[result[k]]];
            }

            idst += result.size();

            auto num_triangles = result.size() / 3;

            for (size_t k = 0; k != num_triangles; ++k) {
                dst->num_face_vertices[fdst + k]   = 3;
                dst->material_ids[fdst + k]        = material_id;
                dst->smoothing_group_ids[fdst + k] = smoothing_id;
            }

            fdst += num_triangles;
        }
    }
    return true;
}

inline bool
TriangulateTasksParallel(size_t concurrency, const Array<float>& positions, const std::vector<TriangulateTask>& tasks)
{
    auto task_index  = std::atomic_size_t{ 0 };
    auto num_threads = std::atomic_size_t{ concurrency };
    auto completed   = std::promise<void>();
    auto success     = std::atomic_bool{ true };

    auto func = [&]() {
        auto fetched_index = std::atomic_fetch_add(&task_index, size_t(1));

        while (fetched_index < tasks.size()) {
            if (false == TriangulateSingleTask(positions, tasks[fetched_index])) {
                success = false;
                break;
            }
            fetched_index = std::atomic_fetch_add(&task_index, size_t(1));
        }

        if (1 == std::atomic_fetch_sub(&num_threads, size_t(1))) {
            completed.set_value();
        }
    };

    auto threads = std::vector<std::thread>();
    threads.reserve(concurrency);

    for (size_t i = 0; i != concurrency; ++i) {
        threads.emplace_back(func);
        threads.back().detach();
    }

    // wait for triangulation to finish
    completed.get_future().wait();

    return success;
}

inline bool TriangulateTasksSequential(const Array<float>& positions, const std::vector<TriangulateTask>& tasks)
{
    for (const auto& task : tasks) {
        bool success = TriangulateSingleTask(positions, task);
        if (!success) {
            return false;
        }
    }
    return true;
}

inline bool Triangulate(Result& result)
{
    auto mesh_tasks = std::vector<TriangulateTask>();
    auto tasks      = std::vector<TriangulateTask>();
    auto meshes     = std::vector<Mesh>(result.shapes.size());

    tasks.reserve(result.shapes.size());

    for (size_t i = 0; i != result.shapes.size(); ++i) {
        const auto& mesh = result.shapes[i].mesh;

        mesh_tasks.clear();

        auto cost = size_t{ 0 };

        auto isrc_begin = size_t{ 0 };
        auto isrc_end   = size_t{ 0 };
        auto idst_begin = size_t{ 0 };
        auto idst_end   = size_t{ 0 };

        auto fsrc_begin = size_t{ 0 };
        auto fsrc_end   = size_t{ 0 };
        auto fdst_begin = size_t{ 0 };
        auto fdst_end   = size_t{ 0 };

        auto triangle_sum = size_t{ 0 };

        for (auto num_vertices : mesh.num_face_vertices) {
            auto num_triangles = static_cast<size_t>(num_vertices - 2);

            triangle_sum += num_triangles;

            if (num_vertices == 3) {
                cost += kTriangulateTriangleCost;
            } else if (num_vertices == 4) {
                cost += kTriangulateQuadCost;
            } else {
                auto n = static_cast<size_t>(num_vertices);
                cost += kTriangulatePerIndexCost * n * n;
            }
            if (cost >= kTriangulateSubdivideCost) {
                auto size = fsrc_end - fsrc_begin;
                mesh_tasks.emplace_back(&mesh, nullptr, cost, isrc_begin, idst_begin, fsrc_begin, fdst_begin, size);

                isrc_begin = isrc_end;
                idst_begin = idst_end;
                fsrc_begin = fsrc_end;
                fdst_begin = fdst_end;

                cost = 0;
            }

            isrc_end += num_vertices;
            idst_end += 3 * num_triangles;
            fsrc_end += 1;
            fdst_end += num_triangles;
        }

        if (triangle_sum == mesh.num_face_vertices.size()) {
            continue;
        }

        if (auto size = fsrc_end - fsrc_begin; size > 0) {
            mesh_tasks.emplace_back(&mesh, nullptr, cost, isrc_begin, idst_begin, fsrc_begin, fdst_begin, size);
        }

        meshes[i].indices             = Array<Index>(3 * triangle_sum);
        meshes[i].num_face_vertices   = Array<uint8_t>(triangle_sum);
        meshes[i].material_ids        = Array<int32_t>(triangle_sum);
        meshes[i].smoothing_group_ids = Array<uint32_t>(triangle_sum);

        for (auto& task : mesh_tasks) {
            task.dst = &meshes[i];
        }

        tasks.insert(tasks.end(), mesh_tasks.begin(), mesh_tasks.end());
    }

    // already triangulated
    if (tasks.empty()) {
        return true;
    }

    auto hardware_threads = static_cast<size_t>(std::thread::hardware_concurrency());
    auto concurrency      = std::min(hardware_threads, tasks.size());
    bool success          = true;

    if (concurrency > 1) {
        success = TriangulateTasksParallel(concurrency, result.attributes.positions, tasks);
    } else {
        success = TriangulateTasksSequential(result.attributes.positions, tasks);
    }

    if (!success) {
        result.error = Error{ make_error_code(rapidobj_errc::TriangulationError), {}, {} };
    }

    auto old_meshes = std::vector<Mesh>();
    old_meshes.reserve(result.shapes.size());

    for (size_t i = 0; i != result.shapes.size(); ++i) {
        if (!meshes[i].indices.empty()) {
            old_meshes.push_back(std::move(result.shapes[i].mesh));
            result.shapes[i].mesh = std::move(meshes[i]);
        }
    }

    auto memory = size_t{ 0 };

    for (const auto& old_mesh : old_meshes) {
        memory += SizeInBytes(old_mesh);
    }

    // Free memory in a different thread
    if (memory > kMemoryRecyclingSize) {
        auto recycle = std::thread([](std::vector<Mesh>&&) {}, std::move(old_meshes));
        recycle.detach();
    }

    return success;
}

} // namespace detail

/// <summary>
/// Loads and parses Wavefront geometry definition file (.obj file).
/// </summary>
/// <param name="obj_filepath"> : path of the .obj file to parse.</param>
/// <param name="mtl_library"> : optional material library.</param>
/// <returns>Parsed data stored in Result class.</returns>
inline Result ParseFile(const std::filesystem::path& obj_filepath, const MaterialLibrary& mtl_library)
{
    return detail::ParseFile(obj_filepath, mtl_library);
}

/// <summary>
/// Loads and parses Wavefront geometry definition data from an input stream.
/// Because input streams are sequential, which prevents parsing parallelization,
/// this function is less performant compared to the ParseFile() function.
/// </summary>
/// <param name="obj_stream"> : input stream to parse.</param>
/// <param name="mtl_library"> : optional material library.</param>
/// <returns>Parsed data stored in Result class.</returns>
inline Result ParseStream(std::istream& obj_stream, const MaterialLibrary& mtl_library)
{
    return detail::ParseStream(obj_stream, mtl_library);
}

inline bool Triangulate(Result& result)
{
    return detail::Triangulate(result);
}

} // namespace rapidobj

#endif
