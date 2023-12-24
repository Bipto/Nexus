#include "Window.hpp"

namespace Nexus::UI
{
    Window::Window(int x, int y, int width, int height, const std::string &title, Nexus::Graphics::Font *font, InputState *inputState)
        : m_WindowTitle(title), m_Font(font), m_InputState(inputState)
    {
        m_Rectangle = Nexus::Graphics::Rectangle(x, y, width, height);
    }

    void Window::Render(Nexus::Graphics::BatchRenderer *batchRenderer, Nexus::Graphics::RenderTarget target)
    {
        auto titlebarRect = GetTitlebarRectangle();

        auto mousePos = m_InputState->GetMouse().GetMousePosition();
        if (titlebarRect.ContainsPoint(mousePos) && m_InputState->GetMouse().IsLeftMouseHeld())
        {
            m_Dragging = true;
        }

        if (m_Dragging && m_InputState->GetMouse().WasLeftMouseReleased())
        {
            m_Dragging = false;
        }

        if (m_Dragging)
        {
            auto mouseMovement = m_InputState->GetMouse().GetMouseMovement();
            m_Rectangle.SetX(m_Rectangle.GetLeft() + mouseMovement.X);
            m_Rectangle.SetY(m_Rectangle.GetTop() + mouseMovement.Y);
        }

        titlebarRect = GetTitlebarRectangle();
        auto closeButtonRect = GetCloseButtonRectangle();

        if (closeButtonRect.ContainsPoint(mousePos) && m_InputState->GetMouse().WasLeftMouseClicked())
        {
            m_Closing = true;
        }

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = target.GetSize().X;
        vp.Height = target.GetSize().Y;

        Nexus::Graphics::Scissor scissor;
        scissor.X = 0;
        scissor.Y = 0;
        scissor.Width = target.GetSize().X;
        scissor.Height = target.GetSize().Y;

        batchRenderer->Begin(vp, scissor);
        batchRenderer->DrawQuadFill(m_Rectangle, m_BackgroundColour);
        batchRenderer->DrawQuadFill(titlebarRect, m_TitlebarColour);

        batchRenderer->DrawString(m_WindowTitle, {m_Rectangle.GetLeft(), m_Rectangle.GetTop()}, m_FontSize, {1.0f, 1.0f, 1.0f, 1.0f}, m_Font, titlebarRect);

        if (closeButtonRect.ContainsPoint(mousePos))
        {
            batchRenderer->DrawQuadFill(closeButtonRect, {0.6f, 0.0f, 0.0f, 1.0f});
        }
        else
        {
            batchRenderer->DrawQuadFill(closeButtonRect, {0.65f, 0.0f, 0.0f, 1.0f});
        }

        batchRenderer->DrawCross(closeButtonRect, 3.0f, {1.0f, 1.0f, 1.0f, 1.0f});

        auto bottomLeftCircle = GetBottomLeftCircle();
        auto bottomRightCircle = GetBottomRightCircle();

        if (bottomLeftCircle.ContainsPoint(mousePos))
        {
            if (m_InputState->GetMouse().WasLeftMouseClicked())
            {
                m_ResizeLeft = true;
            }
        }
        if (bottomRightCircle.ContainsPoint(mousePos))
        {
            if (m_InputState->GetMouse().IsLeftMouseHeld())
            {
                m_ResizeRight = true;
            }
        }

        if (m_ResizeLeft || bottomLeftCircle.ContainsPoint(mousePos))
        {
            batchRenderer->DrawCircleFill(bottomLeftCircle, {1.0f, 1.0f, 1.0f, 1.0f}, 24);
            batchRenderer->DrawCircle(bottomLeftCircle, {0.0f, 0.0f, 0.0f, 1.0f}, 24, 1.5f);

            if (m_ResizeLeft)
            {
                auto newWidth = m_Rectangle.GetWidth() - m_InputState->GetMouse().GetMouseMovement().X;
                auto newX = m_Rectangle.GetLeft() + m_InputState->GetMouse().GetMouseMovement().X;

                auto newHeight = m_Rectangle.GetHeight() + m_InputState->GetMouse().GetMouseMovement().Y;

                if (newWidth < m_MinimumSize.x)
                {
                    newWidth = m_MinimumSize.x;
                    newX = m_Rectangle.GetLeft();
                    m_ResizeLeft = false;
                    m_ResizeDragLeftVisible = false;
                }

                if (newHeight < m_MinimumSize.y)
                {
                    newHeight = m_MinimumSize.y;
                    m_ResizeLeft = false;
                    m_ResizeDragLeftVisible = false;
                }

                m_Rectangle.SetX(newX);
                m_Rectangle.SetWidth(newWidth);
                m_Rectangle.SetHeight(newHeight);
            }

            if (m_InputState->GetMouse().WasLeftMouseReleased())
            {
                m_ResizeLeft = false;
            }
        }

        if (m_ResizeRight || bottomRightCircle.ContainsPoint(mousePos))
        {
            batchRenderer->DrawCircleFill(bottomRightCircle, {1.0f, 1.0f, 1.0f, 1.0f}, 24);
            batchRenderer->DrawCircle(bottomRightCircle, {0.0f, 0.0f, 0.0f, 1.0f}, 24, 1.5f);

            if (m_ResizeRight)
            {
                auto newWidth = m_Rectangle.GetWidth() + m_InputState->GetMouse().GetMouseMovement().X;
                auto newHeight = m_Rectangle.GetHeight() + m_InputState->GetMouse().GetMouseMovement().Y;

                if (newWidth < m_MinimumSize.x)
                {
                    newWidth = m_MinimumSize.x;
                    m_ResizeRight = false;
                    m_ResizeDragRightVisible = false;
                }

                if (newHeight < m_MinimumSize.y)
                {
                    newHeight = m_MinimumSize.y;
                    m_ResizeRight = false;
                    m_ResizeDragRightVisible = false;
                }

                m_Rectangle.SetWidth(newWidth);
                m_Rectangle.SetHeight(newHeight);
            }

            if (m_InputState->GetMouse().WasLeftMouseReleased())
            {
                m_ResizeRight = false;
            }
        }

        batchRenderer->End();
    }

    void Window::Update()
    {
    }

    bool Window::IsClosing()
    {
        return m_Closing;
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

    Nexus::Graphics::Circle Window::GetBottomLeftCircle()
    {
        return Nexus::Graphics::Circle({m_Rectangle.GetLeft(), m_Rectangle.GetBottom()}, 10);
    }

    Nexus::Graphics::Circle Window::GetBottomRightCircle()
    {
        return Nexus::Graphics::Circle({m_Rectangle.GetRight(), m_Rectangle.GetBottom()}, 10);
    }
}