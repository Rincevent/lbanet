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


#if !defined(__LbaNetModel_ClientExtendedEvents_h)
#define __LbaNetModel_ClientExtendedEvents_h

#include <ClientServerEvents.h>



enum LbanetKey
{
	LbanetKey_Forward = 0,
	LbanetKey_Backward,
	LbanetKey_Left,
	LbanetKey_Right,
	LbanetKey_StrafeL,
	LbanetKey_StrafeR,
	LbanetKey_Up,
	LbanetKey_Down,
	LbanetKey_Action,
	LbanetKey_NAction,
	LbanetKey_Weapon,
	LbanetKey_Stance1,
	LbanetKey_Stance2,
	LbanetKey_Stance3,
	LbanetKey_Stance4,
	LbanetKey_CenterCamera,
	LbanetKey_Shortcut0,
	LbanetKey_Shortcut1,
	LbanetKey_Shortcut2,
	LbanetKey_Shortcut3,
	LbanetKey_Shortcut4,
	LbanetKey_Shortcut5,
	LbanetKey_Shortcut6,
	LbanetKey_Shortcut7,
	LbanetKey_Shortcut8,
	LbanetKey_Shortcut9,
	LbanetKey_All
};

/*
************************************************************************************************************************
*                                                  class LoginEvent
*
*	class used during login
************************************************************************************************************************
*/
class LoginEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	LoginEvent(const std::string &Name, const std::string &Password, bool Local)
		: _Name(Name), _Password(Password), _Local(Local)
	{
	}

	std::string _Name;
	std::string _Password;
	bool _Local;
};

/*
************************************************************************************************************************
*                                                  class ResizeEvent
*
*	class used during login
************************************************************************************************************************
*/
class ResizeEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	ResizeEvent(int ResX, int ResY, bool Fullscreen)
		: _ResX(ResX), _ResY(ResY), _Fullscreen(Fullscreen)
	{
	}

	int _ResX;
	int _ResY;
	bool _Fullscreen;
};


/*
************************************************************************************************************************
*                                                  class GuiExitEvent
*
*	used when the active gui is being quitted by the user
************************************************************************************************************************
*/
class GuiExitEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	GuiExitEvent()
	{
	}
};


/*
************************************************************************************************************************
*                                                  class QuitGameEvent
*
*	used when player quit the game
************************************************************************************************************************
*/
class QuitGameEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	QuitGameEvent()
	{
	}
};




/*
************************************************************************************************************************
*                                                  class ChangeWorldEvent
*
*	used when the user is changing world
************************************************************************************************************************
*/
class ChangeWorldEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	ChangeWorldEvent(const std::string & NewWorldName, bool patch)
		: _NewWorldName(NewWorldName), _patch(patch)
	{
	}

	std::string _NewWorldName;
	bool _patch;
};


/*
************************************************************************************************************************
*                                                  class FocusChatEvent
*
*	used to focus chat box edit line
************************************************************************************************************************
*/
class FocusChatEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	FocusChatEvent(bool focus)
		: _focus(focus)
	{
	}

	bool _focus;
};



/*
************************************************************************************************************************
*                                                  class DisplayGUIEvent
*
*	used when the user is switching gui
************************************************************************************************************************
*/
class DisplayGUIEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	DisplayGUIEvent(int GuiNumber)
		: _GuiNumber(GuiNumber)
	{
	}

	int _GuiNumber;
};

/*
************************************************************************************************************************
*                                                  class NewFontSizeEvent
*
*	used when client change font size
************************************************************************************************************************
*/
class NewFontSizeEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	NewFontSizeEvent()
	{
	}
};




/*
************************************************************************************************************************
*                                                  class PlayerNameColorChangedEvent
*
*	used when client change color of player name display
************************************************************************************************************************
*/
class PlayerNameColorChangedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerNameColorChangedEvent(const std::string &color)
		: _color(color)
	{
	}

	std::string _color;
};


/*
************************************************************************************************************************
*                                                  class PlayerLocationChangedEvent
*
*	used when client change color of player name display
************************************************************************************************************************
*/
class PlayerLocationChangedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerLocationChangedEvent(const std::string &location)
		: _location(location)
	{
	}

	std::string _location;
};



