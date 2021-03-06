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


#include "LocalDatabaseHandler.h"
#include "StringHelperFuncs.h"

#include <IceUtil/Time.h>

#include <sqlite3.h>

#include <exception>



/***********************************************************
constructor
***********************************************************/
LocalDatabaseHandler::LocalDatabaseHandler(const std::string & filename)
: _db(NULL)
{
	int res = sqlite3_open(filename.c_str(), &_db);
	if( res )
	{
		sqlite3_close(_db);
		_db = NULL;
		throw(std::exception((std::string("Can't open database file ") + filename + std::string(" : ") + sqlite3_errmsg(_db)).c_str()));
	}
}


/***********************************************************
player has changed world
***********************************************************/
LbaNet::SavedWorldInfo LocalDatabaseHandler::ChangeWorld(const std::string& NewWorldName, long PlayerId, 
															int defaultinventorysize)
{
	long worldid = -1;
	LbaNet::SavedWorldInfo resP;

	Lock sync(*this);
	if(!_db)
		return resP;

	// build sql statement
	std::stringstream query;
	query << "SELECT uw.id, uw.lastmap, uw.lastposx, uw.lastposy, uw.lastposz, uw.lastrotation, uw.InventorySize, uw.Shortcuts, uw.LifePoint, uw.ManaPoint, uw.MaxLife, uw.MaxMana, w.id, uw.ModelName, uw.ModelOutfit, uw.ModelWeapon, uw.ModelMode, uw.RendererType, uw.EquipedWeapon, uw.EquipedOutfit, uw.EquipedMount, uw.SkinColor, uw.EyesColor, uw.HairColor";
	query << " FROM lba_usertoworld uw, lba_worlds w";
	query << " WHERE uw.userid = '"<<PlayerId<<"'";
	query << " AND w.name = '"<<NewWorldName<<"'";
	query << " AND uw.worldid = w.id";


	// execute statement
	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - ChangeWorld for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		// check if world already exist in database
		if(nbrow > 0)
		{
			// unpack the result
			long uworldid = atol(pazResult[nbcollumn]);

			// player pos part
			resP.ppos.MapName = pazResult[nbcollumn+1];
			resP.ppos.X = (float)atof(pazResult[nbcollumn+2]);
			resP.ppos.Y = (float)atof(pazResult[nbcollumn+3]);
			resP.ppos.Z = (float)atof(pazResult[nbcollumn+4]);
			resP.ppos.Rotation = (float)atof(pazResult[nbcollumn+5]);

			// player inventory part
			resP.inventory.InventorySize = atoi(pazResult[nbcollumn+6]);

			std::vector<std::string> tokens;
			std::string shortcutstr = pazResult[nbcollumn+7];
			StringHelper::Tokenize(shortcutstr, tokens, "#");
			for(size_t i=0; i<tokens.size(); ++i)
			{
				LbaNet::ItemInfo itm;
				itm.Id = atol(tokens[i].c_str());
				resP.inventory.UsedShorcuts.push_back(itm);
			}

			resP.lifemana.CurrentLife = (float)atof(pazResult[nbcollumn+8]);
			resP.lifemana.CurrentMana = (float)atof(pazResult[nbcollumn+9]);
			resP.lifemana.MaxLife = (float)atof(pazResult[nbcollumn+10]);
			resP.lifemana.MaxMana = (float)atof(pazResult[nbcollumn+11]);

			resP.model.ModelName = pazResult[nbcollumn+13];
			resP.model.Outfit = pazResult[nbcollumn+14];
			resP.model.Weapon = pazResult[nbcollumn+15];
			resP.model.Mode = pazResult[nbcollumn+16];
			resP.model.State = LbaNet::StNormal;

			int renderT = atoi(pazResult[nbcollumn+17]);
			switch(renderT)
			{
				case 0: // -> osg model
					resP.model.TypeRenderer = LbaNet::RenderOsgModel;
				break;

				case 1: // 1 -> sprite
					resP.model.TypeRenderer = LbaNet::RenderSprite;
				break;

				case 2: // -> LBA1 model
					resP.model.TypeRenderer = LbaNet::RenderLba1M;
				break;

				case 3: // -> LBA2 model
					resP.model.TypeRenderer = LbaNet::RenderLba2M;
				break;
			}

			resP.EquipedWeapon = atol(pazResult[nbcollumn+18]);
			resP.EquipedOutfit = atol(pazResult[nbcollumn+19]);
			resP.EquipedMount = atol(pazResult[nbcollumn+20]);

			resP.model.SkinColor = atoi(pazResult[nbcollumn+21]);
			resP.model.EyesColor = atoi(pazResult[nbcollumn+22]);
			resP.model.HairColor = atoi(pazResult[nbcollumn+23]);


			worldid = atol(pazResult[nbcollumn+12]);


			// free the results
			sqlite3_free_table(pazResult);



			//set the user as connected
			query.str("");
			query << "UPDATE lba_usertoworld SET lastvisited = datetime('now') WHERE id = '"<<uworldid<<"'";
			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update usertoworldmap.lastvisited failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}


			query.str("");
			query << "SELECT * FROM lba_inventory";
			query << " WHERE worldid = '"<<uworldid<<"'";
			dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": WorldServer - get inventory for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
			else
			{
				// get data
				for(int i=0; i<nbrow; ++i)
				{
					LbaNet::ItemPosInfo itm;
					itm.Count = atoi(pazResult[(nbcollumn*(i+1))+2]);
					itm.Position = atoi(pazResult[(nbcollumn*(i+1))+3]);
					resP.inventory.InventoryStructure[atol(pazResult[(nbcollumn*(i+1))+1])] = itm;
				}

				// free the results
				sqlite3_free_table(pazResult);
			}
		}
		else	// if world does not exist
		{
			query.str("");
			query <<"SELECT id FROM lba_worlds WHERE name = '"<<NewWorldName<<"'";
			dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": WorldServer - get world id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
			else
			{
				// world does not exist yet in saved file
				if(nbrow == 0)
				{
					//add new world name to DB
					{
						query.str("");
						query << "INSERT INTO lba_worlds (name, description) VALUES('";
						query << NewWorldName << "', '')";
						dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
						if(dbres != SQLITE_OK)
						{
							// record error
							std::cerr<<IceUtil::Time::now()<<": LBA_Server - INSERT INTO lba_worlds failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
							sqlite3_free(zErrMsg);
						}
					}

					// redo world query
					{
						query.str("");
						query <<"SELECT id FROM lba_worlds WHERE name = '"<<NewWorldName<<"'";
						dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

						if(dbres != SQLITE_OK)
						{
							// record error
							std::cerr<<IceUtil::Time::now()<<": WorldServer - get world id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
							sqlite3_free(zErrMsg);
						}
					}
				}


				if(nbrow > 0)
				{
					// get worldid
					worldid = atol(pazResult[nbcollumn]);

					// free the results
					sqlite3_free_table(pazResult);


					// insert new data row
					query.str("");
					query << "INSERT INTO lba_usertoworld (userid, worldid, lastvisited, ModelName, ModelOutfit, ModelWeapon, ModelMode, lastmap) VALUES('";
					query << PlayerId << "', '"<<worldid<<"', datetime('now'), '', '', '', '', '')";
					dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
					if(dbres != SQLITE_OK)
					{
						// record error
						std::cerr<<IceUtil::Time::now()<<": LBA_Server - INSERT lba_usertoworld failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
						sqlite3_free(zErrMsg);
					}
				}
			}

			resP.inventory.InventorySize = defaultinventorysize;
			for(int i=0; i<10; ++i)
			{
				LbaNet::ItemInfo itm;
				itm.Id = -1;
				resP.inventory.UsedShorcuts.push_back(itm);
			}
		}


		//insert world name into user
		{
			query.str("");
			query << "UPDATE lba_users SET currentworldid = '"<<worldid<<"' WHERE id = '"<<PlayerId<<"'";
			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - UPDATE users failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
		}

	}


	return resP;
}


