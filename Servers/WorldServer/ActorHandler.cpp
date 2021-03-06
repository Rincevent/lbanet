#include "ActorHandler.h"
#include <fstream>
#include "ScriptEnvironmentBase.h"
#include "ObjectsDescription.h"
#include "DynamicObject.h"
#include "SynchronizedTimeHandler.h"
#include <math.h>
#include "LogHandler.h"
#include "NaviMeshHandler.h"
#include "NavMeshAgent.h"

#define	_LBA1_MODEL_ANIMATION_SPEED_	1.8f


/***********************************************************
constructor
***********************************************************/
ActorObjectInfo::ActorObjectInfo(long id)
: ObjectId(id), ExcludeFromNavMesh(false), AttachToActorId(-1)
{
	DisplayDesc.RotX = 0;
	DisplayDesc.RotY = 0;
	DisplayDesc.RotZ = 0;

	DisplayDesc.TransX = 0;
	DisplayDesc.TransY = 0;
	DisplayDesc.TransZ = 0;

	DisplayDesc.ScaleX = 1;
	DisplayDesc.ScaleY = 1;
	DisplayDesc.ScaleZ = 1;

	DisplayDesc.SkinColor = -1;
	DisplayDesc.EyesColor = -1;
	DisplayDesc.HairColor = -1;
	DisplayDesc.OutfitColor = -1;
	DisplayDesc.WeaponColor = -1;
	DisplayDesc.MountSkinColor = -1;
	DisplayDesc.MountHairColor = -1;

	DisplayDesc.ModelId = 0;
	DisplayDesc.ColorR = 1;
	DisplayDesc.ColorG = 1;
	DisplayDesc.ColorB = 1;
	DisplayDesc.ColorA = 1;

	DisplayDesc.UseBillboard = false;

	DisplayDesc.UseTransparentMaterial = false;
	DisplayDesc.MatAlpha = 1;
		
	DisplayDesc.ColorMaterialType = 0;
	DisplayDesc.MatAmbientColorR = 0.2f;
	DisplayDesc.MatAmbientColorG = 0.2f;
	DisplayDesc.MatAmbientColorB = 0.2f;
	DisplayDesc.MatAmbientColorA = 1;			
	DisplayDesc.MatDiffuseColorR = 0.8f;
	DisplayDesc.MatDiffuseColorG = 0.8f;
	DisplayDesc.MatDiffuseColorB = 0.8f;
	DisplayDesc.MatDiffuseColorA = 1;
	DisplayDesc.MatSpecularColorR = 0;
	DisplayDesc.MatSpecularColorG = 0;
	DisplayDesc.MatSpecularColorB = 0;
	DisplayDesc.MatSpecularColorA = 1;	
	DisplayDesc.MatEmissionColorR = 0;
	DisplayDesc.MatEmissionColorG = 0;
	DisplayDesc.MatEmissionColorB = 0;
	DisplayDesc.MatEmissionColorA = 1;
	DisplayDesc.MatShininess = 0;	



	PhysicDesc.Density = 1;
	PhysicDesc.SizeX = 0;
	PhysicDesc.SizeY = 0;
	PhysicDesc.SizeZ = 0;

	ExtraInfo.NameColorR = 0;
	ExtraInfo.NameColorG = 0;
	ExtraInfo.NameColorB = 0;

	HitPowerOnTouch = -1;
}





/***********************************************************
	// set enum for render type as lua does not map enum
	//1 - RenderOsgModel
	//2 - RenderSprite
	//3 - RenderLba1M
	//4 - RenderLba2M
***********************************************************/
void ActorObjectInfo::SetRenderType(int rtype)
{
	switch(rtype)
	{
		case 0:
			DisplayDesc.TypeRenderer = LbaNet::NoRender;
		break;
		case 1:
			DisplayDesc.TypeRenderer = LbaNet::RenderOsgModel;
		break;
		case 2:
			DisplayDesc.TypeRenderer = LbaNet::RenderSprite;
		break;
		case 3:
			DisplayDesc.TypeRenderer = LbaNet::RenderLba1M;
		break;
		case 4:
			DisplayDesc.TypeRenderer = LbaNet::RenderLba2M;
		break;
		case 5:
			DisplayDesc.TypeRenderer = LbaNet::RenderCross;
		break;
		case 6:
			DisplayDesc.TypeRenderer = LbaNet::RenderBox;
		break;
		case 7:
			DisplayDesc.TypeRenderer = LbaNet::RenderCapsule;
		break;
		case 8:
			DisplayDesc.TypeRenderer = LbaNet::RenderSphere;
		break;
		case 9:
			DisplayDesc.TypeRenderer = LbaNet::RenderParticle;
		break;
	}
}


/***********************************************************
	// set enum for PhysicalShape as lua does not map enum
	//1 - NoShape
	//2 - BoxShape
	//3 - CapsuleShape
	//4 - SphereShape
	//5 - TriangleMeshShape
***********************************************************/
void ActorObjectInfo::SetPhysicalShape(int shape)
{
	switch(shape)
	{
		case 1:
			PhysicDesc.TypeShape = LbaNet::NoShape;
		break;
		case 2:
			PhysicDesc.TypeShape = LbaNet::BoxShape;
		break;
		case 3:
			PhysicDesc.TypeShape = LbaNet::CapsuleShape;
		break;
		case 4:
			PhysicDesc.TypeShape = LbaNet::SphereShape;
		break;
		case 5:
			PhysicDesc.TypeShape = LbaNet::TriangleMeshShape;
		break;
	}
}

	
/***********************************************************
	// set enum for SetPhysicalActorType as lua does not map enum
	//1 - StaticAType
	//2 - KynematicAType
	//3 - DynamicAType
	//4 - CharControlAType
***********************************************************/
void ActorObjectInfo::SetPhysicalActorType(int ptype)
{
	switch(ptype)
	{
		case 1:
			PhysicDesc.TypePhysO = LbaNet::StaticAType;
		break;
		case 2:
			PhysicDesc.TypePhysO = LbaNet::KynematicAType;
		break;
		case 3:
			PhysicDesc.TypePhysO = LbaNet::DynamicAType;
		break;
		case 4:
			PhysicDesc.TypePhysO = LbaNet::CharControlAType;
		break;
	}
}


/***********************************************************
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
***********************************************************/
void ActorObjectInfo::SetModelState(int state)
{
	switch(state)
	{
		case 1:
			DisplayDesc.State = LbaNet::NoState;
		break;
		case 2:
			DisplayDesc.State = LbaNet::StNormal;
		break;
		case 3:
			DisplayDesc.State = LbaNet::StDying;
		break;
		case 4:
			DisplayDesc.State = LbaNet::StDrowning;
		break;
		case 5:
			DisplayDesc.State = LbaNet::StDrowningGas;
		break;
		case 6:
			DisplayDesc.State = LbaNet::StBurning;
		break;
		case 7:
			DisplayDesc.State = LbaNet::StSmallHurt;
		break;
		case 8:
			DisplayDesc.State = LbaNet::StMediumHurt;
		break;
		case 9:
			DisplayDesc.State = LbaNet::StBigHurt;
		break;
		case 10:
			DisplayDesc.State = LbaNet::StHurtFall;
		break;
		case 11:
			DisplayDesc.State = LbaNet::StFinishedFall;
		break;
		case 12:
			DisplayDesc.State = LbaNet::StFalling;
		break;
		case 13:
			DisplayDesc.State = LbaNet::StJumping;
		break;
		case 14:
			DisplayDesc.State = LbaNet::StMovingObject;
		break;
		case 15:
			DisplayDesc.State = LbaNet::StUseWeapon;
		break;
		case 16:
			DisplayDesc.State = LbaNet::StImmune;
		break;
		case 17:
			DisplayDesc.State = LbaNet::StHidden;
		break;
		case 18:
			DisplayDesc.State = LbaNet::StScripted;
		break;
		case 19:
			DisplayDesc.State = LbaNet::StProtectedHurt;
		break;
		case 20:
			DisplayDesc.State = LbaNet::StRestrictedMovingObject;
		break;
		case 21:
			DisplayDesc.State = LbaNet::StFighting;
		break;
	}
}


