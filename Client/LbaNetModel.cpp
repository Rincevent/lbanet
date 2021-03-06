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


#include "LbaNetModel.h"
#include "LogHandler.h"
#include "PhysXEngine.h"
#include "OSGHandler.h"
#include "CameraController.h"
#include "SynchronizedTimeHandler.h"
#include "StaticObject.h"
#include "CharacterControllerLBA.h"
#include "ExternalPlayer.h"
#include "EventsQueue.h"
#include "ClientLuaHandler.h"
#include "DataLoader.h"
#include "ExternalActor.h"
#include "ProjectileHandler.h"
#include "ClientExtendedTypes.h"
#include "ItemObject.h"
#include "Localizer.h"
#include "LbaNetEngine.h"
#include "SoundObjectHandlerClient.h"
#include "HolomapHandler.h"
#include "Holomap.h"

#include "DataDirHandler.h"


/***********************************************************
	Constructor
***********************************************************/
LbaNetModel::LbaNetModel(LbaNetEngine * engineptr)
: m_playerObjectId(0), m_paused(false), m_newworld(false),
	m_videoscriptid(-1), m_fixedimagescriptid(-1), m_image_assoc_music(false),
	m_showing_loading(false), m_display_extra_screens(false), m_scripttofinish(-1),
	m_engineptr(engineptr), m_isafk(false), m_checkafk(false)
{
	LogHandler::getInstance()->LogToFile("Initializing model class...");

	//initialize controllers
	m_controllerChar = boost::shared_ptr<CharacterController>(new CharacterController());
	m_controllerCam = boost::shared_ptr<CameraController>(new CameraController());
	ResetPlayerObject();
}


/***********************************************************
	Destructor
***********************************************************/
LbaNetModel::~LbaNetModel()
{
}


/***********************************************************
set player id
***********************************************************/
void LbaNetModel::SetPlayerId(long Id)
{
	m_playerObjectId = Id;
}




/***********************************************************
change the world
***********************************************************/
void LbaNetModel::ChangeWorld(const std::string & NewWorld)
{
	// clean old world
	CleanupWorld();

	//data loader init
	DataLoader::getInstance()->SetWorldName(NewWorld);
	m_current_world_name = NewWorld;

	//script part
	m_luaHandler = boost::shared_ptr<ClientLuaHandler>(new  ClientLuaHandler());
	m_luaHandler->LoadFile("LuaCommon/ClientHelperFunctions.lua");

	// custom client file
	m_luaHandler->LoadFile("Worlds/"+m_current_world_name+"/Lua/CustomClient.lua");

	//init holomap from lua
	InitHolomap();


	// indicate that we changed world
	m_newworld = true;
}


/***********************************************************
init holomap from lua
***********************************************************/
void LbaNetModel::InitHolomap()
{
	//! clear old data
	HolomapHandler::getInstance()->clear();

	if(m_luaHandler)
	{
		m_luaHandler->LoadFile("Worlds/"+m_current_world_name+"/Lua/Holomap.lua");
		m_luaHandler->CallLua("InitHolomap", this);
	}
}




/***********************************************************
refresh lua file
***********************************************************/
void LbaNetModel::RefreshLua()
{
	// custom client file
	if(m_luaHandler)
		m_luaHandler->LoadFile("Worlds/"+m_current_world_name+"/Lua/CustomClient.lua");
}


/***********************************************************
do all check to be done when idle
***********************************************************/
void LbaNetModel::Process(double tnow, float tdiff)
{

	// finish lua scripts if needed
	if(m_scripttofinish >= 0)
	{
		int tmp = m_scripttofinish;
		m_scripttofinish = -1;
		ScriptIsFinished(tmp, false);
	}

	// load object one by one not to block the main thread forever
	//if(m_toadd.size() > 0)
	//{
	//	ObjecToAdd &tmp = m_toadd.front();
	//	ObjectInfo obj = CreateObject(tmp._OType, tmp._ObjectId, tmp._DisplayDesc, 
	//									tmp._PhysicDesc, tmp._extrainfo, tmp._lifeinfo);

	//	AddObject(tmp._OType, tmp._OwnerId, obj, tmp._DisplayDesc, tmp._extrainfo, tmp._lifeinfo, 
	//					(tmp._PhysicDesc.TypePhysO == LbaNet::CharControlAType), tmp._PhysicDesc.AllowFreeMove);

	//	m_toadd.pop_front();
	//}


	if(m_paused)
		return;


	//process start client scripts
	for(size_t i=0; i< m_tostart_scripts.size(); ++i)
	{
		int scriptid = -1;
		StartScript(m_tostart_scripts[i].first, m_tostart_scripts[i].second, scriptid);
		if(scriptid < 0)
		{
			// send back to server if script failed
			EventsQueue::getSenderQueue()->AddEvent(new LbaNet::ScriptExecutionFinishedEvent(
				SynchronizedTimeHandler::GetCurrentTimeDouble(), m_tostart_scripts[i].first, "", -1));
		}
	}
	m_tostart_scripts.clear();




	// process all _npcObjects
	{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.begin();
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator end = _npcObjects.end();
	for(; it != end; ++it)
		it->second->Process(tnow, tdiff, this);
	}

	// process all _playerObjects
	{
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.begin();
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator end = _playerObjects.end();
	for(; it != end; ++it)
		it->second->Process(tnow, tdiff, this);
	}

	// process all _ghostObjects
	{
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.begin();
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator end = _ghostObjects.end();
	for(; it != end; ++it)
		it->second->Process(tnow, tdiff, this);
	}

	// process all _itemsObjects
	{
	std::map<long, boost::shared_ptr<ItemObject> >::iterator it = _itemsObjects.begin();
	std::map<long, boost::shared_ptr<ItemObject> >::iterator end = _itemsObjects.end();
	for(; it != end; ++it)
		it->second->Process(tnow, tdiff);
	}

	

	//process projectiles
	std::map<long, boost::shared_ptr<ProjectileHandler> >::iterator itproj = _projectileObjects.begin();
	while(itproj != _projectileObjects.end())
	{
		if(itproj->second->Process(tnow, tdiff))
		{
			if(itproj->second->IsPlayerOwner(m_playerObjectId))
			{
				if(m_controllerChar)
					m_controllerChar->SetProjectileLaunched(false);
			}

			itproj->second->Clear();
			_projectileObjects.erase(itproj++);
		}
		else
			++itproj;
	}


	// process all editor Objects
	{
		std::map<long, boost::shared_ptr<DynamicObject> >::iterator it = _editorObjects.begin();
		std::map<long, boost::shared_ptr<DynamicObject> >::iterator end = _editorObjects.end();
		for(; it != end; ++it)
			it->second->Process(tnow, tdiff);
	}


	//process player object
	if(m_controllerChar)
		m_controllerChar->Process(tnow, tdiff, this);

	if(m_controllerCam)
		m_controllerCam->Process(tnow, tdiff);



	// check for finished scripts
	CheckFinishedAsynScripts();



	//turn afk after a while with no events
	if(m_controllerChar)
	{
		bool kpress = m_controllerChar->PressingAnyKey();

		if(!kpress)
		{
			if(!m_isafk)
			{
				if(!m_checkafk)
				{
					m_checkafk = true;
					m_checkafk_time = tnow;
				}
				else
				{
					if((tnow-m_checkafk_time) > 60000)
					{
						//set to afk
						m_checkafk = false;
						m_isafk = true;
						EventsQueue::getReceiverQueue()->AddEvent(new SendChatTextEvent("/afk"));
					}
				}
			}
		}
		else
		{
			if(m_isafk)
			{
				//reset afk
				m_isafk = false;
				EventsQueue::getReceiverQueue()->AddEvent(new SendChatTextEvent("/back"));
			}
			if(m_checkafk)
				m_checkafk = false;
		}
	}
}





