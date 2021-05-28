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

#include <jp/visx_gui.hpp>

using namespace jp::visx::gui;

wxIMPLEMENT_APP(Application);

MainFrame *MainFrame::frame = nullptr;
Application *Application::application = nullptr;
wxMenu *MainFrame::menu_file = nullptr;
wxMenu *MainFrame::menu_help = nullptr;
wxMenuBar *MainFrame::menubar = nullptr;

bool Application::OnInit(void) {
	// Initialize the frame.
	MainFrame *frame = MainFrame::frame = new MainFrame();
	frame->Show(true);
	application = this;
	return true;
}

MainFrame::MainFrame(void) : wxFrame(NULL, wxID_ANY, "VisX") {
	wxMenu *menu_file = MainFrame::menu_file = new wxMenu();
#ifdef __APPLE__
	menu_file->Append(wxID_EXIT, "E&xit.\tCmd+Q", "Exits the program.");
#else
	menu_file->Append(wxID_EXIT, "E&xit.\tAlt+F4", "Exits the program.");
#endif

	wxMenu *menu_help = MainFrame::menu_help = new wxMenu();
	menu_help->Append(wxID_ABOUT, "&About...", "Displays information about the program.");

	wxMenuBar *menubar = MainFrame::menubar = new wxMenuBar();
	menubar->Append(menu_file, "&File");
	menubar->Append(menu_help, "&Help");

	SetMenuBar(menubar);

	Bind(wxEVT_MENU, &MainFrame::onExit, this, wxID_EXIT);
	Bind(wxEVT_MENU, &MainFrame::onAbout, this, wxID_ABOUT);
}

void MainFrame::onExit(wxCommandEvent &event) {
	Close(true);
}

void MainFrame::onAbout(wxCommandEvent &event) {
	wxMessageBox("VisX is a program which serves to help calculate and demonstrate various data and values in the field of physics.\n\nThis program uses wxWidgets to display content to the screen. wxWidgets' modified LGPL license does not require distribution of its sources.\nMore information about this project can be found at https://github.com/ljtpetersen/visx.\n\nCopyright (C) 2021 James Petersen\n\nThis program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public Licnse as published by\nthe free Software Foundation, version 3.\n\nThis program is distributed in the hope that it will be useful, but\nWITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\nGeneral Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with this program. If not, see <http://www.gnu.org/licenses/>.", "About VisX", wxOK | wxICON_INFORMATION);
}

