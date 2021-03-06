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

#ifndef _ACTOR_HANDLER_H__
#define _ACTOR_HANDLER_H__

#include <string>
#include <LbaTypes.h>
#include "Conditions.h"
#include "ScriptedActor.h"
#include "CommonTypes.h"
#include "ActorScriptPart.h"
#include <ClientServerEvents.h>

class NaviMeshHandler;
class NavMeshAgent;


/***********************************************************************
class used by lua to add objects on the map
 ***********************************************************************/
struct ActorObjectInfo
{
	//constructor
	ActorObjectInfo()
		: ObjectId(-1), AttachToActorId(-1)
	{
		DisplayDesc.ModelId = 0;

		DisplayDesc.RotX = 0;
		DisplayDesc.RotY = 0;
		DisplayDesc.RotZ = 0;

		DisplayDesc.TransX = 0;
		DisplayDesc.TransY = 0;
		DisplayDesc.TransZ = 0;

		DisplayDesc.ScaleX = 1;
		DisplayDesc.ScaleY = 1;
		DisplayDesc.ScaleZ = 1;	

		DisplayDesc.ColorR = 0;
		DisplayDesc.ColorG = 0;
		DisplayDesc.ColorB = 0;
		DisplayDesc.ColorA = 0;

		DisplayDesc.ColorMaterialType = 0;
		DisplayDesc.UseBillboard = false;
		DisplayDesc.UseTransparentMaterial = false;
		DisplayDesc.MatAlpha = 1;

		PhysicDesc.AllowFreeMove = false;
		PhysicDesc.Density = 0;
		PhysicDesc.SizeX = 0;
		PhysicDesc.SizeY = 0;
		PhysicDesc.SizeZ = 0;
		ExtraInfo.NameColorR = 0;
		ExtraInfo.NameColorG = 0;
		ExtraInfo.NameColorB = 0;


		LifeInfo.Display = false;
		ExtraInfo.Display = false;
		ExcludeFromNavMesh = false;

		HitPowerOnTouch = -1;
	}

	//constructor
	ActorObjectInfo(long id);

	// set enum for render type as lua does not map enum
	//1 - RenderOsgModel
	//2 - RenderSprite
	//3 - RenderLba1M
	//4 - RenderLba2M
	void SetRenderType(int rtype);

	// set enum for PhysicalShape as lua does not map enum
	//1 - NoShape
	//2 - BoxShape
	//3 - CapsuleShape
	//4 - SphereShape
	//5 - TriangleMeshShape
	void SetPhysicalShape(int shape);

	// set enum for SetPhysicalActorType as lua does not map enum
	//1 - StaticAType - used for static npc
	//2 - KynematicAType - used for moving npc
	//3 - DynamicAType - used for dynamic object (not implemented)
	//4 - CharControlAType - used for object movable by the player
	void SetPhysicalActorType(int ptype);


	// set the model state - used by lua instead of  the enum
	//1 - NoState
	//2 - StNormal
	//3 - StDying
	//4 - StDrowning
	//5 - StDrowningGas
	//6 - StBurning
	//7 - StSmallHurt
	//8 - StMediumHurt
	//9 - StBigHurt
	//10 - StHurtFall
	//11 - StFinishedFall
	//12 - StFalling
	//13 - StJumping
	//14 - StMovingObject
	//15 - StUseWeapon
	//16 - StImmune
	//17 - StHidden
	//18 - StScripted
	//19 - StProtectedHurt
	//20 - StRestrictedMovingObject
	//21 - StFighting
	void SetModelState(int state);

	// accessors
	int GetRenderType();
	int GetPhysicalShape();
	int GetPhysicalActorType();
	int GetModelState();

	
	//! used by lua to add color swap to object info
	void AddColorSwap(int modelpart, int oldcolor, int newcolor);


	ConditionBasePtr			Condition;


	long						ObjectId;


	LbaNet::ModelInfo			DisplayDesc;
	LbaNet::ObjectPhysicDesc	PhysicDesc;
	LbaNet::LifeManaInfo		LifeInfo;
	LbaNet::ObjectExtraInfo		ExtraInfo;

	float						HitPowerOnTouch;

