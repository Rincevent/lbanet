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

#include "PhysXObjectHandlers.h"
#include "NxPhysics.h"
#include "NxCapsuleController.h"
#include "PhysXEngine.h"
#include "NxVec3.h"
#include "ObjectsDescription.h"
#include "LogHandler.h"
#include "DataDirHandler.h"



/***********************************************************
	Constructor
***********************************************************/
PhysXObjectHandlerBase::PhysXObjectHandlerBase(boost::shared_ptr<ActorUserData> UserData)
		: _UserData(UserData)
{}




/***********************************************************
get last move
***********************************************************/	
void PhysXObjectHandlerBase::GetLastMove(float &X, float &Y, float &Z)
{
	if(_UserData)
		_UserData->GetMove(X, Y, Z);
}


/***********************************************************
get last rotation
***********************************************************/
float PhysXObjectHandlerBase::GetLastRotation()
{
	if(_UserData)
		return _UserData->GetRotation();

	return 0;
}


/***********************************************************
reset last move
***********************************************************/
void PhysXObjectHandlerBase::ResetMove()
{
	if(_UserData)
	{
		_UserData->SetMove(0, 0, 0);
		_UserData->SetRotation(0);
	}
}


/***********************************************************
check if actor is on top of other
***********************************************************/
bool PhysXObjectHandlerBase::OnTopOff(PhysicalObjectHandlerBase * actor)
{
	if(actor)
	{
		NxActor* my = GetphysXInternalActor();
		NxActor* other = actor->GetphysXInternalActor();
		return PhysXEngine::getInstance()->CheckOnTopOff(my, other);
	}

	return false;
}




/***********************************************************
	Constructor
***********************************************************/
PhysXActorsHandler::PhysXActorsHandler(boost::shared_ptr<ActorUserData> UserData,
												NxActor* Actor, float sizeY,
												boost::shared_ptr<PhysicalDescriptionBase> desc)
		: PhysXObjectHandlerBase(UserData), _Actor(Actor), _sizeY(sizeY), _desc(desc)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Created new PhysXActor.");
	#endif
}


/***********************************************************
	destructor
***********************************************************/
PhysXActorsHandler::~PhysXActorsHandler()
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Destroyed PhysXActor.");
	#endif

	PhysXEngine::getInstance()->DestroyActor(_Actor);
}

/***********************************************************
get object position in the world
***********************************************************/
void PhysXActorsHandler::GetPosition(float &X, float &Y, float &Z)
{	
	if(!_Actor) return;

	NxVec3 vec = _Actor->getGlobalPosition();
	X = vec.x;
	Y = vec.y-(_sizeY/2);
	Z = vec.z;
}

/***********************************************************
get object rotation on all axis
***********************************************************/
void PhysXActorsHandler::GetRotation(LbaQuaternion& Q)
{	
	if(!_Actor) return;

	NxQuat quat = _Actor->getGlobalOrientationQuat();
	Q.X = quat.x;
	Q.Y = quat.y;
	Q.Z = quat.z;
	Q.W = quat.w;
}

/***********************************************************
get object rotation on all axis
***********************************************************/
float PhysXActorsHandler::GetRotationYAxis()
{
	NxQuat quat = _Actor->getGlobalOrientationQuat();
	float Yangle;
	NxVec3 axis(0, 1, 0);
	quat.getAngleAxis(Yangle, axis);
	return Yangle;
}

/***********************************************************
set object position in the world
***********************************************************/
void PhysXActorsHandler::SetPosition(float X, float Y, float Z)
{
	_Actor->setGlobalPosition(NxVec3(X, Y+(_sizeY/2), Z));
	_resetted = true;
}

/***********************************************************
set object rotation on all axis
***********************************************************/
void PhysXActorsHandler::SetRotation(const LbaQuaternion& Q)
{	
	_Actor->setGlobalOrientationQuat(NxQuat(NxVec3(Q.X, Q.Y, Q.Z), Q.W));
	_resetted = true;
}

/***********************************************************
move object in the world
***********************************************************/
void PhysXActorsHandler::Move(float deltaX, float deltaY, float deltaZ, bool checkcollision)
{
	float currPosX, currPosY, currPosZ;
	GetPosition(currPosX, currPosY, currPosZ);
	MoveTo(currPosX+deltaX, currPosY+deltaY, currPosZ+deltaZ);
}