/***********************************************************
player update his current position in the world
***********************************************************/
void LocalDatabaseHandler::UpdatePositionInWorld(const LbaNet::PlayerPosition& Position,
											const std::string& WorldName, long PlayerId)
{
	Lock sync(*this);
	if(!_db)
		return;


	std::stringstream query;
	query << "UPDATE lba_usertoworld SET lastmap = '"<<Position.MapName<<"',";
	query << "lastposx = '"<<Position.X<<"',";
	query << "lastposy = '"<<Position.Y<<"',";
	query << "lastposz = '"<<Position.Z<<"',";
	query << "lastrotation = '"<<Position.Rotation<<"'";
	query << " WHERE userid = '"<<PlayerId<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

	char *zErrMsg = 0;
	int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update UpdatePositionInWorld failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
}



/***********************************************************
update player life information
***********************************************************/
void LocalDatabaseHandler::UpdateLife(const LbaNet::LifeManaInfo & lifeinfo, 
								const std::string& WorldName,long PlayerId)
{
	Lock sync(*this);
	if(!_db)
		return;

	// quit previous world
	if(WorldName != "")
	{
		std::stringstream query;
		query << "UPDATE lba_usertoworld SET LifePoint = '"<<lifeinfo.CurrentLife<<"'";
		query << ", ManaPoint = '"<<lifeinfo.CurrentMana<<"'";
		query << ", MaxLife = '"<<lifeinfo.MaxLife<<"'";
		query << ", MaxMana = '"<<lifeinfo.MaxMana<<"'";
		query << " WHERE userid = '"<<PlayerId<<"'";
		query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

		char *zErrMsg = 0;
		int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
		if(dbres != SQLITE_OK)
		{
			// record error
			std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update life failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
			sqlite3_free(zErrMsg);
		}
	}
}