	bool						ExcludeFromNavMesh;

	long						AttachToActorId;
};



/***********************************************************************
class taking care of the maping between lua and the server interfaces
 ***********************************************************************/
class ActorHandler : public ScriptedActor
{
public:
	//! constructor
	ActorHandler(const ActorObjectInfo & actorinfo);
	
	//! destructor
	virtual ~ActorHandler(void);

	//! set script handler
	void SetScriptHandler(ScriptEnvironmentBase* scripthandler);

	//! set NavMesh handler
	void SetNavMeshHandler(boost::shared_ptr<NaviMeshHandler> navmeshH);
			

	//! get actor info
	const ActorObjectInfo & GetActorInfo()
	{ return m_actorinfo; }

	//! set actor info
	void SetActorInfo(const ActorObjectInfo & ainfo);

	// save actor to lua file
	virtual void SaveToLuaFile(std::ostream & file, std::string forcedid = "");

	//! get actor id
	long GetId()
	{ return m_actorinfo.ObjectId;}

	//! clear color swap info
	void ClearColorSwap();


	//! used by lua to get an actor Position
	virtual LbaVec3 GetActorPosition(bool fromattackscript);

	//! used by lua to get an actor Rotation
	float GetActorRotation(bool fromattackscript);

	//! used by lua to get an actor Rotation
	LbaQuaternion GetActorRotationQuat(bool fromattackscript);

	//! used by lua to update an actor animation
	void UpdateActorAnimation(const std::string & AnimationString, bool updatefromlua, bool fromattackscript);

	//! used by lua to update an actor mode
	void UpdateActorMode(const std::string & Mode, bool updatefromlua, bool fromattackscript);

	//! called when a script has finished
	void ScriptFinished(int scriptid, const std::string & functioname);


	//! update Model
	void UpdateActorModel(const std::string & Model, bool updatefromlua, bool fromattackscript);

	//! update Model
	void UpdateActorOutfit(const std::string & Outfit, bool updatefromlua, bool fromattackscript);

	//! update Model
	void UpdateActorWeapon(const std::string & Weapon, bool updatefromlua, bool fromattackscript);

	//! update Model
	virtual void SendSignal(int Signalnumber);

	//! get player current position
	void TeleportTo(float PosX, float PosY, float PosZ);

	//! set rotation
	void SetRotation(float angle, bool fromattackscript);

	//! show/hide
	void ShowHide(bool Show, bool fromattackscript);


	//! process actor
	std::vector<LbaNet::ClientServerEventBasePtr> Process(double tnow, float tdiff);


	//! add script part to the script
	int AddScriptPart(ActorScriptPartBasePtr part);

	//! replace script part to the script
	void ReplaceScriptPart(ActorScriptPartBasePtr olds, ActorScriptPartBasePtr news);

	//! remove script part to the script
	void RemoveScriptPart(ActorScriptPartBasePtr part);

	//! update position of the script
	void UpdateScriptPosition(ActorScriptPartBasePtr part, int position);

	//! set the start script for movement
	void SetMovementStartScript(const std::string& functionName);

	//! get script
	std::vector<ActorScriptPartBasePtr> GetScript()
	{ return m_script; }

	//! clear the running script
	void ClearRunningScript();

	//! set if the actor is visible on the scene or not
	void setAppearDisapear(bool appear);
	bool getAppearDisapear();

	//! waypoint stuff
	std::pair<int, int> StartWaypoint(const LbaVec3 &point);
	std::pair<int, int> AddWaypoint(const LbaVec3 &point);



	//! used by lua to move an actor
	//! the actor will move using animation speed
	virtual void ActorStraightWalkTo(int ScriptId, bool asynchronus, float PosX, float PosY, float PosZ);
	virtual void ActorWalkToPoint(int ScriptId, bool asynchronus, float PosX, float PosY, float PosZ, float RotationSpeedPerSec, bool moveForward);


	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void ActorRotate(int ScriptId, bool asynchronus, float Angle, float RotationSpeedPerSec, 
					bool ManageAnimation);