/***********************************************************
add object to the scene
***********************************************************/
void LbaNetModel::AddObject(int OType, Ice::Long OwnerId, const ObjectInfo &desc, 
								const LbaNet::ModelInfo &DisplayDesc,
								const LbaNet::ObjectExtraInfo &extrainfo,
								const LbaNet::LifeManaInfo &lifeinfo,
								bool movable, bool freemove)
{
	boost::shared_ptr<DynamicObject> createobj;

	//special case for main player
	if(OType ==2 && (m_playerObjectId == desc.Id))
	{
		// change to character controller
		ObjectInfo tmp(desc);
		static_cast<PhysicalDescriptionWithShape *>(tmp.PhysInfo.get())->ActorType = LbaNet::CharControlAType;
		createobj = tmp.BuildSelf(OsgHandler::getInstance());
	}
	else
		createobj = desc.BuildSelf(OsgHandler::getInstance());


	// add materials
	boost::shared_ptr<DisplayObjectHandlerBase> matdisobj = createobj->GetDisplayObject();
	if(matdisobj)
	{
		matdisobj->SetTransparencyMaterial(DisplayDesc.UseTransparentMaterial, DisplayDesc.MatAlpha);
		matdisobj->SetColorMaterial(DisplayDesc.ColorMaterialType, 
									DisplayDesc.MatAmbientColorR, 
									DisplayDesc.MatAmbientColorG, 
									DisplayDesc.MatAmbientColorB, 
									DisplayDesc.MatAmbientColorA, 
									DisplayDesc.MatDiffuseColorR, 
									DisplayDesc.MatDiffuseColorG, 
									DisplayDesc.MatDiffuseColorB, 
									DisplayDesc.MatDiffuseColorA, 
									DisplayDesc.MatSpecularColorR, 
									DisplayDesc.MatSpecularColorG, 
									DisplayDesc.MatSpecularColorB, 
									DisplayDesc.MatSpecularColorA, 
									DisplayDesc.MatEmissionColorR, 
									DisplayDesc.MatEmissionColorG, 
									DisplayDesc.MatEmissionColorB, 
									DisplayDesc.MatEmissionColorA, 
									DisplayDesc.MatShininess);
	}


	switch(OType)
	{
		// 1 -> npc object
		case 1:
			{

				boost::shared_ptr<PhysicalObjectHandlerBase> physo = createobj->GetPhysicalObject();
				if(physo)
				{
					boost::shared_ptr<ActorUserData> udata = physo->GetUserData();
					if(udata)
						udata->SetAllowFreeMove(freemove);
				}
				

				_npcObjects[desc.Id] = boost::shared_ptr<ExternalActor>(new ExternalActor(createobj, DisplayDesc, movable));
				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "A_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					_npcObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectExtraInfoUpdate(extrainfo), false);
					_npcObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectLifeInfoUpdate(lifeinfo), false);
				}
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == desc.Id)
			{
				if(m_controllerChar)
					m_controllerChar->SetPhysicalCharacter(createobj, DisplayDesc);
				if(m_controllerCam)
					m_controllerCam->SetCharacter(createobj);

				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "M_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					m_controllerChar->UpdateDisplay(new LbaNet::ObjectExtraInfoUpdate(extrainfo));
					m_controllerChar->UpdateDisplay(new LbaNet::ObjectLifeInfoUpdate(lifeinfo));
				}
			}
			else
			{
				_playerObjects[desc.Id] = boost::shared_ptr<ExternalPlayer>(new ExternalPlayer(createobj, DisplayDesc));
				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "P_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					_playerObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectExtraInfoUpdate(extrainfo), false);
					_playerObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectLifeInfoUpdate(lifeinfo), false);
				}
			}
		break;

		// 3 -> ghost object
		case 3:
			if(m_playerObjectId != OwnerId)
			{
				_ghostObjects[desc.Id] = boost::shared_ptr<ExternalPlayer>(new ExternalPlayer(createobj, DisplayDesc));

				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "G_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					_ghostObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectExtraInfoUpdate(extrainfo), false);
					_ghostObjects[desc.Id]->UpdateDisplay(new LbaNet::ObjectLifeInfoUpdate(lifeinfo), false);
				}
			}
		break;


		// editor object
		case 4:
			{
				_editorObjects[desc.Id] = createobj;
				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "E_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					createobj->GetDisplayObject()->Update(new LbaNet::ObjectExtraInfoUpdate(extrainfo), false);
					createobj->GetDisplayObject()->Update(new LbaNet::ObjectLifeInfoUpdate(lifeinfo), false);
				}
			}
		break;


		// 5 -> item object
		case 5:
			if(m_playerObjectId != OwnerId)
			{
				boost::shared_ptr<PhysicalObjectHandlerBase> physo = createobj->GetPhysicalObject();
				if(physo)
				{
					boost::shared_ptr<ActorUserData> udata = physo->GetUserData();
					if(udata)
						udata->SetAllowFreeMove(true);
				}

				_itemsObjects[desc.Id] = boost::shared_ptr<ItemObject>(new ItemObject(createobj));

				if(createobj->GetDisplayObject())
				{
					std::stringstream strs;
					strs << "I_" << desc.Id;
					createobj->GetDisplayObject()->SetName(strs.str());
					createobj->GetDisplayObject()->Update(new LbaNet::ObjectExtraInfoUpdate(extrainfo), false);
					createobj->GetDisplayObject()->Update(new LbaNet::ObjectLifeInfoUpdate(lifeinfo), false);
				}
			}
		break;
	}
}


/***********************************************************
remove object from the scene
***********************************************************/
void LbaNetModel::RemObject(int OType, long id)
{
	switch(OType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(id);
			if(it != _npcObjects.end())
				_npcObjects.erase(it);
			}
		break;

		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == id)
			{
				ResetPlayerObject();
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find(id);
				if(it != _playerObjects.end())
					_playerObjects.erase(it);
			}
		break;

		// 3 -> ghost object
		case 3:
			{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find(id);
			if(it != _ghostObjects.end())
				_ghostObjects.erase(it);
			}
		break;

		// editor object
		case 4:
			{
			std::map<long, boost::shared_ptr<DynamicObject> >::iterator it = _editorObjects.find(id);
			if(it != _editorObjects.end())
				_editorObjects.erase(it);
			}
		break;


		// 5 -> item object
		case 5:
			{
			std::map<long, boost::shared_ptr<ItemObject> >::iterator it = _itemsObjects.find(id);
			if(it != _itemsObjects.end())
				_itemsObjects.erase(it);
			}
		break;
	}
}

/***********************************************************
clean up map
***********************************************************/
void LbaNetModel::CleanupMap()
{
	//clean up player
	ResetPlayerObject();

	//clear dynamic object of the current scene
	_npcObjects.clear();
	_playerObjects.clear();
	_ghostObjects.clear();
	_itemsObjects.clear();

	//clear scripts
	m_playingscriptactors.clear();
	m_controllerChar->SetProjectileLaunched(false);

	_editorObjects.clear();
}