/***********************************************************
update player life information
***********************************************************/
void LocalDatabaseHandler::UpdateModel(const LbaNet::ModelInfo & modelinfo, 
							const std::string& WorldName,long PlayerId,
								long equipedweapon, long equipedoutfit,
								long equipedmount)
{
	Lock sync(*this);
	if(!_db)
		return;

	// quit previous world
	if(WorldName != "")
	{
		std::stringstream query;
		query << "UPDATE lba_usertoworld SET ModelName = '"<<modelinfo.ModelName<<"'";
		query << ", ModelOutfit = '"<<modelinfo.Outfit<<"'";
		query << ", ModelWeapon = '"<<modelinfo.Weapon<<"'";
		query << ", ModelMode = '"<<modelinfo.Mode<<"'";
		query << ", EquipedWeapon = '"<<equipedweapon<<"'";
		query << ", EquipedOutfit = '"<<equipedoutfit<<"'";
		query << ", EquipedMount = '"<<equipedmount<<"'";
		query << ", SkinColor = '"<<modelinfo.SkinColor<<"'";
		query << ", EyesColor = '"<<modelinfo.EyesColor<<"'";
		query << ", HairColor = '"<<modelinfo.HairColor<<"'";

		int rtype = 0;
		switch(modelinfo.TypeRenderer)
		{
			case LbaNet::RenderOsgModel:
				rtype = 0;
			break;

			case LbaNet::RenderSprite:
				rtype = 1;
			break;

			case LbaNet::RenderLba1M:
				rtype = 2;
			break;

			case LbaNet::RenderLba2M:
				rtype = 3;
			break;
		}

		query << ", RendererType = '"<<rtype<<"'";
		query << " WHERE userid = '"<<PlayerId<<"'";
		query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

		char *zErrMsg = 0;
		int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
		if(dbres != SQLITE_OK)
		{
			// record error
			std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update model failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
			sqlite3_free(zErrMsg);
		}
	}
}




/***********************************************************
quit current world
***********************************************************/
void LocalDatabaseHandler::QuitWorld(const std::string& LastWorldName,long PlayerId)
{
	Lock sync(*this);
	if(!_db)
		return;

	// quit previous world
	if(LastWorldName != "")
	{
		std::stringstream query;
		query << "UPDATE lba_usertoworld SET timeplayedmin = timeplayedmin + ((strftime('%s','now') - strftime('%s', lastvisited))/60), timeplayedsession = timeplayedsession + ((strftime('%s','now') - strftime('%s', lastvisited))/60)";
		query << " WHERE userid = '"<<PlayerId<<"'";
		query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<LastWorldName<<"')";

		char *zErrMsg = 0;
		int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
		if(dbres != SQLITE_OK)
		{
			// record error
			std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update usertoworldmap.timeplayedmin failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
			sqlite3_free(zErrMsg);
		}
	}
}