/***********************************************************
move object to a position in the world
***********************************************************/
void PhysXActorsHandler::MoveTo(float X, float Y, float Z)
{
	float currPosX, currPosY, currPosZ;
	GetPosition(currPosX, currPosY, currPosZ);

	float moveX = X - currPosX;
	float moveY = Y - currPosY;
	float moveZ = Z - currPosZ;
	if(_UserData)
		_UserData->SetMove(moveX, moveY, moveZ);

	_Actor->moveGlobalPosition(NxVec3(X, Y+(_sizeY/2), Z));
}


/***********************************************************
rotate object in the world
***********************************************************/
void PhysXActorsHandler::RotateYAxis(float deltaY)
{
	// get current Y
	NxQuat quat = _Actor->getGlobalOrientationQuat();

	if(_UserData)
		_UserData->SetRotation(deltaY);


	NxQuat addq(deltaY, NxVec3(0, 1, 0));

	// move to new angle
	_Actor->moveGlobalOrientationQuat(addq*quat);
}

/***********************************************************
rotate object in the world
***********************************************************/
void PhysXActorsHandler::RotateTo(const LbaQuaternion& Q)
{
	float Yangle;
	NxF32 tabq[4];
	NxQuat quat = _Actor->getGlobalOrientationQuat();
	NxVec3 axis(0, 1, 0);
	quat.getAngleAxis(Yangle, axis);
	quat.getXYZW(tabq);

	NxQuat res(NxVec3(Q.X, Q.Y, Q.Z), Q.W);
	float wanted;
	NxF32 tabq2[4];
	res.getAngleAxis(wanted, axis);
	res.getXYZW(tabq2);


	if(_UserData)
	{
		float move = wanted-Yangle;

		if(tabq[3] > 0)
		{
			if(tabq2[3] < 0)
			{
				// sign change
				wanted = 360 - wanted;
				move = Yangle - wanted;
			}	
		}
		else
		{
			if(tabq2[3] > 0)
			{
				// sign change
				wanted = 360 - wanted;
				move = Yangle - wanted;
			}	
		}


		_UserData->SetRotation(move);

	}


	_Actor->moveGlobalOrientationQuat(res);
}


/***********************************************************
show or hide the object
***********************************************************/
void PhysXActorsHandler::ShowOrHide(bool Show)
{
	PhysXEngine::getInstance()->HideShowActor(_Actor, Show);
}


/***********************************************************
update physic with new data
***********************************************************/
void PhysXActorsHandler::Update(LbaNet::PhysicObjectUpdateBasePtr update)
{
	const std::type_info& info = typeid(*update);

	// SizeUpdate
	if(info == typeid(LbaNet::SizeUpdate))
	{
		LbaNet::SizeUpdate * castedptr = 
			dynamic_cast<LbaNet::SizeUpdate *>(update.get());

		if(_Actor && _desc)
		{		
			if(_desc)
			{
				if(_desc->ResetSize(castedptr->SizeX, castedptr->SizeY, castedptr->SizeZ))
				{
					float X, Y, Z;
					GetPosition(X, Y, Z);

					#ifdef _DEBUG
					LogHandler::getInstance()->LogToFile("Rebuilding PhysXActor.");
					#endif

					PhysXEngine::getInstance()->DestroyActor(_Actor);

					_Actor = _desc->RebuildActor(X, Y, Z, _UserData);
				}
			}

			_sizeY = castedptr->SizeY;
		}


		return;
	}

	PhysicalObjectHandlerBase::Update(update);
}



/***********************************************************
ignore collision between two pairs of actors
***********************************************************/
void PhysXActorsHandler::IgnoreCollisionWith(PhysicalObjectHandlerBase * actor)
{
	if(!actor)
		return;

	NxActor* other = actor->GetphysXInternalActor();

	if(_Actor && other)
		PhysXEngine::getInstance()->IgnoreActorContact(_Actor, other);
}

/***********************************************************
add force to actor - only for dynamic actors
***********************************************************/
void PhysXActorsHandler::AddForce(float X, float Y, float Z)
{
	if(_Actor)
		_Actor->addForce(NxVec3(X, Y, Z));
}


/***********************************************************
return internal actor - only for physX actor
***********************************************************/
NxActor* PhysXActorsHandler::GetphysXInternalActor()
{
	return _Actor;
}


