#include "cards.hpp"

#include <gtest/gtest.h>

TEST(CardsTest, LoadJsonData)
{
    load_json_data();
}

TEST(CardsTest, LoadPngData)
{
    auto asset_image_result = load_png_data();
    ASSERT_TRUE(asset_image_result.has_value()) << "Failed to load PNG data: " << asset_image_result.error();

    auto asset_image = asset_image_result.value();
    ASSERT_NE(asset_image, nullptr);
    ASSERT_GT(asset_image->width(), 0);
    ASSERT_GT(asset_image->height(), 0);
    ASSERT_GT(asset_image->channels(), 0);
    ASSERT_NE(asset_image->data(), nullptr);
}