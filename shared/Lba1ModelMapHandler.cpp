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

#include "Lba1ModelMapHandler.h"
#include "StringHelperFuncs.h"

#include <fstream>

Lba1ModelMapHandler* Lba1ModelMapHandler::_singletonInstance = NULL;

//const short	LbaNetModel::m_body_color_map[] = {-1, 2, 19, 32, 36, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240, 243};

/***********************************************************
singleton pattern
***********************************************************/
Lba1ModelMapHandler * Lba1ModelMapHandler::getInstance()
{
    if(!_singletonInstance)
    {
        _singletonInstance = new Lba1ModelMapHandler();
		return _singletonInstance;
    }
    else
    {
		return _singletonInstance;
    }
}


/***********************************************************
Constructor
***********************************************************/
Lba1ModelMapHandler::Lba1ModelMapHandler()
{
	// read the file and get data
	{
		std::ifstream file((Lba1ModelDataPath()+"lba1_model_animation.csv").c_str());
		if(file.is_open())
		{
			// read first information line
			std::string line;
			std::vector<std::string> infos;

			std::getline(file, line);
			StringHelper::Tokenize(line, infos, ",");

			while(!file.eof())
			{
				std::getline(file, line);
				std::vector<std::string> tokens;
				StringHelper::Tokenize(line, tokens, ",");

				if(tokens.size() > 12)
				{
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].modelnumber = atoi(tokens[4].c_str());
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].bodynumber = atoi(tokens[5].c_str());
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].WeaponType = atoi(tokens[6].c_str());
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].Size.X = (float)atof(tokens[7].c_str());
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].Size.Y = (float)atof(tokens[8].c_str());
					_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].Size.Z = (float)atof(tokens[9].c_str());

					for(size_t i=10; i<tokens.size(); ++i)
					{
						std::string anims = tokens[i];
						std::string animstring = infos[i];
						std::vector<std::string> tokens2;
						StringHelper::Tokenize(anims, tokens2, ";");
						std::vector<int> animvec;
						for(size_t j=0; j<tokens2.size(); ++j)
							animvec.push_back(atoi(tokens2[j].c_str()));

						_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].animations[animstring] = animvec;
					}
				}


			}
		}
	}

	// read color file
	{
		std::ifstream file((Lba1ModelDataPath()+"lba1_model_color.csv").c_str());
		if(file.is_open())
		{
			while(!file.eof())
			{
				std::string line;
				std::getline(file, line);
				std::vector<std::string> tokens;
				StringHelper::Tokenize(line, tokens, ",");
				if(tokens.size() >= 5)
				{
					ModeData &mdata = _data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]];

					for(size_t i=4; i<tokens.size(); ++i)
					{
						std::string color = tokens[i];
						if(color.size() > 0)
						{
							std::string colostr = color.substr(1);
							int coloridx = atoi(colostr.c_str());
							switch(color[0])
							{
								case 'P':
									mdata.polycolors.push_back(coloridx);
								break;
								case 'L':
									mdata.linecolors.push_back(coloridx);
								break;
								case 'S':
									mdata.spherecolors.push_back(coloridx);
								break;
							}
						}
					}
				}
			}
		}
	}

	// read color alternatives file
	{
		std::ifstream file((Lba1ModelDataPath()+"lba1_model_color_alternatives.csv").c_str());
		if(file.is_open())
		{
			while(!file.eof())
			{
				std::string line;
				std::getline(file, line);
				std::vector<std::string> tokens;
				StringHelper::Tokenize(line, tokens, ",");
				if(tokens.size() > 6)
				{
					ModeData &mdata = _data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]];
					std::string colorpart = tokens[4];

					// get default color
					{
						std::string defaultcs = tokens[5];
						std::vector<std::string> tokensc;
						StringHelper::Tokenize(defaultcs, tokensc, ";");
						for(size_t cc=0; cc< tokensc.size(); ++cc)
						{
							std::string colostr = tokensc[cc].substr(1);
							LbaNet::Lba1ColorIndex indx;
							indx.Color = atoi(colostr.c_str());
							char colortype = tokensc[cc][0];
							switch(colortype)
							{
								case 'P':
									indx.ModelPart = LbaNet::PolygonColor;
								break;
								case 'L':
									indx.ModelPart = LbaNet::LineColor;
								break;
								case 'S':
									indx.ModelPart = LbaNet::SphereColor;
								break;
							}

							mdata.coloralternatives[colorpart]._defaults.push_back(indx);
						}
					}

					for(size_t i=6; i<tokens.size(); ++i)
					{
						std::string defaultcs = tokens[i];
						std::vector<std::string> tokensc;
						StringHelper::Tokenize(defaultcs, tokensc, ";");
						mdata.coloralternatives[colorpart]._alternatives.push_back(std::vector<int>());
						for(size_t cc=0; cc< tokensc.size(); ++cc)
						{
							int color = atoi(tokensc[cc].c_str());
							mdata.coloralternatives[colorpart]._alternatives[i-6].push_back(color);
						}
					}
				}
			}
		}
	}



	// read the anim properties file and get data
	{
		std::ifstream file((Lba1ModelDataPath()+"lba1_model_animation_properties.csv").c_str());
		if(file.is_open())
		{
			// read first information line
			std::string line;

			std::getline(file, line);
			StringHelper::Tokenize(line, _header_infos, ",");

			while(!file.eof())
			{
				std::getline(file, line);
				std::vector<std::string> tokens;
				StringHelper::Tokenize(line, tokens, ",");

				if(tokens.size() > 4)
				{
					for(size_t i=4; i<tokens.size(); ++i)
					{
						std::string anims = tokens[i];
						if(anims.size() > 0)
						{
							AnimationData AD;

							std::string animstring = _header_infos[i];
							std::vector<std::string> tokens2;
							StringHelper::Tokenize(anims, tokens2, ";");
							for(size_t j=0; j<tokens2.size(); ++j)
							{
								std::vector<std::string> tokens3;
								StringHelper::Tokenize(tokens2[j], tokens3, ":");
								if(tokens3.size() == 2)
								{
									int keyframe = atoi(tokens3[0].c_str());
									if(keyframe < 0)
									{
										AD.repeatedsoundpath = tokens3[1];
									}
									else
									{
										AnimationFrameData afd;

										std::vector<std::string> tokens4;
										StringHelper::Tokenize(tokens3[1], tokens4, "|");
										for(size_t k=0; k<tokens4.size(); ++k)
										{
											if(tokens4[k] == "S")
												afd.starthit = true;
											else if (tokens4[k] == "E")
												afd.endthit = true;
											else if (tokens4[k] == "P")
												afd.startprojectile = true;
											else
												afd.soundpath = tokens4[k];
										}

										AD.animationframes[keyframe] = afd;
									}
								}
							}

							_data[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].animationsD[animstring] = AD;


						}
					}
				}


			}
		}
	}

}