/***********************************************************
enable or disable the gravity
***********************************************************/
void PhysXActorsHandler::EnableDisableGravity(bool enabled)
{
	if(_Actor)
	{
		if(enabled)
			_Actor->clearBodyFlag(NX_BF_DISABLE_GRAVITY);
		else
			_Actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	}
}


/***********************************************************
check the material of the current floor stepped
***********************************************************/
short PhysXActorsHandler::GetFloorMaterial()
{
	if(_Actor)
		return PhysXEngine::getInstance()->GetFloorMaterial(_Actor);

	return 0;
}







/***********************************************************
	Constructor
***********************************************************/
PhysXControllerHandler::PhysXControllerHandler(boost::shared_ptr<ActorUserData> UserData,
												NxController* Controller,
												boost::shared_ptr<SimpleRotationHandler> rotH,
												float sizeY,
												boost::shared_ptr<PhysicalDescriptionBase> desc)
		: PhysXObjectHandlerBase(UserData), _Controller(Controller), _rotH(rotH),
			_sizeY(sizeY), _desc(desc), _lastfloor(0)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Created new PhysXController.");
	#endif
}



/***********************************************************
	destructor
***********************************************************/
PhysXControllerHandler::~PhysXControllerHandler()
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Destroyed PhysXController.");
	#endif

	PhysXEngine::getInstance()->DestroyCharacter(_Controller);
}


/***********************************************************
get object position in the world
***********************************************************/
void PhysXControllerHandler::GetPosition(float &X, float &Y, float &Z)
{	
	if(!_Controller) return;

	NxExtendedVec3 vec = _Controller->getPosition();
	X = (float)vec.x;
	Y = (float)vec.y-(_sizeY/2);
	Z = (float)vec.z;
}

/***********************************************************
get object rotation on all axis
***********************************************************/
void PhysXControllerHandler::GetRotation(LbaQuaternion& Q)
{
	_rotH->GetRotation(Q);
}

/***********************************************************
get object rotation on all axis
***********************************************************/
float PhysXControllerHandler::GetRotationYAxis()
{
	return _rotH->GetRotationYAxis();
}

/***********************************************************
set object position in the world
***********************************************************/
void PhysXControllerHandler::SetPosition(float X, float Y, float Z)
{	
	if(!_Controller) return;

	PhysXEngine::getInstance()->SetCharacterPos(_Controller, NxVec3(X, Y+(_sizeY/2), Z));
	_resetted = true;
}

/***********************************************************
set object rotation on all axis
***********************************************************/
void PhysXControllerHandler::SetRotation(const LbaQuaternion& Q)
{
	_rotH->SetRotation(Q);
}


/***********************************************************
rotate object in the world
***********************************************************/
void PhysXControllerHandler::RotateYAxis(float deltaY)
{	
	if(_UserData)
		_UserData->SetRotation(deltaY);

	_rotH->RotateYAxis(deltaY);
}

/***********************************************************
move object in the world
***********************************************************/
void PhysXControllerHandler::Move(float deltaX, float deltaY, float deltaZ, bool checkcollision)
{
	if(_UserData)
		_UserData->SetMove(deltaX, deltaY, deltaZ);

	if(!_Controller) 
		return;

	unsigned int flags = 
		PhysXEngine::getInstance()->MoveCharacter(_Controller, NxVec3(deltaX, deltaY, deltaZ), checkcollision);

	if(_UserData)
	{
		if((bool)(flags & NXCC_COLLISION_DOWN))
		{
			_UserData->SetTouchingGround(true);
			_lastfloor = _UserData->GetFloorMaterial();
		}
		else
		{
			_UserData->SetTouchingGround(false);
			_lastfloor = 0;
		}
	}
}

/***********************************************************
move object to a position in the world
***********************************************************/
void PhysXControllerHandler::MoveTo(float X, float Y, float Z)
{
	if(!_Controller) 
		return;

	NxExtendedVec3 vec = _Controller->getPosition();
	float moveX = X - (float)vec.x;
	float moveY = (Y+(_sizeY/2)) - (float)vec.y;
	float moveZ = Z - (float)vec.z;

	if(_UserData)
		_UserData->SetMove(moveX, moveY, moveZ);

	PhysXEngine::getInstance()->MoveCharacter(_Controller, NxVec3(moveX, moveY, moveZ), false);
}

