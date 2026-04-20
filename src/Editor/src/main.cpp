#include <memory>

#include <wx/aui/aui.h>
#include <wx/wx.h>

#include "RHI/GraphicsDevice.hpp"
#include "RHI/IGraphicsAPI.hpp"
#include "RHI/Swapchain.hpp"

#ifdef _WIN32
	#include <windows.h>
#endif

class MyApp : public wxApp
{
  public:
	bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
{
  public:
	MyFrame();
	~MyFrame();

  private:
	void OnHello(wxCommandEvent &event);
	void OnExit(wxCommandEvent &event);
	void OnAbout(wxCommandEvent &event);
	void OnResize(wxSizeEvent &event);
	void OnRenderTimer(wxTimerEvent &event);
	void OnRender();
	void OnPaint(wxPaintEvent &event);

	void CreateGraphicsResources();

	wxAuiManager m_mgr;
	wxTimer		 m_RenderTimer = {};

	std::unique_ptr<Nexus::Graphics::IGraphicsAPI>	  m_GraphicsAPI	   = {};
	std::unique_ptr<Nexus::Graphics::IGraphicsDevice> m_GraphicsDevice = {};
	Nexus::Graphics::CommandQueueHandle				  m_CommandQueue   = {};
	Nexus::Graphics::SurfaceHandle					  m_Surface		   = {};
	Nexus::Graphics::SwapchainHandle				  m_Swapchain	   = {};
	Nexus::Graphics::CommandListHandle				  m_CommandList	   = {};

	wxPanel *m_Panel		 = nullptr;
	bool	 m_ResizePending = false;
	wxSize	 m_PanelSize	 = {};
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

	MyFrame *frame = new MyFrame();
	frame->Show(true);
	return true;
}

MyFrame::MyFrame()
	: wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE),
	  m_mgr(this)	 // <-- Initialize AUI manager
{
	m_RenderTimer.SetOwner(this);
	Connect(wxEVT_TIMER, wxTimerEventHandler(MyFrame::OnRenderTimer), NULL, this);

	Bind(wxEVT_TIMER, &MyFrame::OnRenderTimer, this);
	m_RenderTimer.Start(16);

	// --- Central panel ---
	m_Panel = new wxPanel(this);
	m_Panel->Bind(wxEVT_SIZE, &MyFrame::OnResize, this);
	m_Panel->SetDoubleBuffered(false);

	m_Panel->SetBackgroundColour(wxBG_STYLE_CUSTOM);

	// Add central panel to AUI
	m_mgr.AddPane(m_Panel, wxAuiPaneInfo().CenterPane());

	// --- Dockable left panel ---
	wxPanel *leftPanel = new wxPanel(this);
	new wxStaticText(leftPanel, wxID_ANY, "Left Dock Panel", wxPoint(10, 10));

	m_mgr.AddPane(leftPanel, wxAuiPaneInfo().Left().Caption("Tools").CloseButton(true).MaximizeButton(true));

	// --- Dockable bottom log panel ---
	wxTextCtrl *logCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 120), wxTE_MULTILINE | wxTE_READONLY);

	m_mgr.AddPane(logCtrl, wxAuiPaneInfo().Bottom().Caption("Output Log").CloseButton(true).MaximizeButton(true));

	// Apply layout
	m_mgr.Update();

	// --- Menus ---
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_Hello, "&Hello...\tCtrl-H");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);

	CreateStatusBar();
	SetStatusText("Welcome to wxWidgets!");

	Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
	Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
	Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

	CreateGraphicsResources();
}
MyFrame::~MyFrame()
{
	m_mgr.UnInit();
}

void MyFrame::OnExit(wxCommandEvent &event)
{
	Close(true);
}

void MyFrame::OnAbout(wxCommandEvent &event)
{
	wxMessageBox("This is a wxWidgets Hello World example", "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent &event)
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

	if (size.GetWidth() == 0 || size.GetHeight() == 0)
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

	m_CommandList->Begin();
	m_CommandList->SetFramebuffer(m_Swapchain->GetCurrentFramebuffer());
	m_CommandList->ClearColourTarget(0, {1.0f, 0.0f, 0.0f, 1.0f});
	m_CommandList->End();

	m_CommandQueue->SubmitCommandList(m_CommandList);
	m_GraphicsDevice->WaitForIdle();

	m_Swapchain->SwapBuffers({});
}

void MyFrame::OnPaint(wxPaintEvent &event)
{
	OnRender();
	wxPaintDC dc(this);
}

void MyFrame::CreateGraphicsResources()
{
	Nexus::Graphics::GraphicsAPICreateInfo apiCreateInfo = {.API = Nexus::Graphics::GraphicsAPI::OpenGL, .Debug = true};
	m_GraphicsAPI = std::unique_ptr<Nexus::Graphics::IGraphicsAPI>(Nexus::Graphics::IGraphicsAPI::CreateAPI(apiCreateInfo));

	std::vector<std::shared_ptr<Nexus::Graphics::IPhysicalDevice>> physicalDevices = m_GraphicsAPI->GetPhysicalDevices();
	m_GraphicsDevice = std::unique_ptr<Nexus::Graphics::IGraphicsDevice>(m_GraphicsAPI->CreateGraphicsDevice(physicalDevices[0]));

	// iterate through all available command queues
	std::vector<Nexus::Graphics::QueueFamilyInfo> queueFamilies = m_GraphicsDevice->GetQueueFamilies();
	for (const Nexus::Graphics::QueueFamilyInfo &queueFamily : queueFamilies)
	{
		if (queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Graphics) &&
			queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Compute) &&
			queueFamily.HasCapability(Nexus::Graphics::QueueCapabilities::Transfer))
		{
			// create graphics queue
			{
				Nexus::Graphics::CommandQueueDescription queueDesc = {};
				queueDesc.QueueFamilyIndex						   = queueFamily.QueueFamily;
				queueDesc.QueueIndex							   = 0;
				queueDesc.DebugName								   = "Application Graphics Queue";
				m_CommandQueue									   = m_GraphicsDevice->CreateCommandQueue(queueDesc);
			}
		}
	}

	HWND hwnd = reinterpret_cast<HWND>(m_Panel->GetHandle());
	HDC	 hdc  = ::GetDC(hwnd);

	m_Surface = m_GraphicsDevice->CreateSurfaceFromWin32(reinterpret_cast<uintptr_t>(hwnd),
														 reinterpret_cast<uintptr_t>(hdc),
														 reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)));

	Nexus::Graphics::SwapchainDescription swapchainDesc = {};
	swapchainDesc.Width									= m_Panel->GetSize().GetWidth();
	swapchainDesc.Height								= m_Panel->GetSize().GetHeight();
	swapchainDesc.Samples								= 1;
	swapchainDesc.Surface								= m_Surface;

	m_Swapchain = m_CommandQueue->CreateSwapchain(swapchainDesc);

	m_CommandList = m_CommandQueue->CreateCommandList();
}
