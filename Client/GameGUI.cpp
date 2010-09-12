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

#include "GameGUI.h"
#include <CEGUI.h>
#include "LogHandler.h"
#include "MusicHandler.h"
#include "MessageBox.h"
#include "ChooseNumberBox.h"
#include "EventsQueue.h"
#include "ClientExtendedEvents.h"
#include "ClientExtendedTypes.h"
#include "DataLoader.h"

#include "InventoryBox.h"
#include "ShortcutBox.h"
#include "ContainerBox.h"
#include "LetterEditorBox.h"
#include "LetterViewerBox.h"
#include "DialogBox.h"
#include "JournalBox.h"
#include "MailBox.h"
#include "ShopBox.h"


/***********************************************************
constructor
***********************************************************/
GameGUI::GameGUI()
{
	_gameguis["ChatBox"] = boost::shared_ptr<GameGUIBase>(new ChatBox());

	CommunityBox * combox = new CommunityBox();
	InventoryBox * invbox = new InventoryBox(50);
	_gameguis["CommunityBox"] = boost::shared_ptr<GameGUIBase>(combox);
	_gameguis["TeleportBox"] = boost::shared_ptr<GameGUIBase>(new TeleportBox());
	_gameguis["InventoryBox"] = boost::shared_ptr<GameGUIBase>(invbox);
	_gameguis["ShortcutBox"] = boost::shared_ptr<GameGUIBase>(new ShortcutBox(50, invbox));
	_gameguis["JournalBox"] = boost::shared_ptr<GameGUIBase>(new JournalBox());
	_gameguis["ContainerBox"] = boost::shared_ptr<GameGUIBase>(new ContainerBox(50));
	_gameguis["LetterEditorBox"] = boost::shared_ptr<GameGUIBase>(new LetterEditorBox());
	_gameguis["LetterViewerBox"] = boost::shared_ptr<GameGUIBase>(new LetterViewerBox());
	_gameguis["DialogBox"] = boost::shared_ptr<GameGUIBase>(new NPCDialogBox());
	_gameguis["MailBox"] = boost::shared_ptr<GameGUIBase>(new MailBox(combox));
	_gameguis["ShopBox"] = boost::shared_ptr<GameGUIBase>(new NPCShopBox(50));


}


/***********************************************************
destructor
***********************************************************/
GameGUI::~GameGUI()
{
}

/***********************************************************
initialize the GUI
***********************************************************/
void GameGUI::Initialize()
{
	try
	{
		_root = CEGUI::WindowManager::getSingleton().loadWindowLayout( "GameGuiRoot.layout" );

		std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.begin();
		std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator end = _gameguis.end();
		for(; it != end; ++it)
			it->second->Initialize(_root);

		CGMessageBox::getInstance()->Initialize(_root);
		ChooseNumberBox::getInstance()->Initialize(_root);


		CEGUI::WindowManager::getSingleton().getWindow("HeadInterfaceBG")->disable();
		CEGUI::WindowManager::getSingleton().getWindow("MenuCharInterfaceBG")->disable();

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnchaticon"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleChatIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btncomicon"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleComIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnteleporticon"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleTeleIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnquit"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleQuitIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnsound"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleSoundIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnchangeworld"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleChangeWorldIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnoption"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleOptionIconClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame/bok"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleCloseTextClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btntunic"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleInventoryClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnweapon"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleShortcutClicked, this));

		static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnquest"))->subscribeEvent (
			CEGUI::PushButton::EventClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleJournalClicked, this));

		

		
		CEGUI::FrameWindow * frw = static_cast<CEGUI::FrameWindow *> (
			CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame"));
		frw->subscribeEvent (CEGUI::FrameWindow::EventCloseClicked,
			CEGUI::Event::Subscriber (&GameGUI::HandleCloseTextClicked, this));


		CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->hide();
	}
	catch(CEGUI::Exception &ex)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception init login gui: ") + ex.getMessage().c_str());
		_root = NULL;
	}

	RefreshSOundButton();
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleChatIconClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("ChatBox");
	return true;
}



/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleComIconClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("CommunityBox");
	return true;
}




/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleTeleIconClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("TeleportBox");
	return true;
}

/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleQuitIconClicked (const CEGUI::EventArgs& e)
{
	EventsQueue::getReceiverQueue()->AddEvent(new DisplayGUIEvent(3));
	return true;
}