/***********************************************************
accessor
***********************************************************/
int ActorObjectInfo::GetRenderType()
{
	switch(DisplayDesc.TypeRenderer)
	{
		case LbaNet::RenderOsgModel:
			return 1;
		break;
		case LbaNet::RenderSprite:
			return 2;
		break;
		case LbaNet::RenderLba1M:
			return 3;
		break;
		case LbaNet::RenderLba2M:
			return 4;
		break;
		case LbaNet::RenderCross:
			return 5;
		break;
		case LbaNet::RenderBox:
			return 6;
		break;
		case LbaNet::RenderCapsule:
			return 7;
		break;
		case LbaNet::RenderSphere:
			return 8;
		break;
		case LbaNet::RenderParticle:
			return 9;
		break;
	}

	return 0;
}

/***********************************************************
accessor
***********************************************************/
int ActorObjectInfo::GetPhysicalShape()
{
	switch(PhysicDesc.TypeShape)
	{
		case LbaNet::NoShape:
			return 1;
		break;
		case LbaNet::BoxShape:
			return 2;
		break;
		case LbaNet::CapsuleShape:
			return 3;
		break;
		case LbaNet::SphereShape:
			return 4;
		break;
		case LbaNet::TriangleMeshShape:
			return 5;
		break;
	}

	return 0;
}

/***********************************************************
accessor
***********************************************************/
int ActorObjectInfo::GetPhysicalActorType()
{
	switch(PhysicDesc.TypePhysO)
	{
		case LbaNet::StaticAType:
			return 1;
		break;
		case LbaNet::KynematicAType:
			return 2;
		break;
		case LbaNet::DynamicAType:
			return 3;
		break;
		case LbaNet::CharControlAType:
			return 4;
		break;
	}

	return 0;
}

/***********************************************************
accessor
***********************************************************/
int ActorObjectInfo::GetModelState()
{
	switch(DisplayDesc.State)
	{
		case LbaNet::NoState:
			return 1;
		break;
		case LbaNet::StNormal:
			return 2;
		break;
		case LbaNet::StDying:
			return 3;
		break;
		case LbaNet::StDrowning:
			return 4;
		break;
		case LbaNet::StDrowningGas:
			return 5;
		break;
		case LbaNet::StBurning:
			return 6;
		break;
		case LbaNet::StSmallHurt:
			return 7;
		break;
		case LbaNet::StMediumHurt:
			return 8;
		break;
		case LbaNet::StBigHurt:
			return 9;
		break;
		case LbaNet::StHurtFall:
			return 10;
		break;
		case LbaNet::StFinishedFall:
			return 11;
		break;
		case LbaNet::StFalling:
			return 12;
		break;
		case LbaNet::StJumping:
			return 13;
		break;
		case LbaNet::StMovingObject:
			return 14;
		break;
		case LbaNet::StUseWeapon:
			return 15;
		break;
		case LbaNet::StImmune:
			return 16;
		break;
		case LbaNet::StHidden:
			return 17;
		break;
		case LbaNet::StScripted:
			return 18;
		break;
		case LbaNet::StProtectedHurt:
			return 19;
		break;
		case LbaNet::StRestrictedMovingObject:
			return 20;
		break;
		case LbaNet::StFighting:
			return 21;
		break;
	}

	return 0;
}


 /***********************************************************
used by lua to add color swap to object info
***********************************************************/
void ActorObjectInfo::AddColorSwap(int modelpart, int oldcolor, int newcolor)
{
	switch(modelpart)
	{
		case 1:
		{
			LbaNet::Lba1ColorIndex idx;
			idx.ModelPart = LbaNet::PolygonColor;
			idx.Color = oldcolor;
			DisplayDesc.ColorSwaps[idx] = newcolor;
		}
		break;

		case 2:
		{
			LbaNet::Lba1ColorIndex idx;
			idx.ModelPart = LbaNet::SphereColor;
			idx.Color = oldcolor;
			DisplayDesc.ColorSwaps[idx] = newcolor;
		}
		break;

		case 3:
		{
			LbaNet::Lba1ColorIndex idx;
			idx.ModelPart = LbaNet::LineColor;
			idx.Color = oldcolor;
			DisplayDesc.ColorSwaps[idx] = newcolor;
		}
		break;
	}
}









/***********************************************************
constructor
***********************************************************/
ActorHandler::ActorHandler(const ActorObjectInfo & actorinfo)
: m_launchedscript(-1), m_paused(false), m_scripthandler(NULL), _oldtdiff(1), 
	m_resetposition(false), m_resetrotation(false), _freemove(false),
	m_attachedactorid(-1), m_lastupdatetime(0), m_appear(true), m_pausedMoveScript(false)
{
	SetActorInfo(actorinfo);
	m_lastrecordedpos = LbaVec3(m_actorinfo.PhysicDesc.Pos.X,
								m_actorinfo.PhysicDesc.Pos.Y,
								m_actorinfo.PhysicDesc.Pos.Z);
}


/***********************************************************
destructor
***********************************************************/
ActorHandler::~ActorHandler(void)
{
	ClearRunningScript();
	m_scripthandler = NULL;
}


/***********************************************************
set NavMesh handler
***********************************************************/
void ActorHandler::SetNavMeshHandler(boost::shared_ptr<NaviMeshHandler> navmeshH)
{
	m_navimesh = navmeshH;
	CreateActor();
}


void ActorHandler::setAppearDisapear(bool appear)
{
	m_appear = appear;
}

bool ActorHandler::getAppearDisapear()
{
	return m_appear;
}


/***********************************************************
clear the running script
***********************************************************/
void ActorHandler::ClearRunningScript()
{
	//destroy launched script
	if(m_launchedscript > 0 && m_scripthandler)
	{
		m_scripthandler->StropThread(m_launchedscript);
	}

	m_launchedscript = -1;
	ClearAllScripts();
	if(_character)
		_character->ClearWaypoints();
}




/***********************************************************
waypoint stuff
***********************************************************/
std::pair<int, int> ActorHandler::StartWaypoint(const LbaVec3 &point)
{
	if(_character)
		return _character->StartWaypoint(point);

	return std::make_pair<int, int>(0, 0);
}

/***********************************************************
waypoint stuff
***********************************************************/
std::pair<int, int> ActorHandler::AddWaypoint(const LbaVec3 &point)
{
	if(_character)
		return _character->AddWaypoint(point);

	return std::make_pair<int, int>(0, 0);
}



/***********************************************************
set script handler
***********************************************************/
void ActorHandler::SetScriptHandler(ScriptEnvironmentBase* scripthandler)
{
	m_scripthandler = scripthandler;
}


/***********************************************************
set actor info
***********************************************************/
void ActorHandler::SetActorInfo(const ActorObjectInfo & ainfo)
{
	m_actorinfo = ainfo;

	//TODO - maybe not always be the default
	m_actorinfo.LifeInfo.Display = false;
	m_actorinfo.ExtraInfo.Display = false;

#ifndef EXTRACT_INFO
	CreateActor();
#endif

}




