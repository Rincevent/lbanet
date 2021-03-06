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
class DialogPart;
class Spawn;
class PointFlag;
class DynamicObject;
class Holomap;
class HolomapLocation;
class HolomapTravelPath;
struct ActorObjectInfo;

#include <boost/shared_ptr.hpp>
#include <ClientServerEvents.h>


#include "CommonTypes.h"
#include "LuaHandlerBase.h"

class ActionBase;
typedef boost::shared_ptr<ActionBase> ActionBasePtr;


// needed to use ice handles in luabnd
namespace IceInternal {
	template <class T>
	T* get_pointer(const Handle<T> & p)
	{
		return p.get();
	}
}

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
					
	// add spawn
	virtual void AddSpawn(boost::shared_ptr<Spawn> spawn) = 0;

	// add point
	virtual void AddPoint(boost::shared_ptr<PointFlag> point) = 0;
	


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
	virtual void OpenContainer(long clientid, boost::shared_ptr<LbaNet::ContainerSharedInfo> sharedinfo) = 0;


	//! used by lua to get an actor Position
	virtual LbaVec3 GetActorPosition(int ScriptId, long ActorId) = 0;

	//! used by lua to get an actor Rotation
	virtual float GetActorRotation(int ScriptId, long ActorId) = 0;

	//! used by lua to get an actor Rotation
	virtual LbaQuaternion GetActorRotationQuat(int ScriptId, long ActorId) = 0;

	//! used by lua to update an actor animation
	virtual void UpdateActorAnimation(int ScriptId, long ActorId, const std::string & AnimationString) = 0;

	//! used by lua to update an actor mode
	virtual void UpdateActorMode(int ScriptId, long ActorId, const std::string & Mode) = 0;


	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	void ActorStraightWalkTo(int ScriptId, long ActorId, const LbaVec3 &Position);

	//! used by lua to move an actor or player
	//! the actor will rotate toward and move to given point using animation speed
	void ActorWalkToPoint(int ScriptId, long ActorId, const LbaVec3 &Position, float RotationSpeedPerSec, bool moveForward);


	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	void ActorRotate(int ScriptId, long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation);

	//! used by lua to wait until an actor animation is finished
	//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
	void ActorAnimate(int ScriptId, long ActorId, bool AnimationMove, int nbAnimation);

	//! used by lua to tell that the actor should be reserved for the script
	virtual void ReserveActor(int ScriptId, long ActorId) = 0;



	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	void ActorGoTo(int ScriptId, long ActorId, const LbaVec3 &Position, float Speed);


	//! used by lua to move an actor or player
	//! the actor will change model
	virtual void UpdateActorModel(int ScriptId, long ActorId, const std::string & Name) = 0;

	//! used by lua to move an actor or player
	//! the actor will change outfit
	virtual void UpdateActorOutfit(int ScriptId, long ActorId, const std::string & Name) = 0;

	//! used by lua to move an actor or player
	//! the actor will change weapon
	virtual void UpdateActorWeapon(int ScriptId, long ActorId, const std::string & Name) = 0;


	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	void ActorWaitForSignal(int ScriptId, long ActorId, int Signalnumber);

	//! used by lua to move an actor or player
	//! the actor will change mode
	virtual void SendSignalToActor(long ActorId, int Signalnumber) = 0;


	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	virtual void TeleportActorTo(int ScriptId, long ActorId, const LbaVec3 &Position) = 0;


	//! used by lua to move an actor or player
	//! the actor change rotation
	virtual void SetActorRotation(int ScriptId, long ActorId, float Angle) = 0;
	

	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	void ActorRotateFromPoint(int ScriptId, long ActorId, float Angle, const LbaVec3 &Position, 
									float RotationSpeedPerSec);


	//! used by lua to rotate an actor
	//! the actor will follow a defined waypoint
	void ActorFollowWaypoint(int ScriptId, long ActorId, int waypointindex1, int waypointindex2);


	//! asynchronus version of ActorFollowWaypoint
	void ActorFollowGivenWaypoint(int ScriptId, long ActorId, 
									const LbaVec3 & Pm1, const LbaVec3 & P0,
									const LbaVec3 & P1, const LbaVec3 & P2, 
									const LbaVec3 & P3, const LbaVec3 & P4);

	//! used by lua to move an actor or player
	//! the actor show/hide
	virtual void ActorShowHide(int ScriptId, long ActorId, bool Show) = 0;

	//! make an actor sleep for a given amount of time
	void ActorSleep(int ScriptId, long ActorId, int nbMilliseconds);

	//! set the function to use as current movement script for the given actor
	virtual void ActorSetMovementScript(int ScriptId, long ActorId, const std::string & functionName) = 0;

	//! execute lua script given as a string
	void ExecuteScriptString( const std::string &ScriptString );



	//! called when a script is finished
	void ScriptIsFinished(int scriptid, bool wasasynchronus);

	//! start lua script in a separate thread
	//! return script id if successed or -1 else
	void StartScript(const std::string & FunctionName, bool inlinefunction, int &ThreadReference);

	//! start lua script in a separate thread
	//! return script id if successed or -1 else
	void StartScript(const std::string & FunctionName, long ActorId, bool inlinefunction, int &ThreadReference);


	//! wait until script part is finished
	void WaitForAsyncScript(int ScriptId, int ScriptPartId);

	//! check for finished asynchronus scripts
	void CheckFinishedAsynScripts();


	//! asynchronus version of ActorStraightWalkTo
	int Async_ActorStraightWalkTo(long ActorId, const LbaVec3 &Position);

	//! used by lua to move an actor or player
	//! the actor will rotate toward and move to given point using animation speed
	int Async_ActorWalkToPoint(int ScriptId, long ActorId, const LbaVec3 &Position, float RotationSpeedPerSec, bool moveForward);

	//! asynchronus version of ActorRotate
	int Async_ActorRotate(long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation);

	//! asynchronus version of ActorAnimate
	int Async_ActorAnimate(long ActorId, bool AnimationMove, int nbAnimation);

	//! asynchronus version of ActorGoTo
	int Async_ActorGoTo(long ActorId, const LbaVec3 &Position, float Speed);

	//! asynchronus version of WaitForSignal
	int Async_WaitForSignal(long ActorId, int Signalnumber);

	//! asynchronus version of ActorRotateFromPoint
	int Async_ActorRotateFromPoint(long ActorId, float Angle, const LbaVec3 &Position, 
									float RotationSpeedPerSec);

	//! asynchronus version of ActorFollowWaypoint
	int Async_ActorFollowWaypoint(long ActorId, int waypointindex1, int waypointindex2);

	//! asynchronus version of ActorFollowWaypoint
	int Async_ActorFollowGivenWaypoint(long ActorId, 
									const LbaVec3 & Pm1, const LbaVec3 & P0,
									const LbaVec3 & P1, const LbaVec3 & P2, 
									const LbaVec3 & P3, const LbaVec3 & P4);

	//! make a lua script sleep for one cycle
	void WaitOneCycle(int ScriptId);


	//! resume a thread given the id
	void ResumeThread(int scriptid);

	//! resume a thread given the id
	void StropThread(int scriptid);

	//! add/remove item from player inventory
	virtual void AddOrRemoveItem(long PlayerId, long ItemId, int number, int InformClientType) = 0;


	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void HurtActor(int ObjectType, long ObjectId, float HurtValue, bool HurtLife,
								int PlayedAnimation) = 0;


	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void KillActor(int ObjectType, long ObjectId) = 0;

	//! switch the model of an actor
	virtual void SwitchActorModel(long ActorId, const std::string & newmodelname) = 0;

	//! revert the switch the model of an actor
	virtual void RevertActorModel(long ActorId) = 0;


	//! open dialog with player
	virtual void StartDialog(long PlayerId, long NpcId, long npcnametextid, bool simpledialog, 
												boost::shared_ptr<DialogPart> dialogroot) = 0;


	//! stop target player
	virtual void NpcUntargetPlayer(long NpcId, long PlayerId) = 0;


	//! start quest
	virtual void StartQuest(long PlayerId, long Questid) = 0;

	//! end quest
	virtual void TriggerQuestEnd(long PlayerId, long Questid) = 0;

	//! condition
	virtual bool QuestStarted(long PlayerId, long Questid) = 0;

	//! condition
	virtual bool QuestFinished(long PlayerId, long Questid) = 0;

	//! condition
	virtual bool QuestAvailable(long PlayerId, long Questid) = 0;

	//! condition
	virtual bool ChapterStarted(long PlayerId, int Chapter) = 0;

	//! open shop
	virtual void OpenShop(long PlayerId, const LbaNet::ItemsMap &items, 
									const LbaNet::ItemInfo & currencyitem) = 0;

	//! open mailbox
	virtual void OpenMailbox(long PlayerId) = 0;

	//! get actor info
	virtual boost::shared_ptr<DynamicObject> GetActor(int ObjectType, long ObjectId) = 0;


	// AttachActor
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void AttachActor(long ActorId, int AttachedObjectType, long AttachedObjectId) = 0;

	// DettachActor
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void DettachActor(long ActorId, long AttachedObjectId) = 0;


	//! send event to player
	virtual void SendEvents(long PlayerId, const LbaNet::EventsSeq & evts) = 0;


	//! send event to player
	virtual int GetInventoryItemCount(long PlayerId, long Itemid) = 0;

	//! return ghost owner player
	virtual long GetGhostOwnerPlayer(long ghostid){return -1;}


	//! record player killed npc
	virtual void PlayerKilledNpc(long PlayerId, long NpcId, const LbaNet::ItemsMap & givenitems){}

	// execute custom lua function
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual bool CheckCustomCondition(int ObjectType, long ObjectId,
							const std::string & FunctionName){return false;}


	//! used by lua to get an actor Position
	virtual LbaVec3 GetGhostPosition(long PlayerId, long ActorId)
	{return LbaVec3(-1, -1, -1);}

	virtual LbaNet::PlayerPosition GetGhostPPosition(long ghostid)
	{return LbaNet::PlayerPosition();}

	//! used by lua to log into file
	void LogToFile(const std::string &text);

	//! used by lua to get an player Position
	virtual LbaVec3 GetPlayerPositionVec(long PlayerId)
	{return LbaVec3(-1, -1, -1);}

	//!  used by lua to get player position
	virtual LbaNet::PlayerPosition GetPlayerPosition(long clientid) = 0;


	//! npc rotate to player
	virtual void RotateToTargettedPlayer(int ScriptId, long ActorId, float ToleranceAngle, float speed){}

	//! npc follow player
	virtual void FollowTargettedPlayer(int ScriptId, long ActorId, float DistanceStopFollow){}

	//! npc use weapon
	//! 1-> first contact weapon, 2 -> first distance weapon
	//! 3-> second contact weapon, 4 -> second distance weapon
	virtual void UseWeapon(int ScriptId, long ActorId, int WeaponNumber){}

	//! npc start use weapon - will not stop until changing state - only usefull for distance weapon
	//! 1-> first contact weapon, 2 -> first distance weapon
	//! 3-> second contact weapon, 4 -> second distance weapon
	virtual void StartUseWeapon(int ScriptId, long ActorId, int WeaponNumber){}


	//! return targeted player
	virtual long GetTargettedAttackPlayer(long ActorId){return -1;}

	//! check if target is in range
	virtual bool IsTargetInRange(float MaxDistance, long ActorId){return false;}

	//! check if target is in rotation range
	virtual float GetTargetRotationDiff(long ActorId){return 360;}

	//! launch projectile
	virtual long LaunchProjectile(LbaNet::ProjectileInfo pinfo){return -1;}

	//! destroy projectile
	virtual void DestroyProjectile(long projectileid){}

	//! get weapon distance
	//! 1-> first contact weapon, 2 -> first distance weapon
	//! 3-> second contact weapon, 4 -> second distance weapon
	virtual float GetNpcWeaponReachDistance(long ActorId, int WeaponNumber){return 0;}

	// check if actor can play animation
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual bool CanPlayAnimation(int ObjectType, long ObjectId, const std::string & anim) = 0;


	// check if actor is moving
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual bool ActorMoving(int ObjectType, long ObjectId) = 0;



	//! check if thread is still running
	bool ThreadRunning(int ScriptId);

	// run attack script
	virtual void RunAttackScript(long ActorId, const std::string & FunctionName) = 0;


	// set DB flag
	virtual void SetDBFlag(long PlayerId, const std::string & flagid, int value) = 0;

	// get DB flag
	virtual int GetDBFlag(long PlayerId, const std::string & flagid) = 0;


	// PlayClientVideo
	virtual void PlayClientVideo(long ClientId,	const std::string & VideoPath) = 0;

	// DisplayImage
	virtual void DisplayImage(int ScriptId, const std::string & imagepath, long NbSecondDisplay, 
								bool FadeIn, const LbaColor &FadeInColor,
								bool FadeOut, const LbaColor &FadeOutColor, 
								const std::string & OptionalMusicPath) = 0;

	// DisplayScrollingText
	virtual void DisplayScrollingText(int ScriptId, const std::string & imagepath, 
										const LbaVecLong &textIds, 
										const std::string & OptionalMusicPath) = 0;


	// reset to game screen after displaying extra gl stuff
	virtual void StartDisplayExtraScreen(int ScriptId) = 0;

	// reset to game screen after displaying extra gl stuff
	virtual void EndDisplayExtraScreen() = 0;

	//! used by lua to make an actor play a sound
	//! there is 5 available channels (0 to 5)
	// play a cetain number of time (-1 = loop forever)
	virtual void ActorStartSound(int ScriptId, long ActorId, int SoundChannel, 
										const std::string & soundpath, int numberTime, bool randomPitch) = 0;

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorStopSound(int ScriptId, long ActorId, int SoundChannel) = 0;

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorPauseSound(int ScriptId, long ActorId, int SoundChannel) = 0;

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorResumeSound(int ScriptId, long ActorId, int SoundChannel) = 0;

	//! TeleportPlayerAtEndScript
	virtual void TeleportPlayerAtEndScript(int ScriptId, const std::string &newmap, long newspawn) = 0;

	//! make actor appear or disappear in map
	virtual void ActorAppearDisappear(int ScriptId, long ActorId, bool appear) = 0;

	//! make actor movement script pause until it is resumed by another command 
	virtual void ActorWaitForResume(int ScriptId, long ActorId) = 0;

	//! AddHolomap
	virtual void AddHolomap(boost::shared_ptr<Holomap> holo) = 0;

	//! AddHolomap
	virtual void AddHolomapLoc(boost::shared_ptr<HolomapLocation> holo) = 0;

	//! AddHolomap
	virtual void AddHolomapPath(boost::shared_ptr<HolomapTravelPath> holo) = 0;

	//! display holomap
	virtual void DisplayHolomap(int ScriptId, long PlayerId, int mode, long holoid) = 0;

	//! execute an action on a given zone
	virtual void ExecuteActionOnZone(ActionBasePtr action, const LbaSphere & zone, ActionArgumentBase* args) = 0;


	//! generate an id to be used for a dynamic actor creation
	virtual long GenerateDynamicActorId() = 0;

	//! remove an actor from the map
	virtual void RemoveActor(long Id) = 0;

	//! add a managed ghost to the map
	virtual long AddManagedGhost(long ManagingPlayerid, const ActorObjectInfo& ainfo, bool UseAsDecoy, bool moving) = 0;

	//! remove managed ghost from the map
	virtual void RemoveManagedGhost(long id) = 0;

	//! execute a custom action script with delay
	virtual void ExecuteDelayedAction(const std::string & fctname, long ms, int ObjectType, long actorId, ActionArgumentBase* args) = 0;

	//! play a sound to everybody
	virtual void PlaySound(const std::string & soundpath, bool Use3d, float  PosX, float  PosY, float  PosZ) = 0;

