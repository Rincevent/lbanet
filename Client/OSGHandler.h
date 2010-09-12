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


#ifndef _LBANET_OSG_HANDLER_H_
#define _LBANET_OSG_HANDLER_H_

#include <vector>
#include <string>
#include <osg/ref_ptr>
#include <boost/shared_ptr.hpp>

#include "CommonTypes.h"
#include "DisplayHandlerBase.h"

namespace osg
{
	class Node;
	class PositionAttitudeTransform;
	class Group;
	class MatrixTransform;
	class LightSource;
	class ClipNode;
	class Camera;
}

namespace osgViewer
{
	class Viewer;
	class GraphicsWindow;
}

namespace osgShadow
{
	class ShadowedScene;
}

class GuiHandler;


//*************************************************************************************************
//*                               class OsgHandler
//*************************************************************************************************
/**
* @brief Class takign care of the OSG 3D engine
*
*/
class OsgHandler : public DisplayHandlerBase
{
public:

	// singleton pattern
	static OsgHandler * getInstance();

	//! destructor
	~OsgHandler();

	//! initialize
	void Initialize(const std::string &WindowName, const std::string &DataPath,
							boost::shared_ptr<GuiHandler> GuiH);

	//! finalize function
	void Finalize();


	//! change screen resolution
	void Resize(int resX, int resY, bool fullscreen);


	//! toggle fullscreen or windowed mode
	void ToggleFullScreen();
	
	//! set if the view is perspective or ortho
	void TogglePerspectiveView(bool Perspective);

	//! set camera distance
	void SetCameraDistance(double distance);

	//! delta update camera distance
	void DeltaUpdateCameraDistance(double delta);

	//! set camera zenit
	void SetCameraZenit(double zenit);

	//! delta update camera zenit
	void DeltaUpdateCameraZenit(double delta);

	//! set camera target
	void SetCameraTarget(double TargetX, double TargetY, double TargetZ);
	void GetCameraTarget(double &TargetX, double &TargetY, double &TargetZ);

	//! clear all nodes of the display tree
	//! typically called when changing map
	//! set if new map uses lighning or not
	void ResetDisplayTree();

	//! update display - returns true if need to terminate
	bool Update();

	//! load osg files into a osg node
	osg::ref_ptr<osg::Node> LoadOSGFile(const std::string & filename);

	//! add a actor to the display list - return handler to actor position
	osg::ref_ptr<osg::MatrixTransform> AddActorNode(osg::ref_ptr<osg::Node> node);

	//! readd a removed actor to the display list
	void ReAddActorNode(osg::ref_ptr<osg::Node> node);

	//! remove actor from the graph
	void RemoveActorNode(osg::ref_ptr<osg::Node> node);

	//! set light
	void SetLight(const LbaMainLightInfo &LightInfo);


	//! set clip plane cut layer
	void SetClipPlane(float layer);

	//! set screen attributes
	void SetScreenAttributes(int resX, int resY, bool fullscreen);

	//! set screen attributes
	void GetScreenAttributes(int &resX, int &resY, bool &fullscreen);
	
	//! check if the view is perspective or ortho
	bool IsPerspectiveView()
	{return _isPerspective;}



	//! create simple display object
	virtual boost::shared_ptr<DisplayObjectHandlerBase> CreateSimpleObject(const std::string & filename,
															boost::shared_ptr<DisplayTransformation> Tr);


	//! create capsule object
	virtual boost::shared_ptr<DisplayObjectHandlerBase> CreateCapsuleObject(float radius, float height, 
															float colorR, float colorG, float colorB, float colorA,
															boost::shared_ptr<DisplayTransformation> Tr);

	//! get windows handle (win32 only)
	void* GetWindowsHandle();

protected:
	//! constructor
	OsgHandler();

	//! set or reset screen
	void ResetScreen();

	//! set or reset camera projection matrix
	void ResetCameraProjectiomMatrix();

	//! set or reset camera transform
	void ResetCameraTransform();

private:
	// singleton
	static OsgHandler * _singletonInstance;

	// pointer to the screen surface
	osgViewer::GraphicsWindow*		_gw;


	// screen info
	bool	_isFullscreen;
	int		_posX;
	int		_posY;
	int		_resX;
	int		_resY;
	int		_viewportX;
	int		_viewportY;

	bool	_displayShadow;


	// camera info
	bool	_isPerspective;
	double	_targetx;
	double	_targety;
	double	_targetz;
	double	_fov;
	double	_distance;
	double	_zenit;
	float	_current_clip_layer;


	// osg handlers
	osg::ref_ptr<osgViewer::Viewer>					_viewer;
	osg::ref_ptr<osg::PositionAttitudeTransform>	_rootNode3d;
	osg::ref_ptr<osg::Group>						_root;
	osg::ref_ptr<osg::Group>						_rootNodeGui;
	osg::ref_ptr<osg::PositionAttitudeTransform>	_translNode;
	osg::ref_ptr<osg::LightSource>					_lightNode;
	osg::ref_ptr<osg::Group>						_sceneRootNode;
	osg::ref_ptr<osg::ClipNode>						_clipNode;
	osg::ref_ptr<osg::Camera>						_HUDcam;
};


#endif