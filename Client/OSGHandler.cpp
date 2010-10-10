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

#include "OSGHandler.h"
#include "LogHandler.h"
#include "ConfigurationManager.h"
#include "OsgObjectHandler.h"
#include "ObjectsDescription.h"
#include "CEGUIDrawable.h"
#include "OsgEventHandler.h"
#include "GuiHandler.h"


#include <osg/PositionAttitudeTransform>
#include <osg/ClipNode>
#include <osg/ShapeDrawable>
#include <osg/CullFace>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osgShadow/ShadowedScene>
#include <osgShadow/StandardShadowMap>
#include <osgShadow/MinimalShadowMap>
#include <osgShadow/ShadowMap>

// win32 only
#include <osgViewer/api/Win32/GraphicsWindowWin32>



#include <math.h>
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif


OsgHandler* OsgHandler::_singletonInstance = NULL;





/***********************************************************
singleton pattern
***********************************************************/
OsgHandler * OsgHandler::getInstance()
{
    if(!_singletonInstance)
        _singletonInstance = new OsgHandler();

	return _singletonInstance;
}


/***********************************************************
constructor
***********************************************************/
OsgHandler::OsgHandler()
: _isFullscreen(false), _resX(800), _resY(600),
	_cameraType(0), _targetx(10), _targety(10), _targetz(10),
	_viewer(NULL), _root(NULL), _rootNode3d(NULL), _sceneRootNode(NULL), _translNode(NULL),
	_viewportX(800), _viewportY(600), _displayShadow(false), 
	_current_clip_layer(-1), _autoCameraType(1), _azimut(0)
{
	SetCameraDistance(30);
	SetCameraZenit(30);
}


/***********************************************************
destructor
***********************************************************/
OsgHandler::~OsgHandler()
{

}