/***********************************************************
clean up everything
***********************************************************/
void LbaNetModel::CleanupWorld()
{
	CleanupMap();
}


/***********************************************************
pause the game
***********************************************************/
void LbaNetModel::Pause()
{
	m_paused = true;
	m_checkafk = false;
}


/***********************************************************
resume the game
***********************************************************/
void LbaNetModel::Resume()
{
	m_paused = false;
}



/***********************************************************
reset player object
***********************************************************/
void LbaNetModel::ResetPlayerObject()
{
	boost::shared_ptr<PhysicalObjectHandlerBase> physo(new SimplePhysicalObjectHandler(0, 0, 0, LbaQuaternion()));
	boost::shared_ptr<DynamicObject> playerObject = boost::shared_ptr<DynamicObject>(new StaticObject(physo, boost::shared_ptr<DisplayObjectHandlerBase>(), 
		boost::shared_ptr<SoundObjectHandlerBase>(new SoundObjectHandlerClient()), m_playerObjectId));

	if(m_controllerChar)
	{
		m_controllerChar->ClearScripts();
		m_controllerChar->SetPhysicalCharacter(playerObject, LbaNet::ModelInfo(), true);
	}
	if(m_controllerCam)
		m_controllerCam->SetCharacter(playerObject, true);
}




/***********************************************************
 add object from server
 type:
1 -> npc object
2 -> player object
3 -> ghost object
***********************************************************/
ObjectInfo LbaNetModel::CreateObject(int OType, Ice::Long ObjectId, 
								const LbaNet::ModelInfo &DisplayDesc, 
								const LbaNet::ObjectPhysicDesc &PhysicDesc,
								const LbaNet::ObjectExtraInfo &extrainfo,
								const LbaNet::LifeManaInfo &lifeinfo)
{
	float sizeX=PhysicDesc.SizeX, sizeY=PhysicDesc.SizeY, sizeZ=PhysicDesc.SizeZ;
	if(sizeX < 0)
		sizeX = 1;
	if(sizeY < 0)
		sizeY = 1;
	if(sizeZ < 0)
		sizeZ = 1;

	bool mainchar = false;
	if(OType == 2)
		if(ObjectId == m_playerObjectId)
			mainchar = true;

	boost::shared_ptr<DisplayInfo> DInfo = 
		ObjectInfo::ExtractDisplayInfo(0, DisplayDesc, extrainfo, lifeinfo, mainchar, sizeX);




	// create physic object
	boost::shared_ptr<PhysicalDescriptionBase> PInfo;
	switch(PhysicDesc.TypeShape)
	{
		// 0= no shape
		case LbaNet::NoShape:
		{
			PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionNoShape(PhysicDesc.Pos.X, PhysicDesc.Pos.Y, PhysicDesc.Pos.Z, 
										LbaQuaternion(PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0))));

		}
		break;

		
		// 1 = Box
		case LbaNet::BoxShape:
		{
				PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionBox(PhysicDesc.Pos.X, PhysicDesc.Pos.Y, PhysicDesc.Pos.Z, 
										PhysicDesc.TypePhysO, PhysicDesc.Density,
										LbaQuaternion(PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0)),
										sizeX, sizeY, sizeZ,
										PhysicDesc.Collidable));
		}
		break;

		// 2 = Capsule
		case LbaNet::CapsuleShape:
		{
				PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionCapsule(PhysicDesc.Pos.X, PhysicDesc.Pos.Y, PhysicDesc.Pos.Z, 
										PhysicDesc.TypePhysO, PhysicDesc.Density,
										LbaQuaternion(PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0)),
										sizeX, sizeY,
										PhysicDesc.Collidable));
		}
		break;

		// 3 = Sphere
		case LbaNet::SphereShape:
		{
				PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionSphere(PhysicDesc.Pos.X, PhysicDesc.Pos.Y, PhysicDesc.Pos.Z, 
										PhysicDesc.TypePhysO, PhysicDesc.Density,
										LbaQuaternion(PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0)),
										sizeY, 
										PhysicDesc.Collidable, 
										PhysicDesc.Bounciness, 
										PhysicDesc.StaticFriction, PhysicDesc.DynamicFriction));
		}
		break;

		// 4 = triangle mesh
		case LbaNet::TriangleMeshShape:
		{
				PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionTriangleMesh(PhysicDesc.Pos.X, PhysicDesc.Pos.Y, PhysicDesc.Pos.Z,
										LbaQuaternion(PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0)),
										PhysicDesc.Filename,
										PhysicDesc.Collidable));
		}
		break;

	}


	if(DataDirHandler::getInstance()->IsInEditorMode())
		return ObjectInfo(OType, (long)ObjectId, DInfo, PInfo, false); // make all objects dynamic in editor as we can change them
	else
		return ObjectInfo(OType, (long)ObjectId, DInfo, PInfo, (PhysicDesc.TypePhysO == LbaNet::StaticAType));
}


/***********************************************************
 add object from server
 type:
1 -> npc object
2 -> player object
3 -> ghost object
***********************************************************/
void LbaNetModel::AddObject(int OType, Ice::Long ObjectId, Ice::Long OwnerId,
					const LbaNet::ModelInfo &DisplayDesc, 
					const LbaNet::ObjectPhysicDesc &PhysicDesc,
					const LbaNet::ObjectExtraInfo &extrainfo,
					const LbaNet::LifeManaInfo &lifeinfo)
{
	//m_toadd.push_back(ObjecToAdd(OType, ObjectId, OwnerId, DisplayDesc, PhysicDesc, extrainfo, lifeinfo));

	ObjectInfo obj = CreateObject(OType, ObjectId, DisplayDesc, 
									PhysicDesc, extrainfo, lifeinfo);

	AddObject(OType, OwnerId, obj, DisplayDesc, extrainfo, lifeinfo, 
					(PhysicDesc.TypePhysO == LbaNet::CharControlAType), PhysicDesc.AllowFreeMove);
}



/***********************************************************
 remove object from server	
 type:
1 -> npc object
2 -> player object
3 -> ghost object
***********************************************************/
void LbaNetModel::RemoveObject(int OType, Ice::Long ObjectId)
{
	RemObject(OType, (long)ObjectId);
}


/***********************************************************
update object from server
***********************************************************/
void LbaNetModel::UpdateObjectDisplay(int OType, Ice::Long ObjectId, 
									  LbaNet::DisplayObjectUpdateBasePtr update)
{
	switch(OType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
				it->second->UpdateDisplay(update, false);
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				m_controllerChar->UpdateDisplay(update);
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
					it->second->UpdateDisplay(update, false);
			}
		break;

		// 3 -> ghost object
		case 3:
			{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
			if(it != _ghostObjects.end())
				it->second->UpdateDisplay(update, false);
			}
		break;

		// editor object
		case 4:
			{
			std::map<long, boost::shared_ptr<DynamicObject> >::iterator it = _editorObjects.find((long)ObjectId);
			if(it != _editorObjects.end())
				it->second->GetDisplayObject()->Update(update, false);
			}
		break;
	}
}


