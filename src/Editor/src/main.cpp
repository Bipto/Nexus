// Start of wxWidgets "Hello World" Program
#include <wx/aui/aui.h>
#include <wx/wx.h>

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

	wxAuiManager m_mgr;
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

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World"), m_mgr(this)	   // <-- Initialize AUI manager
{
	// --- Central panel ---
	wxPanel		 *panel = new wxPanel(this);
	wxBoxSizer	 *sizer = new wxBoxSizer(wxVERTICAL);
	wxStaticText *text1 = new wxStaticText(panel, wxID_ANY, "Hello, wxWidgets!");
	sizer->Add(text1, 0, wxALL, 20);
	panel->SetSizer(sizer);

	// Add central panel to AUI
	m_mgr.AddPane(panel, wxAuiPaneInfo().CenterPane());

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