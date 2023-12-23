#include "Window.hpp"

namespace Nexus::UI
{
    Window::Window(int x, int y, int width, int height, const std::string &title, Nexus::Graphics::Font *font)
        : m_WindowTitle(title), m_Font(font)
    {
        m_Rectangle = Nexus::Graphics::Rectangle(x, y, width, height);
    }

    void Window::Render(Nexus::Graphics::BatchRenderer *batchRenderer, Nexus::Graphics::RenderTarget target)
    {
        auto titlebarRect = GetTitlebarRectangle();

        auto mousePos = m_InputState->GetMouse().GetMousePosition();
        if (titlebarRect.ContainsPoint(mousePos) && m_InputState->GetMouse().IsLeftMouseHeld())
        {
            auto mouseMovement = m_InputState->GetMouse().GetMouseMovement();
            m_Rectangle.SetX(m_Rectangle.GetLeft() + mouseMovement.X);
            m_Rectangle.SetY(m_Rectangle.GetTop() + mouseMovement.Y);
        }

        titlebarRect = GetTitlebarRectangle();
        auto closeButtonRect = GetCloseButtonRectangle();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = target.GetSize().X;
        vp.Height = target.GetSize().Y;

        Nexus::Graphics::Scissor scissor;
        scissor.X = m_Rectangle.GetLeft();
        scissor.Y = m_Rectangle.GetTop();
        scissor.Width = m_Rectangle.GetWidth();
        scissor.Height = m_Rectangle.GetHeight();

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(m_Rectangle, m_BackgroundColour);
        batchRenderer->DrawQuadFill(titlebarRect, m_TitlebarColour);

        batchRenderer->DrawString(m_WindowTitle, {m_Rectangle.GetLeft(), m_Rectangle.GetTop()}, m_FontSize, {1.0f, 1.0f, 1.0f, 1.0f}, m_Font);

        batchRenderer->DrawQuadFill(closeButtonRect, {1.0f, 0.0f, 0.0f, 1.0f});

        batchRenderer->End();
    }

    void Window::Update()
    {
    }

    Nexus::Graphics::Rectangle Window::GetTitlebarRectangle()
    {
        auto textSize = m_Font->MeasureString(m_WindowTitle, m_FontSize);
        Nexus::Graphics::Rectangle titlebarRect(m_Rectangle.GetLeft(),
                                                m_Rectangle.GetTop(),
                                                m_Rectangle.GetWidth(),
                                                textSize.Y);
        return titlebarRect;
    }

    Nexus::Graphics::Rectangle Window::GetCloseButtonRectangle()
    {
        auto titlebarRectangle = GetTitlebarRectangle();
        Nexus::Graphics::Rectangle closeButtonRect(titlebarRectangle.GetRight() - titlebarRectangle.GetHeight(),
                                                   titlebarRectangle.GetTop(),
                                                   titlebarRectangle.GetHeight(),
                                                   titlebarRectangle.GetHeight());
        return closeButtonRect;
    }
}