/***********************************************************
save actor to lua file
***********************************************************/
void ActorHandler::SaveToLuaFile(std::ostream & file, std::string forcedid)
{
	std::stringstream objidstrs;
	objidstrs<<m_actorinfo.ObjectId;

	std::string objidstr = objidstrs.str();
	if(forcedid != "")
		objidstr = forcedid;


	std::string actName = "lmap.Actor";


	file<<"\t" << actName << " = ActorObjectInfo("<<objidstr<<")"<<std::endl;
	file<<"\t" << actName << ":SetRenderType("<<m_actorinfo.GetRenderType()<<")"<<std::endl;

	file<<"\t" << actName << ".HitPowerOnTouch = "<<m_actorinfo.HitPowerOnTouch<<std::endl;
	file<<"\t" << actName << ".ExcludeFromNavMesh = "<<(m_actorinfo.ExcludeFromNavMesh?"true":"false")<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ModelId = "<<m_actorinfo.DisplayDesc.ModelId<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ModelName = \""<<m_actorinfo.DisplayDesc.ModelName<<"\""<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.Outfit = \""<<m_actorinfo.DisplayDesc.Outfit<<"\""<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.Weapon = \""<<m_actorinfo.DisplayDesc.Weapon<<"\""<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.Mode = \""<<m_actorinfo.DisplayDesc.Mode<<"\""<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.UseLight = "<<(m_actorinfo.DisplayDesc.UseLight?"true":"false")<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.CastShadow = "<<(m_actorinfo.DisplayDesc.CastShadow?"true":"false")<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ColorR = "<<m_actorinfo.DisplayDesc.ColorR<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ColorG = "<<m_actorinfo.DisplayDesc.ColorG<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ColorB = "<<m_actorinfo.DisplayDesc.ColorB<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ColorA = "<<m_actorinfo.DisplayDesc.ColorA<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.TransX = "<<m_actorinfo.DisplayDesc.TransX<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.TransY = "<<m_actorinfo.DisplayDesc.TransY<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.TransZ = "<<m_actorinfo.DisplayDesc.TransZ<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ScaleX = "<<m_actorinfo.DisplayDesc.ScaleX<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ScaleY = "<<m_actorinfo.DisplayDesc.ScaleY<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.ScaleZ = "<<m_actorinfo.DisplayDesc.ScaleZ<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.RotX = "<<m_actorinfo.DisplayDesc.RotX<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.RotY = "<<m_actorinfo.DisplayDesc.RotY<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.RotZ = "<<m_actorinfo.DisplayDesc.RotZ<<std::endl;
	file<<"\t" << actName << ":SetModelState("<<m_actorinfo.GetModelState()<<")"<<std::endl;
	file<<"\t" << actName << ".DisplayDesc.UseBillboard = "<<(m_actorinfo.DisplayDesc.UseBillboard?"true":"false")<<std::endl;
	
	if(m_actorinfo.DisplayDesc.UseTransparentMaterial)
	{
		file<<"\t" << actName << ".DisplayDesc.UseTransparentMaterial = true"<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatAlpha = "<<m_actorinfo.DisplayDesc.MatAlpha<<std::endl;
	}

	if(m_actorinfo.DisplayDesc.ColorMaterialType > 0)
	{
		file<<"\t" << actName << ".DisplayDesc.ColorMaterialType = "<<m_actorinfo.DisplayDesc.ColorMaterialType<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatAmbientColorR = "<<m_actorinfo.DisplayDesc.MatAmbientColorR<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatAmbientColorG = "<<m_actorinfo.DisplayDesc.MatAmbientColorG<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatAmbientColorB = "<<m_actorinfo.DisplayDesc.MatAmbientColorB<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatAmbientColorA = "<<m_actorinfo.DisplayDesc.MatAmbientColorA<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatDiffuseColorR = "<<m_actorinfo.DisplayDesc.MatDiffuseColorR<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatDiffuseColorG = "<<m_actorinfo.DisplayDesc.MatDiffuseColorG<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatDiffuseColorB = "<<m_actorinfo.DisplayDesc.MatDiffuseColorB<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatDiffuseColorA = "<<m_actorinfo.DisplayDesc.MatDiffuseColorA<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatSpecularColorR = "<<m_actorinfo.DisplayDesc.MatSpecularColorR<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatSpecularColorG = "<<m_actorinfo.DisplayDesc.MatSpecularColorG<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatSpecularColorB = "<<m_actorinfo.DisplayDesc.MatSpecularColorB<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatSpecularColorA = "<<m_actorinfo.DisplayDesc.MatSpecularColorA<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatEmissionColorR = "<<m_actorinfo.DisplayDesc.MatEmissionColorR<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatEmissionColorG = "<<m_actorinfo.DisplayDesc.MatEmissionColorG<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatEmissionColorB = "<<m_actorinfo.DisplayDesc.MatEmissionColorB<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatEmissionColorA = "<<m_actorinfo.DisplayDesc.MatEmissionColorA<<std::endl;
		file<<"\t" << actName << ".DisplayDesc.MatShininess = "<<m_actorinfo.DisplayDesc.MatShininess<<std::endl;
	}

	if (m_actorinfo.DisplayDesc.TypeRenderer == LbaNet::RenderParticle)
	{
		LbaNet::ModelExtraInfoParticlePtr particleinfo = LbaNet::ModelExtraInfoParticlePtr::dynamicCast(m_actorinfo.DisplayDesc.ExtraInfo);
		file<<"\t" << actName << "ExtraInfo = ModelExtraInfoParticle()"<<std::endl;
		file<<"\t" << actName << "ExtraInfo.Type = ";
		switch(particleinfo->Type)
		{
			case LbaNet::ParticleExplosion:
				file<<"ModelExtraInfoParticle.ParticleExplosion";
			break;
			case LbaNet::ParticleExplosionDebris:
				file<<"ModelExtraInfoParticle.ParticleExplosionDebris";
			break;
			case LbaNet::ParticleSmoke:
				file<<"ModelExtraInfoParticle.ParticleSmoke";
			break;
			case LbaNet::ParticleSmokeTrail:
				file<<"ModelExtraInfoParticle.ParticleSmokeTrail";
			break;
			case LbaNet::ParticleFire:
				file<<"ModelExtraInfoParticle.ParticleFire";
			break;
			case LbaNet::ParticleCustom:
				file<<"ModelExtraInfoParticle.ParticleCustom";
			break;
		}		
		file<<std::endl;
		if (particleinfo->Type != LbaNet::ParticleCustom)
		{
			LbaNet::PredefinedParticleInfoPtr extrainfo = LbaNet::PredefinedParticleInfoPtr::dynamicCast(particleinfo->Info);
			file<<"\t" << actName << "ParticleExtraInfo = PredefinedParticleInfo()"<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.WindX = "<<extrainfo->WindX<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.WindY = "<<extrainfo->WindY<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.WindZ = "<<extrainfo->WindZ<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.Scale = "<<extrainfo->Scale<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.Intensity = "<<extrainfo->Intensity<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.EmitterDuration = "<<extrainfo->EmitterDuration<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.ParticleDuration = "<<extrainfo->ParticleDuration<<std::endl;
			file<<"\t" << actName << "ParticleExtraInfo.CustomTexture = \""<<extrainfo->CustomTexture<<"\""<<std::endl;
			file<<"\t" << actName << "ExtraInfo.Info = " << actName << "ParticleExtraInfo"<<std::endl;
		}
		else
		{
			// TODO
		}
		file<<"\t" << actName << ".DisplayDesc.ExtraInfo = " << actName << "ExtraInfo"<<std::endl;
	}


	// add color swaps
	{
		LbaNet::Lba1ColorChangeSeq::iterator itsw = m_actorinfo.DisplayDesc.ColorSwaps.begin();
		LbaNet::Lba1ColorChangeSeq::iterator endsw = m_actorinfo.DisplayDesc.ColorSwaps.end();
		for(;itsw != endsw; ++itsw)
		{
			int swaptype;
			switch(itsw->first.ModelPart)
			{
				case LbaNet::PolygonColor:
					swaptype = 1;
				break;

				case LbaNet::SphereColor:
					swaptype = 2;
				break;

				case LbaNet::LineColor:
					swaptype = 3;
				break;
			}

			file<<"\t" << actName << ":AddColorSwap("<<swaptype<<","<<itsw->first.Color<<","<<itsw->second<<")"<<std::endl;
		}
	}


	file<<"\t" << actName << ".PhysicDesc.Pos.X = "<<m_actorinfo.PhysicDesc.Pos.X<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Pos.Y = "<<m_actorinfo.PhysicDesc.Pos.Y<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Pos.Z = "<<m_actorinfo.PhysicDesc.Pos.Z<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Pos.Rotation = "<<m_actorinfo.PhysicDesc.Pos.Rotation<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Density = "<<m_actorinfo.PhysicDesc.Density<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Collidable = "<<(m_actorinfo.PhysicDesc.Collidable?"true":"false")<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.SizeX = "<<m_actorinfo.PhysicDesc.SizeX<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.SizeY = "<<m_actorinfo.PhysicDesc.SizeY<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.SizeZ = "<<m_actorinfo.PhysicDesc.SizeZ<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.AllowFreeMove = "<<(m_actorinfo.PhysicDesc.AllowFreeMove?"true":"false")<<std::endl;
	file<<"\t" << actName << ".PhysicDesc.Filename = \""<<m_actorinfo.PhysicDesc.Filename<<"\""<<std::endl;
	file<<"\t" << actName << ":SetPhysicalActorType("<<m_actorinfo.GetPhysicalActorType()<<")"<<std::endl;
	file<<"\t" << actName << ":SetPhysicalShape("<<m_actorinfo.GetPhysicalShape()<<")"<<std::endl;

	file<<"\t" << actName << ".ExtraInfo.Name = \""<<m_actorinfo.ExtraInfo.Name<<"\""<<std::endl;
	file<<"\t" << actName << ".ExtraInfo.NameColorR = "<<m_actorinfo.ExtraInfo.NameColorR<<std::endl;
	file<<"\t" << actName << ".ExtraInfo.NameColorG = "<<m_actorinfo.ExtraInfo.NameColorG<<std::endl;
	file<<"\t" << actName << ".ExtraInfo.NameColorB = "<<m_actorinfo.ExtraInfo.NameColorB<<std::endl;
	file<<"\t" << actName << ".ExtraInfo.Display = "<<(m_actorinfo.ExtraInfo.Display?"true":"false")<<std::endl;

	file<<"\t" << actName << ".LifeInfo.Display = "<<(m_actorinfo.LifeInfo.Display?"true":"false")<<std::endl;

	if(m_actorinfo.Condition)
	{
		std::stringstream condname;
		condname << actName << "_cond";
		m_actorinfo.Condition->SaveToLuaFile(file, condname.str());

		file<<"\t" << actName << ".Condition = "<<condname.str()<<std::endl;
	}

	file<<"\t" << actName << ".AttachToActorId = "<<m_actorinfo.AttachToActorId<<std::endl;


	file<<"\t" << actName << "H = "<<LuaBuildClass()<<std::endl;


	// register script
	for(size_t i=0; i< m_script.size(); ++i)
	{
		std::stringstream scname;
		scname<<"" << actName << "_Sc"<<i;

		m_script[i]->SaveToLuaFile(file, scname.str());
		file<<"\t" << actName << "H:AddScriptPart("<<scname.str()<<")"<<std::endl;
	}

	if (m_movementStartScript != "")
	{
		file<<"\t" << actName << "H:SetMovementStartScript(\""<<m_movementStartScript<<"\")"<<std::endl;
	}
	file<<"\t" << actName << "H:setAppearDisapear("<<(m_appear?"true":"false")<<")"<<std::endl;
	

	std::stringstream actorname;
	actorname << actName << "H";
	ExtraLua(file, actorname.str());

	file<<"\tenvironment:AddActorObject(" << actName << "H)"<<std::endl<<std::endl;
}