/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleSoundIconClicked (const CEGUI::EventArgs& e)
{
	MusicHandler::getInstance()->EnableDisableSound();
	RefreshSOundButton();
	return true;
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleChangeWorldIconClicked (const CEGUI::EventArgs& e)
{
	EventsQueue::getReceiverQueue()->AddEvent(new DisplayGUIEvent(1));
	return true;
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleOptionIconClicked (const CEGUI::EventArgs& e)
{
	EventsQueue::getReceiverQueue()->AddEvent(new DisplayGUIEvent(4));
	return true;
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleInventoryClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("InventoryBox");
	return true;
}

/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleShortcutClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("ShortcutBox");
	return true;
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleJournalClicked (const CEGUI::EventArgs& e)
{
	ToggleDisplayGUI("JournalBox");
	return true;
}


/***********************************************************
handle send button event
***********************************************************/
bool GameGUI::HandleCloseTextClicked (const CEGUI::EventArgs& e)
{
	try
	{
		CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->hide();
	}
	catch(CEGUI::Exception &ex)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception hidding text window: ") + ex.getMessage().c_str());
		_root = NULL;
	}

	return true;
}




/***********************************************************
refreshthe gui
***********************************************************/
void GameGUI::Refresh()
{
	//_cb.Show();_cb.Show();
	//_comb.Show();_comb.Show();
	//_telb.Show();_telb.Show();
	//_invb.Show();_invb.Show();
	//_shortb.Show();_shortb.Show();

	//_containerb.Refresh();
	//_invb.Refresh();
	//_shortb.Refresh();

	//CEGUI::Window * win = CEGUI::WindowManager::getSingleton().getWindow("PlayerName");
	//win->hide();
	//win->show();
}



/***********************************************************
process what is needed in the game GUI
***********************************************************/
void GameGUI::Process()
{
	//_cb.Process();
	//_comb.Process();
	//_shortb.Process();
	//_invb.Process();
	//_containerb.Process();
	//_dialogb.Process();
	//_journalb.Process();
}





/***********************************************************
refresh gui with info from server
***********************************************************/
void GameGUI::RefreshGUI(const std::string & guiid, const LbaNet::GuiParamsSeq &Parameters, bool Show, bool Hide)
{
	std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
	if(it != _gameguis.end())
	{
		it->second->Refresh(Parameters);

		if(Show)
			it->second->Display();

		if(Hide)
			it->second->Hide();
	}
}

/***********************************************************
refresh gui with info from server
***********************************************************/
//! update gui with info from server
void GameGUI::UpdateGUI(const std::string & guiid,const LbaNet::GuiUpdatesSeq &Updates)
{
	// case if this concern himself directly
	if(guiid == "main")
	{
		for(size_t i=0; i<Updates.size(); ++i)
		{
			LbaNet::GuiUpdateBase * ptr = Updates[i].get();
			const std::type_info& info = typeid(*ptr);

			// RefreshSOundButton
			if(info == typeid(RefreshSoundButtonUpdate))
			{
				RefreshSOundButton();

			}

			// DisplayGameText
			if(info == typeid(LbaNet::DisplayGameText))
			{
				LbaNet::DisplayGameText * castedptr = 
					dynamic_cast<LbaNet::DisplayGameText *>(ptr);

				DisplayGameText((long)castedptr->Textid, castedptr->Show);
			}

			// SetPlayerNameUpdate
			if(info == typeid(SetPlayerNameUpdate))
			{
				SetPlayerNameUpdate * castedptr = 
					dynamic_cast<SetPlayerNameUpdate *>(ptr);

				SetPlayerName(castedptr->_Name);
			}

			// SystemMessageUpdate
			if(info == typeid(LbaNet::SystemMessageUpdate))
			{
				LbaNet::SystemMessageUpdate * castedptr = 
					dynamic_cast<LbaNet::SystemMessageUpdate *>(ptr);

				CGMessageBox::getInstance()->Show(castedptr->Title, castedptr->Message);
			}
		}
	}
	else
	{
		std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
		if(it != _gameguis.end())
			it->second->Update(Updates);
	}
}

/***********************************************************
refresh gui with info from server
***********************************************************/
//! show GUI
void GameGUI::ShowGUI(const std::string & guiid)
{
	std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
	if(it != _gameguis.end())
		it->second->Display();
}

/***********************************************************
hide GUI
***********************************************************/
void GameGUI::HideGUI(const std::string & guiid)
{
	std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
	if(it != _gameguis.end())
		it->second->Hide();
}

/***********************************************************
show/hide GUI
***********************************************************/
void GameGUI::ToggleDisplayGUI(const std::string & guiid)
{
	std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
	if(it != _gameguis.end())
		it->second->ToggleDisplay();
}

/***********************************************************
focus GUI
***********************************************************/
void GameGUI::FocusGUI(const std::string & guiid, bool focus)
{
	std::map<std::string, boost::shared_ptr<GameGUIBase> >::iterator it = _gameguis.find(guiid);
	if(it != _gameguis.end())
		it->second->Focus(focus);
}







/***********************************************************
refresh sound button
***********************************************************/
void GameGUI::RefreshSOundButton()
{
	try
	{
		if(!MusicHandler::getInstance()->IsSoundEnabled())
			static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnsound"))->
												setProperty("NormalImage", "set:soundbutton image:disabled");
		else
			static_cast<CEGUI::PushButton *> (CEGUI::WindowManager::getSingleton().getWindow("btnsound"))->
												setProperty("NormalImage", "set:soundbutton image:normal");
	}
	catch(CEGUI::Exception &ex)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception refreshing the sound button: ") + ex.getMessage().c_str());
		_root = NULL;
	}
}

