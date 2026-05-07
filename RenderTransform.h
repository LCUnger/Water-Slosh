#pragma once

#include <SFML/Graphics.hpp>

#include <algorithm>

#include "Point2.h"

class RenderTransform
{
public:
    static RenderTransform fit_to_target(double domain_width_m, double domain_height_m, sf::Vector2u target_size)
    {
        const float target_width_px = static_cast<float>(target_size.x);
        const float target_height_px = static_cast<float>(target_size.y);
        const float pixels_per_meter = std::min(
            target_width_px / static_cast<float>(domain_width_m),
            target_height_px / static_cast<float>(domain_height_m)
        );

        const float domain_width_px = static_cast<float>(domain_width_m) * pixels_per_meter;
        const float domain_height_px = static_cast<float>(domain_height_m) * pixels_per_meter;
        const sf::Vector2f world_origin_screen_position{
            (target_width_px - domain_width_px) * 0.5f,
            target_height_px - (target_height_px - domain_height_px) * 0.5f
        };

        return RenderTransform{ pixels_per_meter, world_origin_screen_position };
    }

    RenderTransform(float pixels_per_meter, sf::Vector2f world_origin_screen_position)
        : pixels_per_meter_(pixels_per_meter),
          world_origin_screen_position_(world_origin_screen_position)
    {
    }

    sf::Vector2f world_to_screen(const Point2& world_position_m) const
    {
        return {
            world_origin_screen_position_.x + static_cast<float>(world_position_m[0]) * pixels_per_meter_,
            world_origin_screen_position_.y - static_cast<float>(world_position_m[1]) * pixels_per_meter_
        };
    }

    float meters_to_pixels(double meters) const
    {
        return static_cast<float>(meters) * pixels_per_meter_;
    }

    float pixels_per_meter() const { return pixels_per_meter_; }
    sf::Vector2f world_origin_screen_position() const { return world_origin_screen_position_; }

private:
    float pixels_per_meter_{};
    sf::Vector2f world_origin_screen_position_{};
};