/***********************************************************
return the build class
***********************************************************/
std::string ActorHandler::LuaBuildClass()
{
	std::stringstream res;
	res<<"ActorHandler(lmap.Actor)";
	return res.str();
}



/***********************************************************
clear color swap info
***********************************************************/
void ActorHandler::ClearColorSwap()
{
	m_actorinfo.DisplayDesc.ColorSwaps.clear();
}



/***********************************************************
used by lua to get an actor Position
***********************************************************/
LbaVec3 ActorHandler::GetActorPosition(bool fromattackscript)
{
	if(!fromattackscript && m_paused)
		return LbaVec3(m_saved_X, m_saved_Y, m_saved_Z);

	if(_character)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		if(physO)
		{
			float X, Y, Z;
			physO->GetPosition(X, Y, Z);
			return LbaVec3(X, Y, Z);
		}
	}

	return LbaVec3();
}

/***********************************************************
used by lua to get an actor Rotation
***********************************************************/
float ActorHandler::GetActorRotation(bool fromattackscript)
{
	if(!fromattackscript && m_paused)
		return m_saved_rot;

	if(_character)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		if(physO)
			return physO->GetRotationYAxis();
	}

	return 0;
}

/***********************************************************
used by lua to get an actor Rotation
***********************************************************/
LbaQuaternion ActorHandler::GetActorRotationQuat(bool fromattackscript)
{
	if(!fromattackscript && m_paused)
		return m_saved_Q;

	if(_character)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		if(physO)
		{
			LbaQuaternion Q;
			physO->GetRotation(Q);
			return Q;
		}
	}

	return LbaQuaternion();
}

/***********************************************************
used by lua to update an actor animation
***********************************************************/
void ActorHandler::UpdateActorAnimation(const std::string & AnimationString, bool updatefromlua, 
																	bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
		if(disO)
		{
			if(disO->GetCurrentAnimation() != AnimationString)
			{
				disO->Update(new LbaNet::AnimationStringUpdate(AnimationString), 
											updatefromlua && m_paused && !fromattackscript);

				if(!updatefromlua)
					_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
										SynchronizedTimeHandler::GetCurrentTimeDouble(), 
										1, m_actorinfo.ObjectId,
										new LbaNet::AnimationStringUpdate(AnimationString)));
			}
		}
	}
}

/***********************************************************
used by lua to update an actor mode
***********************************************************/
void ActorHandler::UpdateActorMode(const std::string & Mode, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
		if(disO)
		{
			LbaNet::ModelInfo model = disO->GetCurrentModel(updatefromlua && m_paused && !fromattackscript);
			model.Mode = Mode;
			disO->Update(new LbaNet::ModelUpdate(model, false), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::ModelUpdate(model, false)));
		}
	}
}



/***********************************************************
update Model
***********************************************************/
void ActorHandler::UpdateActorModel(const std::string & Model, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
		if(disO)
		{
			LbaNet::ModelInfo model = disO->GetCurrentModel(updatefromlua && m_paused && !fromattackscript);
			model.ModelName = Model;
			disO->Update(new LbaNet::ModelUpdate(model, false), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::ModelUpdate(model, false)));
		}
	}
}


/***********************************************************
update outfit
***********************************************************/
void ActorHandler::UpdateActorOutfit(const std::string & Outfit, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
		if(disO)
		{
			LbaNet::ModelInfo model = disO->GetCurrentModel(updatefromlua && m_paused && !fromattackscript);
			model.Outfit = Outfit;
			disO->Update(new LbaNet::ModelUpdate(model, false), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::ModelUpdate(model, false)));
		}
	}
}


/***********************************************************
update weapon
***********************************************************/
void ActorHandler::UpdateActorWeapon(const std::string & Weapon, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
		if(disO)
		{
			LbaNet::ModelInfo model = disO->GetCurrentModel(updatefromlua && m_paused && !fromattackscript);
			model.Weapon = Weapon;
			disO->Update(new LbaNet::ModelUpdate(model, false), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::ModelUpdate(model, false)));
		}
	}
}



/***********************************************************
update signal
***********************************************************/
void ActorHandler::SendSignal(int Signalnumber)
{
	if(_character)
		_character->AddSignal(Signalnumber);

}



/***********************************************************
teleport
***********************************************************/
void ActorHandler::TeleportTo(float PosX, float PosY, float PosZ)
{
	if(m_paused)
	{
		m_saved_X = PosX;
		m_saved_Y = PosY;
		m_saved_Z = PosZ;
	}
	else
	{
		if(_character)
		{
			boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
			if(physO)
			{
				physO->SetPosition(PosX, PosY, PosZ);

				m_resetposition = true;
			}
		}
	}
}



/***********************************************************
set rotation
***********************************************************/
void ActorHandler::SetRotation(float angle, bool fromattackscript)
{
	if(m_paused && !fromattackscript)
	{
		m_saved_Q = LbaQuaternion();
		m_saved_Q.AddSingleRotation(angle, LbaVec3(0, 1, 0));
	}
	else
	{
		if(_character)
		{
			boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
			if(physO)
			{
				LbaQuaternion Q;
				Q.AddSingleRotation(angle, LbaVec3(0, 1, 0));
				physO->SetRotation(Q);

				m_resetrotation = true;
			}
		}
	}
}