	//! used by lua to wait until an actor animation is finished
	//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
	virtual void ActorAnimate(int ScriptId, bool asynchronus, bool AnimationMove, int nbAnimation);

	
	//! used by lua to move an actor or player
	//! the actor will move using speed
	virtual void ActorGoTo(int ScriptId, float PosX, float PosY, float PosZ, float Speed, bool asynchronus);
	

	//! used by lua to move an actor or player
	//! the actor will wait for signal
	virtual void ActorWaitForSignal(int ScriptId, int Signalnumber, bool asynchronus);

	
	//! used by lua to move an actor or player
	//! the actor will rotate
	virtual void ActorRotateFromPoint(int ScriptId, float Angle, float PosX, float PosY, 
													float PosZ, float Speed, bool asynchronus);

	//! used by lua to move an actor or player
	//! the actor follow waypoint
	virtual void ActorFollowWaypoint(int ScriptId, int waypointindex1, int waypointindex2, bool asynchronus);

	//! make an actor sleep for a given amount of time
	virtual void ActorSleep(int ScriptId, int nbMilliseconds, bool asynchronus);

	//! return actor type
	virtual std::string ActorType();

	//! check if is NPC
	virtual bool IsNPC(){return false;}



	//! switch actor model
	void SwitchModel(const std::string & newmodelname);

	//! revert model
	void ReverModel();



	//! check trigger on player dead
	virtual void PlayerDead(Ice::Long PlayerId){}

	//! check trigger on player leave map
	virtual void PlayerLeaveMap(Ice::Long PlayerId){}

	//! check trigger on player move
	virtual void PlayerMoved(Ice::Long PlayerId, const LbaNet::PlayerPosition &startposition,
										const LbaNet::PlayerPosition &endposition,
										const LbaNet::ModelState & state,
										const std::string & mode, bool cantarget){}


	//! check trigger on object action
	virtual void PlayerAction(Ice::Long PlayerId, const LbaNet::PlayerPosition &info,
									const std::string &ObjectMode){}


	//! stop target player
	virtual void UntargetPlayer(Ice::Long PlayerId){}

	//! get last actor event
	LbaNet::ClientServerEventBasePtr GetLastEvent();

	// AttachActor
	void AttachActor(int AttachedObjectType, long AttachedObjectId);

	// DettachActor
	void DettachActor(long AttachedObjectId);


	//! get last actor event
	LbaNet::ClientServerEventBasePtr AttachActorEvent();


	//! hurt life
	virtual void HurtLife(float amount, bool UseArmor, int HurtingActorType, 
							Ice::Long HurtingActorId, bool forcelooselife){}

	//! hurt mana
	virtual void HurtMana(float amount){}

	//! kill actor
	virtual void Kill(){}

	//! return actor current position
	LbaNet::PlayerPosition GetCurrentPosition();



	//! target player
	virtual void ForceTargetAttackPlayer(Ice::Long PlayerId){}

	//! stop target player
	virtual void ForceStopAttackTarget(Ice::Long PlayerId){}

	//! get last recorded pos
	LbaVec3 GetLastRecordPos()
	{ return m_lastrecordedpos;}

	//! set last recorded pos
	void SetLastRecordPos(LbaVec3 pos)
	{ m_lastrecordedpos = pos;}


	//! return touch hit power - used to make player loose life if actor touch him/hit him
	virtual float GetTouchHitPower(bool & IgnoreArmor);


	//! npc rotate to player
	virtual void RotateToTargettedPlayer(int ScriptId, float ToleranceAngle, float speed){}

	//! npc follow player
	virtual void FollowTargettedPlayer(int ScriptId, float DistanceStopFollow){}

	//! return targeted player
	virtual long GetTargettedAttackPlayer(){return -1;}

	//! npc use weapon
	virtual void UseWeapon(int ScriptId, int WeaponNumber, bool multishot){}

	//! check if target is in range
	virtual bool IsTargetInRange(float MaxDistance){return false;}

	//! check if target is in rotation range
	virtual float GetTargetRotationDiff(){return 360;}



	//! change actor state
	//! 1-> Normal, 2-> hurt, 3-> dead, 4-> chasing, 5-> come back, 6 -> use weapon
	//! return true if change was succesfull
	virtual bool ChangeState(int newstate){return false;}