/***********************************************************
initialize
***********************************************************/
void OsgHandler::Initialize(const std::string &WindowName, const std::string &DataPath,
							boost::shared_ptr<GuiHandler> GuiH)
{
	osgDB::setDataFilePathList(DataPath);


	LogHandler::getInstance()->LogToFile("Initializing graphics window...");




	// get data from configuration file
	{
		LogHandler::getInstance()->LogToFile("Initializing camera...");
		int ctype;
		double camdistance, camzenit;
		ConfigurationManager::GetInstance()->GetDouble("Display.Camera.Distance", camdistance);
		ConfigurationManager::GetInstance()->GetDouble("Display.Camera.Zenit", camzenit);
		ConfigurationManager::GetInstance()->GetInt("Display.Camera.CameraType", ctype);
		ToggleCameraType(ctype);
		SetCameraDistance(camdistance);
		SetCameraZenit(camzenit);


		ConfigurationManager::GetInstance()->GetInt("Display.Screen.PositionX", _posX);
		ConfigurationManager::GetInstance()->GetInt("Display.Screen.PositionY", _posY);
		ConfigurationManager::GetInstance()->GetInt("Display.Screen.ScreenResolutionX", _resX);
		ConfigurationManager::GetInstance()->GetInt("Display.Screen.ScreenResolutionY", _resY);
		ConfigurationManager::GetInstance()->GetBool("Display.Screen.Fullscreen", _isFullscreen);

		ConfigurationManager::GetInstance()->GetBool("Display.ShadowOn", _displayShadow);
	}


	// set SDL screen
	LogHandler::getInstance()->LogToFile("Initializing screen display...");

    _viewer = new osgViewer::Viewer();
	_viewer->setKeyEventSetsDone(0);



    // create the window to draw to.
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->x = _posX;
    traits->y = _posY;
    traits->width = _resX;
    traits->height = _resY;
    traits->windowDecoration = true;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
	traits->windowName = WindowName;
	traits->useCursor = false;
    osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits.get());
    _gw = dynamic_cast<osgViewer::GraphicsWindow*>(gc.get());
    if (!_gw)
        return;

    _viewer->getCamera()->setGraphicsContext(gc.get());
	_viewer->getCamera()->setClearColor(osg::Vec4(0, 0, 0, 1));
    _viewer->getCamera()->setViewport(0,0,_resX,_resY);

	_viewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	_viewer->realize();
	_viewer->getCamera()->getGraphicsContext()->makeCurrent(); 
	_viewer->setReleaseContextAtEndOfFrameHint(false);





	// create the root node used for camera transformation
	_rootNode3d = new osg::PositionAttitudeTransform();
	_rootNode3d->setScale(osg::Vec3d(1, 0.5, 1));
	_rootNode3d->setAttitude(osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,1,0)));
	_translNode = new osg::PositionAttitudeTransform();
	_rootNode3d->addChild(_translNode);



	_root=new osg::Group;
	_rootNodeGui=new osg::Group;
	osg::setNotifyLevel(osg::ALWAYS );

	osg::ref_ptr<osg::Geode> guiGeom=new osg::Geode;
	_HUDcam = new osg::Camera;

	_HUDcam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	_HUDcam->setClearMask(0);
	_HUDcam->setRenderOrder(osg::Camera::POST_RENDER, 100000);

	_HUDcam->setAllowEventFocus(false);
	_HUDcam->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OVERRIDE | 
														osg::StateAttribute::OFF);
    _HUDcam->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE |
														osg::StateAttribute::OFF);
    _HUDcam->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    _HUDcam->getOrCreateStateSet()->setRenderBinDetails(100,"RenderBin");
    _HUDcam->getOrCreateStateSet()->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    _HUDcam->getOrCreateStateSet()->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::OVERRIDE|
																			osg::StateAttribute::ON);
    _HUDcam->setProjectionMatrix(osg::Matrix::ortho2D(0,_resX,0,_resY));

	osg::CullFace *cull = new osg::CullFace();
	cull->setMode(osg::CullFace::BACK);
	_HUDcam->getOrCreateStateSet()->setAttributeAndModes(cull, osg::StateAttribute::ON);
	_HUDcam->addChild(_rootNodeGui.get());





	osg::ref_ptr<CEGUIDrawable> cd = new CEGUIDrawable(_resX, _resY, GuiH);
	_root->addChild(_rootNode3d.get());
	_root->addChild(_HUDcam.get());

	guiGeom->addDrawable(cd.get());
	_rootNodeGui->addChild(guiGeom.get());


	_viewer->setSceneData(_root.get());
	_viewer->setCameraManipulator(NULL);



	_clipNode = new osg::ClipNode();
	_clipNode->setStateSetModes(*_translNode->getOrCreateStateSet(),osg::StateAttribute::ON|osg::StateAttribute::PROTECTED|osg::StateAttribute::OVERRIDE);
	_translNode->addChild(_clipNode);


	_eventH = new OsgEventHandler();
	//TODO remove static and make it configurable
	std::map<LbanetKey, int> kmap;
	kmap[LbanetKey_Forward] = osgGA::GUIEventAdapter::KEY_Up;
	kmap[LbanetKey_Backward] = osgGA::GUIEventAdapter::KEY_Down;
	kmap[LbanetKey_Left] = osgGA::GUIEventAdapter::KEY_Left;
	kmap[LbanetKey_Right] = osgGA::GUIEventAdapter::KEY_Right;
	kmap[LbanetKey_StrafeL] = 'l';
	kmap[LbanetKey_StrafeR] = 'm';
	kmap[LbanetKey_Up] = osgGA::GUIEventAdapter::KEY_Page_Up;
	kmap[LbanetKey_Down] = osgGA::GUIEventAdapter::KEY_Page_Down;
	kmap[LbanetKey_Action] = osgGA::GUIEventAdapter::KEY_Space;
	kmap[LbanetKey_NAction] = 'w';
	kmap[LbanetKey_Weapon] = osgGA::GUIEventAdapter::KEY_Alt_L;
	kmap[LbanetKey_Stance1] = osgGA::GUIEventAdapter::KEY_F1;
	kmap[LbanetKey_Stance2] = osgGA::GUIEventAdapter::KEY_F2;
	kmap[LbanetKey_Stance3] = osgGA::GUIEventAdapter::KEY_F3;
	kmap[LbanetKey_Stance4] = osgGA::GUIEventAdapter::KEY_F4;
	kmap[LbanetKey_CenterCamera] = osgGA::GUIEventAdapter::KEY_Shift_R;
	_eventH->SetKeyMap(kmap);

	// add the stats handler
	_viewer->addEventHandler(_eventH );

	osgViewer::StatsHandler *sth = new osgViewer::StatsHandler;
	sth->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F11);
	_viewer->addEventHandler(sth);

	osgViewer::ScreenCaptureHandler *screenshoth = new osgViewer::ScreenCaptureHandler;
	screenshoth->setKeyEventTakeScreenShot(osgGA::GUIEventAdapter::KEY_F10);
	_viewer->addEventHandler(screenshoth);


    _viewer->realize();

	// reset camera
	ResetCameraProjectiomMatrix();

	//set default root node
	ResetDisplayTree();

	// put everything in the right place
	LogHandler::getInstance()->LogToFile("Initializing of graphics window done.");


}

