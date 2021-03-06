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

#ifndef _LBA_HOLOMAP_H__
#define _LBA_HOLOMAP_H__


#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <LbaTypes.h>

//! HoloCoordinate
class HoloCoordinate
{
public:
	//! constructor
	HoloCoordinate()
		: polarcoords(true), posX(0), posY(0), posZ(0)
		, dir_cen_X(0), dir_cen_Y(0), dir_cen_Z(0)
	{}

	bool							polarcoords;
	float							posX;
	float							posY;
	float							posZ;

	float							dir_cen_X;
	float							dir_cen_Y;
	float							dir_cen_Z;
};

class DisplayInfoHandler
{
public:
	//! constructor
	DisplayInfoHandler();

	// save to lua file
	virtual void SaveToLuaFile(std::ostream & file, const std::string & name);

	// set enum for render type as lua does not map enum
	//1 - RenderOsgModel
	//2 - RenderSprite
	//3 - RenderLba1M
	//4 - RenderLba2M
	void SetRenderType(int rtype);

	// accessors
	int GetRenderType();


	LbaNet::ModelInfo				Dinfo;
};


//! HolomapLocation
class HolomapLocation
{
public:
	//! constructor
	HolomapLocation(long id)
		: _Id(id), _textid(-1), _childholomapid(-1), 
			_parentholomaplocid(-1),
			_parentholomapid(-1)
	{}

	//! destructor
	~HolomapLocation(void){}

	//! get quest id
	long GetId()
	{return _Id;}

	//! set location coordinate
	void SetCoordinate(HoloCoordinate coord)
	{_coordinate = coord;}

	//! get location coordinate
	HoloCoordinate GetCoordinate()
	{return _coordinate;}


	//! get text id
	long GetTextId()
	{return _textid;}

	//! get text id
	void SetTextId(long tid)
	{_textid = tid;}


	//! get text id
	long GetParentLocId()
	{return _parentholomaplocid;}

	//! get text id
	void SetParentLocId(long id)
	{_parentholomaplocid = id;}

	//! get parent holo id
	long GetParentHoloId()
	{return _parentholomapid;}

	//! get parent holo id
	void SetParentHoloId(long id)
	{_parentholomapid = id;}

	//! get text id
	long GetChildHoloId()
	{return _childholomapid;}

	//! get text id
	void SetChildHoloId(long id)
	{_childholomapid = id;}


	//! accessor
	std::string GetName() const
	{return _Name;}

	//! accessor
	void SetName(const std::string &name)
	{_Name = name;}
	

	// save to lua file
	virtual void SaveToLuaFile(std::ostream & file);

private:
	long							_Id;
	HoloCoordinate					_coordinate;
	std::string						_Name;

	long							_textid;
	long							_parentholomaplocid;
	long							_parentholomapid;
	long							_childholomapid;
};



//! HolomapTravelPath
class HolomapTravelPath
{
public:
	//! constructor
	HolomapTravelPath(long id)
		: _Id(id), _parentholomapid(-1), _movingSpeed(0.1f)
	{}

	//! destructor
	~HolomapTravelPath(void){}

	//! get quest id
	long GetId()
	{return _Id;}

	//! add location coordinate
	void AddCoordinate(const HoloCoordinate &coord)
	{_coordinates.push_back(coord);}

	//! remove location coordinate
	void RemoveCoordinate(int index);

	//! get path coordinates
	std::vector<HoloCoordinate> & GetCoordinates()
	{return _coordinates;}

	// model accessors
	void SetVehicleModel(const DisplayInfoHandler& model)
	{_vehiclemodel = model;}

	const LbaNet::ModelInfo &GetVehicleModel()
	{return _vehiclemodel.Dinfo;}

	//! accessor
	std::string GetName() const
	{return _Name;}

	//! accessor
	void SetName(const std::string &name)
	{_Name = name;}


	//! get parent holo id
	long GetParentHoloId()
	{return _parentholomapid;}