/***********************************************************
update object physic
***********************************************************/
void LbaNetModel::UpdateObjectPhysic(int OType, Ice::Long ObjectId, 
								  LbaNet::PhysicObjectUpdateBasePtr update)
{
	switch(OType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
				it->second->UpdatePhysic(update);
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				m_controllerChar->UpdatePhysic(update);
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
					it->second->UpdatePhysic(update);
			}
		break;

		// 3 -> ghost object
		case 3:
			{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
			if(it != _ghostObjects.end())
				it->second->UpdatePhysic(update);
			}
		break;

		// editor object
		case 4:
			{
			std::map<long, boost::shared_ptr<DynamicObject> >::iterator it = _editorObjects.find((long)ObjectId);
			if(it != _editorObjects.end())
				it->second->GetPhysicalObject()->Update(update);
			}
		break;
	}
}


/***********************************************************
update object sound
***********************************************************/
void LbaNetModel::UpdateObjectSound(int OType, Ice::Long ObjectId, 
								  LbaNet::SoundObjectUpdateBasePtr update)
{
	switch(OType)
	{
		// 1 -> npc object
		case 1:
		{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
			{
				it->second->GetActor()->GetSoundObject()->Update(update, false);
				it->second->GetActor()->UpdateSoundPosition();
			}
		}
		break;


		// 2 -> player object
		case 2:
		{
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				m_controllerChar->GetActor()->GetSoundObject()->Update(update, false);
				m_controllerChar->GetActor()->UpdateSoundPosition();
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
				{
					it->second->GetActor()->GetSoundObject()->Update(update, false);
					it->second->GetActor()->UpdateSoundPosition();
				}
			}
		}
		break;

		// 3 -> ghost object
		case 3:
		{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
			if(it != _ghostObjects.end())
			{
				it->second->GetActor()->GetSoundObject()->Update(update, false);
				it->second->GetActor()->UpdateSoundPosition();
			}
		}
		break;

		// editor object
		case 4:
		{
			std::map<long, boost::shared_ptr<DynamicObject> >::iterator it = _editorObjects.find((long)ObjectId);
			if(it != _editorObjects.end())
			{
				it->second->GetSoundObject()->Update(update, false);
				it->second->UpdateSoundPosition();
			}
		}
		break;
	}
}





/***********************************************************
key pressed
***********************************************************/
void LbaNetModel::KeyPressed(LbanetKey keyid)
{
	if(m_controllerChar)
		m_controllerChar->KeyPressed(keyid);

	if(m_controllerCam)
		m_controllerCam->KeyPressed(keyid);
}

/***********************************************************
key released
***********************************************************/
void LbaNetModel::KeyReleased(LbanetKey keyid)
{
	if(m_controllerChar)
		m_controllerChar->KeyReleased(keyid);

	if(m_controllerCam)
		m_controllerCam->KeyReleased(keyid);
}


/***********************************************************
when update player position
***********************************************************/
void LbaNetModel::PlayerMovedUpdate(Ice::Long PlayerId, double updatetime, 
									const LbaNet::PlayerMoveInfo &info,
									bool teleport)
{
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)PlayerId);
	if(it != _playerObjects.end())
	{
		it->second->UpdateMove(updatetime, info, teleport);
	}
	else
	{
		if(teleport && (m_playerObjectId == (long)PlayerId))
		{
			m_controllerChar->Teleport(info);

			EventsQueue::getSenderQueue()->AddEvent(new LbaNet::ReadyToPlayEvent(
				SynchronizedTimeHandler::GetCurrentTimeDouble()));
		}
	}
}

/***********************************************************
when update npc position
***********************************************************/
void LbaNetModel::NpcChangedUpdate(Ice::Long NpcId, double updatetime, 
									float CurrPosX, float CurrPosY, float CurrPosZ,
									float CurrRotation, const std::string &CurrAnimation,
									bool ResetPosition, bool ResetRotation,
									const LbaNet::PlayingSoundSequence	&Sounds,
									LbaNet::NpcUpdateBasePtr Update)
{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)NpcId);
	if(it != _npcObjects.end())
	{
		it->second->NpcChangedUpdate(updatetime, CurrPosX, CurrPosY, CurrPosZ, CurrRotation,
										CurrAnimation, ResetPosition, ResetRotation, Sounds, Update, this);
	}
}




/***********************************************************
called when we enter a new map
***********************************************************/
void LbaNetModel::NewMap(const std::string & NewMap, const std::string & Script,
							int AutoCameraType, const std::string & MapDescription)
{
	Pause();

	// clean old map
	CleanupMap();

	m_current_map_name = NewMap;


	// todo remove that
	OsgHandler::getInstance()->ResetDisplayTree(0);
	LbaMainLightInfo lightinfo(0, 100, 50);
	OsgHandler::getInstance()->SetLight(0, lightinfo);


	// ask server to get a refresh of all objects
	EventsQueue::getSenderQueue()->AddEvent(new LbaNet::RefreshObjectRequestEvent(
										SynchronizedTimeHandler::GetCurrentTimeDouble()));


	OsgHandler::getInstance()->ToggleAutoCameraType(AutoCameraType);

	// update player location in chat
	std::string location = m_current_world_name + "/";
	if(MapDescription != "")
		location += MapDescription.substr(0, MapDescription.find(','));
	else
		location += m_current_map_name;

	EventsQueue::getReceiverQueue()->AddEvent(new PlayerLocationChangedEvent(location));
}


/***********************************************************
map is fully refreshed
***********************************************************/
void LbaNetModel::RefreshEnd()
{
	// tell server that we are ready to play
	EventsQueue::getSenderQueue()->AddEvent(new LbaNet::ReadyToPlayEvent(
										SynchronizedTimeHandler::GetCurrentTimeDouble()));

	OsgHandler::getInstance()->OptimizeScene();

	Resume();

	if(m_newworld)
	{
		m_newworld = false;
		RefreshPlayerPortrait();
	}


}


/***********************************************************
center the camera on player
***********************************************************/
void LbaNetModel::CenterCamera()
{
	if(m_controllerCam)
		m_controllerCam->Center();
}





/***********************************************************
used by lua to get an actor Position
if id < 1 then it get player position
***********************************************************/
LbaVec3 LbaNetModel::GetActorPosition(int ScriptId, long ActorId)
{
	LbaVec3 res;

	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->GetPosition(res.x, res.y, res.z);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->GetPosition(res.x, res.y, res.z);
	}

	return res;
}


/***********************************************************
used by lua to get an actor Rotation
if id < 1 then it get player position
***********************************************************/
float LbaNetModel::GetActorRotation(int ScriptId, long ActorId)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			return it->second->GetRotationYAxis();
	}
	else
	{
		// on player
		if(m_controllerChar)
			return m_controllerChar->GetRotation();
	}

	return 0;
}


 /***********************************************************
used by lua to get an actor Rotation
if id < 1 then it get player position
***********************************************************/
LbaQuaternion LbaNetModel::GetActorRotationQuat(int ScriptId, long ActorId)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
		{
			LbaQuaternion res;
			it->second->GetRotation(res);
			return res;
		}
	}
	else
	{
		// on player
		if(m_controllerChar)
			return m_controllerChar->GetRotationQuat();
	}

	return LbaQuaternion();
}



/***********************************************************
 used by lua to update an actor animation
 if id < 1 then it get player position
***********************************************************/
void LbaNetModel::UpdateActorAnimation(int ScriptId, long ActorId, const std::string & AnimationString)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->UpdateDisplay(new LbaNet::AnimationStringUpdate(AnimationString), true);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->UpdateAnimation(AnimationString);
	}
}



