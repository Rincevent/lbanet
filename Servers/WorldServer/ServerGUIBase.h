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


#ifndef _LBANET_SERVER_GUI_BASE_H_
#define _LBANET_SERVER_GUI_BASE_H_


#include <ClientServerEvents.h>
#include <boost/shared_ptr.hpp>

class DialogPart;
class MapHandler;

// base class containing params when showing a gui
class ShowGuiParamBase
{

};

// class containing text param
class TextBoxParam : public ShowGuiParamBase
{
public:
	//! constructor
	TextBoxParam(long TextId)
		: _textid(TextId){}

	long	_textid;
};


// class containing text param
class ContainerParam : public ShowGuiParamBase
{
public:
	//! constructor
	ContainerParam(boost::shared_ptr<LbaNet::ContainerSharedInfo> sharedinfo,
						const LbaNet::ItemsMap & inventory,
						int InventorySize)
		: _sharedinfo(sharedinfo), _inventory(inventory),
			_InventorySize(InventorySize)
	{}

	int											_InventorySize;
	boost::shared_ptr<LbaNet::ContainerSharedInfo>		_sharedinfo;
	LbaNet::ItemsMap							_inventory;
};

// class containing text param
class ShopParam : public ShowGuiParamBase
{
public:
	//! default
	ShopParam(){}

	//! constructor
	ShopParam(const LbaNet::ItemsMap & shopinventory,
						const LbaNet::ItemInfo & currencyitem)
		: _shopinventory(shopinventory), _currencyitem(currencyitem)
	{}


	LbaNet::ItemsMap							_shopinventory;
	LbaNet::ItemInfo							_currencyitem;
};



// class containing text param
class DialogParam : public ShowGuiParamBase
{
public:
	//! constructor
	DialogParam(long npcid, long npcnametextid, bool simpledialog, boost::shared_ptr<DialogPart> dialogroot)
		: _npcid(npcid), _npcnametextid(npcnametextid), _simpledialog(simpledialog), _dialogroot(dialogroot)
	{}

	long										_npcid;
	long										_npcnametextid;
	bool										_simpledialog;
	boost::shared_ptr<DialogPart>				_dialogroot;
};


struct OpenedGuiInfo
{
	LbaNet::PlayerPosition		PositionAtOpening;
	double						TimeAtOpening;
	double						CloseTimer;
};

//*************************************************************************************************
//*                               class ServerGUIBase
//*************************************************************************************************
/**
* @brief Base class for GUIs hanlder
*
*/
class ServerGUIBase
{
public:

	//! constructor
	ServerGUIBase(MapHandler*	owner)
		: _owner(owner)
	{}


	//! destructor
	virtual ~ServerGUIBase(){}

	//! update gui with info from server
	virtual void Update(Ice::Long clientid, const LbaNet::GuiUpdateBasePtr &Update) = 0;


	//! show the GUI for a certain player
	virtual void ShowGUI(Ice::Long clientid, const LbaNet::PlayerPosition &curPosition,
						boost::shared_ptr<ShowGuiParamBase> params) = 0;


	//! hide the GUI for a certain player
	virtual void HideGUI(Ice::Long clientid) = 0;


	//! a player quitted the map - check if we need to do something (e.g. close the gui)
	void PlayerLeftMap(Ice::Long clientid);

	//! a player moved - check if we need to do something (e.g. close the gui)
	void PlayerMoved(Ice::Long clientid, const LbaNet::PlayerPosition &curPosition);

	//! process guis if needed
	void Process(double currenttime);

protected:
	//! add a player to the opened gui
	void AddOpenedGui(Ice::Long clientid, const LbaNet::PlayerPosition &curPosition,
						double closetimer = -1);

	//! remove a player from the opened gui
	void RemoveOpenedGui(Ice::Long clientid);

	//! check if client has gui opened
	bool HasOpenedGui(Ice::Long clientid);

protected:
	// used in mechanism to hide a gui if the player move away
	std::map<Ice::Long, OpenedGuiInfo>		_openedguis;

	MapHandler*								_owner;
};


#endif
