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

// Returns shader id or error message
auto compile_card_shader(std::string_view shader_relative_path, GLenum shader_type) -> std::expected<GLuint, error_message_t>;

// True if loading card textures succeeded, false otherwise
bool load_card_textures();

auto load_json_data() -> std::expected<nlohmann::json, error_message_t>;
auto load_png_data() -> std::expected<std::shared_ptr<asset_image>, error_message_t>;

// For reading shader code
auto read_file_content(const std::filesystem::path& path)
    -> std::expected<std::string, error_message_t>;
#endif // _GAME_CARDS_HPP__