	//! get weapon distance
	//! 1-> first contact weapon, 2 -> first distance weapon
	//! 3-> second contact weapon, 4 -> second distance weapon
	virtual float GetWeaponReachDistance(int WeaponNumber){return 0;}
	
	//! check if script is attack script
	virtual bool IsAttackScript(int ScriptId) {return false;}

	//! return true if actor is hidden
	bool IsHidden();
	
	//! check if actor is attacking
	virtual bool IsAttacking() {return false;}

	//! check if reset position
	bool ResetPosition(){return m_resetposition;}

	//! check if reset position
	virtual bool IsDead(){return false;}


	//! make actor play a sound
	void APlaySound(int SoundChannel, const std::string & soundpath, int numberTime, bool randomPitch, 
						bool updatefromlua, bool fromattackscript);

	//! make actor stop a sound
	void AStopSound(int SoundChannel, bool updatefromlua, bool fromattackscript);

	//! make actor pause a sound
	void APauseSound(int SoundChannel, bool updatefromlua, bool fromattackscript);

	//! make actor resume a sound
	void AResumeSound(int SoundChannel, bool updatefromlua, bool fromattackscript);

	//! check if player is immune from us
	virtual bool PlayerImune(long playerid){return false;}

	//! inform that npc touched player
	virtual void TouchedPlayer(long playerid, bool killed){}

	//! set the script to be used as movement script (replacing the current one)
	void setMovementScript(const std::string& functionName);

private:
	//! check if update client
	void UpdateClients(double tnow, float tdiff);
	bool ShouldforceUpdate();

public:
	//********************
	//! used by editor only
	std::vector<int>		initpolycolors;
	std::vector<int>		initspherecolors;
	std::vector<int>		initlinecolors;

	std::vector<int>		currentpolycolors;
	std::vector<int>		currentspherecolors;
	std::vector<int>		currentlinecolors;

	//! get actor info
	ActorObjectInfo & GetEditorActorInfo()
	{ return m_actorinfo; }

	//! refresh actor
	void Refresh()
	{ SetActorInfo(m_actorinfo); }


protected:

	//! create actor
	void CreateActor();

	//! pause the playing script
	void Pause();

	//! resume the playing script
	void Resume();

	//! start script
	void StartScript();

	//! return the build class
	virtual std::string LuaBuildClass();

	//! write extra lua
	virtual void ExtraLua(std::ostream & file, const std::string & name){}

	//! reset actor
	void ResetActor();

	//! show hide actor
	void ShowHideInternal(bool show);

	//! process child
	virtual void ProcessChild(double tnow, float tdiff){}

	//! update actor from attached
	void UpdateFromAttached(double tnow);

	//! navmesh functions
	void GoToPosWithNavMesh(float& PosX, float& PosY, float& PosZ, bool backward);
	void StopUsingNavMesh();

protected:
	ActorObjectInfo										m_actorinfo;
	int													m_launchedscript;
	bool												m_paused;

	ScriptEnvironmentBase*								m_scripthandler;


	float												m_saved_X;
	float												m_saved_Y;
	float												m_saved_Z;
	float												m_saved_rot;
	LbaQuaternion										m_saved_Q;
	bool												m_savedshow;
	bool												m_appear;



	std::vector<ActorScriptPartBasePtr>					m_script;

	std::vector<LbaNet::ClientServerEventBasePtr>		_events;

	bool												m_resetposition;
	bool												m_resetrotation;


	std::string											m_saved_model;

	LbaNet::NpcChangedEventPtr							m_lastevent;

	int													m_attachedactortype;
	long												m_attachedactorid;

	bool												_freemove;
	boost::shared_ptr<NaviMeshHandler>					m_navimesh;
	boost::shared_ptr<NavMeshAgent>						m_NavMAgent;

	LbaVec3												m_lastrecordedpos;

	double												m_lastupdatetime;

	std::string											m_movementStartScript;
	std::string											m_replaceMovementScript;
	bool												m_pausedMoveScript;

	LbaNet::PlayerMoveInfo								_lastupdate;
	LbaNet::PlayerMoveInfo								_currentupdate;
	float												_oldtdiff;
};

#endif
