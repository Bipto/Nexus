#include <memory>

#include <wx/aui/aui.h>
#include <wx/treectrl.h>
#include <wx/wx.h>

#include "Nexus-Core/Engine.hpp"

#include "RHI/GraphicsDevice.hpp"
#include "RHI/IGraphicsAPI.hpp"
#include "RHI/Swapchain.hpp"

#include "UI/Layout.hpp"

#include "wxWidgets/wxWidgetsLayout.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

class MyApp : public wxApp
{
  public:
    bool OnInit() override;

  private:
    Nexus::UI::wxWidgetsLayout m_Layout;
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
  public:
    MyFrame();
    ~MyFrame();

  private:
    void OnHello();
    void OnExit();
    void OnAbout();
    void OnResize(wxSizeEvent &event);
    void OnRenderTimer(wxTimerEvent &event);
    void OnRender();
    void OnPaint(wxPaintEvent &event);

    void CreateGraphicsResources();

    wxAuiManager m_mgr;
    wxTimer m_RenderTimer = {};

    Nexus::Engine m_Engine;

    Nexus::Graphics::SurfaceHandle m_Surface = {};
    Nexus::Graphics::SwapchainHandle m_Swapchain = {};
    Nexus::Graphics::CommandListHandle m_CommandList = {};

    wxPanel *m_Panel = nullptr;
    bool m_ResizePending = false;
    wxSize m_PanelSize = {};
    Nexus::UI::wxWidgetsLayout m_Layout{};
};

enum
{
    ID_Hello = 1
};

bool MyApp::OnInit()
{
    SetAppearance(Appearance::System);

#ifdef _WIN32
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif

    auto frame = m_Layout.CreateFrame("Hello World");

    Nexus::UI::IMenubar *menubar = frame->CreateMenubar();

    Nexus::UI::IMenu *fileMenu = menubar->CreateMenu("&File");

    Nexus::UI::IMenu *newMenu = fileMenu->AppendSubMenu("New");
    Nexus::UI::IMenuItem *fileItem = newMenu->Append("File");

    Nexus::UI::IMenuItem *helloItem = fileMenu->Append("&Hello...\tCtrl-H");
    helloItem->OnClick([this]() { m_Layout.LogMessage("Hello world from wxWidgets!"); });
    fileMenu->AppendSeparator();

    Nexus::UI::IMenuItem *exitItem = fileMenu->Append("Exit");
    exitItem->OnClick([this]() { OnExit(); });

    Nexus::UI::IMenu *helpMenu = menubar->CreateMenu("&Help");
    Nexus::UI::IMenuItem *aboutItem = helpMenu->Append("About");
    aboutItem->OnClick([this]() {
        m_Layout.ShowMessageBox("This is a wxWidgets Hello World example", "About Hello World");
    });

    Nexus::UI::IStatusBar *statusBar = frame->CreateStatusbar();
    statusBar->SetStatusText("Welcome to wxWidgets");
    statusBar->SetHelpText("This is help text for the status bar");

    return true;
}

