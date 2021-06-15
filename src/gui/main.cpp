/* src/gui/main.cpp
 *
 * This file is part of the VisX project (https://github.com/ljtpetersen/visx).
 * Copyright (c) 2021 James Petersen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <jp/visx/gui/main.hpp>

using namespace jp::visx::gui;

MainFrame *MainFrame::frame = nullptr;
Application *Application::application = nullptr;
#ifndef __APPLE__
wxMenu *MainFrame::menu_file = nullptr;
#endif
wxMenu *MainFrame::menu_help = nullptr;
wxMenuBar *MainFrame::menubar = nullptr;

wxIMPLEMENT_APP(jp::visx::gui::Application);
wxDECLARE_APP(jp::visx::gui::Application);

bool Application::OnInit(void) {
	// Initialize the frame.
	MainFrame *frame = MainFrame::frame = new MainFrame();
	frame->Show(true);
	application = this;
	return true;
}

MainFrame::MainFrame(void) : wxFrame(NULL, wxID_ANY, "VisX") {
	wxMenu *menu_help = MainFrame::menu_help = new wxMenu();

#ifndef __APPLE__
	wxMenu *menu_file = MainFrame::menu_file = new wxMenu();
	menu_file->Append(wxID_EXIT, "E&xit.\tAlt+F4", "Exits the program.");

	menu_help->Append(wxID_ABOUT, "&About...", "Displays information about the program.");
#else
	menu_help->Append(ID_ABOUT, "&About...", "Displays information about the program.");
#endif

	wxMenuBar *menubar = MainFrame::menubar = new wxMenuBar();
#ifndef __APPLE__
	menubar->Append(menu_file, "&File");
#endif

	menubar->Append(menu_help, "&Help");

	SetMenuBar(menubar);

	this->SetClientSize(800, 600);
}

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
#ifdef __APPLE__
	EVT_MENU(ID_ABOUT, MainFrame::onAbout)
#else
	EVT_MENU(wxID_ABOUT, MainFrame::onAbout)
	EVT_MENU(wxID_EXIT, MainFrame::onExit)
#endif
	EVT_SIZE(MainFrame::onSizeChange)
END_EVENT_TABLE()

void MainFrame::onExit(wxCommandEvent &event) {
	Close(true);
}

void MainFrame::onAbout(wxCommandEvent &event) {
	wxMessageBox(JP_VISX_GUI_ABOUTSTR, "About VisX", wxOK | wxICON_INFORMATION);
}

void MainFrame::onSizeChange(wxSizeEvent &event) {
}

