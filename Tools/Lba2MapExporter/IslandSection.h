#ifndef ISLANDSECTION_H_
#define ISLANDSECTION_H_

#pragma once

#include <map>
#include <vector>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/ShapeDrawable>

#include "HQRReader.h"
#include "ILEdataStructures.h"

#include "ImageManager.h"


struct PhysicalInfo
{
	std::vector<float> vertexes;
	std::vector<unsigned int> indices;
	std::vector<short> materials;
	
};

struct OBLMinMaxBox
{
	OBLMinMaxBox();
	short	mx;
	short	my;
	short	mz;
	short	Mx;
	short	My;
	short	Mz;
};

struct CutGroup
{
	CutGroup(const std::string&, unsigned char, unsigned char, unsigned char, unsigned char, bool transparent);
	std::string					materialName;
	unsigned char				x;
	unsigned char				y;
	unsigned char				width;
	unsigned char				height;

	osg::Vec3Array*				myVerticesPoly;
	osg::DrawElementsUInt*		myprimitive;
	osg::Vec4Array*				colors;
	osg::Vec2Array*				myTexts;
	std::vector<int>			myprimitivenumpoints;

	int							currentIndex;

	bool						usetexture;
	bool						istransparent;

};

class IslandSection
{
public:
	IslandSection(const std::string& islandName, HQRReader *ILE, HQRReader *OBL, int idx, int xpos, int ypos);
	osg::ref_ptr<osg::Geode> load(PhysicalInfo & physinfo);
	osg::ref_ptr<osg::Group>	loadObjects(std::map<int, bool > &mObjLibrary,
												int & objcounter);


private:
	osg::ref_ptr<osg::Geode>	loadGround(PhysicalInfo & physinfo);

	bool	loadSingleObject(IslandObjectInfo *objInfo,
													std::map<int, bool > &mObjLibrary);
	void	addPolygonSection(unsigned char *, OBLPolygonHeader *, std::map<unsigned int, CutGroup>&, 
								CutGroup&, PhysicalInfo & physinfo, int&, OBLMinMaxBox&,
											osg::Vec3Array* myVertices);

	void	loadCutGroups(unsigned char *, std::map<unsigned int, CutGroup>&);

private:
	HQRReader	*mILE;
	HQRReader	*mOBL;

	std::string			mIslandName;
	//Ogre::SceneManager *mSceneMgr;
	int					mIndex;
	int					mXpos;
	int					mYpos;
	//Ogre::SceneNode		*mNode;
	//Ogre::Entity		*mGround;
	//Ogre::SceneNode		*mGroundNode;
	unsigned int		mNumObjects;
	IslandObjectInfo	*mObjectInfo;
};

#endif /* ISLANDSECTION_H_ */
