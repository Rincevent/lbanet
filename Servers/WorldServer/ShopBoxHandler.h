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

#ifndef _SHOP_BOX_HANDLER_H__
#define _SHOP_BOX_HANDLER_H__


#include "ServerGUIBase.h"

namespace LbaNet
{
	class ClientServerEventBase;
}


//! take care of update comming from client ShopBox GUI
class ShopBoxHandler :	public ServerGUIBase
{
public:
	//! constructor
	ShopBoxHandler(MapHandler*	owner)
		: ServerGUIBase(owner)
	{}

	//! destructor
	virtual ~ShopBoxHandler(void){}

	//! update gui with info from server
	virtual void Update(Ice::Long clientid, const LbaNet::GuiUpdateBasePtr &Update);
	
	//! show the GUI for a certain player
	virtual void ShowGUI(Ice::Long clientid, const LbaNet::PlayerPosition &curPosition,
						boost::shared_ptr<ShowGuiParamBase> params);

	//! hide the GUI for a certain player
	virtual void HideGUI(Ice::Long clientid);


protected:

	//! buy item
	void BuyItem(long clientid, long ItemId);

	//! update money to client
	LbaNet::ClientServerEventBase * UpdateMoney(long currencyid, long clientid);

private:
	std::map<long, ShopParam>		_openedshops;
};

#endif