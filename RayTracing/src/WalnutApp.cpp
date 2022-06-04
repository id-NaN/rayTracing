#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
        bool rendered = false;

		ImGui::Begin("Settings");
        ImGui::Text("Last Render: %3fms", static_cast<double>(last_render_time_));
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
        if (image_)
        {
            ImGui::Image(
                image_->GetDescriptorSet(),
                {
                    static_cast<float>(image_->GetWidth()),
                    static_cast<float>(image_->GetHeight())
                }
            );
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if (!rendered && (
            !image_ ||
            viewport_width_ != image_->GetWidth() ||
            viewport_height_ != image_->GetHeight()
        ))
        {
            Render();
        }
	}

    void Render()
	{
        Timer timer;
	    if (
            !image_ ||
            viewport_width_ != image_->GetWidth() ||
            viewport_height_ != image_->GetHeight()
        )
	    {
            image_ = std::make_shared<Image>(
                viewport_width_,
                viewport_height_,
                ImageFormat::RGBA
            );
            delete[] image_data_;
            image_data_ = new uint32_t[static_cast<uint64_t>(viewport_width_) * static_cast<uint64_t>(viewport_height_)];
	    }

        for (uint32_t i = 0; i < viewport_width_ * viewport_height_; i++)
        {
            image_data_[i] = Random::UInt();
            image_data_[i] |= 0xff000000;
        }

        image_->SetData(image_data_);

        last_render_time_ = timer.ElapsedMillis();
	}

private:
    std::shared_ptr<Image> image_;
    uint32_t* image_data_ = nullptr;
    uint32_t viewport_width_ = 0;
    uint32_t viewport_height_ = 0;
    float last_render_time_ = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
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