/***********************************************************
update player inventory structure
***********************************************************/
void LocalDatabaseHandler::UpdateInventory(const LbaNet::InventoryInfo &Inventory, const std::string& WorldName,
									  long PlayerId)
{
	if(WorldName == "")
		return;

	Lock sync(*this);
	if(!_db)
		return;



	std::stringstream shortcutstring;
	LbaNet::ShortcutsSeq::const_iterator it = Inventory.UsedShorcuts.begin();
	LbaNet::ShortcutsSeq::const_iterator end = Inventory.UsedShorcuts.end();
	if(it != end)
	{
		shortcutstring<<it->Id;
		++it;
	}
	for(;it != end; ++it)
		shortcutstring<<"#"<<it->Id;


	std::stringstream query;
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<PlayerId<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";;

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - pb UpdateInventory for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			long uworldid = atol(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);

			query.str("");
			query << "UPDATE lba_usertoworld SET InventorySize = '"<<Inventory.InventorySize<<"',";
			query << "Shortcuts = '"<<shortcutstring.str()<<"' ";
			query << " WHERE id = '"<<uworldid<<"'";

			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update usertoworldmap_inv failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}



			query.str("");
			query << "DELETE FROM lba_inventory";
			query << " WHERE worldid = '"<<uworldid<<"'";

			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update usertoworldmap_inv delete failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}


			LbaNet::ItemsMap::const_iterator iti = Inventory.InventoryStructure.begin();
			LbaNet::ItemsMap::const_iterator endi = Inventory.InventoryStructure.end();
			for(;iti != endi; ++iti)
			{
				query.str("");
				query << "INSERT INTO lba_inventory (worldid, objectid, number, InventoryPlace) VALUES('";
				query << uworldid << "', '" << iti->first << "', '" << iti->second.Count << "', '" << iti->second.Position << "')";
				
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - Update usertoworldmap_inv INSERT failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}
		}
	}

}

/***********************************************************
add friend function
***********************************************************/
bool LocalDatabaseHandler::AskFriend(long myId, const std::string &friendname, long & friendid,
										std::string &myname)
{
	// no friend system in local mode
	return false;
}

/***********************************************************
accept friend invitation
***********************************************************/
bool LocalDatabaseHandler::AcceptFriend(long myId, long friendid, std::string &friendname,
											std::string &myname)
{
	// no friend system in local mode
	return false;
}

/***********************************************************
remove friend function
***********************************************************/
void LocalDatabaseHandler::RemoveFriend(long myId, long friendid)
{
	// no friend system in local mode
}

/***********************************************************
get friends function
***********************************************************/
LbaNet::FriendsSeq LocalDatabaseHandler::GetFriends(long myId)
{
	// no friend system in local mode
	LbaNet::FriendsSeq resF;
	return resF;
}



/***********************************************************
store letter to the server and return the letter id
***********************************************************/
long LocalDatabaseHandler::AddLetter(long myId, const std::string& title, const std::string& message)
{
	// no letter system in local mode
	return -1;
}


/***********************************************************
return letter info
***********************************************************/
LbaNet::LetterInfo LocalDatabaseHandler::GetLetterInfo(Ice::Long LetterId)
{
	// no letter system in local mode
	LbaNet::LetterInfo resF;
	resF.LetterId = -1;
	return resF;
}



/***********************************************************
get quest information
***********************************************************/
void LocalDatabaseHandler::GetQuestInfo(const std::string& WorldName, long PlayerId,
					std::vector<long> &questStarted, std::vector<long> &questFinished)
{
	if(WorldName == "")
		return;

	Lock sync(*this);
	if(!_db)
		return;

	std::stringstream query;
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<PlayerId<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - Quest SELECT failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			long uworldid = atol(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);


			// do second query
			query.str("");
			query << "SELECT questid, status FROM lba_quests";
			query << " WHERE worldid = '"<<uworldid<<"'";
			dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": WorldServer - Quest SELECT failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
			else
			{
				for(int i=0; i<nbrow; ++i)
				{
					long qid = atol(pazResult[(nbcollumn*(i+1))]);
					int status = atoi(pazResult[(nbcollumn*(i+1))+1]);

					if(status == 0)
						questStarted.push_back(qid);

					if(status == 1)
						questFinished.push_back(qid);
				}

				// free the results
				sqlite3_free_table(pazResult);
			}

		}
	}
}


