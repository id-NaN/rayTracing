#pragma once

#include <glm/glm.hpp>

#include "memory"

#include "Walnut/Image.h"


class Renderer
{
public:
    Renderer() = default;

    void on_resize(uint32_t width, uint32_t height);
    void render();

    std::shared_ptr<Walnut::Image> get_final_image() const { return final_image_; }
private:
    glm::vec4 per_pixel(glm::vec2 coordinate);
private:
    std::shared_ptr<Walnut::Image> final_image_;
    uint32_t* image_data_ = nullptr;
};
