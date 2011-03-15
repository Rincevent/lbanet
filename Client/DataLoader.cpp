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

#include "DataLoader.h"
#include "LogHandler.h"
#include "XmlReader.h"
#include "Localizer.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
namespace fs = boost::filesystem;


DataLoader* DataLoader::_singletonInstance = NULL;



/***********************************************************
	Constructor
***********************************************************/
DataLoader::DataLoader()
{
	LogHandler::getInstance()->LogToFile("Initializing data handler class...");
}



/***********************************************************
	Destructor
***********************************************************/
DataLoader::~DataLoader()
{

}


/***********************************************************
singleton pattern
***********************************************************/
DataLoader * DataLoader::getInstance()
{
	if(!_singletonInstance)
		_singletonInstance = new DataLoader();

	return _singletonInstance;
}






/***********************************************************
load world information into memory
***********************************************************/
void DataLoader::SetWorldName(std::string WorldName)
{
	if(_currentworldname != WorldName)
	{
		_currentworldname = WorldName;

		Localizer::getInstance()->SetWorldName(WorldName);
	}
}



/***********************************************************
get the list of available worlds
***********************************************************/
void DataLoader::GetAvailableWorlds(std::vector<LbaNet::WorldDesc> & list)
{
	// get all xml file of the directory
	std::vector<std::string> files;
	{
		fs::path full_path( fs::system_complete( "./Data/Worlds" ) );

		if ( !fs::exists( full_path ) )
		{
			LogHandler::getInstance()->LogToFile(std::string("\nData/Worlds directory Not found: ") + full_path.file_string());
			return;
		}

		if ( fs::is_directory( full_path ) )
		{
			fs::directory_iterator end_iter;
			for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter;	++dir_itr )
			{
				if ( fs::is_directory( dir_itr->status() ) )
				{
					files.push_back(dir_itr->path().string() + "/WorldDescription.xml");
				}
			}
		}
	}

	// for each file add an entry
	for(size_t i=0; i< files.size(); ++i)
	{
		WorldDesc desc;
		if(XmlReader::LoadWorldDesc(files[i], desc))
			list.push_back(desc);
	}
}

/***********************************************************
get information about a specific world
***********************************************************/
void DataLoader::GetWorldInformation(const std::string &Filename, LbaNet::WorldInformation &res)
{
	XmlReader::LoadWorldInfo( "./Data/Worlds/" + Filename + "/WorldDescription.xml", res);
}


/***********************************************************
saved information about a specific world
***********************************************************/

void DataLoader::SaveWorldInformation(const std::string &Filename, const LbaNet::WorldInformation &res)
{
	XmlReader::SaveWorldInfo( "./Data/Worlds/" + Filename + "/WorldDescription.xml", res);
}