/***********************************************************
get windows handle (win32 only)
***********************************************************/
void* OsgHandler::GetWindowsHandle()
{
	osgViewer::GraphicsWindowWin32* gw32 =  dynamic_cast<osgViewer::GraphicsWindowWin32*>(_gw);
	if(gw32)
		return (void*)gw32->getHWND();
		
	return NULL;
}



/***********************************************************
finalize function
***********************************************************/
void OsgHandler::Finalize()
{

	// write data to configuration file
	{
		ConfigurationManager::GetInstance()->SetDouble("Display.Camera.Distance", _distance);
		ConfigurationManager::GetInstance()->SetDouble("Display.Camera.Zenit", _zenit);
		ConfigurationManager::GetInstance()->SetInt("Display.Camera.CameraType", _cameraType);

		ConfigurationManager::GetInstance()->SetInt("Display.Screen.PositionX", _posX);
		ConfigurationManager::GetInstance()->SetInt("Display.Screen.PositionY", _posY);
		ConfigurationManager::GetInstance()->SetInt("Display.Screen.ScreenResolutionX", _resX);
		ConfigurationManager::GetInstance()->SetInt("Display.Screen.ScreenResolutionY", _resY);
		ConfigurationManager::GetInstance()->SetBool("Display.Screen.Fullscreen", _isFullscreen);

		ConfigurationManager::GetInstance()->SetBool("Display.ShadowOn", _displayShadow);
	}

	// clean up everything
	ResetScreen();
	_sceneRootNode = NULL;
	_rootNode3d = NULL;
	_root = NULL;
	_viewer = NULL;
	_translNode =NULL;
	_HUDcam =NULL;
	_rootNodeGui =NULL;
	_lightNode =NULL;
	_clipNode =NULL;
}



/***********************************************************
change screen resolution
***********************************************************/
void OsgHandler::Resize(int resX, int resY, bool fullscreen)
{
	if(resX != _resX || resY != _resY || fullscreen != _isFullscreen)
	{
		_resX = resX;
		_resY = resY;
		_isFullscreen = fullscreen;

		ResetScreen();

		ConfigurationManager::GetInstance()->SetInt("Display.Screen.ScreenResolutionX", _resX);
		ConfigurationManager::GetInstance()->SetInt("Display.Screen.ScreenResolutionY", _resY);
		ConfigurationManager::GetInstance()->SetBool("Display.Screen.Fullscreen", _isFullscreen);
	}
}


/***********************************************************
toggle fullscreen or windowed mode
***********************************************************/
void OsgHandler::ToggleFullScreen()
{
	_isFullscreen = !_isFullscreen;
	ResetScreen();

	ConfigurationManager::GetInstance()->SetBool("Display.Screen.Fullscreen", _isFullscreen);
}