/***********************************************************
show/hide
***********************************************************/
void ActorHandler::ShowHide(bool Show, bool fromattackscript)
{
	if(m_paused && !fromattackscript)
	{
		m_savedshow = Show;
	}
	else
	{
		ShowHideInternal(Show);
	}
}





/***********************************************************
called when a script has finished
***********************************************************/
void ActorHandler::ScriptFinished(int scriptid, const std::string & functioname)
{
	if(m_launchedscript == scriptid)
		m_launchedscript = -1;
}



/***********************************************************
process actor
***********************************************************/
std::vector<LbaNet::ClientServerEventBasePtr> ActorHandler::Process(double tnow, float tdiff)
{
	if(m_scripthandler && !m_paused)
	{
		// attach to actor if not yet done
		if (m_actorinfo.AttachToActorId > 0 && m_attachedactorid < 0)
		{
			AttachActor(1, m_actorinfo.AttachToActorId);
		}


		if(m_launchedscript < 0)
		{
			// start script if needed 
			StartScript();
		}

		if(m_replaceMovementScript != "")
		{
			// clear old script and start new one
			ClearRunningScript();
			std::string tmp = m_replaceMovementScript;
			m_replaceMovementScript = "";
			m_pausedMoveScript = false;
			m_scripthandler->StartScript(tmp, GetId(), false, m_launchedscript);
		}

		// process script in normal case
		if(m_launchedscript >= 0 && !m_pausedMoveScript)
		{
			// process script
			if(!ProcessScript(tnow, tdiff, m_scripthandler))
			{
				// if not moved check attached
				UpdateFromAttached(tnow);
			}
		}
		else
		{
			UpdateFromAttached(tnow);
		}
	}

	// process child
	ProcessChild(tnow, tdiff);


	//check if update clients
	if(_freemove)
		UpdateClients(tnow, tdiff);


	//send events
	std::vector<LbaNet::ClientServerEventBasePtr> res;
	res.swap(_events);
	return res;
}


	//! update actor from attached

/***********************************************************
create actor
***********************************************************/
void ActorHandler::UpdateFromAttached(double tnow)
{
	if(_attachedactor)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physo = _character->GetPhysicalObject();
		boost::shared_ptr<PhysicalObjectHandlerBase> attchedphys = _attachedactor->GetPhysicalObject();
		if(physo && attchedphys)
		{
			physo->RotateYAxis(attchedphys->GetLastRotation());
			float addspeedX=0, addspeedY=0, addspeedZ=0;
			attchedphys->GetLastMove(addspeedX, addspeedY, addspeedZ);
			physo->Move(addspeedX, addspeedY, addspeedZ);
		}


		// send an update event every 3 secs to synchronize
		if ((tnow - m_lastupdatetime) > 3000)
		{
			m_lastupdatetime = tnow;
			boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
			float checkX, checkY, checkZ;
			physo->GetPosition(checkX, checkY, checkZ);
			float rotation = physo->GetRotationYAxis();
			std::string anim;	
			if(disO)
				anim = disO->GetCurrentAnimation();

			LbaNet::NpcChangedEventPtr UpdateEvent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
								m_actorinfo.ObjectId, checkX, checkY, checkZ, rotation, anim, 
								true, true, _character->GetSoundObject()->GetSoundVector(false), NULL);

			_events.push_back(UpdateEvent);
		}
	}
}


/***********************************************************
create actor
***********************************************************/
void ActorHandler::CreateActor()
{
	boost::shared_ptr<DisplayInfo> DInfo;

	// create display object
	switch(m_actorinfo.DisplayDesc.TypeRenderer)
	{
		//2 -> LBA1 model 
		case LbaNet::RenderLba1M:
		{
			//TODO configurable animation speed
			boost::shared_ptr<DisplayObjectDescriptionBase> dispobdesc
				(new Lba1ModelObjectDescription(0, m_actorinfo.DisplayDesc, _LBA1_MODEL_ANIMATION_SPEED_,
												m_actorinfo.DisplayDesc.UseLight, 
												m_actorinfo.DisplayDesc.CastShadow, 
												m_actorinfo.ExtraInfo,
												m_actorinfo.LifeInfo, false));

			DInfo = boost::shared_ptr<DisplayInfo>(new DisplayInfo(
								boost::shared_ptr<DisplayTransformation>(), dispobdesc));
		}
		break;

		//3-> LBA2 model
		case LbaNet::RenderLba2M:
		{
			//TODO - LBA2 model renderer
		}
		break;
	}

	// create physic object
	boost::shared_ptr<PhysicalDescriptionBase> PInfo;
	if(m_actorinfo.PhysicDesc.TypeShape == LbaNet::NoShape)
		PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionNoShape(m_actorinfo.PhysicDesc.Pos.X, m_actorinfo.PhysicDesc.Pos.Y, 
													m_actorinfo.PhysicDesc.Pos.Z, 
										LbaQuaternion(m_actorinfo.PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0))));
	else
		PInfo = boost::shared_ptr<PhysicalDescriptionBase>(new 
					PhysicalDescriptionWithShape(m_actorinfo.PhysicDesc.Pos.X, m_actorinfo.PhysicDesc.Pos.Y, 
													m_actorinfo.PhysicDesc.Pos.Z, 
													m_actorinfo.PhysicDesc.TypePhysO,
													m_actorinfo.PhysicDesc.Density,
										LbaQuaternion(m_actorinfo.PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0)),
											m_actorinfo.PhysicDesc.Collidable));


	ObjectInfo obj(4, (long)m_actorinfo.ObjectId, DInfo, PInfo, 
			(m_actorinfo.PhysicDesc.TypePhysO == LbaNet::StaticAType));


	m_NavMAgent = boost::shared_ptr<NavMeshAgent>();
	if(m_actorinfo.PhysicDesc.TypeShape != LbaNet::NoShape && 
		!m_actorinfo.ExcludeFromNavMesh && 
		m_navimesh && m_actorinfo.PhysicDesc.TypePhysO != LbaNet::StaticAType)
		m_NavMAgent = m_navimesh->AddAgent(m_actorinfo.PhysicDesc, m_navimesh);

	_character = obj.BuildServer(m_NavMAgent);
}


/***********************************************************
pause the playing script
***********************************************************/
void ActorHandler::Pause()
{
	if(!m_paused)
	{
		m_paused = true;
		if(_character)
		{
			boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
			if(disO)
				disO->SaveState();

			// store physical info
			boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
			if(physO)
			{
				physO->GetPosition(m_saved_X, m_saved_Y, m_saved_Z);
				m_saved_rot = physO->GetRotationYAxis();
				physO->GetRotation(m_saved_Q);

				m_savedshow = _character->IsShown();
			}

			boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
			if(soundO)
				soundO->SaveState();
		}

		//store running script
		StoreScript();
	}
}



/***********************************************************
resume the playing script
***********************************************************/
void ActorHandler::Resume()
{
	if(m_paused)
	{
		m_paused = false;
		if(_character)
		{
			//restore display info
			boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
			if(disO)
			{
				disO->RestoreState();

				// inform clients of restoration
				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::ModelUpdate(disO->GetCurrentModel(false), false)));

				_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::AnimationStringUpdate(disO->GetCurrentAnimation())));
			}


			boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
			if(soundO)
			{
				soundO->RestoreState();

				_events.push_back(new LbaNet::UpdateSoundObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::PlaySoundSeqUpd(soundO->GetSoundVector(false))));
			}

			// restore physical info
			boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
			if(physO)
			{
				physO->SetPosition(m_saved_X, m_saved_Y, m_saved_Z);
				physO->SetRotation(m_saved_Q);	

				m_resetposition = true;
				m_resetrotation = true;

				
				SetLastRecordPos(LbaVec3(m_saved_X, m_saved_Y, m_saved_Z));
			}

			// restore hidden/show state
			ShowHideInternal(m_savedshow);
		}

		//store running script
		RestoreScript();


		// update to last position
		if(_currentScripts.size() == 0) // if no script played reset to default
		{
			boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
			if(disO)
				_events.push_back(new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
														m_actorinfo.ObjectId, m_saved_X, m_saved_Y, m_saved_Z, 
														m_saved_Q.GetRotationSingleAngle(), 
														disO->GetCurrentAnimation(), 
														true, true, 
														_character->GetSoundObject()->GetSoundVector(false), NULL));
		}
	}
}