protected:

	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	virtual void InternalActorStraightWalkTo(int ScriptId, long ActorId, const LbaVec3 &Position, 
										bool asynchronus = false) = 0;

	//! used by lua to move an actor or player
	//! the actor will rotate and move using animation speed
	virtual void InternalActorWalkToPoint(int ScriptId, long ActorId, const LbaVec3 &Position, float RotationSpeedPerSec, bool moveForward, 
											bool asynchronus = false) = 0;

	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void InternalActorRotate(int ScriptId, long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation, bool asynchronus = false) = 0;

	//! used by lua to wait until an actor animation is finished
	//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
	virtual void InternalActorAnimate(int ScriptId, long ActorId, bool AnimationMove, int nbAnimation, bool asynchronus = false) = 0;


	//! used by lua to move an actor or player
	//! the actor will move using speed
	virtual void InternalActorGoTo(int ScriptId, long ActorId, const LbaVec3 &Position, float Speed, bool asynchronus = false) = 0;
	

		//! used by lua to move an actor or player
	//! the actor will wait for signal
	virtual void InternalActorWaitForSignal(int ScriptId, long ActorId, int Signalnumber, bool asynchronus = false) = 0;


	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void InternalActorRotateFromPoint(int ScriptId, long ActorId, float Angle, const LbaVec3 &Position, 
												float RotationSpeedPerSec, bool asynchronus = false) = 0;


	//! used by lua to make actor follow waypoint
	virtual void InternalActorFollowWaypoint(int ScriptId, long ActorId, int waypointindex1, 
												int waypointindex2, bool asynchronus = false) = 0;


	//! used by lua to make actor follow waypoint
	virtual void InternalActorFollowGivenWaypoint(int ScriptId, long ActorId, 
											const LbaVec3 & Pm1, const LbaVec3 & P0,
											const LbaVec3 & P1, const LbaVec3 & P2, 
											const LbaVec3 & P3, const LbaVec3 & P4, 
											bool asynchronus = false) = 0;


	//! make an actor sleep for a given amount of time
	virtual void InternalActorSleep(int ScriptId, long ActorId, int nbMilliseconds, bool asynchronus = false) = 0;


	//! called when a script has finished
	virtual void ScriptFinished(int scriptid, const std::string & functioname) = 0;


protected:
	boost::shared_ptr<LuaHandlerBase>					m_luaHandler;

private:
	int													m_generatednumber;
	std::map<int, bool>									m_asyncscripts;
	std::map<int, bool>									m_waitingscripts;

	std::map<int, int>									m_sleepingscripts;
};


#endif
