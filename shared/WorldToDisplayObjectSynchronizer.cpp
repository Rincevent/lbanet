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

#include "WorldToDisplayObjectSynchronizer.h"
#include "LogHandler.h"
#include <sstream>


#define ABS(X) (((X)<0)?(-(X)):(X))

/***********************************************************
constructor
***********************************************************/
WorldToDisplayObjectSynchronizer::WorldToDisplayObjectSynchronizer(boost::shared_ptr<PhysicalObjectHandlerBase> phH,
																	boost::shared_ptr<DisplayObjectHandlerBase> disH,
																	boost::shared_ptr<SoundObjectHandlerBase> soundH,
																	long id)
	: DynamicObject(phH, disH, soundH, id),
		_lastDisplayPositionX(0), _lastDisplayPositionY(0), _lastDisplayPositionZ(0)
{

	#ifdef _DEBUG
		std::stringstream strs;
		strs<<"Created new WorldToDisplayObjectSynchronizer of id "<<_id;
		LogHandler::getInstance()->LogToFile(strs.str());
	#endif
}

/***********************************************************
destructor
***********************************************************/
WorldToDisplayObjectSynchronizer::~WorldToDisplayObjectSynchronizer()
{
	#ifdef _DEBUG
		std::stringstream strs;
		strs<<"Destroyed new WorldToDisplayObjectSynchronizer of id "<<_id;
		LogHandler::getInstance()->LogToFile(strs.str());
	#endif
}


/***********************************************************
synchronization function - will typically be called on every frames
***********************************************************/
int WorldToDisplayObjectSynchronizer::Process(double time, float tdiff)
{
	StraightSync();

	//check if we have action on animation to execute
	VerifyActionOnAnim();


	float animX=0, animY=0, animZ=0;
	if(_disH)
	{
		animX = _disH->GetCurrentAssociatedSpeedX();
		animY = _disH->GetCurrentAssociatedSpeedY();
		animZ = _disH->GetCurrentAssociatedSpeedZ();
	}


	if(_phH)
	{
		_phH->ResetMove();
		_phH->Process(time, tdiff, animX, animY, animZ);
	}

	if(_disH)
		return _disH->Process(time, tdiff);

	return 0;
}



/***********************************************************
sync display and physic
***********************************************************/
void WorldToDisplayObjectSynchronizer::Synchronize()
{
	StraightSync();
}


/***********************************************************
directly synchronize value between physic and display
***********************************************************/
void WorldToDisplayObjectSynchronizer::StraightSync()
{
	// get value from physic object
	float posX, posY, posZ;
	LbaQuaternion Quat;
	_phH->GetPosition(posX, posY, posZ);
	_phH->GetRotation(Quat);


	// for each value test if they are equal with display value
	//if not move display value to the correct value
	bool positionchanged = false;
	if(!equal(posX, _lastDisplayPositionX))
	{
		positionchanged = true;
		_lastDisplayPositionX = posX;
	}

	if(!equal(posY, _lastDisplayPositionY))
	{
		positionchanged = true;
		_lastDisplayPositionY = posY;
	}

	if(!equal(posZ, _lastDisplayPositionZ))
	{
		positionchanged = true;
		_lastDisplayPositionZ = posZ;
	}

	// update displayed position if needed
	if(_disH && positionchanged)
		_disH->SetPosition(_lastDisplayPositionX, _lastDisplayPositionY, _lastDisplayPositionZ);


	// check rotations
	bool directionchanged = false;
	if(		!equal(Quat.X, _lastDisplayRotation.X)
		||	!equal(Quat.Y, _lastDisplayRotation.Y)
		||	!equal(Quat.Z, _lastDisplayRotation.Z)
		||	!equal(Quat.W, _lastDisplayRotation.W))
	{
		directionchanged = true;
		_lastDisplayRotation = Quat;
		if(_disH)
			_disH->SetRotation(_lastDisplayRotation);
	}

	if(positionchanged || directionchanged)
		UpdateSoundPosition();
}