/***********************************************************
set quest information
***********************************************************/
void LocalDatabaseHandler::SetQuestInfo(const std::string& WorldName, long PlayerId,
					const std::vector<long> &questStarted, const std::vector<long> &questFinished)
{
	if(WorldName == "")
		return;

	Lock sync(*this);
	if(!_db)
		return;

	std::stringstream query;
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<PlayerId<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - Quest update failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			long uworldid = atol(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);



			query.str("");
			query << "DELETE FROM lba_quests";
			query << " WHERE worldid = '"<<uworldid<<"'";
				
			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - Quest DELETE  failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}


			std::vector<long>::const_iterator iti = questStarted.begin();
			std::vector<long>::const_iterator endi = questStarted.end();
			for(;iti != endi; ++iti)
			{
				query.str("");
				query << "INSERT INTO lba_quests (worldid, questid, status) VALUES('";
				query << uworldid << "', '" << *iti << "', '" << 0  << "')";
		
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - Quest INSERT  failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}

			iti = questFinished.begin();
			endi = questFinished.end();
			for(;iti != endi; ++iti)
			{
				query.str("");
				query << "INSERT INTO lba_quests (worldid, questid, status) VALUES('";
				query << uworldid << "', '" << *iti << "', '" << 1  << "')";
		
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - Quest INSERT  failed for user id "<<PlayerId<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}
		}
	}
}


/***********************************************************
record player kill
***********************************************************/
void LocalDatabaseHandler::RecordKill(const std::string& WorldName, long KilledId, int Reason, long KillerId)
{
	Lock sync(*this);
	if(!_db)
		return;


	if(WorldName != "")
	{
		std::stringstream query;
		query << "UPDATE lba_usertoworld ";
		switch(Reason)
		{
			case 2:
				query << "SET FallDeath = FallDeath + 1";
			break;
			case 3:
				query << "SET MonsterDeath = MonsterDeath + 1";
			break;
			case 4:
				query << "SET PvpDeath = PvpDeath + 1";
			break;
			case 6:
				query << "SET DrowningDeath = DrowningDeath + 1";
			break;
			default:
				query << "SET OtherDeath = OtherDeath + 1";
			break;
		}
		query << " WHERE userid = '"<<KilledId<<"'";
		query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";


		char *zErrMsg = 0;
		int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
		if(dbres != SQLITE_OK)
		{
			// record error
			std::cerr<<IceUtil::Time::now()<<": LBA_Server - RecordKill failed for user id "<<KilledId<<" : "<<zErrMsg<<std::endl;
			sqlite3_free(zErrMsg);
		}
	}

	if(WorldName != "")
	{
		if(Reason == 4)
		{
			std::stringstream query;
			query << "UPDATE lba_usertoworld ";
			query << "SET PvpKill = PvpKill + 1";
			query << " WHERE userid = '"<<KillerId<<"'";
			query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";


			char *zErrMsg = 0;
			int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - RecordKiller failed for user id "<<KillerId<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
		}
	}
}

/***********************************************************
record npc kill by player
***********************************************************/
void LocalDatabaseHandler::RecordNPCKill(const std::string& WorldName, long KilledId, long PlayerKillerId)
{
	Lock sync(*this);
	if(!_db)
		return;

	if(KilledId < 0 || PlayerKillerId < 0)
		return;

	if(WorldName != "")
	{
		std::stringstream query;
		query << "UPDATE lba_usertoworld ";
		query << "SET MonsterKill = MonsterKill + 1";
		query << " WHERE userid = '"<<PlayerKillerId<<"'";
		query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";


		char *zErrMsg = 0;
		int dbres = dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
		if(dbres != SQLITE_OK)
		{
			// record error
			std::cerr<<IceUtil::Time::now()<<": LBA_Server - RecordKiller failed for user id "<<PlayerKillerId<<" : "<<zErrMsg<<std::endl;
			sqlite3_free(zErrMsg);
		}
	}
}



/***********************************************************
send a pm to someone
***********************************************************/
void LocalDatabaseHandler::SendPM(const LbaNet::PMInfo &pm)
{
	// no pm system in local mode
}

/***********************************************************
delete a pm
***********************************************************/
void LocalDatabaseHandler::DeletePM(Ice::Long pmid)
{
	// no pm system in local mode
}

/***********************************************************
MarkReadPM
***********************************************************/
void LocalDatabaseHandler::MarkReadPM(Ice::Long pmid)
{
	// no pm system in local mode
}


/***********************************************************
get all pm in your mailbox
***********************************************************/
LbaNet::PMsSeq LocalDatabaseHandler::GetInboxPM(long playerid)
{
	// no pm system in local mode
	LbaNet::PMsSeq pmsres;
	return pmsres;
}