/***********************************************************
rotate object in the world
***********************************************************/
void PhysXControllerHandler::RotateTo(const LbaQuaternion& Q)
{
	float Yangle = _rotH->GetRotationYAxis();
	float wanted = Q.GetRotationSingleAngle();
	if(_UserData)
		_UserData->SetRotation(wanted-Yangle);

	_rotH->RotateTo(Q);
}



/***********************************************************
show or hide the object
***********************************************************/
void PhysXControllerHandler::ShowOrHide(bool Show)
{
	PhysXEngine::getInstance()->HideShowCharacter(_Controller, Show);
}


/***********************************************************
update physic with new data
***********************************************************/
void PhysXControllerHandler::Update(LbaNet::PhysicObjectUpdateBasePtr update)
{
	const std::type_info& info = typeid(*update);

	// SizeUpdate
	if(info == typeid(LbaNet::SizeUpdate))
	{
		LbaNet::SizeUpdate * castedptr = 
			dynamic_cast<LbaNet::SizeUpdate *>(update.get());


		if(_Controller && _desc)
		{
			if(_desc)
			{
				if(_desc->ResetSize(castedptr->SizeX, castedptr->SizeY, castedptr->SizeZ))
				{
					float X, Y, Z;
					GetPosition(X, Y, Z);

					#ifdef _DEBUG
					LogHandler::getInstance()->LogToFile("Rebuilding PhysXController.");
					#endif

					PhysXEngine::getInstance()->DestroyCharacter(_Controller);
					_Controller = _desc->RebuildController(X, Y+0.5f, Z, _UserData);
				}
			}

			_sizeY = castedptr->SizeY;
		}

		return;
	}

	PhysicalObjectHandlerBase::Update(update);
}


/***********************************************************
return internal actor - only for physX actor
***********************************************************/
NxActor* PhysXControllerHandler::GetphysXInternalActor()
{
	if(!_Controller)
		return NULL;

	return _Controller->getActor();
}










/***********************************************************
	Constructor
***********************************************************/
PhysicalDescriptionBox::PhysicalDescriptionBox(float posX, float posY, float posZ,
												LbaNet::PhysicalActorType Otype, float Odensity,
												const LbaQuaternion &rot,
												float sX, float sY, float sZ,
												bool Collidable)
	:PhysicalDescriptionWithShape(posX, posY, posZ, Otype, Odensity, rot, Collidable),
			sizeX(sX), sizeY(sY), sizeZ(sZ)
{

}


/***********************************************************
	destructor
***********************************************************/
PhysicalDescriptionBox::~PhysicalDescriptionBox()
{

}


/***********************************************************
change size
***********************************************************/
bool PhysicalDescriptionBox::ResetSize(float sX, float sY, float sZ)
{
	if(	(fabs(sizeX - sX) > 0.0001f) || 
		(fabs(sizeY - sY) > 0.0001f) || 
		(fabs(sizeZ - sZ) > 0.0001f) )
	{
		sizeX = sX;
		sizeY = sY;
		sizeZ = sZ;
		return true;
	}

	return false;
}



/***********************************************************
RebuildActor
***********************************************************/
NxActor* PhysicalDescriptionBox::RebuildActor(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	return PhysXEngine::getInstance()->CreateBox(NxVec3(X, Y + sizeY/2, Z), 
												sizeX, sizeY, sizeZ, 
												density, ActorType, udata.get(), rotation, collidable);
}



/***********************************************************
RebuildController
***********************************************************/
NxController* PhysicalDescriptionBox::RebuildController(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	return PhysXEngine::getInstance()->CreateCharacterBox(NxVec3(X, Y + sizeY/2, Z), 
															NxVec3(sizeX, sizeY, sizeZ), udata.get());
}