/***********************************************************
set or reset screen
***********************************************************/
void OsgHandler::ResetScreen()
{
	LogHandler::getInstance()->LogToFile("Resetting screen resolution...");


	// iterate over all windows
	osgViewer::Viewer::Windows windows;
    _viewer->getWindows(windows);
    for(osgViewer::Viewer::Windows::iterator itr = windows.begin();  itr != windows.end(); ++itr)
    {
		osgViewer::GraphicsWindow *window = *itr;

		osg::GraphicsContext::WindowingSystemInterface    *wsi = osg::GraphicsContext::getWindowingSystemInterface();
		if (wsi == NULL) 
		{
			LogHandler::getInstance()->LogToFile("Error, no WindowSystemInterface available, cannot toggle window resolution change.");
			return;
		}

		unsigned int    screenWidth;
		unsigned int    screenHeight;
		wsi->getScreenResolution(*(window->getTraits()), screenWidth, screenHeight);

		if(_isFullscreen)
		{
			window->setWindowDecoration(false);
			window->setWindowRectangle(0, 0, screenWidth, screenHeight);
			_viewportX = screenWidth;
			_viewportY = screenHeight;
			
		}
		else
		{
			window->setWindowDecoration(true);
			window->setWindowRectangle(_posX, _posY, _resX, _resY);
			_viewportX = _resX;
			_viewportY = _resY;
		}

		_viewer->getCamera()->setViewport(0,0,_viewportX,_viewportY);
		window->grabFocus();
		window->raiseWindow();
    }

	ResetCameraProjectiomMatrix();

	_HUDcam->setProjectionMatrix(osg::Matrix::ortho2D(0,_resX,0,_resY));
}





/***********************************************************
set or reset camera projection matrix
***********************************************************/
void OsgHandler::ResetCameraProjectiomMatrix()
{
	if(!_viewer)
		return;

	if(_cameraType > 1 || ((_cameraType == 0) && (_autoCameraType > 1)))
	{
		_viewer->getCamera()->setProjectionMatrixAsPerspective(_fov,_viewportX/(double)_viewportY, 0.01,2000);
	}
	else
	{
		float factorX = _viewportX * 0.0001;
		float factorY = _viewportY * 0.0001;

		_viewer->getCamera()->setProjectionMatrixAsOrtho(-factorX*_distance, factorX*_distance,
															-factorY*_distance, factorY*_distance,
															-2000, 2000);
	}

	ResetCameraTransform();
}



/***********************************************************
set or reset camera transform
***********************************************************/
void OsgHandler::ResetCameraTransform()
{
	if(_viewer)
	{
		osg::Matrixd viewMatrix;
		if(_cameraType > 1 || ((_cameraType == 0) && (_autoCameraType > 1)))
		{
			osg::Matrixd cameraRotation1;
			osg::Matrixd cameraTrans;

			cameraRotation1.makeRotate(osg::DegreesToRadians(_zenit), osg::Vec3(1,0,0));
			cameraTrans.makeTranslate( 0,0,-_distance );
			viewMatrix = cameraRotation1* cameraTrans;
		}
		else
		{
			osg::Matrixd cameraRotation1;
			osg::Matrixd cameraTrans;

			cameraRotation1.makeRotate(osg::DegreesToRadians(30.0), osg::Vec3(1,0,0));
			cameraTrans.makeTranslate( 0,0,-1000 );
			viewMatrix = cameraRotation1 * cameraTrans;
		}


		_viewer->getCamera()->setViewMatrix(viewMatrix);
	}

	ResetCameraAzimut();
}



/***********************************************************
set or reset camera Azimut
***********************************************************/
void OsgHandler::ResetCameraAzimut()
{
	if(_rootNode3d)
	{
		if(_cameraType == 3 || ((_cameraType == 0) && (_autoCameraType == 3)))
		{
			_rootNode3d->setAttitude(osg::Quat(osg::DegreesToRadians(180-_azimut), osg::Vec3(0,1,0)));
		}
		else
		{
			_rootNode3d->setAttitude(osg::Quat(osg::DegreesToRadians(-45.0), osg::Vec3(0,1,0)));
		}
	}
}


/***********************************************************
set if the view is perspective or ortho
***********************************************************/
void OsgHandler::ToggleCameraType(int cameraType)
{
	if(_cameraType != cameraType)
	{
		_cameraType = cameraType;
		ResetCameraDistances();

		ConfigurationManager::GetInstance()->SetInt("Display.Camera.CameraType", _cameraType);
	}
}