/***********************************************************
set DB flag
***********************************************************/
void LocalDatabaseHandler::SetDBFlag(const std::string& WorldName, long playerid, 
									 const std::string & flagid, int value)
{
	if(WorldName == "")
		return;

	Lock sync(*this);
	if(!_db)
		return;

	std::stringstream query;
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<playerid<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - GetDBFlag failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			int wid = atoi(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);

			query.str("");
			query << "INSERT OR IGNORE INTO lba_playerflag(wid, flagid)";
			query << " VALUES('"<<wid<<"', '"<<flagid<<"');";
			query << " UPDATE lba_playerflag SET value = '"<<value<<"'";
			query << " WHERE wid = '"<<wid<<"' AND flagid = '"<<flagid<<"';";
		
			dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
			if(dbres != SQLITE_OK)
			{
				// record error
				std::cerr<<IceUtil::Time::now()<<": LBA_Server - SetDBFlag  failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
				sqlite3_free(zErrMsg);
			}
		}
	}
}

/***********************************************************
get DB flag
***********************************************************/
int LocalDatabaseHandler::GetDBFlag(const std::string& WorldName, long playerid, 
									const std::string & flagid)
{
	if(WorldName == "")
		return -1;

	Lock sync(*this);
	if(!_db)
		return -1;

	std::stringstream query;
	query << "SELECT value FROM lba_playerflag";
	query << " WHERE flagid = '"<<flagid<<"' AND wid = (";
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<playerid<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"'))";

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - GetDBFlag failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			int res = atoi(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);

			return res;
		}
	}

	return -1;
}


/***********************************************************
reset world
***********************************************************/
void LocalDatabaseHandler::ResetWorld(const std::string& WorldName, long playerid)
{
	if(WorldName == "")
		return;

	Lock sync(*this);
	if(!_db)
		return;


	std::stringstream query;
	query << "SELECT id FROM lba_usertoworld";
	query << " WHERE userid = '"<<playerid<<"'";
	query << " AND worldid = (SELECT id FROM lba_worlds WHERE name = '"<<WorldName<<"')";

	char *zErrMsg = 0;
	char **pazResult = 0;
	int nbrow, nbcollumn;
	int dbres = sqlite3_get_table(_db, query.str().c_str(), &pazResult, &nbrow, &nbcollumn, &zErrMsg);

	if(dbres != SQLITE_OK)
	{
		// record error
		std::cerr<<IceUtil::Time::now()<<": WorldServer - reset world failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
		sqlite3_free(zErrMsg);
	}
	else
	{
		if(nbrow > 0)
		{
			// unpack the result
			int wid = atoi(pazResult[nbcollumn]);

			// free the results
			sqlite3_free_table(pazResult);


			{
				query.str("");
				query << "UPDATE lba_usertoworld";
				query << " SET  timeplayedsession = 0 , lastmap = \"\"";
				query << " WHERE id = '"<<wid<<"'";
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - reset world  failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}

			{
				query.str("");
				query << "DELETE FROM lba_inventory where worldid = '"<<wid<<"'";
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - reset world  failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}

			{
				query.str("");
				query << "DELETE FROM lba_quests where worldid = '"<<wid<<"'";
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - reset world  failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}

			{
				query.str("");
				query << "DELETE FROM lba_playerflag where wid = '"<<wid<<"'";
				dbres = sqlite3_exec(_db, query.str().c_str(), 0, 0, &zErrMsg);
				if(dbres != SQLITE_OK)
				{
					// record error
					std::cerr<<IceUtil::Time::now()<<": LBA_Server - reset world  failed for user id "<<playerid<<" : "<<zErrMsg<<std::endl;
					sqlite3_free(zErrMsg);
				}
			}
		}
	}
}


/***********************************************************
check login
return -1 if login incorrect - else return the user id
***********************************************************/
long LocalDatabaseHandler::CheckLogin(const std::string & PlayerName, const std::string & Password)
{
	return 1;
}

/***********************************************************
set the user as disconnected in the database
***********************************************************/
void LocalDatabaseHandler::DisconnectUser(long Id)
{
}

/***********************************************************
check if player is an admin for the given world
***********************************************************/
bool LocalDatabaseHandler::IsWorldAdmin(long PlayerId, const std::string& Worldname)
{
	return false;
}