/*
************************************************************************************************************************
*                                                  class SendChatTextEvent
*
*	used by chatbox to inform engine to send chat text
************************************************************************************************************************
*/
class SendChatTextEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	SendChatTextEvent(const std::string & text)
		: _text(text)
	{
	}

	std::string _text;
};



/*
************************************************************************************************************************
*                                                  class VideoFinishedEvent
*
*	used when player quit the game
************************************************************************************************************************
*/
class VideoFinishedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	VideoFinishedEvent()
	{
	}
};


/*
************************************************************************************************************************
*                                                  class PlayerStatusUpdateEvent
*
*	used when a player update its status
************************************************************************************************************************
*/
class PlayerStatusUpdateEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerStatusUpdateEvent(const std::string & name, const std::string & status, 
															const std::string & color,
															const std::string & location)
		: _name(name), _status(status), _color(color), _location(location)
	{
	}

	std::string _name;
	std::string _status;
	std::string _color;
	std::string _location;
};



/*
************************************************************************************************************************
*                                                  class PlayerKeyPressedEvent
*
*	used when a player press key
************************************************************************************************************************
*/
class PlayerKeyPressedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerKeyPressedEvent(LbanetKey keyid)
		: _keyid(keyid)
	{
	}

	LbanetKey _keyid;
};




/*
************************************************************************************************************************
*                                                  class PlayerKeyReleasedEvent
*
*	used when a player release key
************************************************************************************************************************
*/
class PlayerKeyReleasedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerKeyReleasedEvent(LbanetKey keyid)
		: _keyid(keyid)
	{
	}

	LbanetKey _keyid;
};


/*
************************************************************************************************************************
*                                                  class UpdateGuiLifeEvent
*
*	used when a player release key
************************************************************************************************************************
*/
class UpdateGuiLifeEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	UpdateGuiLifeEvent(float lifepercent, float manapercent)
		: _lifepercent(lifepercent), _manapercent(manapercent)
	{
	}

	float _lifepercent;
	float _manapercent;
};


/*
************************************************************************************************************************
*                                                  class CenterCameraEvent
*
*	used when camera should center on player
************************************************************************************************************************
*/
class CenterCameraEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	CenterCameraEvent()
	{
	}
};


/*
************************************************************************************************************************
*                                                  class EditorPlayerMovedEvent
*
*	tell editor that player moved
************************************************************************************************************************
*/
class EditorPlayerMovedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	EditorPlayerMovedEvent(float posx, float posy, float posz)
		: _posx(posx), _posy(posy), _posz(posz)
	{
	}

	float _posx;
	float _posy;
	float _posz;
};


/*
************************************************************************************************************************
*                                                  class EditorTeleportEvent
*
*	editor tp the player on the map
************************************************************************************************************************
*/
class EditorTeleportEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	EditorTeleportEvent(float posx, float posy, float posz)
		: _posx(posx), _posy(posy), _posz(posz)
	{
	}

	float _posx;
	float _posy;
	float _posz;
};


/*
************************************************************************************************************************
*                                                  class EditorCameraChangeEvent
*
*	editor tp the player on the map
************************************************************************************************************************
*/
class EditorCameraChangeEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	EditorCameraChangeEvent(bool forceghost)
		: _forceghost(forceghost)
	{
	}

	bool _forceghost;
};


/*
************************************************************************************************************************
*                                                  class ObjectPickedEvent
*
*	editor tp the player on the map
************************************************************************************************************************
*/
class ObjectPickedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	ObjectPickedEvent(const std::string & name, float px, float py, float pz)
		: _name(name), _px(px), _py(py), _pz(pz)
	{
	}

	std::string _name;
	float _px, _py, _pz;
};


/*
************************************************************************************************************************
*                                                  class PickedArrowMovedEvent
*
*	editor tp the player on the map
************************************************************************************************************************
*/
class PickedArrowMovedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PickedArrowMovedEvent(int pickedarrow)
		: _pickedarrow(pickedarrow)
	{
	}

	int		_pickedarrow;
};

/*
************************************************************************************************************************
*                                                  class RefreshLuaEvent
*
*	editor ask to Refresh Lua
************************************************************************************************************************
*/
class RefreshLuaEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	RefreshLuaEvent()
	{
	}
};