/***********************************************************
set if the view is perspective or ortho
***********************************************************/
void OsgHandler::ToggleAutoCameraType(int cameraType)
{
	if(_autoCameraType != cameraType)
	{
		_autoCameraType = cameraType;
		if(_cameraType == 0)
			ResetCameraDistances();
	}
}	



/***********************************************************
delta update camera distance
***********************************************************/
void OsgHandler::DeltaUpdateCameraDistance(double delta)
{
	SetCameraDistance(_distance+delta);
}


/***********************************************************
set camera distance
***********************************************************/
void OsgHandler::SetCameraDistance(double distance, bool forced)
{
	if(forced || _distance != distance)
	{
		_distance = distance;
		int maxdistance = 150;
		if(_cameraType == 1 || ((_cameraType == 0) && (_autoCameraType == 1)))
			maxdistance = 1000;

		int mindistance = 30;
		if(_cameraType > 1 || ((_cameraType == 0) && (_autoCameraType > 1)))
			mindistance = 4;


		if(_distance < mindistance)
			_distance = mindistance;
		if(_distance > maxdistance)
			_distance = maxdistance;

		_fov=atan(40./_distance)*180./M_PI;

		ResetCameraProjectiomMatrix();
	}
}


/***********************************************************
delta update camera zenit
***********************************************************/
void OsgHandler::DeltaUpdateCameraZenit(double delta)
{
	SetCameraZenit(_zenit+delta);
}

/***********************************************************
set camera zenit
***********************************************************/
void OsgHandler::SetCameraZenit(double zenit, bool forced)
{
	if(forced || _zenit != zenit)
	{
		_zenit = zenit;

		int maxdistance = 70;

		int mindistance = 10;
		if(_cameraType > 2 || ((_cameraType == 0) && (_autoCameraType > 2)))
			mindistance = -70;


		if(_zenit < mindistance)
			_zenit = mindistance;
		if(_zenit > maxdistance)
			_zenit = maxdistance;

		ResetCameraTransform();
	}
}


/***********************************************************
set camera azimut
***********************************************************/
void OsgHandler::SetCameraAzimut(double azimut, bool forced)
{
	if(forced || _azimut != azimut)
	{
		_azimut = azimut;
		if(_azimut < 0)
			_azimut += 360;
		if(_azimut > 360)
			_azimut -= 360;

		ResetCameraAzimut();
	}
}


/***********************************************************
delta update camera zenit
***********************************************************/
void OsgHandler::DeltaUpdateCameraAzimut(double delta)
{
	SetCameraAzimut(_azimut+delta);
}



/***********************************************************
reset camera distances when changeing cam type
***********************************************************/
void OsgHandler::ResetCameraDistances()
{
	SetCameraDistance(_distance, true);
	SetCameraZenit(_zenit, true);
	SetCameraAzimut(_azimut, true);
}

/***********************************************************
set camera target
***********************************************************/
void OsgHandler::SetCameraTarget(double TargetX, double TargetY, double TargetZ)
{
	_targetx = TargetX;
	_targety = TargetY;
	_targetz = TargetZ;

	//ResetCameraTransform();
	if(_translNode)
		_translNode->setPosition(osg::Vec3d( -_targetx,-_targety,-_targetz ));
}
/***********************************************************
set camera target
***********************************************************/
void OsgHandler::GetCameraTarget(double &TargetX, double &TargetY, double &TargetZ)
{
	TargetX = _targetx;
	TargetY = _targety;
	TargetZ = _targetz;

}