/***********************************************************
add script part to the script
***********************************************************/
int ActorHandler::AddScriptPart(ActorScriptPartBasePtr part)
{
	m_script.push_back(part);

	if(m_launchedscript >= 0)
	{
		CreateActor();
		ClearRunningScript();	
	}

	return m_script.size()-1;
}

/***********************************************************
remove script part to the script
***********************************************************/
void ActorHandler::RemoveScriptPart(ActorScriptPartBasePtr part)
{
	std::vector<ActorScriptPartBasePtr>::iterator it = std::find(m_script.begin(), m_script.end(), part);
	if(it != m_script.end())
		m_script.erase(it);

	if(m_launchedscript >= 0)
	{
		CreateActor();
		ClearRunningScript();
	}
}

/***********************************************************
replace script part to the script
***********************************************************/
void ActorHandler::ReplaceScriptPart(ActorScriptPartBasePtr olds, ActorScriptPartBasePtr news)
{
	std::vector<ActorScriptPartBasePtr>::iterator it = std::find(m_script.begin(), m_script.end(), olds);
	if(it != m_script.end())
		*it = news;

	if(m_launchedscript >= 0)
	{
		ClearRunningScript();
		CreateActor();
	}
}


/***********************************************************
update position of the script
***********************************************************/
void ActorHandler::UpdateScriptPosition(ActorScriptPartBasePtr part, int position)
{
	std::vector<ActorScriptPartBasePtr>::iterator it = std::find(m_script.begin(), m_script.end(), part);
	if(it != m_script.end())
	{
		ActorScriptPartBasePtr keep = *it;
		m_script.erase(it);

		std::vector<ActorScriptPartBasePtr>::iterator itinsert = m_script.begin();
		for(int cc=0; cc<position; ++cc)
			++itinsert;

		m_script.insert(itinsert, keep);
	}

	if(m_launchedscript >= 0)
	{
		ClearRunningScript();
		CreateActor();
	}
}

/***********************************************************
update position of the script
***********************************************************/
void ActorHandler::SetMovementStartScript(const std::string& functionName)
{
	m_movementStartScript = functionName;
	m_replaceMovementScript = functionName;
}



/***********************************************************
start script
***********************************************************/
void ActorHandler::StartScript()
{
	if (m_script.size() > 0)
	{
		LogHandler::getInstance()->LogToFile("Actor Starting script", GetId());

		// create lua script
		std::stringstream fctname;
		fctname<<"Actor"<<m_actorinfo.ObjectId<<"Script";

		std::stringstream scripts;
		scripts<<"function "<<fctname.str()<<"(ScriptId, Environment)"<<std::endl;
		scripts<<"while Environment:ThreadRunning(ScriptId) do"<<std::endl;
		for(size_t i=0; i< m_script.size(); ++i)
			m_script[i]->WriteExecutionScript(scripts, m_actorinfo.ObjectId, this);

		scripts<<"Environment:WaitOneCycle(ScriptId)"<<std::endl; // used to free thread so that we dont loop forever
		scripts<<"end"<<std::endl;
		scripts<<"end"<<std::endl;

		//std::ofstream file("ccc.txt");
		//file<<scripts.str();
		//file.close();

		//std::ofstream checkscript("checksc.txt");
		//checkscript<<scripts.str()<<std::endl;
		// register it
		m_scripthandler->ExecuteScriptString(scripts.str());

		// start the script
		m_scripthandler->StartScript(fctname.str(), false, m_launchedscript);
	}
}




/***********************************************************
used by lua to move an actor or player
if id < 1 then it moves players
the actor will move using animation speed
***********************************************************/
void ActorHandler::ActorStraightWalkTo(int ScriptId, bool asynchronus, float PosX, float PosY, float PosZ)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorStraightWalkTo(ScriptId, asynchronus, PosX, PosY, PosZ);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	
	m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
						m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
						new LbaNet::StraightWalkToNpcUpd(PosX, PosY, PosZ));

	_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}

void ActorHandler::ActorWalkToPoint(int ScriptId, bool asynchronus, float PosX, float PosY, float PosZ, float RotationSpeedPerSec, bool moveForward)
{
	ChangeState(1);
	GoToPosWithNavMesh(PosX, PosY, PosZ, !moveForward);
	ScriptedActor::ActorWalkToPoint(ScriptId, asynchronus, PosX, PosY, PosZ, RotationSpeedPerSec, moveForward);

	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	//m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
	//	m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
	//	m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
	//	new LbaNet::WalkToPointNpcUpd(PosX, PosY, PosZ, RotationSpeedPerSec, moveForward));

	//_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}



/***********************************************************
//! used by lua to rotate an actor
//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
***********************************************************/
void ActorHandler::ActorRotate(int ScriptId, bool asynchronus, float Angle, float RotationSpeedPerSec, 
								bool ManageAnimation)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorRotate(ScriptId, asynchronus, Angle, RotationSpeedPerSec, ManageAnimation);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
						m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
						new LbaNet::RotateNpcUpd(Angle, RotationSpeedPerSec, ManageAnimation));

	_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}


/***********************************************************
//! used by lua to wait until an actor animation is finished
//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
***********************************************************/
void ActorHandler::ActorAnimate(int ScriptId, bool asynchronus, bool AnimationMove, int nbAnimation)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorAnimate(ScriptId, asynchronus, AnimationMove, nbAnimation);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
						m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
						new LbaNet::AnimateNpcUpd(AnimationMove, 1));

	_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}



/***********************************************************
//! used by lua to move an actor or player
//! the actor will move using speed
***********************************************************/
void ActorHandler::ActorGoTo(int ScriptId, float PosX, float PosY, float PosZ, float Speed, bool asynchronus)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorGoTo(ScriptId, PosX, PosY, PosZ, Speed, asynchronus);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
						m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
						new LbaNet::GoToNpcUpd(PosX, PosY, PosZ, Speed));

	_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}
	



/***********************************************************
//! used by lua to move an actor or player
//! the actor will wait for signal
***********************************************************/
void ActorHandler::ActorWaitForSignal(int ScriptId, int Signalnumber, bool asynchronus)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorWaitForSignal(ScriptId, Signalnumber, asynchronus);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	if(m_resetposition || m_resetrotation)
	{
		m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
							m_resetposition, m_resetrotation, 
							_character->GetSoundObject()->GetSoundVector(false), NULL);

		_events.push_back(m_lastevent);
	}

	m_resetposition = false;
	m_resetrotation = false;
}

	

/***********************************************************
//! used by lua to move an actor or player
//! the actor will rotate
***********************************************************/
void ActorHandler::ActorRotateFromPoint(int ScriptId, float Angle, float PosX, float PosY, 
													float PosZ, float Speed, bool asynchronus)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorRotateFromPoint(ScriptId, Angle, PosX, PosY, PosZ, Speed, asynchronus);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
						m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
						new LbaNet::RotateFromPointNpcUpd(Angle, PosX, PosY, PosZ, Speed));

	_events.push_back(m_lastevent);

	m_resetposition = false;
	m_resetrotation = false;
}


	