/***********************************************************
//! used by lua to update an actor mode
//! if id < 1 then it get player position
***********************************************************/
void LbaNetModel::UpdateActorMode(int ScriptId, long ActorId, const std::string & Mode)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->UpdateActorMode(Mode, true);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->UpdateActorMode(Mode);
	}
}



/***********************************************************
used by lua to move an actor or player
if id < 1 then it moves players
the actor will move using animation speed
***********************************************************/
void LbaNetModel::InternalActorStraightWalkTo(int ScriptId, long ActorId, const LbaVec3 &Position, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorStraightWalkTo(ScriptId, asynchronus, Position.x, Position.y, Position.z);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorStraightWalkTo(ScriptId, asynchronus, Position.x, Position.y, Position.z);
	}
}

void LbaNetModel::InternalActorWalkToPoint(int ScriptId, long ActorId, const LbaVec3 &Position, float RotationSpeedPerSec, bool moveForward, 
											bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorWalkToPoint(ScriptId, asynchronus, Position.x, Position.y, Position.z, RotationSpeedPerSec, moveForward);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorWalkToPoint(ScriptId, asynchronus, Position.x, Position.y, Position.z, RotationSpeedPerSec, moveForward);
	}
}



/***********************************************************
//! used by lua to rotate an actor
//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
***********************************************************/
void LbaNetModel::InternalActorRotate(int ScriptId, long ActorId, float Angle, float RotationSpeedPerSec,
									bool ManageAnimation, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorRotate(ScriptId, asynchronus, Angle, RotationSpeedPerSec, ManageAnimation);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorRotate(ScriptId, asynchronus, Angle, RotationSpeedPerSec, ManageAnimation);
	}
}


/***********************************************************
//! used by lua to wait until an actor animation is finished
//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
***********************************************************/
void LbaNetModel::InternalActorAnimate(int ScriptId, long ActorId, bool AnimationMove, int nbAnimation, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorAnimate(ScriptId, asynchronus, AnimationMove, nbAnimation);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorAnimate(ScriptId, asynchronus, AnimationMove, nbAnimation);
	}
}



/***********************************************************
//! called when a script has finished
***********************************************************/
void LbaNetModel::ScriptFinished(int scriptid, const std::string & functioname)
{
	std::string tpend_script_map = "";
	long tpend_script_spawn = -1;

	std::map<int, std::pair<std::string, long> >::iterator it =	m_tpend_script.find(scriptid);
	if(it != m_tpend_script.end())
	{
		tpend_script_map = it->second.first;
		tpend_script_spawn = it->second.second;
		m_tpend_script.erase(it);

		if(tpend_script_map != "")
			Pause();
	}


	// inform server that script is finished
	EventsQueue::getSenderQueue()->AddEvent(new LbaNet::ScriptExecutionFinishedEvent(
		SynchronizedTimeHandler::GetCurrentTimeDouble(), functioname, tpend_script_map, tpend_script_spawn));


	ReleaseActorFromScript(scriptid);
}


/***********************************************************
//! set the actor as playing script
***********************************************************/
void LbaNetModel::ReserveActor(int ScriptId, long ActorId)
{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
	if(it != _npcObjects.end())
	{
		it->second->SetPlayingScript(true);
		m_playingscriptactors[ScriptId].insert(ActorId);
	}
}


/***********************************************************
//! release scripted actors
***********************************************************/
void LbaNetModel::ReleaseActorFromScript(int scriptid)
{
	std::map<int, std::set<long> >::iterator itpl = m_playingscriptactors.find(scriptid);
	if(itpl != m_playingscriptactors.end())
	{
		std::set<long>::iterator its = itpl->second.begin();
		std::set<long>::iterator ends = itpl->second.end();
		for(; its != ends; ++its)
		{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(*its);
			if(it != _npcObjects.end())
				it->second->SetPlayingScript(false);
		}

		m_playingscriptactors.erase(itpl);
	}
}




/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will change model
***********************************************************/
void LbaNetModel::UpdateActorModel(int ScriptId, long ActorId, const std::string & Name)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->UpdateActorModel(Name, true);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->UpdateActorModel(Name);
	}
}



/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will change outfit
***********************************************************/
void LbaNetModel::UpdateActorOutfit(int ScriptId, long ActorId, const std::string & Name)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->UpdateActorOutfit(Name, true);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->UpdateActorOutfit(Name);
	}
}



/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will change weapon
***********************************************************/
void LbaNetModel::UpdateActorWeapon(int ScriptId, long ActorId, const std::string & Name)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->UpdateActorWeapon(Name, true);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->UpdateActorWeapon(Name);
	}
}



/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will change mode
***********************************************************/
void LbaNetModel::SendSignalToActor(long ActorId, int Signalnumber)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->SendSignal(Signalnumber);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->SendSignal(Signalnumber);
	}
}





/***********************************************************
//! used by lua to move an actor or player
//! the actor will move using animation speed
***********************************************************/
void LbaNetModel::TeleportActorTo(int ScriptId, long ActorId, const LbaVec3 &Position)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->TeleportTo(Position.x, Position.y, Position.z);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->TeleportTo(Position.x, Position.y, Position.z);
	}
}



/***********************************************************
//! used by lua to move an actor or player
//! the actor change rotation
***********************************************************/
void LbaNetModel::SetActorRotation(int ScriptId, long ActorId, float Angle)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->SetRotation(Angle);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->SetRotation(Angle);
	}
}


/***********************************************************
//! used by lua to move an actor or player
//! the actor show/hide
***********************************************************/
void LbaNetModel::ActorShowHide(int ScriptId, long ActorId, bool Show)
{
	if(ActorId >= 0)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ShowHide(Show);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ShowHide(Show);
	}
}



/***********************************************************
// show/hide object
// ObjectType ==>
//! 1 -> npc object
//! 2 -> player object
//! 3 -> movable object
***********************************************************/
void LbaNetModel::ShowHideActor(int ObjectType, long ObjectId, bool SHow)
{
	switch(ObjectType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
				it->second->ShowHide(SHow);
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				m_controllerChar->ShowHide(SHow);
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
					it->second->ShowHide(SHow);
			}
		break;

		// 3 -> ghost object
		case 3:
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
				if(it != _ghostObjects.end())
					it->second->ShowHide(SHow);
			}
		break;
	}
}



/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will move using speed
***********************************************************/
void LbaNetModel::InternalActorGoTo(int ScriptId, long ActorId, const LbaVec3 &Position, 
										float Speed, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorGoTo(ScriptId, Position.x, Position.y, Position.z, Speed, asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorGoTo(ScriptId, Position.x, Position.y, Position.z, Speed, asynchronus);
	}
}
	


/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will wait for signal
***********************************************************/
void LbaNetModel::InternalActorWaitForSignal(int ScriptId, long ActorId, int Signalnumber, 
												bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorWaitForSignal(ScriptId, Signalnumber, asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorWaitForSignal(ScriptId, Signalnumber, asynchronus);
	}
}

/***********************************************************
	//! used by lua to move an actor or player
	//! the actor will sleep
***********************************************************/
void LbaNetModel::InternalActorSleep(int ScriptId, long ActorId, int nbMilliseconds, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorSleep(ScriptId, nbMilliseconds, asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorSleep(ScriptId, nbMilliseconds, asynchronus);
	}
}


