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


#if !defined(__Lbanet_localizer_h)
#define __Lbanet_localizer_h

#include <string>
#include <map>
#include <vector>

/***********************************************************************
 * Module:  Localizer.h
 * Purpose: Class used to localize game text to other languages
 ***********************************************************************/
class Localizer
{
public:

	enum LocalizeType { GUI, Map, Quest, Inventory, Name };

	//! destructor
	~Localizer();

	// singleton pattern
	static Localizer * getInstance();

	//! set current world name
	void SetWorldName(std::string WorldName);


	//! get the text given a text id
	std::string GetText(LocalizeType type, long TextId, std::string lang="");


	//! set game language
	void SetLanguage(std::string lang);

	//! get language
	std::string GetLanguage()
	{return _lang;}

	//! get languages available
	std::vector<std::string> GetLanguages();


	//! get the voice given a text id
	std::vector<std::string> GetVoices(LocalizeType type, long TextId, std::string lang="");


	//! get voices dir path
	std::string GetVoiceDirPath(const std::string &lang);

	//!***************************
	//! editor functions
	std::map<long, std::string> GetMap(LocalizeType texttype, const std::string &lang);

	//! return new index in case of insertion
	long AddToMap(LocalizeType texttype, long id, const std::string &text, const std::string &lang);

	void RemoveFromMap(LocalizeType texttype, long id, const std::string &lang);


	//! return new index in case of insertion
	long AddToMapVoice(LocalizeType texttype, long id, const std::vector<std::string> &voices, const std::string &lang);

	void RemoveFromMapVoice(LocalizeType texttype, long id, const std::string &lang);

	void SaveTexts();
	//!***************************


protected:
	//! constructor
   Localizer();

	//! refresh text files
	void RefreshTexts();

	//! refresh text files
	void RefreshGuiTexts();

	//!***************************
	//! editor functions
	//! check map loaded, if not load it
	void CheckMapPresent(const std::string & lang);
	void CheckVoiceMapPresent(const std::string & lang);
	//!***************************

private:
	static Localizer *		_singletonInstance;

	// current world loaded
	std::string				_currentworldname;

	// current language used
	std::string				_lang;

	// contain preloaded text
	std::map<long, std::string>		_gui_texts;
	std::map<long, std::string>		_map_texts;
	std::map<long, std::string>		_inventory_texts;
	std::map<long, std::string>		_quest_texts;
	std::map<long, std::string>		_name_texts;

	// contains preloaded voices
	std::map<long, std::vector<std::string> >		_map_voices;
	std::map<long, std::vector<std::string> >		_inventory_voices;


	//!***************************
	//! editor stuff

	// contain preloaded text
	std::map<std::string, std::map<long, std::string> >		_map_texts_ed;
	std::map<std::string, std::map<long, std::string> >		_inventory_texts_ed;
	std::map<std::string, std::map<long, std::string> >		_quest_texts_ed;
	std::map<std::string, std::map<long, std::string> >		_name_texts_ed;

	// contains preloaded voices
	std::map<std::string, std::map<long, std::vector<std::string> > >		_map_voices_ed;
	std::map<std::string, std::map<long, std::vector<std::string> > >		_inventory_voices_ed;
	//!***************************

};

#endif