/***********************************************************
//! used by lua to move an actor or player
//! the actor follow waypoint
***********************************************************/
void ActorHandler::ActorFollowWaypoint(int ScriptId, int waypointindex1, int waypointindex2, bool asynchronus)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorFollowWaypoint(ScriptId, waypointindex1, waypointindex2, asynchronus);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();




	std::vector<LbaVec3> waypoints = _character->GetWaypoints(waypointindex1);
	if(waypointindex2 < (int)waypoints.size())
	{
		LbaVec3 _Pm1, _P0, _P1, _P2, _P3, _P4;

		_P2 =  waypoints[waypointindex2];

		if((waypointindex2+1) < (int)waypoints.size())
			_P3 = waypoints[waypointindex2+1];
		else
			_P3 = _P2;

		if((waypointindex2+2) < (int)waypoints.size())
			_P4 = waypoints[waypointindex2+2];
		else
			_P4 = _P3;

		--waypointindex2;
		if(waypointindex2 >= 0)
			_P1 =  waypoints[waypointindex2];
		else
		{
			_P1.x = posX;
			_P1.y = posY;
			_P1.z = posZ;
		}

		--waypointindex2;
		if(waypointindex2 >= 0)
			_P0 =  waypoints[waypointindex2];
		else
			_P0 =  _P1;

		--waypointindex2;
		if(waypointindex2 >= 0)
			_Pm1 =  waypoints[waypointindex2];
		else
			_Pm1 =  _P1;


		// inform clients
		m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
							m_resetposition, m_resetrotation, _character->GetSoundObject()->GetSoundVector(false), 
							new LbaNet::FollowWaypointNpcUpd(
												_Pm1.x, _P0.x, _P1.x, _P2.x, _P3.x, _P4.x,
												_Pm1.y, _P0.y, _P1.y, _P2.y, _P3.y, _P4.y,
												_Pm1.z, _P0.z, _P1.z, _P2.z, _P3.z, _P4.z));

		_events.push_back(m_lastevent);

		m_resetposition = false;
		m_resetrotation = false;
	}
}

/***********************************************************
//! make an actor sleep for a given amount of time
***********************************************************/
void ActorHandler::ActorSleep(int ScriptId, int nbMilliseconds, bool asynchronus)
{
	ChangeState(1);
	StopUsingNavMesh();
	ScriptedActor::ActorSleep(ScriptId, asynchronus, nbMilliseconds);


	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	std::string anim;	
	if(disO)
		anim = disO->GetCurrentAnimation();

	// inform clients
	if(m_resetposition || m_resetrotation)
	{
		m_lastevent = new LbaNet::NpcChangedEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
			m_actorinfo.ObjectId, posX, posY, posZ, rotation, anim, 
			m_resetposition, m_resetrotation, 
			_character->GetSoundObject()->GetSoundVector(false), NULL);

		_events.push_back(m_lastevent);
	}

	m_resetposition = false;
	m_resetrotation = false;
}


/***********************************************************
//! switch actor model
***********************************************************/
void ActorHandler::SwitchModel(const std::string & newmodelname)
{
	//if(newmodelname != "")
	{
		m_saved_model = m_actorinfo.DisplayDesc.ModelName;
		m_actorinfo.DisplayDesc.ModelName = newmodelname;
		_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
							SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							1, m_actorinfo.ObjectId,
							new LbaNet::ModelUpdate(m_actorinfo.DisplayDesc, false)));
	}
}

/***********************************************************
//! revert model
***********************************************************/
void ActorHandler::ReverModel()
{
	if(m_saved_model != "")
	{
		m_actorinfo.DisplayDesc.ModelName = m_saved_model;
		_events.push_back(new LbaNet::UpdateDisplayObjectEvent(
							SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							1, m_actorinfo.ObjectId,
							new LbaNet::ModelUpdate(m_actorinfo.DisplayDesc, false)));
		m_saved_model = "";
	}
}



/***********************************************************
get last actor event
***********************************************************/
LbaNet::ClientServerEventBasePtr ActorHandler::GetLastEvent()
{
	if(m_lastevent)
	{
		// update to last position
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		boost::shared_ptr<DisplayObjectHandlerBase> disO = _character->GetDisplayObject();

		physO->GetPosition(m_lastevent->CurrPosX, m_lastevent->CurrPosY, m_lastevent->CurrPosZ);
		m_lastevent->CurrRotation = physO->GetRotationYAxis();

		std::string anim;	
		if(disO)
			m_lastevent->CurrAnimation = disO->GetCurrentAnimation();
	}




	return m_lastevent;
}


/***********************************************************
AttachActor
***********************************************************/
void ActorHandler::AttachActor(int AttachedObjectType, long AttachedObjectId)
{
	_attachedactor = m_scripthandler->GetActor(AttachedObjectType, AttachedObjectId);
	if(_attachedactor)
	{
		m_attachedactorid = AttachedObjectId;
		m_attachedactortype = AttachedObjectType;

		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		float posX, posY, posZ;
		physO->GetPosition(posX, posY, posZ);
		float rotation = physO->GetRotationYAxis();

		_events.push_back(new LbaNet::NpcAttachActorEvent(
							SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							m_actorinfo.ObjectId, posX, posY, posZ, rotation,
							m_attachedactortype, m_attachedactorid));
	}
	else
		m_attachedactorid = -1;
}

/***********************************************************
DettachActor
***********************************************************/
void ActorHandler::DettachActor(long AttachedObjectId)
{
	_attachedactor = boost::shared_ptr<DynamicObject>();
	m_attachedactorid = -1;

	boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
	float posX, posY, posZ;
	physO->GetPosition(posX, posY, posZ);
	float rotation = physO->GetRotationYAxis();

	_events.push_back(new LbaNet::NpcAttachActorEvent(
						SynchronizedTimeHandler::GetCurrentTimeDouble(), 
						m_actorinfo.ObjectId, posX, posY, posZ, rotation,
						m_attachedactortype, m_attachedactorid));
}


/***********************************************************
get last actor event
***********************************************************/
LbaNet::ClientServerEventBasePtr ActorHandler::AttachActorEvent()
{
	if(m_attachedactorid >= 0)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		float posX, posY, posZ;
		physO->GetPosition(posX, posY, posZ);
		float rotation = physO->GetRotationYAxis();

		return new LbaNet::NpcAttachActorEvent(
							SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							m_actorinfo.ObjectId, posX, posY, posZ, rotation,
							m_attachedactortype, m_attachedactorid);
	}

	return LbaNet::ClientServerEventBasePtr();
}

/***********************************************************
return actor type
***********************************************************/
std::string ActorHandler::ActorType()
{ 
	switch(m_actorinfo.PhysicDesc.TypePhysO)
	{
		case LbaNet::KynematicAType:
			return "Scripted";
		break;

		case LbaNet::CharControlAType:
			return "Movable";
		break;
	}

	return "Static";
}


/***********************************************************
show hide actor
***********************************************************/
void ActorHandler::ShowHideInternal(bool show)
{
	if(show != _character->IsShown())
	{
		_character->ShowOrHide(show);

		// inform clients of restoration
		_events.push_back(new LbaNet::ShowHideEvent(
							SynchronizedTimeHandler::GetCurrentTimeDouble(), 
							1, m_actorinfo.ObjectId, show));
	}
}

/***********************************************************
return true if actor is hidden
***********************************************************/
bool ActorHandler::IsHidden()
{
	return !_character->IsShown();
}


/***********************************************************
reset actor
***********************************************************/
void ActorHandler::ResetActor()
{
	// resume if paused
	Resume();

	// reset position
	if(_character)
	{
		boost::shared_ptr<PhysicalObjectHandlerBase> physO = _character->GetPhysicalObject();
		if(physO)
		{
			physO->SetPosition(m_actorinfo.PhysicDesc.Pos.X, 
								m_actorinfo.PhysicDesc.Pos.Y,
								m_actorinfo.PhysicDesc.Pos.Z);

			LbaQuaternion Q(m_actorinfo.PhysicDesc.Pos.Rotation, LbaVec3(0, 1, 0));
			physO->SetRotation(Q);	

			m_resetposition = true;
			m_resetrotation = true;

			SetLastRecordPos(LbaVec3(	m_actorinfo.PhysicDesc.Pos.X, 
										m_actorinfo.PhysicDesc.Pos.Y, 
										m_actorinfo.PhysicDesc.Pos.Z));
		}
	}

	// reset script
	ClearRunningScript();

	StopUsingNavMesh();
}