/***********************************************************
	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
***********************************************************/
void LbaNetModel::InternalActorRotateFromPoint(int ScriptId, long ActorId, float Angle, const LbaVec3 &Position, 
												float RotationSpeedPerSec, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorRotateFromPoint(ScriptId, Angle, 
												Position.x, Position.y, Position.z, RotationSpeedPerSec, 
												asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorRotateFromPoint(ScriptId, Angle, 
													Position.x, Position.y, Position.z, RotationSpeedPerSec, 
													asynchronus);
	}
}


/***********************************************************
 used by lua to make actor follow waypoint
***********************************************************/
void LbaNetModel::InternalActorFollowWaypoint(int ScriptId, long ActorId, int waypointindex1, 
												int waypointindex2, bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorFollowWaypoint(ScriptId, waypointindex1, waypointindex2, asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorFollowWaypoint(ScriptId, waypointindex1, waypointindex2, asynchronus);
	}
}


/***********************************************************
 used by lua to make actor follow waypoint
***********************************************************/
void LbaNetModel::InternalActorFollowGivenWaypoint(int ScriptId, long ActorId, 
											const LbaVec3 & Pm1, const LbaVec3 & P0,
											const LbaVec3 & P1, const LbaVec3 & P2, 
											const LbaVec3 & P3, const LbaVec3 & P4, 
											bool asynchronus)
{
	if(ActorId >= 0)
	{
		ReserveActor(ScriptId, ActorId);

		// on actor
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
			it->second->ActorFollowWaypoint(ScriptId, Pm1, P0, P1, P2, P3, P4, asynchronus);
	}
	else
	{
		// on player
		if(m_controllerChar)
			m_controllerChar->ActorFollowWaypoint(ScriptId, Pm1, P0, P1, P2, P3, P4, asynchronus);
	}
}



/***********************************************************
internally update player state
***********************************************************/
void LbaNetModel::UpdatePlayerState(LbaNet::ModelState	NewState)
{
	if(m_controllerChar)
		m_controllerChar->UpdateState(NewState);
}



/***********************************************************
actor target player
***********************************************************/
void LbaNetModel::NpcTargetPlayer(long ActorId, long PlayerId)
{
	boost::shared_ptr<PhysicalObjectHandlerBase> object;

	if(m_playerObjectId == (long)PlayerId)
	{
		object = m_controllerChar->GetPhysicalObject();
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)PlayerId);
		if(it != _playerObjects.end())
			object = it->second->GetPhysicalObject();
	}


	// on actor
	if(object)
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
		if(it != _npcObjects.end())
		{
			it->second->Target(object);
		}
	}
}


/***********************************************************
actor untarget player
***********************************************************/
void LbaNetModel::NpcUnTargetPlayer(long ActorId)
{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find(ActorId);
	if(it != _npcObjects.end())
		it->second->UnTarget();
}



/***********************************************************
create projectile
***********************************************************/
void LbaNetModel::CreateProjectile(const LbaNet::ProjectileInfo & Info)
{
	// get projectile owner object
	boost::shared_ptr<DynamicObject> ownerdynobj;

	if(Info.OwnerActorId >= 0)
	{
		switch(Info.OwnerActorType)
		{
			// 1 -> npc object
			case 1:
			{
				std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)Info.OwnerActorId);
				if(it != _npcObjects.end())
				{
					ownerdynobj = it->second->GetActor();
				}
			}
			break;


			// 2 -> player object
			case 2:
				//special treatment if main player
				if(m_playerObjectId == (long)Info.OwnerActorId)
				{
					if(m_controllerChar)
					{
						if(Info.Comeback)
							m_controllerChar->SetProjectileLaunched(true);

						ownerdynobj = m_controllerChar->GetActor();
					}
				}
				else
				{
					std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)Info.OwnerActorId);
					if(it != _playerObjects.end())
					{
						ownerdynobj = it->second->GetActor();
					}
				}
			break;
		}
	}

	if(ownerdynobj)
	{
		//create projectile handler
		boost::shared_ptr<ProjectileHandler> projh = boost::shared_ptr<ProjectileHandler>(
			new ProjectileHandler(this, Info, Info.ManagingClientId == m_playerObjectId, ownerdynobj,
									Info.AngleOffset, Info.SoundAtStart, Info.SoundOnBounce));
		
		_projectileObjects[(long)Info.Id] = projh;
	}
}


/***********************************************************
destroy projectile
***********************************************************/
void LbaNetModel::DestroyProjectile(long Id)
{
	std::map<long, boost::shared_ptr<ProjectileHandler> >::iterator it = _projectileObjects.find(Id);
	if(it != _projectileObjects.end())
		it->second->Destroy();
}



/***********************************************************
get actor info
***********************************************************/
boost::shared_ptr<DynamicObject> LbaNetModel::GetActor(int ObjectType, long ObjectId)
{
	switch(ObjectType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
				return it->second->GetActor();
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				return m_controllerChar->GetActor();
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
					return it->second->GetActor();
			}
		break;

		// 3 -> ghost object
		case 3:
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
				if(it != _ghostObjects.end())
					return it->second->GetActor();
			}
		break;

	}

	return boost::shared_ptr<DynamicObject>();
}



/***********************************************************
AttachActor
***********************************************************/
void LbaNetModel::AttachActor(long ActorId, int AttachedObjectType, long AttachedObjectId)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->SetAttached(GetActor(AttachedObjectType, AttachedObjectId));
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->SetAttached(GetActor(AttachedObjectType, AttachedObjectId));
	}
}

/***********************************************************
DettachActor
***********************************************************/
void LbaNetModel::DettachActor(long ActorId, long AttachedObjectId)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->SetAttached(boost::shared_ptr<DynamicObject>());
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->SetAttached(boost::shared_ptr<DynamicObject>());
	}
}



/***********************************************************
attached actor to npc
***********************************************************/
void LbaNetModel::NpcAttachActor(long NpcId, float posX, float posY, float posZ, float rotation,
							int AttachedObjectType, long AttachedObjectId)
{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)NpcId);
	if(it != _npcObjects.end())
		it->second->ServerAttachActor(GetActor(AttachedObjectType, AttachedObjectId), posX, posY, posZ, rotation);
}




/***********************************************************
refresh player portrait
***********************************************************/
void LbaNetModel::RefreshPlayerPortrait()
{
	if(!DataDirHandler::getInstance()->IsInEditorMode())
	{
		//save player image
		if(m_controllerChar)
			m_controllerChar->SavePlayerDisplayToFile(DataDirHandler::getInstance()->GetDataDirPath() + "/GUI/imagesets/charportrait.png");

		//refresh GUI
		LbaNet::GuiUpdatesSeq updseq;
		updseq.push_back(new RefreshCharPortraitUpdate());
		EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::UpdateGameGUIEvent(
			SynchronizedTimeHandler::GetCurrentTimeDouble(), "main", updseq));
	}
}


/***********************************************************
when update player position
***********************************************************/
void LbaNetModel::GhostMovedUpdate(Ice::Long GhostId, double updatetime, 
									const LbaNet::PlayerMoveInfo &info)
{
	std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)GhostId);
	if(it != _ghostObjects.end())
	{
		it->second->UpdateMove(updatetime, info, false);
	}
}

/***********************************************************
when update player position
***********************************************************/
void LbaNetModel::NPCMovedUpdate(Ice::Long NpcId, double updatetime, 
									const LbaNet::PlayerMoveInfo &info)
{
	std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)NpcId);
	if(it != _npcObjects.end())
	{
		it->second->UpdateMove(updatetime, info, false);
	}
}