/***********************************************************
display game text
***********************************************************/
bool GameGUI::DisplayGameText(long textid, bool show)
{
	try
	{
		CEGUI::MultiLineEditbox * txs =
		static_cast<CEGUI::MultiLineEditbox *> (CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame/text"));

		if(show)
		{
			std::string str = DataLoader::getInstance()->GetText(textid);
			int idxs = 0;
			bool firsttime=true;
			while((idxs = str.find(" @ ")) != std::string::npos)
			{
				std::string tmp = str.substr(0, idxs);
				if(tmp == "")
					tmp = "\n";

				if(firsttime)
				{
					firsttime = false;
					txs->setText((const unsigned char *)tmp.c_str());
				}
				else
					txs->appendText((const unsigned char *)tmp.c_str());

				while(((idxs+4) < (int)str.size()) && (str[idxs+3] == '@') && (str[idxs+4] == ' '))
				{
					txs->appendText("\n");
					idxs+= 2;
				}

				str = str.substr(idxs+3);
			}

			if(firsttime)
			{
				firsttime = false;
				txs->setText((const unsigned char *)str.c_str());
			}
			else
				txs->appendText((const unsigned char *)str.c_str());


			if(!CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->isVisible())
			{
				CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->show();
				return true;
			}
			return false;
		}
		else
		{
			if(CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->isVisible())
			{
				CEGUI::WindowManager::getSingleton().getWindow("DisplayGameTextFrame")->hide();
				return true;
			}
			return false;
		}

	}
	catch(CEGUI::Exception &ex)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception showing text window: ") + ex.getMessage().c_str());
		_root = NULL;
	}

	return true;
}




/***********************************************************
set player name
***********************************************************/
void GameGUI::SetPlayerName(const std::string & name)
{
	try
	{
		CEGUI::WindowManager::getSingleton().getWindow("PlayerName")->setText(name);
	}
	catch(CEGUI::Exception &ex)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception SetPlayerName: ") + ex.getMessage().c_str());
		_root = NULL;
	}
}










/***********************************************************
set the list of teleport places
***********************************************************/
//void GameGUI::SetTeleportList(const std::map<std::string, TPInfo> &_lists)
//{
//	_telb.SetTeleportList(_lists);
//}
//
//

//
//
///***********************************************************
//focus the chatbox
//***********************************************************/
//void GameGUI::FocusChatbox(bool focus)
//{
//	_cb.Focus(focus);
//}
//
//
///***********************************************************
//set irc thread
//***********************************************************/
//void GameGUI::SetIrcThread(IrcThread * IT)
//{
//	_cb.SetIrc(IT);
//}
//
//
//
//
//
//
//
///***********************************************************
//display inventory
//***********************************************************/
//void GameGUI::ShowInventory()
//{
//	_invb.Show();
//}


/***********************************************************
show dialog with NPC
***********************************************************/
//void GameGUI::ShowDialog(long ActorId, const std::string &ActorName, DialogHandlerPtr Dialog,
//							bool Show,	const std::map<long, TraderItem> &inventory)
//{
//	_dialogb.ShowDialog(ActorId, ActorName, Dialog, Show, inventory);
//}



/***********************************************************
display the chatbox on screen
***********************************************************/
//void GameGUI::ShowMails(const std::vector<LbaNet::PMInfo> &pms)
//{
//	_mailbox.Show(pms);
//}
//
///***********************************************************
//display the chatbox on screen
//***********************************************************/
//void GameGUI::HideMails()
//{
//	_mailbox.HideMails();
//}


/***********************************************************
get friend list
***********************************************************/
//std::vector<std::string> GameGUI::GetFriendList()
//{
//	return _comb.GetFriendList();
//}