/***********************************************************
Destructor
***********************************************************/
Lba1ModelMapHandler::~Lba1ModelMapHandler()
{

}


/***********************************************************
check if can play given animation
***********************************************************/
bool Lba1ModelMapHandler::CanPlayAnimation(	const std::string & modelname,
											const std::string & outfit,
											const std::string & weapon,
											const std::string & mode,
											const std::string & animation)
{
	if(modelname == "" || outfit == "" || weapon == "" || mode == "" || animation == "")
		return false;

	std::map<std::string, std::vector<int> > & anims = 
		_data[modelname].outfits[outfit].weapons[weapon].modes[mode].animations;

	std::map<std::string, std::vector<int> >::iterator it = anims.find(animation);
	if(it != anims.end())
		if(it->second.size() > 0)
			return (it->second[0] >= 0);
		 

	return false;
}


/***********************************************************
get model available anims
***********************************************************/
int Lba1ModelMapHandler::GetModelAvailableAnims(	const std::string & modelname,
								const std::string & outfit,
								const std::string & weapon,
								const std::string & mode,
								std::vector<std::string> & resanimation)
{
	if(modelname == "" || outfit == "" || weapon == "" || mode == "")
		return -1;

	std::map<std::string, std::vector<int> > & anims = 
		_data[modelname].outfits[outfit].weapons[weapon].modes[mode].animations;

	if(anims.size() == 0) // not found
		return -1;

	resanimation.clear();
	std::map<std::string, std::vector<int> >::iterator itm = anims.begin();
	std::map<std::string, std::vector<int> >::iterator endm = anims.end();
	for(; itm != endm; ++itm)
	{
		std::vector<int> &anims = itm->second;
		if(anims.size() > 0)
			if(anims[0] >= 0)
				resanimation.push_back(itm->first);
	}

	return 0;
}