/***********************************************************
called when item looted by player
***********************************************************/
void LbaNetModel::ItemLooted(long itemid)
{
	//hide item
	std::map<long, boost::shared_ptr<ItemObject> >::iterator it = _itemsObjects.find(itemid);
	if(it != _itemsObjects.end())
		it->second->Hide();

	// send to server
	EventsQueue::getSenderQueue()->AddEvent(new LbaNet::ItemLootEvent(
										SynchronizedTimeHandler::GetCurrentTimeDouble(),
										itemid));	
}



/***********************************************************
create projectile object
***********************************************************/
boost::shared_ptr<DynamicObject> LbaNetModel::CreateProjectileObject(const LbaNet::ProjectileInfo & Info)
{
	LbaNet::ObjectExtraInfo extrainfo;
	extrainfo.Display = false;
	LbaNet::LifeManaInfo lifeinfo;
	lifeinfo.Display = false;

	ObjectInfo obj = CreateObject(5, Info.Id, 
								Info.DisplayDesc, Info.PhysicDesc, extrainfo, lifeinfo);

	boost::shared_ptr<DynamicObject> dynobj = obj.BuildSelf(OsgHandler::getInstance());
	
	// add materials
	boost::shared_ptr<DisplayObjectHandlerBase> matdisobj = dynobj->GetDisplayObject();
	if(matdisobj)
	{
		matdisobj->SetTransparencyMaterial(Info.DisplayDesc.UseTransparentMaterial, Info.DisplayDesc.MatAlpha);
		matdisobj->SetColorMaterial(Info.DisplayDesc.ColorMaterialType, 
									Info.DisplayDesc.MatAmbientColorR, 
									Info.DisplayDesc.MatAmbientColorG, 
									Info.DisplayDesc.MatAmbientColorB, 
									Info.DisplayDesc.MatAmbientColorA, 
									Info.DisplayDesc.MatDiffuseColorR, 
									Info.DisplayDesc.MatDiffuseColorG, 
									Info.DisplayDesc.MatDiffuseColorB, 
									Info.DisplayDesc.MatDiffuseColorA, 
									Info.DisplayDesc.MatSpecularColorR, 
									Info.DisplayDesc.MatSpecularColorG, 
									Info.DisplayDesc.MatSpecularColorB, 
									Info.DisplayDesc.MatSpecularColorA, 
									Info.DisplayDesc.MatEmissionColorR, 
									Info.DisplayDesc.MatEmissionColorG, 
									Info.DisplayDesc.MatEmissionColorB, 
									Info.DisplayDesc.MatEmissionColorA, 
									Info.DisplayDesc.MatShininess);
	}

	return dynobj;
}

/***********************************************************
// check if actor can play animation
// ObjectType ==>
//! 1 -> npc object
//! 2 -> player object
//! 3 -> movable object
***********************************************************/
bool LbaNetModel::CanPlayAnimation(int ObjectType, long ObjectId, const std::string & anim)
{
	switch(ObjectType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ObjectId);
			if(it != _npcObjects.end())
				return it->second->CanPlayAnimation(anim);
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)ObjectId)
			{
				return m_controllerChar->CanPlayAnimation(anim);
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)ObjectId);
				if(it != _playerObjects.end())
					return it->second->CanPlayAnimation(anim);
			}
		break;

		// 3 -> ghost object
		case 3:
			{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)ObjectId);
			if(it != _ghostObjects.end())
				return it->second->CanPlayAnimation(anim);
			}
		break;
	}

	return false;
}


/***********************************************************
display shout on screen
***********************************************************/
void LbaNetModel::DisplayShout(const LbaNet::ShoutTextInfo &shoutinfo)
{
	boost::shared_ptr<DisplayObjectHandlerBase> diso;


	switch(shoutinfo.ObjectType)
	{
		// 1 -> npc object
		case 1:
			{
			std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)shoutinfo.ObjectId);
			if(it != _npcObjects.end())
				diso = it->second->GetActor()->GetDisplayObject();
			}
		break;


		// 2 -> player object
		case 2:
			//special treatment if main player
			if(m_playerObjectId == (long)shoutinfo.ObjectId)
			{
				diso = m_controllerChar->GetActor()->GetDisplayObject();
			}
			else
			{
				std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _playerObjects.find((long)shoutinfo.ObjectId);
				if(it != _playerObjects.end())
					diso = it->second->GetActor()->GetDisplayObject();
			}
		break;

		// 3 -> ghost object
		case 3:
			{
			std::map<long, boost::shared_ptr<ExternalPlayer> >::iterator it = _ghostObjects.find((long)shoutinfo.ObjectId);
			if(it != _ghostObjects.end())
				diso = it->second->GetActor()->GetDisplayObject();
			}
		break;
	}

	if(diso)
	{
		std::string txt = Localizer::getInstance()->GetText(Localizer::Map, shoutinfo.TextId);
		diso->SetShoutText(txt, shoutinfo.TextSize, shoutinfo.TextcolorR, shoutinfo.TextcolorG, shoutinfo.TextcolorB, 5);
	}
}

/***********************************************************
PlayClientVideo
***********************************************************/
void LbaNetModel::PlayClientVideo(long ScriptId, const std::string & VideoPath)
{
	m_videoscriptid = (int) ScriptId;

	//tell engine to start the video
	EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::PlayVideoSequenceEvent(
		SynchronizedTimeHandler::GetCurrentTimeDouble(), VideoPath));
}

/***********************************************************
video play finished
***********************************************************/
void LbaNetModel::ClientVideoFinished()
{
	if(m_videoscriptid >= 0)
	{
		int tmp = m_videoscriptid;
		m_videoscriptid = -1;
		ScriptIsFinished(tmp, false);
	}
	else
	{
		//tell server that video is finished
		EventsQueue::getSenderQueue()->AddEvent(new LbaNet::VideoSequenceFinishedEvent(
			SynchronizedTimeHandler::GetCurrentTimeDouble()));

		EventsQueue::getReceiverQueue()->AddEvent(new SwitchToGameEvent());
	}
}
/***********************************************************
reset to game screen after displaying extra gl stuff
***********************************************************/
void LbaNetModel::StartDisplayExtraScreen(int ScriptId)
{
	if(m_display_extra_screens)
	{
		m_waiting_display_thread.push_back(ScriptId);
	}
	else
	{
		m_display_extra_screens = true;
		m_scripttofinish = ScriptId;
	}
}



/***********************************************************
reset to game screen after displaying extra gl stuff
***********************************************************/
void LbaNetModel::EndDisplayExtraScreen()
{
	if(m_display_extra_screens)
	{
		m_display_extra_screens = false;
		EventsQueue::getReceiverQueue()->AddEvent(new SwitchToGameEvent());

		if(m_waiting_display_thread.size() > 0)
		{
			int tmp = m_waiting_display_thread.front();
			m_waiting_display_thread.pop_front();
			m_display_extra_screens = true;
			ScriptIsFinished(tmp, false);
		}
	}
}