/*
************************************************************************************************************************
*                                                  class internalUpdateStateEvent
*
*	editor ask to Refresh Lua
************************************************************************************************************************
*/
class InternalUpdateStateEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	InternalUpdateStateEvent(LbaNet::ModelState	NewState)
		: _NewState(NewState)
	{
	}

	LbaNet::ModelState		_NewState;
};

/*
************************************************************************************************************************
*                                                  class GuiRefreshPlayerColorEvent
*
*	editor ask to Refresh Lua
************************************************************************************************************************
*/
class GuiRefreshPlayerColorEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	GuiRefreshPlayerColorEvent(int skincolor, int eyescolor, int haircolor)
		: _skincolor(skincolor), _eyescolor(eyescolor), _haircolor(haircolor)
	{
	}

	int _skincolor;
	int _eyescolor;
	int _haircolor;
};

/*
************************************************************************************************************************
*                                                  class RefreshPlayerPortrait
*
*	refresh player portrait
************************************************************************************************************************
*/
class RefreshPlayerPortraitEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	RefreshPlayerPortraitEvent()
	{
	}
};

/*
************************************************************************************************************************
*                                                  class PlayerLootItemEvent
*
************************************************************************************************************************
*/
class PlayerLootItemEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	PlayerLootItemEvent(long itemid)
		: _itemid(itemid)
	{
	}

long _itemid;
};


/*
************************************************************************************************************************
*                                                  class SwitchToGameEvent
*
************************************************************************************************************************
*/
class SwitchToGameEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	SwitchToGameEvent()
	{
	}
};

/*
************************************************************************************************************************
*                                                  class SwitchMusicEvent
*
************************************************************************************************************************
*/
class SwitchMusicEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	SwitchMusicEvent(const std::string & musicpath)
		: _musicpath(musicpath)
	{
	}

	std::string _musicpath;
};


/*
************************************************************************************************************************
*                                                  class ResetMusicEvent
*
************************************************************************************************************************
*/
class ResetMusicEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	ResetMusicEvent()
	{
	}
};



/*
************************************************************************************************************************
*                                                  class SwitchToFixedImageEvent
*
************************************************************************************************************************
*/
class SwitchToFixedImageEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	SwitchToFixedImageEvent(const std::string & imagepath, long NbSecondDisplay, 
								bool FadeIn, float FadeInColorR, float FadeInColorG, float FadeInColorB,
								bool FadeOut, float FadeOutColorR, float FadeOutColorG, float FadeOutColorB)
		: _imagepath(imagepath), _NbSecondDisplay(NbSecondDisplay),
			_FadeIn(FadeIn), _FadeInColorR(FadeInColorR), _FadeInColorG(FadeInColorG), _FadeInColorB(FadeInColorB),
			_FadeOut(FadeOut), _FadeOutColorR(FadeOutColorR), _FadeOutColorG(FadeOutColorG), _FadeOutColorB(FadeOutColorB)
	{
	}

	std::string		_imagepath;
	long			_NbSecondDisplay; 
	bool			_FadeIn; 
	float			_FadeInColorR; 
	float			_FadeInColorG; 
	float			_FadeInColorB; 
	bool			_FadeOut; 
	float			_FadeOutColorR; 
	float			_FadeOutColorG; 
	float			_FadeOutColorB;
};


/*
************************************************************************************************************************
*                                                  class SwitchBigTextEvent
*
************************************************************************************************************************
*/
class SwitchBigTextEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	SwitchBigTextEvent(const std::string & imagepath, const std::vector<long> textIds)
		: _imagepath(imagepath), _textIds(textIds)
	{
	}

	std::string			_imagepath;
	std::vector<long>	_textIds;
};



/*
************************************************************************************************************************
*                                                  class DisplayExtraGLFinishedEvent
*
************************************************************************************************************************
*/
class DisplayExtraGLFinishedEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	DisplayExtraGLFinishedEvent()
	{
	}
};



/*
************************************************************************************************************************
*                                                  class DisplayExtraGLFinishedEvent
*
************************************************************************************************************************
*/
class ShowHideVoiceSpriteEvent : public LbaNet::ClientServerEventBase
{
public:
	//! constructor
	ShowHideVoiceSpriteEvent(long ActorId, 
								bool Show, bool Left)
				: _ActorId(ActorId), _Show(Show), _Left(Left)
	{
	}

	long			_ActorId; 
	bool			_Show; 
	bool			_Left; 
};



#endif