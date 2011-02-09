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


#ifndef _LBANET_SCRIPT_ENV_BASE_H_
#define _LBANET_SCRIPT_ENV_BASE_H_


class ActorHandler;
class TriggerBase;
class ActionBase;
class ClientScriptBase;
class ActionArgumentBase;
class ContainerSharedInfo;

#include <boost/shared_ptr.hpp>

#include "CommonTypes.h"
#include "LuaHandlerBase.h"

//*************************************************************************************************
//*                               class ScriptEnvironmentBase
//*************************************************************************************************
/**
* @brief Base class for lua script environment
*
*/
class ScriptEnvironmentBase
{
public:

	//! constructor
	ScriptEnvironmentBase()
		: m_generatednumber(0)
	{}


	//! destructor
	virtual ~ScriptEnvironmentBase(){}


	// function used by LUA to add actor
	virtual void AddActorObject(boost::shared_ptr<ActorHandler> actor) = 0;

	// add a trigger 
	virtual void AddTrigger(boost::shared_ptr<TriggerBase> trigger) = 0;


	// teleport an object
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void Teleport(int ObjectType, long ObjectId,
							const std::string &NewMapName, 
							long SpawningId,
							float offsetX, float offsetY, float offsetZ) = 0;


	// execute client script - does not work on npc objects
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void ExecuteClientScript(int ObjectType, long ObjectId,
										const std::string & ScriptName,
										bool InlineFunction) = 0;


	// execute custom lua function
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void ExecuteCustomAction(int ObjectType, long ObjectId,
										const std::string & FunctionName,
										ActionArgumentBase * args) = 0;


	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void DisplayTxtAction(int ObjectType, long ObjectId,
										long TextId) = 0;



	// send error message to client
	virtual void SendErrorMessage(long clientid, const std::string & messagetitle, 
											const std::string &  messagecontent) = 0;

	// open container on client side
	virtual void OpenContainer(long clientid, boost::shared_ptr<ContainerSharedInfo> sharedinfo) = 0;


	//! used by lua to get an actor Position
	virtual LbaVec3 GetActorPosition(long ActorId) = 0;

	//! used by lua to get an actor Rotation
	virtual float GetActorRotation(long ActorId) = 0;

	//! used by lua to get an actor Rotation
	virtual LbaQuaternion GetActorRotationQuat(long ActorId) = 0;

	//! used by lua to update an actor animation
	virtual void UpdateActorAnimation(long ActorId, const std::string & AnimationString) = 0;

	//! used by lua to update an actor mode
	virtual void UpdateActorMode(long ActorId, const std::string & Mode) = 0;


	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	virtual void ActorStraightWalkTo(int ScriptId, long ActorId, const LbaVec3 &Position, 
										bool asynchronus = false) = 0;

	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void ActorRotate(int ScriptId, long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation, bool asynchronus = false) = 0;

	//! used by lua to wait until an actor animation is finished
	//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
	virtual void ActorAnimate(int ScriptId, long ActorId, bool AnimationMove, bool asynchronus = false) = 0;


	//! used by lua to tell that the actor should be reserved for the script
	virtual void ReserveActor(int ScriptId, long ActorId) = 0;



	//! execute lua script given as a string
	void ExecuteScriptString( const std::string &ScriptString );



	//! called when a script is finished
	void ScriptFinished(int scriptid, bool wasasynchronus);

	//! start lua script in a separate thread
	//! return script id if successed or -1 else
	int StartScript(const std::string & FunctionName, bool inlinefunction);


	//! wait until script part is finished
	void WaitForAsyncScript(int ScriptId, int ScriptPartId);

	//! check for finished asynchronus scripts
	void CheckFinishedAsynScripts();


	//! asynchronus version of ActorStraightWalkTo
	int Async_ActorStraightWalkTo(long ActorId, const LbaVec3 &Position);

	//! asynchronus version of ActorRotate
	int Async_ActorRotate(long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation);

	//! asynchronus version of ActorAnimate
	int Async_ActorAnimate(long ActorId, bool AnimationMove);


	//! resume a thread given the id
	void ResumeThread(int scriptid);

	//! resume a thread given the id
	void StropThread(int scriptid);


protected:

	//! called when a script has finished
	virtual void ScriptFinished(int scriptid, const std::string & functioname) = 0;





protected:
	boost::shared_ptr<LuaHandlerBase>					m_luaHandler;

private:
	int													m_generatednumber;
	std::map<int, bool>									m_asyncscripts;
	std::map<int, bool>									m_waitingscripts;
};


#endif
