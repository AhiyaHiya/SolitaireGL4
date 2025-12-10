#ifndef _GAME_CARDS_HPP__
#define _GAME_CARDS_HPP__

#include "types.hpp"

#include <nlohmann/json.hpp>
#include <stb_image.h>

// clang-format off
#include <glad/gl.h>
// clang-format on

#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>

// For holding image data from the large PNG tile map
class asset_image
{
public:
    asset_image(std::shared_ptr<stbi_uc> data,
                std::int32_t             width,
                std::int32_t             height,
                std::int32_t             channels)
        : data_(data)
        , width_(width)
        , height_(height)
        , channels_(channels)
    {
    }

    auto data() const -> std::shared_ptr<stbi_uc> { return data_; }
    auto width() const -> std::int32_t { return width_; }
    auto height() const -> std::int32_t { return height_; }
    auto channels() const -> std::int32_t { return channels_; }

    auto stride_of_data_at(const std::uint32_t x, const std::uint32_t y) const -> stbi_uc*
    {
        return data_.get() + (y * width_ * channels_) + (x * channels_);
    }

private:
    std::shared_ptr<stbi_uc> data_;
    std::int32_t             width_;
    std::int32_t             height_;
    std::int32_t             channels_;
};

// For holding the position of the card to draw
struct card
{
    float        x, y;  // Position in pixel coordinates (bottom-left origin)
    std::int32_t index; // 0-51 for face, 52 for back
    bool         face_up;
};

// For holding ids of assets/shaders/etc.
struct card_renderer
{
    GLuint shader_program = 0;
    GLuint texture_array  = 0;
    GLuint vao            = 0;
    GLuint vbo            = 0;

    // The variables here match what is in the shader code
    GLint uProjection = -1;
    GLint uPosition   = -1;
    GLint uSize       = -1;
    GLint uCardIndex  = -1;
    GLint uFaceUp     = -1;
    GLint uCardTextures = -1;
};

// -------------------- FUNCTIONS SECTION ---------------------

[[nodiscard]]
auto create_vao_vbo() -> std::pair<GLuint, GLuint>;

// Returns shader id or error message
auto compile_card_shader(std::string_view shader_relative_path, GLenum shader_type)
    -> std::expected<GLuint, error_message_t>;

auto create_card_renderer() -> std::expected<std::shared_ptr<card_renderer>, error_message_t>;

void draw_cards(const std::shared_ptr<card_renderer>& cr);

auto link_shader_program(GLuint vertex_shader_id, GLuint fragment_shader_id)
    -> std::expected<GLuint, error_message_t>;

// True if loading card textures succeeded, false otherwise
auto load_card_textures() -> std::expected<GLuint, error_message_t>;

auto load_json_data() -> std::expected<nlohmann::json, error_message_t>;
auto load_png_data() -> std::expected<std::shared_ptr<asset_image>, error_message_t>;

// For reading shader code
auto read_file_content(const std::filesystem::path& path)
    -> std::expected<std::string, error_message_t>;
#endif // _GAME_CARDS_HPP__
