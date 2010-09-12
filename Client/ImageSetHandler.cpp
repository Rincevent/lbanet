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

#include <CEGUI.h>
#include "ImageSetHandler.h"


/***********************************************************
get inventory image
***********************************************************/
std::string ImageSetHandler::GetInventoryImage(std::string file)
{	
	std::string res = "inv_" + file;

	if(!CEGUI::ImagesetManager::getSingleton().isDefined(res))
	{
		CEGUI::ImagesetManager::getSingleton().
			createFromImageFile(res, "Inventory/" + file + ".png");

		GetInventoryMiniImage(file);
	}

	return res;
}


/***********************************************************
get inventory image
***********************************************************/
std::string ImageSetHandler::GetInventoryMiniImage(std::string file)
{
	std::string res = "mini_" + file;

	if(!CEGUI::ImagesetManager::getSingleton().isDefined(res))
	{
		CEGUI::Imageset &ims = CEGUI::ImagesetManager::getSingleton().
			createFromImageFile(res, "minis/" + file + ".png");
		ims.setAutoScalingEnabled(false);
	}

	return res;
}




/***********************************************************
get image of stance
***********************************************************/
std::string ImageSetHandler::GetStanceImage(int stance)
{
	std::string file;
	switch(stance)
	{
		case 1:
		file = "normal";
		break;
		case 2:
		file = "Sporty";
		break;
		case 3:
		file = "Aggro";
		break;
		case 4:
		file = "Discreet";
		break;
	}

	std::string res = "stance" + file;

	if(!CEGUI::ImagesetManager::getSingleton().isDefined(res))
	{
		CEGUI::ImagesetManager::getSingleton().
			createFromImageFile(res, "Inventory/" + file + ".png");
	}

	return res;
}
