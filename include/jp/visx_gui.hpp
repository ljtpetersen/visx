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
// On Apple (for now), there is no need for a file
// or a help menu.
#ifndef __APPLE__
				static wxMenu *menu_file;
#endif
				static wxMenu *menu_help;
				static wxMenuBar *menubar;
				MainFrame(void);

			private:
				void onExit(wxCommandEvent &event);
				void onAbout(wxCommandEvent &event);
			};

			enum {
				ID_NULL,
#ifdef __APPLE__
				ID_ABOUT,
#endif
			};
		} // namespace gui
	} // namespace visx
} // namespace jp

#endif

