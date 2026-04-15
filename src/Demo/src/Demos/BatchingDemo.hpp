#pragma once

#include "Demo.hpp"
#include "Nexus-Core/Renderer/BatchRenderer.hpp"
#include "Nexus-Core/nxpch.hpp"

namespace Demos
{
	class BatchingDemo : public Demo
	{
	  public:
		BatchingDemo(const std::string						   &name,
					 Nexus::Application						   *app,
					 Nexus::ImGuiUtils::ImGuiGraphicsRenderer  *imGuiRenderer,
					 Nexus::Ref<Nexus::Graphics::ICommandQueue> commandQueue)
			: Demo(name, app, imGuiRenderer, commandQueue)
		{
		}

		virtual ~BatchingDemo()
		{
		}

		virtual void Load() override
		{
			Nexus::Ref<Nexus::Graphics::ISwapchain> swapchain	= Nexus::GetApplication()->GetPrimarySwapchain();
			uint32_t								sampleCount = swapchain->GetDescription().Samples;
			m_CommandList										= m_CommandQueue->CreateCommandList();
			m_BatchRenderer										= Nexus::Scope<Nexus::Graphics::BatchRenderer>(
				new Nexus::Graphics::BatchRenderer(m_GraphicsDevice, m_CommandQueue, false, sampleCount));
			GenerateShapes();
		}

		void GenerateShapes()
		{
			auto [x, y] = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();
			m_Quads.clear();

			std::random_device dev;
			std::mt19937	   rng(dev());

			std::uniform_real_distribution<float> random_x(0, x - (m_MaxQuadSize / 2));
			std::uniform_real_distribution<float> random_y(0, y - (m_MaxQuadSize / 2));
			std::uniform_real_distribution<float> random_color_channel(0.0f, 1.0f);

			// for quads
			std::uniform_real_distribution<float> random_size(m_MinQuadSize, m_MaxQuadSize);

			// generate quads
			for (int i = 0; i < m_QuadCount; i++)
			{
				QuadInfo info;
				info.Position = {random_x(rng), random_y(rng)};
				info.Colour	  = {random_color_channel(rng), random_color_channel(rng), random_color_channel(rng), 1.0f};
				info.Size	  = {random_size(rng), random_size(rng)};
				m_Quads.emplace_back(info);
			}
		}

		virtual void Render(Nexus::TimeSpan time) override
		{
			m_CommandList->Begin();

			Nexus::Ref<Nexus::Graphics::ISwapchain> swapchain	= Nexus::GetApplication()->GetPrimarySwapchain();
			Nexus::Graphics::FramebufferHandle		framebuffer = swapchain->GetCurrentFramebuffer();
			m_CommandList->SetFramebuffer(framebuffer);

			m_CommandList->ClearColourTarget(0, {m_ClearColour.r, m_ClearColour.g, m_ClearColour.b, 1.0f});
			m_CommandList->End();

			m_CommandQueue->SubmitCommandLists(&m_CommandList, 1, nullptr);
			m_GraphicsDevice->WaitForIdle();

			auto [x, y] = Nexus::GetApplication()->GetPrimaryWindow()->GetWindowSize();

			Nexus::Graphics::Viewport vp;
			vp.X		= 0;
			vp.Y		= 0;
			vp.Width	= x;
			vp.Height	= y;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;

			Nexus::Graphics::Scissor scissor;
			scissor.X	   = 0;
			scissor.Y	   = 0;
			scissor.Width  = x;
			scissor.Height = y;

			m_BatchRenderer->Begin(framebuffer, vp, scissor);

			for (const auto &quad : m_Quads)
			{
				Nexus::Graphics::Rectangle rect(quad.Position.x, quad.Position.y, quad.Size.x, quad.Size.y);
				m_BatchRenderer->DrawQuadFill(rect, quad.Colour);
			}

			m_BatchRenderer->End();
			m_GraphicsDevice->WaitForIdle();
		}

		virtual void OnResize(Nexus::Point2D<uint32_t> size) override
		{
			GenerateShapes();
		}

		virtual void RenderUI() override
		{
			if (ImGui::DragInt("Quad Count", &m_QuadCount))
			{
				GenerateShapes();
			}

			if (ImGui::DragFloat("Min Quad Size", &m_MinQuadSize, 1.0f, 1.0f, 50.0f))
			{
				GenerateShapes();
			}

			if (ImGui::DragFloat("Max Quad Size", &m_MaxQuadSize, 1.0f, 50.0f, 250.0f))
			{
				GenerateShapes();
			}
		}

		virtual std::string GetInfo() const override
		{
			return "Rendering 2D geometry efficiently using a batch renderer";
		}

	  private:
		struct QuadInfo
		{
			glm::vec2 Position;
			glm::vec2 Size;
			glm::vec4 Colour;
		};

		struct CircleInfo
		{
			glm::vec2 Position;
			float	  Radius;
			glm::vec4 Colour;
		};

	  private:
		Nexus::Ref<Nexus::Graphics::ICommandList> m_CommandList;
		glm::vec3								  m_ClearColour = {100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f};

		Nexus::Scope<Nexus::Graphics::BatchRenderer> m_BatchRenderer = nullptr;

		std::vector<QuadInfo> m_Quads;
		int					  m_QuadCount	= 50;
		float				  m_MinQuadSize = 5;
		float				  m_MaxQuadSize = 50;
	};
}	 // namespace Demos