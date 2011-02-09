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


#ifndef __LBA_NET_COMMON_TYPES_H__
#define __LBA_NET_COMMON_TYPES_H__

#include <string>


/***********************************************************************
Internal vector 3 class
 ***********************************************************************/
class LbaVec3
{
public:
	//! constructor
	LbaVec3()
		: x(0), y(0), z(0)
	{}

	//! constructor
	LbaVec3(float X, float Y, float Z)
		: x(X), y(Y), z(Z)
	{}

	float x;
	float y;
	float z;
};


/***********************************************************************
Internal quaternion class
based on nvidia PhysX NxQuat public code
 ***********************************************************************/
class LbaQuaternion
{
public:
	//!constructor
	LbaQuaternion();

	//!constructor
	LbaQuaternion(float x, float y, float z, float w)
		: X(x), Y(y), Z(z), W(w){}

	//! constructor from 3 angles
	LbaQuaternion(float anglex, float angley, float anglez);


	//!constructor with angle in degree and axis vec - degree
	LbaQuaternion(float angle, LbaVec3 vec);


	//! add rotation to quaternion - degree
	void AddSingleRotation(float angle, const LbaVec3 &vec);

	//! get direction vector
	LbaVec3 GetDirection(const LbaVec3 &vec);

	//! get object rotation on a single angle - degree
	float GetRotationSingleAngle();

	//! operator *
	LbaQuaternion operator *(const LbaQuaternion & q);

	//! get the angle between a vector and the 0 angle
	static float GetAngleFromVector(const LbaVec3 &vec);


	//! rotate a vector by the quaternion
	void rotate(LbaVec3 & v) const;

	//! set current quat as left * right
	void multiply(const LbaQuaternion& left, const LbaVec3& right);


	float X;
	float Y;
	float Z;
	float W;
};



/***********************************************************************
This class describe main light information for the scene
 ***********************************************************************/
class LbaMainLightInfo
{
public:

	//! constructor
	LbaMainLightInfo()
		: UseLight(false)
	{}

	//! constructor
	LbaMainLightInfo(float PosX, float PosY, float PosZ)
		: UseLight(true), DirectionalLight(false), 
			UseAttenuation(false), UseExtendedSpotInfo(false),
			LOnPosX(PosX), LOnPosY(PosY), LOnPosZ(PosZ)
	{
		LOnAmbientR = 0.5f;
		LOnAmbientG = 0.5f;
		LOnAmbientB = 0.5f;

		LOnDiffuseR = 0.8f;
		LOnDiffuseG = 0.8f;
		LOnDiffuseB = 0.8f;

		LOnSpecularR = 1.0f;
		LOnSpecularG = 1.0f;
		LOnSpecularB = 1.0f;
	}

	//! members
	bool UseLight;
	bool DirectionalLight;
	bool UseAttenuation;
	bool UseExtendedSpotInfo;

	float LOnPosX;
	float LOnPosY;
	float LOnPosZ;

	float LOnDirectionX;
	float LOnDirectionY;
	float LOnDirectionZ;


	float LOnAmbientR;
	float LOnAmbientG;
	float LOnAmbientB;

	float LOnDiffuseR;
	float LOnDiffuseG;
	float LOnDiffuseB;

	float LOnSpecularR;
	float LOnSpecularG;
	float LOnSpecularB;

	float Constant_attenuation;             // constant
	float Linear_attenuation;               // linear
	float Quadratic_attenuation;            // quadratic
	float Spot_exponent;                    // exponent
	float Spot_cutoff;                      // spread

};





#endif
