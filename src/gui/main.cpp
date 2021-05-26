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
#include <QString>

using namespace jp::visx::gui;

int main(int argc, char *argv[]) {
	application = new QApplication(argc, argv);
	main_window = new QMainWindow{nullptr};
	main_window->setWindowTitle(QString{"VisX"});
	main_window->resize(800, 600);
	
	main_menubar = new QMenuBar{main_window};
	main_window->setMenuBar(main_menubar);
	main_menus[MM_HELP] = new QMenu{QString{"Help"}, main_menubar};
	{
		QMenu *m_help = main_menus[MM_HELP];
	}
	for (uint i = 0; i < NELS(main_menus); ++i) {
		main_menubar->addMenu(main_menus[i]);
	}
	
	main_window->show();
	return QApplication::exec();
}

