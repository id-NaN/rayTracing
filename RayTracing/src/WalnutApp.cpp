#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

#include "Renderer.h"

using namespace Walnut;

class RaytracingLayer : public Layer
{
public:
    void OnUIRender() override
    {
        bool rendered = false;

        ImGui::Begin("Settings");
        ImGui::Text("Last Render: %.3fms", static_cast<double>(last_render_time_));
        if (ImGui::Button("Render"))
        {
            rendered = true;
            Render();
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        viewport_width_ = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        viewport_height_ = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

        std::shared_ptr<Image> image = renderer_.get_final_image();
        if (image)
        {
            ImGui::Image(
                image->GetDescriptorSet(),
                {
                    static_cast<float>(image->GetWidth()),
                    static_cast<float>(image->GetHeight())
                },
                ImVec2(0, 1),
                ImVec2(1, 0)
            );
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if (!rendered && (
            !image ||
            viewport_width_ != image->GetWidth() ||
            viewport_height_ != image->GetHeight()
        ))
        {
            Render();
        }
    }

    void Render()
    {
        Timer timer;

        renderer_.on_resize(viewport_width_, viewport_height_);
        renderer_.render();

        last_render_time_ = timer.ElapsedMillis();
    }

private:
    Renderer renderer_;

    uint32_t viewport_width_ = 0;
    uint32_t viewport_height_ = 0;
    float last_render_time_ = 0;
};

Application* Walnut::CreateApplication(int argc, char** argv)
{
    ApplicationSpecification spec;
    spec.Name = "Ray Tracing";

    auto app = new Application(spec);
    app->PushLayer<RaytracingLayer>();
    app->SetMenubarCallback([app]()
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                app->Close();
            }
            ImGui::EndMenu();
        }
    });
    return app;
}