/***********************************************************
get model number and body number
***********************************************************/
int Lba1ModelMapHandler::GetModelInfo(	const std::string & modelname,
										const std::string & outfit,
										const std::string & weapon,
										const std::string & mode,
										const std::string & animation,
										int & resmodel,
										int & resbody,
										std::vector<int> & resanimation)
{
	if(modelname == "" || outfit == "" || weapon == "" || mode == "" || animation == "")
		return -1;

	std::map<std::string, std::vector<int> > & anims = 
		_data[modelname].outfits[outfit].weapons[weapon].modes[mode].animations;


	//check if given animation is a number
	if(animation[0] == '#')
	{
		resanimation.clear();
		int anim = atoi(animation.substr(1).c_str());
		if(anim < 0)
			anim = 0;
		resanimation.push_back(anim);
		resmodel = _data[modelname].outfits[outfit].weapons[weapon].modes[mode].modelnumber;
		resbody = _data[modelname].outfits[outfit].weapons[weapon].modes[mode].bodynumber;
		return 0;		
	}



	// do first trial with current mode
	{
		std::map<std::string, std::vector<int> >::iterator it = anims.find(animation);
		if(it != anims.end())
		{
			resanimation = it->second;
			if(resanimation.size() > 0)
			{
				// normal case
				if(resanimation[0] >= 0)
				{
					resmodel = _data[modelname].outfits[outfit].weapons[weapon].modes[mode].modelnumber;
					resbody = _data[modelname].outfits[outfit].weapons[weapon].modes[mode].bodynumber;
					return 0;
				}

				// animation forced to no - e.g. cant go backward with dinofly
				if(resanimation[0] == -2) 
					return -1;
			}
		}
	}

	// if no success before - try with default mode
	std::map<std::string, ModeData> & modes = 
		_data[modelname].outfits[outfit].weapons[weapon].modes;

	if(modes.size() > 0)
	{
		ModeData & tmpmodeld = modes.begin()->second;
		std::map<std::string, std::vector<int> >::iterator it = 
		tmpmodeld.animations.find(animation);

		if(it != tmpmodeld.animations.end())
		{
			resanimation = it->second;
			if(resanimation.size() > 0)
				if(resanimation[0] >= 0)
				{
					resmodel = tmpmodeld.modelnumber;
					resbody = tmpmodeld.bodynumber;
					return 0;
				}
		}
	}

	// if still no success then no way
	return -1;
}


/***********************************************************
get model animation info
***********************************************************/
int Lba1ModelMapHandler::GetModelAnimationInfo(	const std::string & modelname,
								const std::string & outfit,
								const std::string & weapon,
								const std::string & mode,
								const std::string & animation,
								AnimationData & animdata)
{
	animdata.repeatedsoundpath = "";
	animdata.animationframes.clear();

	if(modelname == "" || outfit == "" || weapon == "" || mode == "" || animation == "")
		return -1;

	std::map<std::string, AnimationData > & anims = 
		_data[modelname].outfits[outfit].weapons[weapon].modes[mode].animationsD;

	std::map<std::string, AnimationData >::iterator it = anims.find(animation);
	if(it != anims.end())
	{
		animdata = it->second;	
		return 0;
	}

	// if still no success then no way
	return -1;
}



/***********************************************************
get model number and extra info
***********************************************************/
int Lba1ModelMapHandler::GetModelExtraInfo(	const std::string & modelname,
								const std::string & outfit,
								const std::string & weapon,
								const std::string & mode,
								int & resWeaponType,
								ModelSize & resSize)
{
	if(modelname == "" || outfit == "" || weapon == "" || mode == "")
		return -1;

	ModeData &mdata = _data[modelname].outfits[outfit].weapons[weapon].modes[mode];

	resWeaponType = mdata.WeaponType;
	resSize = mdata.Size;
	return 0;
}

/***********************************************************
get model color
***********************************************************/
int Lba1ModelMapHandler::GetModelColor(	const std::string & modelname,
								const std::string & outfit,
								const std::string & weapon,
								const std::string & mode,
								std::vector<int> & polycolors,
								std::vector<int> & spherecolors,
								std::vector<int> & linecolors)
{
	polycolors.clear();
	spherecolors.clear();
	linecolors.clear();


	if(modelname == "" || outfit == "" || weapon == "" || mode == "")
		return -1;

	ModeData &mdata = _data[modelname].outfits[outfit].weapons[weapon].modes[mode];

	polycolors = mdata.polycolors;
	spherecolors = mdata.spherecolors;
	linecolors = mdata.linecolors;

	return 0;
}