/***********************************************************
set light
***********************************************************/
void OsgHandler::SetLight(const LbaMainLightInfo &LightInfo)
{
	if(!_lightNode)
		return;

	if(LightInfo.UseLight)
	{
		osg::Light* myLight1 = new osg::Light();
		myLight1->setLightNum(0);

		osg::Vec4 lightpos;
		lightpos.set(LightInfo.LOnPosX, LightInfo.LOnPosY, LightInfo.LOnPosZ,0.0f);
		myLight1->setPosition(lightpos);

		if(LightInfo.DirectionalLight)
		{
			osg::Vec3 lightdirection;
			lightdirection.set(LightInfo.LOnDirectionX, LightInfo.LOnDirectionY, LightInfo.LOnDirectionZ);
			myLight1->setDirection(lightdirection);
		}

		myLight1->setAmbient(osg::Vec4(LightInfo.LOnAmbientR,LightInfo.LOnAmbientG,LightInfo.LOnAmbientB, 1.0));
		myLight1->setDiffuse(osg::Vec4(LightInfo.LOnDiffuseR,LightInfo.LOnDiffuseG,LightInfo.LOnDiffuseB, 1.0));
		myLight1->setSpecular(osg::Vec4(LightInfo.LOnSpecularR,LightInfo.LOnSpecularG,LightInfo.LOnSpecularB, 1.0));

		if(LightInfo.UseAttenuation)
		{
			myLight1->setConstantAttenuation(LightInfo.Constant_attenuation);
			myLight1->setLinearAttenuation(LightInfo.Linear_attenuation);
			myLight1->setQuadraticAttenuation(LightInfo.Quadratic_attenuation);
		}

		if(LightInfo.UseExtendedSpotInfo)
		{
			myLight1->setSpotExponent(LightInfo.Spot_exponent);
			myLight1->setSpotCutoff(LightInfo.Spot_cutoff);
		}

		_lightNode->setLight(myLight1);
		_lightNode->setLocalStateSetModes(osg::StateAttribute::ON);
		_lightNode->setStateSetModes(*_translNode->getOrCreateStateSet(),osg::StateAttribute::ON);
	}
	else
	{
		_lightNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
	}

}


/***********************************************************
clear all nodes of the display tree
typically called when changing map
***********************************************************/
void OsgHandler::ResetDisplayTree()
{
	if(!_translNode)
		return;

	if(_lightNode)
		_translNode->removeChild(_lightNode);

	_lightNode = new osg::LightSource();
	_translNode->addChild(_lightNode);


	// check if we use shadow or not
	if(_displayShadow)
	{
		osg::ref_ptr<osgShadow::ShadowedScene> shadowscene = new osgShadow::ShadowedScene();
		shadowscene->setReceivesShadowTraversalMask(ReceivesShadowTraversalMask);
		shadowscene->setCastsShadowTraversalMask(CastsShadowTraversalMask);

		osg::ref_ptr<osgShadow::StandardShadowMap> shmap = new osgShadow::StandardShadowMap();
		//osg::ref_ptr<osgShadow::MinimalShadowMap> shmap = new osgShadow::MinimalShadowMap();
		shadowscene->setShadowTechnique(shmap.get());
		_sceneRootNode = shadowscene;
	}
	else
	{
		_sceneRootNode = new osg::Group();	
	}

	_lightNode->addChild(_sceneRootNode);

	_sceneNoLightRootNode = new osg::Group();	
	_translNode->addChild(_sceneNoLightRootNode);

	// grid to check dimensions
	//{
	//	osg::ref_ptr<osg::Geode> linesgeode = new osg::Geode();

	//	
	//	{
	//		osg::ref_ptr<osg::Geometry> myGeometry = new osg::Geometry();
	//		osg::Vec3Array* myVertices = new osg::Vec3Array;
	//		osg::DrawElementsUInt* myprimitive = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);

	//		for(int i=0; i<200; ++i)
	//		{
	//			myVertices->push_back(osg::Vec3(0, i, 0));
	//			myVertices->push_back(osg::Vec3(0, i, 200));

	//			myVertices->push_back(osg::Vec3(0, i, 0));
	//			myVertices->push_back(osg::Vec3(200, i, 0));

	//			myVertices->push_back(osg::Vec3(0, 0, i));
	//			myVertices->push_back(osg::Vec3(0, 200, i));

	//			myVertices->push_back(osg::Vec3(i, 0, 0));
	//			myVertices->push_back(osg::Vec3(i, 200, 0));
	//		}

	//		for(int i=0; i<1600; ++i)
	//			myprimitive->push_back(i);


	//		osg::Vec4Array* mycolors = new osg::Vec4Array;
	//		mycolors->push_back(osg::Vec4(1,1,1,1));
	//		myGeometry->setColorArray(mycolors);
	//		myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	//		myGeometry->addPrimitiveSet(myprimitive);
	//		myGeometry->setVertexArray( myVertices ); 
	//		linesgeode->addDrawable(myGeometry.get());
	//	}

	//	osg::StateSet* stateSet = linesgeode->getOrCreateStateSet();
	//	stateSet->setMode(GL_TEXTURE_2D,osg::StateAttribute::OFF);
	//	stateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);

	//	_sceneNoLightRootNode->addChild(linesgeode);
	//}



	//set default light
	LbaMainLightInfo Linf;
	Linf.UseLight = false;
	SetLight(Linf);
}