/***********************************************************
PlayClientVideo
***********************************************************/
void LbaNetModel::DisplayImage(int ScriptId, const std::string & imagepath, long NbSecondDisplay, 
								bool FadeIn, const LbaColor &FadeInColor,
								bool FadeOut, const LbaColor &FadeOutColor, 
								const std::string & OptionalMusicPath)
{
	m_fixedimagescriptid = ScriptId;

	EventsQueue::getReceiverQueue()->AddEvent(new SwitchToFixedImageEvent(imagepath, NbSecondDisplay, 
																			FadeIn, FadeInColor.R, FadeInColor.G, FadeInColor.B,
																			FadeOut, FadeOutColor.R, FadeOutColor.G, FadeOutColor.B));

	if(OptionalMusicPath != "")
	{
		m_image_assoc_music = true;
		EventsQueue::getReceiverQueue()->AddEvent(new SwitchMusicEvent(OptionalMusicPath));
	}
	else
		m_image_assoc_music = false;
}

/***********************************************************
FixedImageDisplayFinished
***********************************************************/
void LbaNetModel::DisplayExtraGLFinished()
{
	if(m_image_assoc_music)
	{
		EventsQueue::getReceiverQueue()->AddEvent(new ResetMusicEvent());
		m_image_assoc_music = false;
	}

	if(m_fixedimagescriptid >= 0)
	{
		int tmp = m_fixedimagescriptid;
		m_fixedimagescriptid = -1;
		ScriptIsFinished(tmp, false);
	}
}


/***********************************************************
DisplayScrollingText
***********************************************************/
void LbaNetModel::DisplayScrollingText(int ScriptId, const std::string & imagepath, 
											const LbaVecLong &textIds, 
											const std::string & OptionalMusicPath)
{
	m_fixedimagescriptid = ScriptId;

	EventsQueue::getReceiverQueue()->AddEvent(new SwitchBigTextEvent(imagepath, textIds.vec));

	if(OptionalMusicPath != "")
	{
		m_image_assoc_music = true;
		EventsQueue::getReceiverQueue()->AddEvent(new SwitchMusicEvent(OptionalMusicPath));
	}
	else
		m_image_assoc_music = false;
}



/***********************************************************
// show or hide loading screen
***********************************************************/
void LbaNetModel::ShowHideLoadingScreen(bool show)
{
	// dont do anything is something else is playing on screen
	if(m_display_extra_screens)
		return;

	if(show)
	{
		//show loading screen
		m_showing_loading = true;

		SwitchToFixedImageEvent evtptr("GUI/imagesets/loading_4_3.png", -1, 
										false, 0, 0, 0, false, 0, 0, 0);
		m_engineptr->SwitchToFixedImage(&evtptr);
	}
	else
	{
		if(m_showing_loading)
		{
			m_showing_loading = false;
			EventsQueue::getReceiverQueue()->AddEvent(new SwitchToGameEvent());
		}
	}
}




/***********************************************************
//! used by lua to make an actor play a sound
//! there is 5 available channels (0 to 5)
***********************************************************/
void LbaNetModel::ActorStartSound(int ScriptId, long ActorId, int SoundChannel, 
									const std::string & soundpath, int numberTime, bool randomPitch)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->GetActor()->GetSoundObject()->APlaySound(SoundChannel, soundpath, numberTime, randomPitch);
		m_controllerChar->GetActor()->UpdateSoundPosition();
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
		{
			it->second->GetActor()->GetSoundObject()->APlaySound(SoundChannel, soundpath, numberTime, randomPitch);
			it->second->GetActor()->UpdateSoundPosition();
		}
	}
}


/***********************************************************
//! used by lua to make an actor stop a sound
//! there is 5 available channels (0 to 5)
***********************************************************/
void LbaNetModel::ActorStopSound(int ScriptId, long ActorId, int SoundChannel)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->GetActor()->GetSoundObject()->AStopSound(SoundChannel);
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->GetActor()->GetSoundObject()->AStopSound(SoundChannel);
	}
}



/***********************************************************
//! used by lua to make an actor stop a sound
//! there is 5 available channels (0 to 5)
***********************************************************/
void LbaNetModel::ActorPauseSound(int ScriptId, long ActorId, int SoundChannel)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->GetActor()->GetSoundObject()->APauseSound(SoundChannel);
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->GetActor()->GetSoundObject()->APauseSound(SoundChannel);
	}
}



/***********************************************************
//! used by lua to make an actor stop a sound
//! there is 5 available channels (0 to 5)
***********************************************************/
void LbaNetModel::ActorResumeSound(int ScriptId, long ActorId, int SoundChannel)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->GetActor()->GetSoundObject()->AResumeSound(SoundChannel);
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->GetActor()->GetSoundObject()->AResumeSound(SoundChannel);
	}
}

/***********************************************************
//! TeleportPlayerAtEndScript
***********************************************************/
void LbaNetModel::TeleportPlayerAtEndScript(int ScriptId, const std::string &newmap, long newspawn)
{
	m_tpend_script[ScriptId] = std::make_pair<std::string, long>(newmap, newspawn);
}


/***********************************************************
//! start client script
***********************************************************/
void LbaNetModel::StartClientScript(const std::string & FunctionName, bool inlinefunction)
{
	m_tostart_scripts.push_back(std::make_pair<std::string, bool>(FunctionName, inlinefunction));
}



/***********************************************************
//! ShowHideVoiceSprite
***********************************************************/
void LbaNetModel::ShowHideVoiceSprite(long ActorId, bool Show, bool Left)
{
	//special treatment if main player
	if(ActorId < 0)
	{
		m_controllerChar->GetActor()->GetDisplayObject()->DisplayOrHideTalkingIcon(Show, Left);
	}
	else
	{
		std::map<long, boost::shared_ptr<ExternalActor> >::iterator it = _npcObjects.find((long)ActorId);
		if(it != _npcObjects.end())
			it->second->GetActor()->GetDisplayObject()->DisplayOrHideTalkingIcon(Show, Left);
	}
}


/***********************************************************
//! AddHolomap
***********************************************************/
void LbaNetModel::AddHolomap(boost::shared_ptr<Holomap> holo)
{
	HolomapHandler::getInstance()->AddHolomap(holo);
}


/***********************************************************
//! AddHolomap
***********************************************************/
void LbaNetModel::AddHolomapLoc(boost::shared_ptr<HolomapLocation> holo)
{
	HolomapHandler::getInstance()->AddHolomapLoc(holo);
}


/***********************************************************
//! AddHolomap
***********************************************************/
void LbaNetModel::AddHolomapPath(boost::shared_ptr<HolomapTravelPath> holo)
{
	HolomapHandler::getInstance()->AddHolomapPath(holo);
}

/***********************************************************
//! DisplayHolomap
***********************************************************/
void LbaNetModel::DisplayHolomap(int ScriptId, long PlayerId, int mode, long holoid)
{
	if(PlayerId == -1)
	{
		m_fixedimagescriptid = ScriptId;
		LbaNet::DisplayHolomapEvent * evt = new LbaNet::DisplayHolomapEvent();
		evt->HolomapLocationOrPathId = holoid;
		evt->Mode = mode;
		EventsQueue::getReceiverQueue()->AddEvent(evt);
	}
}


/***********************************************************
editor tp the player
***********************************************************/
void LbaNetModel::EditorTpPlayer(float posx, float posy, float posz)
{
	LbaNet::PlayerMoveInfo info;
	info.CurrentPos.X = posx;
	info.CurrentPos.Y = posy;
	info.CurrentPos.Z = posz;
	info.CurrentPos.Rotation = 0;
	m_controllerChar->Teleport(info);
}


/***********************************************************
force the camera in ghost mode or not
***********************************************************/
void LbaNetModel::ForceGhost(bool force)
{
	m_controllerChar->ForceGhost(force);
	m_controllerCam->ForceGhost(force);
}