/***********************************************************
get model color
***********************************************************/
int Lba1ModelMapHandler::GetModelColorAlternative(	const std::string & modelname,
								const std::string & outfit,
								const std::string & weapon,
								const std::string & mode,
								const std::string & colorpart,
								int alternativeindex,
								std::vector<LbaNet::Lba1ColorIndex>	&defaultc,
								std::vector<int>	&alternativec)
{
	defaultc.clear();
	alternativec.clear();


	if(modelname == "" || outfit == "" || weapon == "" || mode == "")
		return -1;

	ModeData &mdata = _data[modelname].outfits[outfit].weapons[weapon].modes[mode];

	std::map<std::string, PossibleColorInfo>::iterator itm = mdata.coloralternatives.find(colorpart);
	if(itm == mdata.coloralternatives.end())
		return -1;

	if(alternativeindex < 0)
		return -1;

	if(alternativeindex >= (int)itm->second._alternatives.size())
		return -1;

	defaultc = itm->second._defaults;
	alternativec = itm->second._alternatives[alternativeindex];

	return 0;
}

/***********************************************************
save animation properties to file
***********************************************************/
void Lba1ModelMapHandler::SaveAnimationData()
{
	std::ofstream file((Lba1ModelDataPath()+"lba1_model_animation_properties.csv").c_str());
	if(file.is_open())
	{
		if(_header_infos.size() > 0)
		{
			file<<_header_infos[0];
			for(size_t i=1; i<_header_infos.size(); ++i)
				file<<","<<_header_infos[i];
			file<<std::endl;
		}

		std::map<std::string, ModelData>::iterator itm = _data.begin();
		std::map<std::string, ModelData>::iterator endm = _data.end();
		for(; itm != endm; ++itm)
		{
			ModelData &md = itm->second;

			std::map<std::string, OutfitData>::iterator itm2 = md.outfits.begin();
			std::map<std::string, OutfitData>::iterator endm2 =  md.outfits.end();		
			for(; itm2 != endm2; ++itm2)
			{
				OutfitData &od = itm2->second;

				std::map<std::string, WeaponData>::iterator itm3 = od.weapons.begin();
				std::map<std::string, WeaponData>::iterator endm3 =  od.weapons.end();		
				for(; itm3 != endm3; ++itm3)
				{
					WeaponData &wd = itm3->second;

					std::map<std::string, ModeData>::iterator itm4 = wd.modes.begin();
					std::map<std::string, ModeData>::iterator endm4 =  wd.modes.end();		
					for(; itm4 != endm4; ++itm4)
					{
						ModeData &md = itm4->second;
						std::vector<std::string> towrite(_header_infos.size());
						towrite[0] = itm->first;
						towrite[1] = itm2->first;
						towrite[2] = itm3->first;
						towrite[3] = itm4->first;

						std::map<std::string, AnimationData >::iterator itm5 = md.animationsD.begin();
						std::map<std::string, AnimationData >::iterator endm5 =  md.animationsD.end();		
						for(; itm5 != endm5; ++itm5)
						{
							AnimationData &ad = itm5->second;

							std::stringstream tw;
							if(ad.repeatedsoundpath != "")
								tw << "-1:" << ad.repeatedsoundpath << ";";
							bool firstl = true;

							std::map<int, AnimationFrameData >::iterator itm6 = ad.animationframes.begin();
							std::map<int, AnimationFrameData >::iterator endm6 =  ad.animationframes.end();		
							for(; itm6 != endm6; ++itm6)
							{
								AnimationFrameData &afd = itm6->second;
								if(afd.starthit || afd.endthit || afd.startprojectile || afd.soundpath != "")
								{
									if(firstl)
										firstl = false;
									else
										tw << ";";


									tw << itm6->first <<":"<<afd.soundpath;
									bool somwr = (afd.soundpath != "");
									if(afd.starthit)
									{
										if(somwr)
											tw << "|";
										else
											somwr = true;

										tw << "S";
									}
									if(afd.endthit)
									{
										if(somwr)
											tw << "|";
										else
											somwr = true;

										tw << "E";
									}
									if(afd.startprojectile)
									{
										if(somwr)
											tw << "|";
										else
											somwr = true;

										tw << "P";
									}
								}
							}


							size_t idx = 4;
							for(size_t cur=4; cur<_header_infos.size();++cur)
							{
								if(_header_infos[cur] == itm5->first)
								{
									idx = cur;
									break;
								}
							}
							towrite[idx] = tw.str();
						}

						file<<towrite[0];
						for(size_t vv=1; vv<towrite.size(); ++vv)
							file<<","<<towrite[vv];
						file<<std::endl;
					}
				}
			}
		}

		file.flush();
		file.close();
	}
}