MyFrame::MyFrame()
    : wxFrame(
          nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize,
          wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE
      ),
      m_mgr(this), m_Engine(Nexus::Graphics::GraphicsAPI::OpenGL, Nexus::Audio::AudioAPI::OpenAL)
{
    // std::unique_ptr<Nexus::UI::IMenubar> menubar = m_Layout.CreateMainMenubar();

    // Nexus::UI::IMenu *fileMenu = menubar->CreateMenu("&File");

    // Nexus::UI::IMenu	 *newMenu  = fileMenu->AppendSubMenu("New");
    // Nexus::UI::IMenuItem *fileItem = newMenu->Append("File");

    // Nexus::UI::IMenuItem *helloItem = fileMenu->Append("&Hello...\tCtrl-H");
    // helloItem->OnClick([this]() { OnHello(); });

    // fileMenu->AppendSeparator();

    // Nexus::UI::IMenuItem *exitItem = fileMenu->Append("Exit");
    // exitItem->OnClick([this]() { OnExit(); });

    // Nexus::UI::IMenu	 *helpMenu	= menubar->CreateMenu("&Help");
    // Nexus::UI::IMenuItem *aboutItem = helpMenu->Append("About");
    // aboutItem->OnClick([this]() { OnAbout(); });

    // fileMenu->AppendSeparator();

    // m_Layout.SetMainMenubar(std::move(menubar));

    // std::unique_ptr<Nexus::UI::IStatusBar> statusBar = m_Layout.CreateStatusBar();
    // statusBar->SetStatusText("Welcome to wxWidgets");
    // statusBar->SetHelpText("This is help text for the status bar");

    // m_RenderTimer.SetOwner(this);
    // Connect(wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnRenderTimer), NULL, this);

    // Bind(wxEVT_TIMER, &MyFrame::OnRenderTimer, this);
    // m_RenderTimer.Start(17);

    ////--- Central panel ---
    // m_Panel = new wxPanel(this);
    // m_Panel->Bind(wxEVT_SIZE, &MyFrame::OnResize, this);
    // m_Panel->SetDoubleBuffered(false);

    // m_Panel->SetBackgroundColour(wxBG_STYLE_CUSTOM);

    //// Add central panel to AUI
    // m_mgr.AddPane(m_Panel, wxAuiPaneInfo().CenterPane());

    //// --- Dockable left panel ---
    // wxPanel *leftPanel = new wxPanel(this);

    // wxTreeCtrl *tree = new wxTreeCtrl(leftPanel, wxID_ANY, wxDefaultPosition,
    // wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT);

    // wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    // sizer->Add(tree, 1, wxEXPAND | wxALL, 0);

    // leftPanel->SetSizer(sizer);

    // wxTreeItemId root = tree->AddRoot("Root");

    // wxTreeItemId animals = tree->AppendItem(root, "Animals");
    // wxTreeItemId mammals = tree->AppendItem(animals, "Mammals");
    // wxTreeItemId dogs	 = tree->AppendItem(mammals, "Dogs");
    // wxTreeItemId breeds	 = tree->AppendItem(dogs, "Breeds");

    // tree->AppendItem(breeds, "Labrador");
    // tree->AppendItem(breeds, "Beagle");
    // tree->AppendItem(breeds, "Collie");

    // tree->ExpandAll();

    //// new wxStaticText(leftPanel, wxID_ANY, "Left Dock Panel", wxPoint(10, 10));

    // m_mgr.AddPane(leftPanel,
    // wxAuiPaneInfo().Left().Caption("Tools").CloseButton(true).MaximizeButton(true));

    //// --- Dockable bottom log panel ---
    // wxTextCtrl *logCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition,
    // wxSize(-1, 120), wxTE_MULTILINE | wxTE_READONLY);

    // m_mgr.AddPane(logCtrl, wxAuiPaneInfo().Bottom().Caption("Output
    // Log").CloseButton(true).MaximizeButton(true));

    //// Apply layout
    // m_mgr.Update();

    // CreateGraphicsResources();
}

MyFrame::~MyFrame()
{
    m_mgr.UnInit();
}

void MyFrame::OnExit()
{
    Close(true);
}

void MyFrame::OnAbout()
{
    wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello()
{
    wxLogMessage("Hello world from wxWidgets!");
}

void MyFrame::OnResize(wxSizeEvent &event)
{
    if (!m_Swapchain.IsValid())
    {
        return;
    }

    wxSize size = event.GetSize();

    if ((size.GetWidth() == 0 || size.GetHeight() == 0) || (size == m_PanelSize))
        return;

    m_ResizePending = true;

    m_PanelSize = size;

    event.Skip();
}

void MyFrame::OnRenderTimer(wxTimerEvent &event)
{
    OnRender();
    event.Skip();
}

void MyFrame::OnRender()
{
    if (m_ResizePending)
    {
        m_Swapchain->Resize(m_PanelSize.GetWidth(), m_PanelSize.GetHeight());
        m_ResizePending = false;
    }

    Nexus::Graphics::IGraphicsDevice *device = m_Engine.GetGraphicsDevice();
    Nexus::Graphics::CommandQueueHandle queue = m_Engine.GetGraphicsCommandQueue();

    m_CommandList->Begin();
    m_CommandList->SetFramebuffer(m_Swapchain->GetCurrentFramebuffer());
    m_CommandList->ClearColourTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
    m_CommandList->End();

    queue->SubmitCommandList(m_CommandList);
    device->WaitForIdle();

    m_Swapchain->SwapBuffers({});
}

void MyFrame::OnPaint(wxPaintEvent &event)
{
    OnRender();
    wxPaintDC dc(this);
}

void MyFrame::CreateGraphicsResources()
{
    Nexus::Graphics::IGraphicsDevice *device = m_Engine.GetGraphicsDevice();
    Nexus::Graphics::CommandQueueHandle queue = m_Engine.GetGraphicsCommandQueue();

    HWND hwnd = reinterpret_cast<HWND>(m_Panel->GetHandle());
    HDC hdc = ::GetDC(hwnd);

    m_Surface = device->CreateSurfaceFromWin32(
        reinterpret_cast<uintptr_t>(hwnd), reinterpret_cast<uintptr_t>(hdc),
        reinterpret_cast<uintptr_t>(GetModuleHandle(NULL))
    );

    Nexus::Graphics::SwapchainDescription swapchainDesc = {};
    swapchainDesc.Width = m_Panel->GetSize().GetWidth();
    swapchainDesc.Height = m_Panel->GetSize().GetHeight();
    swapchainDesc.Samples = 1;
    swapchainDesc.Surface = m_Surface;

    m_Swapchain = queue->CreateSwapchain(swapchainDesc);

    m_CommandList = queue->CreateCommandList();
}