/***********************************************************
update display - returns true if need to terminate
***********************************************************/
bool OsgHandler::Update()
{
	if(!_viewer->done())
	{
		// draw the new frame
		_viewer->frame();

		return false;
	}

	return true;
}


/***********************************************************
load osg files into a osg node
***********************************************************/
osg::ref_ptr<osg::Node> OsgHandler::LoadOSGFile(const std::string & filename)
{
	return osgDB::readNodeFile(filename);
}



/***********************************************************
add a actor to the display list - return handler to actor position
***********************************************************/
osg::ref_ptr<osg::MatrixTransform> OsgHandler::AddActorNode(osg::ref_ptr<osg::Node> node,
																bool UseLight, bool CastShadow)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Added Node to display engine");
	#endif

	if(CastShadow)
	{
		node->setNodeMask(ReceivesShadowTraversalMask | CastsShadowTraversalMask);
	}
	else
	{
		node->setNodeMask(ReceivesShadowTraversalMask);
	}

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
	transform->addChild(node);

	if(UseLight && _sceneRootNode)
		_sceneRootNode->addChild(transform);

	if(!UseLight && _sceneNoLightRootNode)
		_sceneNoLightRootNode->addChild(transform);

	return transform;
}


/***********************************************************
add an empty actor to the display list - return handler to actor position
***********************************************************/
osg::ref_ptr<osg::MatrixTransform> OsgHandler::AddEmptyActorNode(bool WithLight)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Added empty Node to display engine");
	#endif

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
	if(WithLight && _sceneRootNode)
		_sceneRootNode->addChild(transform);

	if(!WithLight && _sceneNoLightRootNode)
		_sceneNoLightRootNode->addChild(transform);


	return transform;
}


/***********************************************************
readd a removed actor to the display list
***********************************************************/
void OsgHandler::ReAddActorNode(osg::ref_ptr<osg::Node> node, bool WithLight)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("ReAdded Node to display engine");
	#endif

	if(WithLight && _sceneRootNode)
		_sceneRootNode->addChild(node);

	if(!WithLight && _sceneNoLightRootNode)
		_sceneNoLightRootNode->addChild(node);
}


/***********************************************************
remove actor from the graph
***********************************************************/
void OsgHandler::RemoveActorNode(osg::ref_ptr<osg::Node> node, bool WithLight)
{
	#ifdef _DEBUG
		LogHandler::getInstance()->LogToFile("Removed Node from display engine");
	#endif

	if(WithLight && _sceneRootNode)
		_sceneRootNode->removeChild(node);

	if(!WithLight && _sceneNoLightRootNode)
		_sceneNoLightRootNode->removeChild(node);
}




/***********************************************************
set clip plane cut layer
***********************************************************/
void OsgHandler::SetClipPlane(float layer)
{
	if(_current_clip_layer == layer)
		return;

	_current_clip_layer = layer;

	_clipNode->setStateSetModes(*_translNode->getOrCreateStateSet(),osg::StateAttribute::OFF);
	_clipNode->removeClipPlane((unsigned int)0);


	if(layer > 0)
	{
		osg::ref_ptr<osg::ClipPlane> clipplane = new osg::ClipPlane();
		clipplane->setClipPlane(0, -1, 0, layer+0.01);
		_clipNode->addClipPlane(clipplane);
		_clipNode->setStateSetModes(*_translNode->getOrCreateStateSet(),osg::StateAttribute::ON);
	}
}



/***********************************************************
set screen attributes
***********************************************************/
void OsgHandler::GetScreenAttributes(int &resX, int &resY, bool &fullscreen)
{
	fullscreen = _isFullscreen;
	resX = _resX;
	resY = _resY;
}