/***********************************************************
return actor current position
***********************************************************/
LbaNet::PlayerPosition ActorHandler::GetCurrentPosition()
{
	LbaNet::PlayerPosition res;

	boost::shared_ptr<PhysicalObjectHandlerBase> actphys = GetActor()->GetPhysicalObject();
	if(!actphys)
		return res;

	actphys->GetPosition(res.X, res.Y, res.Z);
	res.Rotation = actphys->GetRotationYAxis();

	return res;
}


/***********************************************************
return touch hit power - used to make plqyer loose life if actor touch him/hit him
***********************************************************/
float ActorHandler::GetTouchHitPower(bool & IgnoreArmor)
{
	IgnoreArmor = true;
	return m_actorinfo.HitPowerOnTouch;
}


/***********************************************************
make actor play a sound
***********************************************************/
void ActorHandler::APlaySound(int SoundChannel, const std::string & soundpath, int numberTime, bool randomPitch, 
								bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
		if(soundO)
		{
			LbaNet::PlayingSound sound = 
				soundO->GetSound(SoundChannel, updatefromlua && m_paused && !fromattackscript);
			sound.SoundPath = soundpath;
			sound.Paused = false;
			sound.NbTime = numberTime;
			sound.RandomPitch = randomPitch;

			soundO->Update(new LbaNet::PlaySoundUpd(sound), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateSoundObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::PlaySoundUpd(sound)));

			//if(!loop)
			//{
			//	// remove non looping sounds so that it does not repeat itself
			//	sound.SoundPath = "";
			//	soundO->Update(new LbaNet::PlaySoundUpd(sound), updatefromlua && m_paused && !fromattackscript);
			//}
		}
	}
}

/***********************************************************
make actor stop a sound
***********************************************************/
void ActorHandler::AStopSound(int SoundChannel, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
		if(soundO)
		{
			LbaNet::PlayingSound sound = 
				soundO->GetSound(SoundChannel, updatefromlua && m_paused && !fromattackscript);
			sound.SoundPath = "";

			soundO->Update(new LbaNet::PlaySoundUpd(sound), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateSoundObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::PlaySoundUpd(sound)));
		}
	}
}

/***********************************************************
make actor pause a sound
***********************************************************/
void ActorHandler::APauseSound(int SoundChannel, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
		if(soundO)
		{
			LbaNet::PlayingSound sound = 
				soundO->GetSound(SoundChannel, updatefromlua && m_paused && !fromattackscript);
			sound.Paused = true;

			soundO->Update(new LbaNet::PlaySoundUpd(sound), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateSoundObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::PlaySoundUpd(sound)));
		}
	}
}

/***********************************************************
make actor resume a sound
***********************************************************/
void ActorHandler::AResumeSound(int SoundChannel, bool updatefromlua, bool fromattackscript)
{
	if(_character)
	{
		boost::shared_ptr<SoundObjectHandlerBase> soundO = _character->GetSoundObject();
		if(soundO)
		{
			LbaNet::PlayingSound sound = 
				soundO->GetSound(SoundChannel, updatefromlua && m_paused && !fromattackscript);
			sound.Paused = false;

			soundO->Update(new LbaNet::PlaySoundUpd(sound), updatefromlua && m_paused && !fromattackscript);

			if(!updatefromlua || !m_paused)
				_events.push_back(new LbaNet::UpdateSoundObjectEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), 
									1, m_actorinfo.ObjectId,
									new LbaNet::PlaySoundUpd(sound)));
		}
	}
}

/***********************************************************
set the script to be used as movement script (replacing the current one)
***********************************************************/
void ActorHandler::setMovementScript(const std::string& functionName)
{
	m_replaceMovementScript = functionName;
}



/***********************************************************
navmesh functions
***********************************************************/
void ActorHandler::GoToPosWithNavMesh(float& PosX, float& PosY, float& PosZ, bool backward)
{
	_freemove = true;
	if(m_NavMAgent)
		m_NavMAgent->GoToPosition(PosX, PosY, PosZ, backward);
}

/***********************************************************
navmesh functions
***********************************************************/
void ActorHandler::StopUsingNavMesh()
{
	_freemove = false;
	if(m_NavMAgent)
		m_NavMAgent->SetResetTarget(false);
}

/***********************************************************
check if we need to send update to server
***********************************************************/
void ActorHandler::UpdateClients(double tnow, float tdiff)
{
	boost::shared_ptr<PhysicalObjectHandlerBase> physo = _character->GetPhysicalObject();
	if(!physo)
		return;

	// get current position
	physo->GetPosition(_currentupdate.CurrentPos.X,
		_currentupdate.CurrentPos.Y,
		_currentupdate.CurrentPos.Z);


	// get current rotation
	_currentupdate.CurrentPos.Rotation = physo->GetRotationYAxis();


	// set speed
	_currentupdate.CurrentSpeedX = (_currentupdate.CurrentPos.X-_lastupdate.CurrentPos.X) / _oldtdiff;
	_currentupdate.CurrentSpeedY = (_currentupdate.CurrentPos.Y-_lastupdate.CurrentPos.Y) / _oldtdiff;
	_currentupdate.CurrentSpeedZ = (_currentupdate.CurrentPos.Z-_lastupdate.CurrentPos.Z) / _oldtdiff;

	//calculate angle speed
	_currentupdate.CurrentSpeedRotation = (_currentupdate.CurrentPos.Rotation-
		_lastupdate.CurrentPos.Rotation);
	while(_currentupdate.CurrentSpeedRotation < -180)
		_currentupdate.CurrentSpeedRotation += 360;
	while(_currentupdate.CurrentSpeedRotation > 180)
		_currentupdate.CurrentSpeedRotation -= 360;


	_currentupdate.CurrentSpeedRotation /= tdiff;


	_oldtdiff = tdiff;


	boost::shared_ptr<DisplayObjectHandlerBase> diso = _character->GetDisplayObject();

	// set animation
	if(diso)
		_currentupdate.AnimationIdx = diso->GetCurrentAnimation();



	// check if we should force the update
	_currentupdate.ForcedChange = ShouldforceUpdate();


	//send to server if needed
	if(_currentupdate.ForcedChange && m_scripthandler)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update to client"<<std::endl;
#endif


		LbaNet::EventsSeq evseq;
		evseq.push_back(new LbaNet::NPCMovedEvent(tnow, GetId(), _currentupdate));
		m_scripthandler->SendEvents(-2, evseq);
	}

	_lastupdate = _currentupdate;
}


/***********************************************************
check if we should force the update
***********************************************************/
bool ActorHandler::ShouldforceUpdate()
{
	if(_lastupdate.AnimationIdx != _currentupdate.AnimationIdx)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update animation"<<std::endl;
#endif
		return true;
	}

	if(fabs(_lastupdate.CurrentSpeedX - _currentupdate.CurrentSpeedX) > 0.00001f)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update speedx"<<std::endl;
#endif
		return true;
	}

	if(fabs(_lastupdate.CurrentSpeedY - _currentupdate.CurrentSpeedY) > 0.00001f)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update speedy"<<std::endl;
#endif
		return true;
	}

	if(fabs(_lastupdate.CurrentSpeedZ - _currentupdate.CurrentSpeedZ) > 0.00001f)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update speedz"<<std::endl;
#endif
		return true;
	}

	if(fabs(_lastupdate.CurrentSpeedRotation - _currentupdate.CurrentSpeedRotation) > 0.0001f)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update speed rotation"<<std::endl;
#endif
		return true;
	}



	float diffpos = fabs(_lastupdate.CurrentPos.X - _currentupdate.CurrentPos.X)
		+ fabs(_lastupdate.CurrentPos.Y - _currentupdate.CurrentPos.Y)
		+  fabs(_lastupdate.CurrentPos.Z - _currentupdate.CurrentPos.Z);
	if(diffpos > 10)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update position"<<std::endl;
#endif
		return true;
	}


	double diffrot = fabs(_lastupdate.CurrentPos.Rotation - _currentupdate.CurrentPos.Rotation);
	if(diffrot > 10)
	{
#ifdef _DEBUG_NPC_
		filecheck<<SynchronizedTimeHandler::GetTimeString()<<" "<<"forced update angle"<<std::endl;
#endif
		return true;
	}


	return false;
}