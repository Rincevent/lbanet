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

#if !defined(__LbaNetModel_1_OsgObjectHandler_h)
#define __LbaNetModel_1_OsgObjectHandler_h


#include "DisplayObjectHandlerBase.h"
#include <osg/ref_ptr>
#include <osg/Node>
#include <boost/shared_ptr.hpp>

namespace osg
{
	class MatrixTransform;
	class PositionAttitudeTransform;
	class Group;
	class AutoTransform;
	class ShapeDrawable;
}

class DisplayTransformation;

/***********************************************************************
 * Module:  OsgObjectHandler.h
 * Author:  vivien
 * Modified: mardi 14 juillet 2009 13:54:52
 * Purpose: Declaration of the class OsgObjectHandler
 ***********************************************************************/
class OsgObjectHandler : public DisplayObjectHandlerBase
{
public:
	//! default constructor
	OsgObjectHandler(int sceneidx, boost::shared_ptr<DisplayTransformation> Tr,
						const LbaNet::ObjectExtraInfo &extrainfo,
						const LbaNet::LifeManaInfo &lifeinfo,
						osg::ref_ptr<osg::Node> particle = osg::ref_ptr<osg::Node>());

	//! constructor
	OsgObjectHandler(int sceneidx, osg::ref_ptr<osg::MatrixTransform> OsgObject, bool uselight,
						const LbaNet::ObjectExtraInfo &extrainfo,
						const LbaNet::LifeManaInfo &lifeinfo,
						osg::ref_ptr<osg::Node> particle = osg::ref_ptr<osg::Node>());

	//! destructor
	virtual ~OsgObjectHandler();


	//!set object position in the world
	virtual void SetPosition(float X, float Y, float Z);

	//! set object rotation on X axis
	virtual void SetRotation(const LbaQuaternion& Q);

	//! show or hide the object
	virtual void ShowOrHide(bool Show);


	//! update display with current frame - used for animation
	virtual int Process(double time, float tdiff);

	//! update display
	virtual int Update(LbaNet::DisplayObjectUpdateBasePtr update,
							bool updatestoredstate);


	//! set name of display object that will be returned during picking
	virtual void SetName(const std::string & name);
	

	//! save object to file
	virtual void SaveToFile(const std::string & filename);

	//! tore object copy
	virtual void StoreObjectCopy();

	//! check if can play animation
	virtual bool CanPlayAnimation(const std::string & anim){return false;}


	//! set transparency material
	virtual void SetTransparencyMaterial(bool OnOff, float Alpha);


	//! set color material
	virtual void SetColorMaterial(	int MaterialType, 		
							float	MatAmbientColorR,
							float	MatAmbientColorG,
							float	MatAmbientColorB,
							float	MatAmbientColorA,			
							float	MatDiffuseColorR,
							float	MatDiffuseColorG,
							float	MatDiffuseColorB,
							float	MatDiffuseColorA,
							float	MatSpecularColorR,
							float	MatSpecularColorG,
							float	MatSpecularColorB,
							float	MatSpecularColorA,
							float	MatEmissionColorR,
							float	MatEmissionColorG,
							float	MatEmissionColorB,
							float	MatEmissionColorA,
							float	MatShininess);


	//! set shout text
	virtual void SetShoutText(const std::string & text, float size, 
								float colorR, float colorG, float colorB, int TTL);


	//! display talking icon
	virtual void DisplayOrHideTalkingIcon(bool display, bool left = true);

protected:
	// update matrix
	void UpdateMatrix();

	// return root object
	osg::ref_ptr<osg::Group> GetRoot();

	// return root object on no light scene
	osg::ref_ptr<osg::Group> GetRootNoLight();

	// update object extra info
	virtual void UpdateExtraInfo(const LbaNet::ObjectExtraInfo &info);

	// refresh text
	void RefreshText();

	// set t dont optimize
	void ForbidOptimize();

	// update object life info
	void UpdateLifeInfo(const LbaNet::LifeManaInfo &info);

	// refresh life/mana bars
	void RefreshLifeManaBars();

	//! update model
	void UpdateModel(const LbaNet::ModelInfo &mInfo);


	//! extract name from string
	std::string ExtractName(const std::string &text);

	//! find shapdrawable
	osg::ShapeDrawable* findShapeDrawable();

	//! refresh object material
	void RefreshMaterial();

	//! process
	virtual int ProcessInternal(double time, float tdiff)
	{return 0;} 

protected:

	float											_posX;
	float											_posY; 
	float											_posZ;
	LbaQuaternion									_Q;
	bool											_displayed;
	bool											_uselight;

	osg::ref_ptr<osg::AutoTransform>				_textgroup;
	LbaNet::ObjectExtraInfo							_extrainfo;

	osg::ref_ptr<osg::AutoTransform>				_barsgroup;
	LbaNet::LifeManaInfo							_lifeinfo;


private:
	osg::ref_ptr<osg::MatrixTransform>				_OsgObject;
	osg::ref_ptr<osg::MatrixTransform>				_OsgObjectNoLight;
	osg::ref_ptr<osg::PositionAttitudeTransform>	_osgpat;
	osg::ref_ptr<osg::PositionAttitudeTransform>	_osgpatNoLight;

	osg::ref_ptr<osg::MatrixTransform>				_ObjectCopy;

	osg::ref_ptr<osg::MatrixTransform>				_OsgVoiceSprite;
	osg::ref_ptr<osg::Node>							_particle;


	bool											_useTransparentMaterial;
	float											_materialAlpha;

	int												_MaterialType; 		
	float											_MatAmbientColorR;
	float											_MatAmbientColorG;
	float											_MatAmbientColorB;
	float											_MatAmbientColorA;			
	float											_MatDiffuseColorR;
	float											_MatDiffuseColorG;
	float											_MatDiffuseColorB;
	float											_MatDiffuseColorA;
	float											_MatSpecularColorR;
	float											_MatSpecularColorG;
	float											_MatSpecularColorB;
	float											_MatSpecularColorA;	
	float											_MatEmissionColorR;
	float											_MatEmissionColorG;
	float											_MatEmissionColorB;
	float											_MatEmissionColorA;
	float											_MatShininess;


	osg::ref_ptr<osg::AutoTransform>				_shouttextgroup;
	double											_ttlshout;
	int												_sceneidx;

};




#endif