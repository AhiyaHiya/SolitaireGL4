#include "cards.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <filesystem>
#include <fstream>
#include <ranges>

// TODO: Clean up implementation
auto read_file_content(const std::filesystem::path& path) -> std::expected<std::string, error_message_t>
{
    // Make sure the file exists
    if (!std::filesystem::exists(path))
    {
        return std::unexpected("File does not exist: " + path.string());
    }

    // Attempt to open the file
    auto file_stream = std::ifstream(path, std::ios::in);
    if (!file_stream.is_open())
    {
        return std::unexpected("Failed to open file: " + path.string());
    }
    // Seek to the end to get the size
    file_stream.seekg(0, std::ios::end);

    // Set up a new string object to hold the content
    const auto size    = file_stream.tellg();
    auto       content = std::string(size, '\0');

    // Bring the reader back to the beginning of the file and read the content
    file_stream.seekg(0);
    file_stream.read(content.data(), size);
    return content;
}

auto compile_card_shader(std::string_view shader_relative_path, GLenum shader_type) -> std::expected<GLuint, error_message_t> // shader id
{
    auto current_working_path = std::filesystem::current_path();
    auto shader_path          = current_working_path / shader_relative_path;

    auto content_result = read_file_content(shader_path);
    if (!content_result.has_value())
    {
        return std::unexpected("Failed to read shader file: " + std::string(shader_relative_path));
    }
    auto shader_source = content_result.value();

    auto           shader_id      = glCreateShader(shader_type);
    constexpr auto shader_count   = 1;
    const GLchar*  shader_src_ptr = shader_source.c_str();
    glShaderSource(shader_id, shader_count, &shader_src_ptr, nullptr);
    glCompileShader(shader_id);

    return shader_id;
}

bool load_card_textures()
{
    // --------------------------- Load the card texture atlas ---------------------------
    // Actual width, height, and channels will be set by stbi_load
    auto asset_image_result = load_png_data();
    if (!asset_image_result.has_value())
    {
        // TODO: convert to spdlog logging
        printf("Failed to load card textures: %s\n", asset_image_result.error().c_str());
        return false;
    }

    auto asset_image = asset_image_result.value();

    // --------------------------- Load JSON data ---------------------------
    // Load up JSON data and try to create textures
    auto json_data_result = load_json_data();
    if (!json_data_result.has_value())
    {
        printf("Failed to load JSON data: %s\n", json_data_result.error().c_str());
        return false;
    }
    auto json_data = json_data_result.value();

    // --------------------------- Create OpenGL texture array ---------------------------
    auto           card_texture_array = GLuint();
    constexpr auto texture_count      = GLsizei(1);
    glGenTextures(texture_count, &card_texture_array);
    glBindTexture(GL_TEXTURE_2D_ARRAY, card_texture_array);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Go through each card entry in the JSON and extract the texture coordinates
    for (const auto& [index, entry] : std::views::enumerate(json_data["frames"].items()))
    {
        const auto& [card_name, frame_data] = entry;

        const auto x = frame_data["x"].get<int>();
        const auto y = frame_data["y"].get<int>();
        const auto w = frame_data["w"].get<int>();
        const auto h = frame_data["h"].get<int>();

        // Load texture
        auto pixel_data = asset_image->stride_of_data_at(x, y);
        glTexImage2D(GL_TEXTURE_2D, index, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
    }

    return true;
}

auto load_json_data() -> std::expected<nlohmann::json, error_message_t>
{
    // Load JSON data from file
    auto current_working_path = std::filesystem::current_path();
    auto cards_path           = current_working_path / "Assets/cards.json";

    auto json_file_stream = std::ifstream(cards_path);
    try
    {
        return nlohmann::json::parse(json_file_stream); // This could throw
    }
    catch (const std::exception& e)
    {
        return std::unexpected(std::string("Failed to parse JSON data: ") + e.what());
    }
}

auto load_png_data() -> std::expected<std::shared_ptr<asset_image>, error_message_t>
{
    // Set up the asset path
    auto current_working_path = std::filesystem::current_path();
    auto cards_path           = current_working_path / "Assets/cards.png";
    if (!std::filesystem::exists(cards_path))
    {
        return std::unexpected("Cards PNG file not found at path: " + cards_path.string());
    }

    // Load the png data; width, height, and channels will be set by stbi_load
    auto  width = 0, height = 0, channels = 0;
    auto* raw_data = stbi_load(cards_path.string().c_str(), &width, &height, &channels, 0);
    if (raw_data == nullptr || width == 0 || height == 0 || channels == 0)
    {
        return std::unexpected("Failed to load PNG data from file: " + cards_path.string());
    }

    // Wrap the data and the image info in the asset_image object
    auto data_ptr = std::shared_ptr<stbi_uc>(raw_data, stbi_image_free);
    return std::make_shared<asset_image>(data_ptr, width, height, channels);
}
