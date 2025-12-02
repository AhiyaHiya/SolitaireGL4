#pragma once

#include <nlohmann/json.hpp>
#include <stb_image.h>

#include <cstdint>
#include <expected>
#include <memory>

class asset_image
{
public:
    asset_image(std::shared_ptr<stbi_uc> data,
        std::int32_t width, std::int32_t height, std::int32_t channels)
        : data_(data), width_(width), height_(height), channels_(channels) {}

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
    std::int32_t width_;
    std::int32_t height_;
    std::int32_t channels_;
};

// True if loading card textures succeeded, false otherwise
bool load_card_textures();

auto load_json_data() -> std::expected<nlohmann::json, std::string>;
auto load_png_data() -> std::expected<std::shared_ptr<asset_image>, std::string>;