	//! get parent holo id
	void SetParentHoloId(long id)
	{_parentholomapid = id;}

	float GetMovingSpeed()
	{return _movingSpeed;}

	void SetMovingSpeed(float s)
	{_movingSpeed = s;}


	// save to lua file
	virtual void SaveToLuaFile(std::ostream & file);

private:
	long									_Id;
	std::vector<HoloCoordinate >			_coordinates;
	std::string								_Name;

	long									_parentholomapid;
	DisplayInfoHandler						_vehiclemodel;

	float									_movingSpeed;
};





//! take care of holomap info
class Holomap
{
public:
	//! constructor
	Holomap(long id)
		: _Id(id), _usePlayerCoordinates(false),
		_3DCoordinateScaleX(1), _3DCoordinateScaleY(1), _3DCoordinateScaleZ(1),
		_distanceBeetweenPathPoints(1)
	{}

	//! destructor
	~Holomap(void){}

	//! get quest id
	long GetId()
	{return _Id;}

	// model accessors
	void SetMapModel(const DisplayInfoHandler& model)
	{_mapmodel = model;}
	void SetArrowModel(const DisplayInfoHandler& model)
	{_arrowmodel = model;}
	void SetPlayerModel(const DisplayInfoHandler& model)
	{_playermodel = model;}
	void SetArrowPlayerModel(const DisplayInfoHandler& model)
	{_arrowplayermodel = model;}
	void SetTravelDotModel(const DisplayInfoHandler& model)
	{_traveldotmodel = model;}

	const LbaNet::ModelInfo &GetMapModel(){return _mapmodel.Dinfo;}
	const LbaNet::ModelInfo &GetArrowModel(){return _arrowmodel.Dinfo;}
	const LbaNet::ModelInfo &GetPlayerModel(){return _playermodel.Dinfo;}
	const LbaNet::ModelInfo &GetArrowPlayerModel(){return _arrowplayermodel.Dinfo;}
	const LbaNet::ModelInfo &GetTravelDotModel(){return _traveldotmodel.Dinfo;}

	//! accessor
	std::string GetName() const
	{return _Name;}

	//! accessor
	void SetName(const std::string &name)
	{_Name = name;}

	// save to lua file
	virtual void SaveToLuaFile(std::ostream & file);

	void SetUsePCoordinates(bool use)
	{ _usePlayerCoordinates = use;}
	bool GetUsePCoordinates()
	{ return _usePlayerCoordinates;}

	void Set3DMap(bool d3m)
	{ _3dmap = d3m;}
	bool Get3DMap()
	{ return _3dmap;}


	void Set3DCoordinateScaleX(float val)
	{ _3DCoordinateScaleX = val;}
	bool Get3DCoordinateScaleX()
	{ return _3DCoordinateScaleX;}

	void Set3DCoordinateScaleY(float val)
	{ _3DCoordinateScaleY = val;}
	bool Get3DCoordinateScaleY()
	{ return _3DCoordinateScaleY;}

	void Set3DCoordinateScaleZ(float val)
	{ _3DCoordinateScaleZ = val;}
	bool Get3DCoordinateScaleZ()
	{ return _3DCoordinateScaleZ;}

	float GetDistanceBetweenPathPoints()
	{return _distanceBeetweenPathPoints;}

	void SetDistanceBetweenPathPoints(float d)
	{_distanceBeetweenPathPoints = d;}

private:
	long													_Id;
	std::string												_Name;

	bool													_usePlayerCoordinates;
	bool													_3dmap;

	DisplayInfoHandler										_mapmodel;
	DisplayInfoHandler										_arrowmodel;
	DisplayInfoHandler										_playermodel;
	DisplayInfoHandler										_arrowplayermodel;
	DisplayInfoHandler										_traveldotmodel;

	float													_3DCoordinateScaleX;
	float													_3DCoordinateScaleY;
	float													_3DCoordinateScaleZ;

	float													_distanceBeetweenPathPoints;

};

#endif
