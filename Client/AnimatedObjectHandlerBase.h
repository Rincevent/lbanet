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

#if !defined(__LbaNetModel_1_AnimatedObjectHandlerBase_h)
#define __LbaNetModel_1_AnimatedObjectHandlerBase_h


#include "OsgObjectHandler.h"


/***********************************************************************
 * Module:  AnimatedObjectHandlerBase.h
 * Purpose: Base class for animated object to display
 ***********************************************************************/
class AnimatedObjectHandlerBase : public OsgObjectHandler
{
public:
	//! constructor
	AnimatedObjectHandlerBase(int sceneidx, boost::shared_ptr<DisplayTransformation> Tr,
									const LbaNet::ObjectExtraInfo &extrainfo,
									const LbaNet::LifeManaInfo &lifeinfo,
									const LbaNet::ModelInfo & info);

	//! destructor
	virtual ~AnimatedObjectHandlerBase();


	//! update display with current frame - used for animation
	//! return 1 if current animation finishes - 0 else
	virtual int ProcessInternal(double time, float tdiff) = 0;

	//! update display
	virtual int Update(LbaNet::DisplayObjectUpdateBasePtr update,
							bool updatestoredstate);

	// get current animation Id
	virtual std::string GetCurrentAnimation() = 0;

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedX() = 0;

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedY() = 0;

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedZ() = 0;

	// update model
	virtual int UpdateModel() = 0;

	// update animation
	virtual int UpdateAnimation() = 0;


	// save current model state
	virtual void SaveState();

	// restore previously saved model state
	virtual void RestoreState();

	//! get current model
	virtual LbaNet::ModelInfo GetCurrentModel(bool storedstate);

protected:
	bool											_paused;
	LbaNet::ModelInfo								_currentmodelinfo;
	std::string										_currentanimationstring;

private:
	bool											_savedpaused;
	LbaNet::ModelInfo								_savedmodelinfo;
	std::string										_savedanimationstring;
};




#endif