/***********************************************************
 build description into a reald physic object
***********************************************************/
boost::shared_ptr<PhysicalObjectHandlerBase> PhysicalDescriptionBox::BuildSelf(int type, long id,
														boost::shared_ptr<PhysicalDescriptionBase> self) const
{
	boost::shared_ptr<ActorUserData> udata = boost::shared_ptr<ActorUserData>(new ActorUserData(ActorType, type, id));

	if(ActorType != LbaNet::CharControlAType)
	{
		NxActor* act = PhysXEngine::getInstance()->CreateBox(NxVec3(positionX, positionY + sizeY/2, positionZ), 
													sizeX, sizeY, sizeZ, 
													density, ActorType, udata.get(), rotation, collidable);
		
		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXActorsHandler(udata, act, sizeY, self));
	}
	else
	{
		NxController* controller = PhysXEngine::getInstance()->CreateCharacterBox(NxVec3(positionX, positionY + sizeY/2, positionZ), 
															NxVec3(sizeX/2, sizeY/2, sizeZ/2), udata.get());

		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXControllerHandler(udata, controller, 
								boost::shared_ptr<SimpleRotationHandler>(new SimpleRotationHandler(rotation)), sizeY, self));
	}
}

/***********************************************************
	Constructor
***********************************************************/
PhysicalDescriptionCapsule::PhysicalDescriptionCapsule(float posX, float posY, float posZ,
														LbaNet::PhysicalActorType Otype, float Odensity,
														const LbaQuaternion &rot,
														float sX, float sY,
														bool Collidable)
	:PhysicalDescriptionWithShape(posX, posY, posZ, Otype, Odensity, rot, Collidable),
		radius(sX/2), height(sY-sX), sizeY(sY)
{

}

/***********************************************************
	destructor
***********************************************************/
PhysicalDescriptionCapsule::~PhysicalDescriptionCapsule()
{

}


/***********************************************************
change size
***********************************************************/
bool PhysicalDescriptionCapsule::ResetSize(float sX, float sY, float sZ)
{
	float newrad = sX/2;
	float newheight = sY-sX;

	if(	(fabs(sizeY - sY) > 0.0001f) || 
		(fabs(radius - newrad) > 0.0001f) || 
		(fabs(height - newheight) > 0.0001f) )
	{
		sizeY = sY;
		radius = newrad;
		height = newheight;
		return true;
	}

	return false;
}

/***********************************************************
RebuildActor
***********************************************************/
NxActor* PhysicalDescriptionCapsule::RebuildActor(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	return PhysXEngine::getInstance()->CreateCapsule(NxVec3(X, Y + sizeY/2, Z), 
																	radius, height,
																	density, ActorType, udata.get(), 
																	rotation, collidable);
}



/***********************************************************
RebuildController
***********************************************************/
NxController* PhysicalDescriptionCapsule::RebuildController(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	return PhysXEngine::getInstance()->CreateCharacter(NxVec3(X, Y + sizeY/2, Z), 
																		radius, height, udata.get());
}

/***********************************************************
 build description into a reald physic object
***********************************************************/
boost::shared_ptr<PhysicalObjectHandlerBase> PhysicalDescriptionCapsule::BuildSelf(int type, long id,
														boost::shared_ptr<PhysicalDescriptionBase> self) const
{
	boost::shared_ptr<ActorUserData> udata = boost::shared_ptr<ActorUserData>(new ActorUserData(ActorType, type, id));

	if(ActorType != LbaNet::CharControlAType)
	{
		NxActor* act = PhysXEngine::getInstance()->CreateCapsule(NxVec3(positionX, positionY + sizeY/2, positionZ), 
																	radius, height,
																	density, ActorType, udata.get(), 
																	rotation, collidable);
		
		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXActorsHandler(udata, act, sizeY, self));
	}
	else
	{
		NxController* controller = PhysXEngine::getInstance()->CreateCharacter(NxVec3(positionX, positionY + sizeY/2, positionZ), 
																		radius, height, udata.get());

		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXControllerHandler(udata, controller, 
								boost::shared_ptr<SimpleRotationHandler>(new SimpleRotationHandler(rotation)), sizeY, self));
	}
}

/***********************************************************
	Constructor
***********************************************************/
PhysicalDescriptionSphere::PhysicalDescriptionSphere(float posX, float posY, float posZ,
														LbaNet::PhysicalActorType Otype, float Odensity,
														const LbaQuaternion &rot,
														float sY,
														bool Collidable,
														float Bounciness, 
														float StaticFriction, float DynamicFriction)
	:PhysicalDescriptionWithShape(posX, posY, posZ, Otype, Odensity, rot, Collidable),
		radius(sY/2), sizeY(sY), bounciness(Bounciness), staticFriction(StaticFriction),
			dynamicFriction(DynamicFriction)
{

}

