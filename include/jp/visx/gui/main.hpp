/* include/jp/visx_gui.hpp
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

#include <jp/def.h>

#ifndef JP_VISX_GUI_HPP
#define JP_VISX_GUI_HPP

#include <jp/def.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#define JP_VISX_GUI_ABOUTSTR "VisX is a program which serves to help calculate and demonstrate various data and values in the field of physics.\n\n\
This program uses wxWidgets to display content to the screen. wxWidgets' modified LGPL license does not require distribution of its sources.\nMore information about this project can be found at https://github.com/ljtpetersen/visx.\n\n\
Copyright (C) 2021 James Petersen\n\n\
This program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public Licnse as published by\nthe free Software Foundation, version 3.\n\n\
This program is distributed in the hope that it will be useful, but\n\
WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU\nGeneral Public License for more details.\n\n\
You should have received a copy of the GNU General Public License\n\
along with this program. If not, see <http://www.gnu.org/licenses/>."

namespace jp {
	namespace visx {
		namespace gui {
			class Application : public wxApp {
			public:
				static Application *application;
				virtual bool OnInit(void);
			};

			class MainFrame : public wxFrame {
			public:
				static MainFrame *frame;
// On Apple (for now), there is no need for a file menu.
#ifndef __APPLE__
				static wxMenu *menu_file;
#endif
				static wxMenu *menu_help;
				static wxMenuBar *menubar;
				MainFrame(void);
				void onExit(wxCommandEvent &event);
				void onAbout(wxCommandEvent &event);
				void onSizeChange(wxSizeEvent &event);

			private:
				DECLARE_EVENT_TABLE();
			};

			enum {
				ID_NULL,
#ifdef __APPLE__
				ID_ABOUT,
#endif
				ID_MODULE_SELECTOR
			};
		} // namespace gui
	} // namespace visx
} // namespace jp

#endif

