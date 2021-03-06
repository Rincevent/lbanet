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

#if !defined(__LbaNetModel_1_LBA1ModelHandler_h)
#define __LbaNetModel_1_LBA1ModelHandler_h


#include "AnimatedObjectHandlerBase.h"
#include "Lba1ModelMapHandler.h"

class LBA1ModelClass;
struct entitiesTableStruct;
class AnimationObserverBase;
namespace osg
{
	class Node;
	class Geode;
	class AutoTransform;
}

/***********************************************************************
 * Module:  Lba1ModelHandler.h
 * Purpose: class for animated Lba1 models
 ***********************************************************************/
class Lba1ModelHandler : public AnimatedObjectHandlerBase
{
public:
	//! constructor
	Lba1ModelHandler(int sceneidx, boost::shared_ptr<DisplayTransformation> Tr,
						const LbaNet::ModelInfo & info, float animationspeed,
						bool UseLight, bool CastShadow,
						const LbaNet::ObjectExtraInfo &extrainfo,
						const LbaNet::LifeManaInfo &lifeinfo,
						bool mainchar);

	//! destructor
	virtual ~Lba1ModelHandler();

	//! update display
	virtual int Update(LbaNet::DisplayObjectUpdateBasePtr update,
							bool updatestoredstate);

	//! update display with current frame - used for animation
	//! return 1 if current animation finishes - 0 else
	virtual int ProcessInternal(double time, float tdiff);

	// get current animation Id
	virtual std::string GetCurrentAnimation();

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedX();

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedY();

	// get current associated speed
	virtual float GetCurrentAssociatedSpeedZ();



	// update model
	virtual int UpdateModel();

	// update animation
	virtual int UpdateAnimation();

	// save current model state
	virtual void SaveState();

	// restore previously saved model state
	virtual void RestoreState();

	//! check if can play animation
	virtual bool CanPlayAnimation(const std::string & anim);

	//! return animation key frame
	virtual int GetCurrentKeyFrame();


protected:
	// refresh model
	int RefreshModel(bool forcecolor = false);

	// RefrehAnimRepeatingSound
	void RefrehAnimRepeatingSound();

	// play sound
	void PlaySound(const std::string & path, int channel, bool repeat);

	// retrieve anim info
	void RetrieveAnimInfo();

	// play sound at keyframe
	void PlayKeyFrameSound(int keyframe);

	// GetFloorSoundPath
	std::string GetFloorSoundPath(int id);

private:
	LBA1ModelClass*									_model;
	osg::ref_ptr<osg::Node>							_osgnode;

	float											_animationspeed;



	int												_currModel;
	int												_currBody;
	int												_currAnimation;
	int												_currkeyframe;


	bool											_UseLight;
	bool											_CastShadow;
	bool											_mainchar;


	std::map<std::pair<LbaNet::Lba1MColorPart, int>, int>	_changedcolors;

	static entitiesTableStruct*						_estruct;

	std::vector<AnimationObserverBase *>			_obvservers;

	AnimationData									_curraniminfo;
	bool											_togglesoundC;

};




#endif