/***********************************************************
create simple display object
***********************************************************/
boost::shared_ptr<DisplayObjectHandlerBase> OsgHandler::CreateSimpleObject(const std::string & filename,
														boost::shared_ptr<DisplayTransformation> Tr,
														bool UseLight, bool CastShadow)
{
	osg::ref_ptr<osg::Node> resnode = LoadOSGFile(filename);

	if(Tr)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
		transform->setPosition(osg::Vec3d(Tr->translationX, Tr->translationY, Tr->translationZ));
		transform->setAttitude(osg::Quat(Tr->rotation.X, Tr->rotation.Y, Tr->rotation.Z, Tr->rotation.W));
		transform->setScale(osg::Vec3d(Tr->scaleX, Tr->scaleY, Tr->scaleZ));

		transform->addChild(resnode);
		resnode = transform;
	}
	
	osg::ref_ptr<osg::MatrixTransform> mat = AddActorNode(resnode, UseLight, CastShadow);
	return boost::shared_ptr<DisplayObjectHandlerBase>(new OsgObjectHandler(mat, UseLight));
}



/***********************************************************
create capsule object
***********************************************************/
boost::shared_ptr<DisplayObjectHandlerBase> OsgHandler::CreateCapsuleObject(float radius, float height, 
														float colorR, float colorG, float colorB, float colorA,
														boost::shared_ptr<DisplayTransformation> Tr)
{
	osg::ref_ptr<osg::Group> resnode = new osg::Group();

	// create capsule
	osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
	osg::ref_ptr<osg::Capsule> caps(new osg::Capsule(osg::Vec3(0,0,0),radius,height));
	osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
	//capsdraw->setColor(osg::Vec4(colorR, colorG, colorB, colorA));
	capsuleGeode->addDrawable(capsdraw);
	resnode->addChild(capsuleGeode);

	// create orientation line
	osg::Geode* lineGeode = new osg::Geode();
	osg::Geometry* lineGeometry = new osg::Geometry();
	lineGeode->addDrawable(lineGeometry); 

	osg::Vec3Array* lineVertices = new osg::Vec3Array();
	lineVertices->push_back( osg::Vec3( 0, 0, 0) );
	lineVertices->push_back( osg::Vec3(0, 5, 0) );
	lineGeometry->setVertexArray( lineVertices ); 

	osg::DrawElementsUInt* dunit = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	dunit->push_back(0);
	dunit->push_back(1);
	lineGeometry->addPrimitiveSet(dunit); 
	resnode->addChild(lineGeode);

	if(Tr)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
		transform->setPosition(osg::Vec3d(Tr->translationX, Tr->translationY, Tr->translationZ));
		transform->setAttitude(osg::Quat(Tr->rotation.X, Tr->rotation.Y, Tr->rotation.Z, Tr->rotation.W));
		transform->setScale(osg::Vec3d(Tr->scaleX, Tr->scaleY, Tr->scaleZ));

		transform->addChild(resnode);
		resnode = transform;
	}
	
	osg::ref_ptr<osg::MatrixTransform> mat = AddActorNode(resnode, true, true);
	return boost::shared_ptr<DisplayObjectHandlerBase>(new OsgObjectHandler(mat, true));
}


/***********************************************************
create PAT
***********************************************************/
osg::ref_ptr<osg::PositionAttitudeTransform> OsgHandler::CreatePAT(boost::shared_ptr<DisplayTransformation> Tr)
{
	if(Tr)
	{
		osg::ref_ptr<osg::PositionAttitudeTransform> transform = new osg::PositionAttitudeTransform();
		transform->setPosition(osg::Vec3d(Tr->translationX, Tr->translationY, Tr->translationZ));
		transform->setAttitude(osg::Quat(Tr->rotation.X, Tr->rotation.Y, Tr->rotation.Z, Tr->rotation.W));
		transform->setScale(osg::Vec3d(Tr->scaleX, Tr->scaleY, Tr->scaleZ));

		return transform;
	}

	return osg::ref_ptr<osg::PositionAttitudeTransform>();
}



