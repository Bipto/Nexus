#include "Window.hpp"

namespace Nexus::UI
{
    Window::Window()
    {
        m_Scrollable = new Scrollable();

        this->OnAddedToCanvas += [&](Control *control, Canvas *canvas)
        {
            m_Scrollable->SetCanvas(canvas);
        };
    }

    Window::~Window()
    {
    }

    void Window::Render(Graphics::BatchRenderer *batchRenderer)
    {
        bool closing = false;

        const auto &contentRegion = GetAvailableContentRegion();
        m_Scrollable->SetPosition({(int)contentRegion.GetLeft(), (int)contentRegion.GetTop()});
        m_Scrollable->SetSize({(int)contentRegion.GetWidth(), (int)contentRegion.GetHeight()});

        const Canvas *canvas = GetCanvas();

        Nexus::Graphics::Viewport vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = canvas->GetSize().X;
        vp.Height = canvas->GetSize().Y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;

        Nexus::Graphics::Rectangle<float> textureRect = GetContentRegionAvailable();

        Nexus::Graphics::Scissor scissor = GetScissor();

        batchRenderer->Begin(vp, scissor);

        batchRenderer->DrawQuadFill(textureRect, m_BackgroundColour);

        if (m_Font)
        {
            const auto titlebarRect = GetTitlebarRectangle();

            batchRenderer->DrawQuadFill(titlebarRect, m_TitlebarBackgroundColour);

            batchRenderer->DrawString(m_Title,
                                      {m_Position.X, m_Position.Y},
                                      m_Font->GetSize(),
                                      m_ForegroundColour,
                                      m_Font);

            const float offset = 5.0f;
            Nexus::Graphics::Rectangle<float> crossRectangle = Nexus::Graphics::Rectangle<float>(titlebarRect.GetRight() - titlebarRect.GetHeight() + offset,
                                                                                                 titlebarRect.GetTop() + offset,
                                                                                                 titlebarRect.GetHeight() - (offset * 2),
                                                                                                 titlebarRect.GetHeight() - (offset * 2));

            Nexus::Graphics::Circle<float> circle = Nexus::Graphics::Circle<float>({crossRectangle.GetLeft() + crossRectangle.GetWidth() / 2,
                                                                                    crossRectangle.GetTop() + crossRectangle.GetHeight() / 2},
                                                                                   titlebarRect.GetHeight() / 2);

            const auto &mousePos = Nexus::Input::GetMousePosition();
            if (crossRectangle.Contains(mousePos.To<float>()))
            {
                batchRenderer->DrawCircleFill(circle, {1.0f, 0.0f, 0.0f, 1.0f}, 32);

                if (Nexus::Input::IsLeftMousePressed())
                {
                    closing = true;
                }
            }

            batchRenderer->DrawCross(crossRectangle, 1.0f, {0.0f, 0.0f, 0.0f, 1.0f});
        }

        const auto &mousePos = Nexus::Input::GetMousePosition();
        Nexus::Graphics::Circle<float> bottomLeft = Nexus::Graphics::Circle<float>({textureRect.GetRight(), textureRect.GetBottom()}, 5);

        if (bottomLeft.Contains({(float)mousePos.X, (float)mousePos.Y}))
        {
            Nexus::Window *window = canvas->GetWindow();
            window->SetCursor(Nexus::Cursor::ArrowNWSE);

            if (Nexus::Input::IsLeftMousePressed())
            {
                m_ResizeX = true;
                m_ResizeY = true;
            }
        }
        else
        {
            Nexus::Window *window = canvas->GetWindow();
            window->SetCursor(Nexus::Cursor::Arrow);
        }

        if (m_ResizeX)
        {
            m_Size.X += Nexus::Input::GetMouseMovement().X;

            if (Nexus::Input::IsLeftMouseReleased())
            {
                m_ResizeX = false;
            }
        }

        if (m_ResizeY)
        {
            m_Size.Y += Nexus::Input::GetMouseMovement().Y;

            if (Nexus::Input::IsLeftMouseReleased())
            {
                m_ResizeY = false;
            }
        }

        batchRenderer->End();

        m_Scrollable->Render(batchRenderer);

        if (closing)
        {
            OnClose.Invoke(this);
        }
    }

    void Window::Update()
    {
        Control::Update();

        const auto titlebarRect = GetTitlebarRectangle();
        const auto &mousePos = Nexus::Input::GetMousePosition();

        if (titlebarRect.Contains(mousePos.To<float>()))
        {
            if (Nexus::Input::IsLeftMousePressed())
            {
                m_Dragging = true;
            }
        }

        if (m_Dragging)
        {
            const auto &mouseMovement = Nexus::Input::GetMouseMovement();
            m_Position.X += mouseMovement.X;
            m_Position.Y += mouseMovement.Y;

            if (Nexus::Input::IsLeftMouseReleased())
            {
                m_Dragging = false;
            }
        }

        if (!m_Dragging)
        {
            m_Scrollable->Update();
        }
    }

    void Window::SetDraggable(bool draggable)
    {
        m_Draggable = draggable;
    }

    const bool Window::IsDraggable() const
    {
        return m_Draggable;
    }

    void Window::SetCloseable(bool closeable)
    {
        m_Closeable = closeable;
    }

    const bool Window::IsCloseable() const
    {
        return m_Closeable;
    }

    void Window::SetTitlebarBackgroundColour(const glm::vec4 &colour)
    {
        m_TitlebarBackgroundColour = colour;
    }

    const glm::vec4 &Window::GetTitlebarBackgroundColour() const
    {
        return m_TitlebarBackgroundColour;
    }

    void Window::AddControl(Control *control)
    {
        m_Scrollable->AddControl(control);
    }

    void Window::RemoveControl(Control *control)
    {
        m_Scrollable->RemoveControl(control);
    }

    void Window::SetCanvas(Canvas *canvas)
    {
        Control::SetCanvas(canvas);
        m_Scrollable->SetCanvas(canvas);
    }

    Scrollable *Window::GetScrollable() const
    {
        return m_Scrollable;
    }

    const Nexus::Graphics::Rectangle<float> Window::GetTitlebarRectangle() const
    {
        Nexus::Point2D<uint32_t> titleSize = m_Font->MeasureString(m_Title, m_Font->GetSize());
        Nexus::Graphics::Rectangle<float> titlebarRect = {m_Position.To<float>(), {(float)m_Size.X, (float)titleSize.Y}};
        return titlebarRect;
    }

    const Nexus::Graphics::Rectangle<float> Window::GetAvailableContentRegion() const
    {
        const Nexus::Graphics::Rectangle<float> titlebarRect = GetTitlebarRectangle();

        Nexus::Graphics::Rectangle<float> rect = Nexus::Graphics::Rectangle<float>(titlebarRect.GetLeft(),
                                                                                   titlebarRect.GetBottom(),
                                                                                   titlebarRect.GetWidth(),
                                                                                   m_Size.Y - titlebarRect.GetHeight());

        return rect;
    }
}