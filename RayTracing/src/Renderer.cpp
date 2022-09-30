#include "cmath"

#include "Walnut/Random.h"

#include "Renderer.h"

namespace utils
{
    static uint32_t convertToRGBA(const glm::vec4& color)
    {
        const uint8_t red = static_cast<uint8_t>(color.r * 255.0f);
        const uint8_t green = static_cast<uint8_t>(color.g * 255.0f);
        const uint8_t blue = static_cast<uint8_t>(color.b * 255.0f);
        const uint8_t alpha = static_cast<uint8_t>(color.a * 255.0f);

        const uint32_t result = alpha << 24 | blue << 16 | green << 8 | red;
        return result;
    }
}

void Renderer::on_resize(uint32_t width, uint32_t height)
{
    if (final_image_)
    {
        if (final_image_->GetWidth() == width && final_image_->GetHeight() == height)
        {
            return;
        }
        final_image_->Resize(width, height);
    }
    else
    {
        final_image_ = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] image_data_;
    image_data_ = new uint32_t[static_cast<uint64_t>(width) * static_cast<uint64_t>(height)];
}

void Renderer::render()
{
    const float aspect_ratio =
        static_cast<float>(final_image_->GetHeight())
        / static_cast<float>(final_image_->GetWidth());
    const float viewport_width = 2;
    const float viewport_height = viewport_width * aspect_ratio;
    for (uint32_t y = 0; y < final_image_->GetHeight(); ++y)
    {
        for (uint32_t x = 0; x < final_image_->GetWidth(); x++)
        {
            glm::vec2 coordinate = {
                static_cast<float>(x) / static_cast<float>(final_image_->GetWidth())
                * viewport_width - viewport_width / 2,
                static_cast<float>(y) / static_cast<float>(final_image_->GetHeight())
                * viewport_height - viewport_height / 2
            };
            glm::vec4 color = per_pixel(coordinate);
            color = clamp(color, 0.0f, 1.0f);
            image_data_[x + y * final_image_->GetWidth()] = utils::convertToRGBA(color);
        }
    }
    final_image_->SetData(image_data_);
}

glm::vec4 Renderer::per_pixel(glm::vec2 coordinate)
{
    const glm::vec3 ray_origin(0, 0, 1);
    const glm::vec3 ray_direction(coordinate.x, coordinate.y, -1);
    // ray_direction = normalize(ray_direction);
    constexpr float radius = 0.5;

    const float a = dot(ray_direction, ray_direction);
    const float b = 2 * dot(ray_origin, ray_direction);
    const float c = dot(ray_origin, ray_origin) - radius * radius;

    const float discriminant = b * b - 4 * a * c;

    if (discriminant < 0.0f)
    {
        return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    const float t = (-b - std::sqrtf(discriminant)) / (2 * a);
    const glm::vec3 hit = ray_origin + ray_direction * t;
    const glm::vec3 normal = normalize(hit);

    const glm::vec3 lightDirection = normalize(glm::vec3(-1, -1, -1));
    float lightIntensity = glm::max(dot(normal, -lightDirection), 0.0f);

    glm::vec3 sphereColor{1.0f, 0.0f, 1.0f};
    sphereColor *= lightIntensity;
    return glm::vec4(sphereColor, 1.0f);
}
