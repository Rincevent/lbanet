/*
------------------------[ Lbanet Source ]-------------------------
Copyright (C) 2009
Author: Vivien Delage [Rincevent_123]
Email : vdelage@gmail.com

-------------------------------[ GNU License ]-------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

-----------------------------------------------------------------------------
*/


#ifndef __LBA_NET_OSG8EVENT_HANDLER_H__
#define __LBA_NET_OSG8EVENT_HANDLER_H__

#include <osgGA/GUIEventHandler>


//*************************************************************************************************
//*                               class OsgEventHandler
//*************************************************************************************************
/**
* @brief Class taking care of user inputs (mouse and keyboard)
*
*/
// class to handle events with a pick
class OsgEventHandler : public osgGA::GUIEventHandler
{
public:
	//! constructor
    OsgEventHandler()
		: _right_button_pressed(false)
	{}

	//! destructor
    ~OsgEventHandler(){}

	//! handle inputs
    bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

private:
	bool	_right_button_pressed;
	int		_mouse_Y;
};


#endif