/***********************************************************
	destructor
***********************************************************/
PhysicalDescriptionSphere::~PhysicalDescriptionSphere()
{

}


/***********************************************************
change size
***********************************************************/
bool PhysicalDescriptionSphere::ResetSize(float sX, float sY, float sZ)
{
	float newrad = sY/2;

	if(	(fabs(sizeY - sY) > 0.0001f) || 
		(fabs(radius - newrad) > 0.0001f)  )
	{
		sizeY = sY;
		radius = newrad;
		return true;
	}

	return false;
}

/***********************************************************
RebuildActor
***********************************************************/
NxActor* PhysicalDescriptionSphere::RebuildActor(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	if(ActorType == LbaNet::DynamicAType)
		return PhysXEngine::getInstance()->CreateSphere(NxVec3(X, Y + sizeY/2, Z), 
																	radius, 
																	density, ActorType, udata.get(), 
																	bounciness, staticFriction, dynamicFriction,
																	rotation, collidable);
	else
		return PhysXEngine::getInstance()->CreateSphere(NxVec3(X, Y + sizeY/2, Z), 
																	radius, 
																	density, ActorType, udata.get(), 
																	rotation, collidable);
}



/***********************************************************
RebuildController
***********************************************************/
NxController* PhysicalDescriptionSphere::RebuildController(float X, float Y, float Z, boost::shared_ptr<ActorUserData> udata)
{
	return PhysXEngine::getInstance()->CreateCharacter(NxVec3(X, Y + sizeY/2, Z), 
																	radius, 0, udata.get());
}

/***********************************************************
 build description into a reald physic object
***********************************************************/
boost::shared_ptr<PhysicalObjectHandlerBase> PhysicalDescriptionSphere::BuildSelf(int type, long id,
														boost::shared_ptr<PhysicalDescriptionBase> self) const
{
	boost::shared_ptr<ActorUserData> udata = boost::shared_ptr<ActorUserData>(new ActorUserData(ActorType, type, id));

	if(ActorType != LbaNet::CharControlAType)
	{
		NxActor* act = PhysXEngine::getInstance()->CreateSphere(NxVec3(positionX, positionY + sizeY/2, positionZ), 
																	radius, 
																	density, ActorType, udata.get(), 
																	rotation, collidable);

		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXActorsHandler(udata, act, sizeY, self));
	}
	else
	{
		NxController* controller = PhysXEngine::getInstance()->CreateCharacter(NxVec3(positionX, 
																		positionY + sizeY/2, positionZ), 
																		radius, 0, udata.get());

		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXControllerHandler(udata, controller, 
								boost::shared_ptr<SimpleRotationHandler>(new SimpleRotationHandler(rotation)), sizeY, self));
	}
}

/***********************************************************
	Constructor
***********************************************************/
PhysicalDescriptionTriangleMesh::PhysicalDescriptionTriangleMesh(float posX, float posY, float posZ,
																	const LbaQuaternion &rot,
																	const std::string &FileName,
																	bool Collidable)
:PhysicalDescriptionWithShape(posX, posY, posZ, LbaNet::StaticAType, 1, rot, Collidable),
		MeshInfoDataFileName(FileName)
{

}

/***********************************************************
	destructor
***********************************************************/
PhysicalDescriptionTriangleMesh::~PhysicalDescriptionTriangleMesh()
{

}


/***********************************************************
 build description into a reald physic object
***********************************************************/
boost::shared_ptr<PhysicalObjectHandlerBase> PhysicalDescriptionTriangleMesh::BuildSelf(int type, long id,
														boost::shared_ptr<PhysicalDescriptionBase> self) const
{
	boost::shared_ptr<ActorUserData> udata = boost::shared_ptr<ActorUserData>(new ActorUserData(ActorType, type, id));

	NxActor* actor = PhysXEngine::getInstance()->LoadTriangleMeshFile(NxVec3(positionX, positionY, positionZ), 
														DataDirHandler::getInstance()->GetDataDirPath() + "/"+MeshInfoDataFileName, udata.get(), 
														rotation, collidable);

	if(actor)
		return boost::shared_ptr<PhysicalObjectHandlerBase>(new PhysXActorsHandler(udata, actor, 0, self));
	else
		return boost::shared_ptr<PhysicalObjectHandlerBase>( 
				new SimplePhysicalObjectHandler(positionX, positionY, positionZ, rotation));
}

