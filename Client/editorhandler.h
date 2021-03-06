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

#ifndef EDITORHANDLER_H
#define EDITORHANDLER_H

#include <QAbstractTableModel>
#include <QObject>
#include <QStyledItemDelegate>
#include <QStringListModel>

#include "ui_editor.h"
#include "ui_addtriggerdialog.h"
#include "ui_openworlddialog.h"
#include "ui_addspawning.h"
#include "ui_addmapdialog.h"
#include "ui_addteleportdialog.h"
#include "ui_addworlddialog.h"
#include "ui_addactordialog.h"
#include "ui_addtextdialog.h"
#include "ui_additemdialog.h"
#include "ui_addstartitemdialog.h"
#include "ui_ImportantNotice.h"
#include "ui_NavMeshOption.h"
#include "ui_addtemplate.h"
#include "ui_edittemplatesdialog.h"
#include "ui_LuaConsole.h"

#include "ScriptEnvironmentBase.h"
#include "treemodel.h"
#include "Conditions.h"
#include "ClientScript.h"
#include "Actions.h"
#include "Localizer.h"
#include "ActorHandler.h"
#include "ProjectileObjectDef.h"
#include "editorhandlerBase.h"

#include <osg/MatrixTransform>
#include <osgManipulator/Translate1DDragger>


#include <LbaTypes.h>
#include <boost/shared_ptr.hpp>

class QTableWidgetItem;
class ServerLuaHandler;
class DialogPart;
class InventoryItemDef;
class NaviMeshHandler;
class QSignalMapper;
class DisplayObjectHandlerBase;
class HoloCoordinate;
class DisplayInfoHandler;

namespace osgManipulator
{
	class Translate1DDragger;
}

//! CustomStringListModel class
class CustomStringListModel : public QStringListModel
{
	Q_OBJECT

public:
	//! constructor
	CustomStringListModel ( QObject * parent = 0 )
		: QStringListModel(parent){}

	//! add data to the list
	void AddData(const QString & Data);

	//!m clear the list
	void Clear();

	//! remove data of the list if exist
	void RemoveData(const QString & Data);


	//! remove data of the list if exist
	void ReplaceData(const QString & OldData, const QString & NewData);

	//! check if data present in the list
	bool DataExist(const QString & Data);

	//! get first data of the list
	QString GetFirstdata();
};


//! used as model for table
class StringTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	//! constructor
	StringTableModel(const QStringList &header, QObject *parent = 0)
	 : QAbstractTableModel(parent), _stringMatrix(header.size()),
		_header(header)
	{}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
					 int role = Qt::DisplayRole) const;

     Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole);


     bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
     bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

	//! clear the table content
	void Clear();

	//! add a row
	void AddOrUpdateRow(long id, const QStringList &data);

	//! get a string
	QString GetString(const QModelIndex &index);

	//! get the corresponding id
	long GetId(const QModelIndex &index);

	//! get next free id
	long GetNextId();

private:
	std::vector<QStringList>		_stringMatrix;
	std::vector<long>				_ids;
	QStringList						_header;
};





//! used as model for table
class MixedTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	//! constructor
	MixedTableModel(const QStringList &header, QObject *parent = 0)
	 : QAbstractTableModel(parent), _objMatrix(header.size()), _header(header)
	{}

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation,
					 int role = Qt::DisplayRole) const;

     Qt::ItemFlags flags(const QModelIndex &index) const;
     bool setData(const QModelIndex &index, const QVariant &value,
                  int role = Qt::EditRole);


     bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex());
     bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex());

	//! clear the table content
	void Clear();

	//! add a row
	void AddRow(const QVariantList &data);

	//! get a string
	QVariant GetData(const QModelIndex &index);

	//! get a string
	QVariant GetData(int col, int row);

	//! set a string
	void SetData(int col, int row, QVariant v);


private:
	std::vector<QVariantList>			_objMatrix;
	QStringList							_header;
};






 class CustomDelegate : public QStyledItemDelegate
 {
     Q_OBJECT

 public:
     CustomDelegate(QObject *parent, QAbstractItemModel * model);

     QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

     void setEditorData(QWidget *editor, const QModelIndex &index) const;
     void setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const;

     void updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const;

	 void Clear();


public slots:

	//! data changed in object view
	void objmodified(int flag);

	//! data changed in object view
	void objmodified2(double flag);

	//! data changed in object view
	void fileobjmodified(QString selectedfile);

	//! accept1 signal
	void fileobjacceptedbegin(QString selectedfile);

	//! accept2 signal
	void fileobjacceptedend();

	//! rejected signal
	void fileobjrejected();


 private:

	 QAbstractItemModel *													_model;
	 QString																_selectedfile;

	 bool																	_accepted;
 };








//! take care of editor
class EditorHandler : public QMainWindow, public ScriptEnvironmentBase, public EditorHandlerBase
{
	Q_OBJECT

public:
	//! constructor
	EditorHandler(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	//! constructor
	virtual ~EditorHandler();

	//! set the osg window
	void SetOsgWindow(QWidget *osgwindow);

	//! refresh display
	void RefreshDisplay();

	//! refresh gui with map info
	void SetMapInfo(const std::string & mapname);

	//! player moved
	void PlayerMoved(float posx, float posy, float posz);


	// function used by LUA to add actor
	virtual void AddActorObject(boost::shared_ptr<ActorHandler> actor);

	// add a trigger of moving type to the map
	virtual void AddTrigger(boost::shared_ptr<TriggerBase> trigger);

	// add spawn
	virtual void AddSpawn(boost::shared_ptr<Spawn> spawn);

	// add point
	virtual void AddPoint(boost::shared_ptr<PointFlag> point);

	// teleport an object
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void Teleport(int ObjectType, long ObjectId,
						const std::string &NewMapName,
						long SpawningId,
						float offsetX, float offsetY, float offsetZ);

	// called when an object is picked
	void PickedObject(const std::string & name, float px, float py, float pz);

	//! when an editor arrow was dragged by mouse
	void PickedArrowMoved(int pickedarrow);


	// execute client script - does not work on npc objects
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void ExecuteClientScript(int ObjectType, long ObjectId,
										const std::string & ScriptName,
										bool InlineFunction){}
	// execute custom lua function
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void ExecuteCustomAction(int ObjectType, long ObjectId,
										const std::string & FunctionName,
										ActionArgumentBase * args){}

	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void DisplayTxtAction(int ObjectType, long ObjectId,
		long TextId){}


	// send error message to client
	virtual void SendErrorMessage(long clientid, const std::string & messagetitle,
											const std::string &  messagecontent){}

	// open container on client side
	virtual void OpenContainer(long clientid, boost::shared_ptr<LbaNet::ContainerSharedInfo> sharedinfo){}

	void ActorAppearDisappear(int ScriptId, long ActorId, bool appear){}

	//! used by lua to get an actor Position
	//! if id < 1 then it get player position
	virtual LbaVec3 GetActorPosition(int ScriptId, long ActorId){return LbaVec3();}

	//! used by lua to get an actor Rotation
	//! if id < 1 then it get player position
	virtual float GetActorRotation(int ScriptId, long ActorId){return 0;}

	//! used by lua to get an actor Rotation
	//! if id < 1 then it get player position
	virtual LbaQuaternion GetActorRotationQuat(int ScriptId, long ActorId){return LbaQuaternion();}

	//! used by lua to update an actor animation
	//! if id < 1 then it get player position
	virtual void UpdateActorAnimation(int ScriptId, long ActorId, const std::string & AnimationString){}

	//! used by lua to update an actor mode
	//! if id < 1 then it get player position
	virtual void UpdateActorMode(int ScriptId, long ActorId, const std::string & Mode){}


	//! used by lua to move an actor or player
	//! if id < 1 then it moves players
	//! the actor will move using animation speed
	virtual void InternalActorStraightWalkTo(int ScriptId, long ActorId, const LbaVec3 &Position,
										bool asynchronus = false){}

	//! used by lua to rotate an actor
	//! if id < 1 then it moves players
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void InternalActorRotate(int ScriptId, long ActorId, float Angle, float RotationSpeedPerSec,
								bool ManageAnimation, bool asynchronus = false){}

	//! used by lua to wait until an actor animation is finished
	//! if id < 1 then it moves players
	//! if AnimationMove = true then the actor will be moved at the same time using the current animation speed
	virtual void InternalActorAnimate(int ScriptId, long ActorId, bool AnimationMove, int nbAnimation, 
										bool asynchronus = false){}

	virtual void InternalActorWaitForSignal(int ScriptId, long ActorId, int Signalnumber, bool asynchronus = false){}


	//! used by lua to tell that the actor should be reserved for the script
	virtual void ReserveActor(int ScriptId, long ActorId){}


	//! called when a script has finished
	virtual void ScriptFinished(int scriptid, const std::string & functioname){}



	//! used by lua to move an actor or player
	//! the actor will change model
	virtual void UpdateActorModel(int ScriptId, long ActorId, const std::string & Name){}

	//! used by lua to move an actor or player
	//! the actor will change outfit
	virtual void UpdateActorOutfit(int ScriptId, long ActorId, const std::string & Name){}

	//! used by lua to move an actor or player
	//! the actor will change weapon
	virtual void UpdateActorWeapon(int ScriptId, long ActorId, const std::string & Name){}

	//! used by lua to move an actor or player
	//! the actor will change mode
	virtual void SendSignalToActor(long ActorId, int Signalnumber){}


	//! used by lua to move an actor or player
	//! the actor will move using animation speed
	void TeleportActorTo(int ScriptId, long ActorId, const LbaVec3 &Position){}


	//! used by lua to move an actor or player
	//! the actor will move using speed
	virtual void InternalActorGoTo(int ScriptId, long ActorId, const LbaVec3 &Position,
										float Speed, bool asynchronus = false){}


	virtual void InternalActorWalkToPoint(int ScriptId, long ActorId, const LbaVec3 &Position, float RotationSpeedPerSec, bool moveForward, 
										bool asynchronus = false){}

	//! set the function to use as current movement script for the given actor
	virtual void ActorSetMovementScript(int ScriptId, long ActorId, const std::string & functionName){}

		//! used by lua to move an actor or player
	//! the actor will wait for signal
	virtual void InternalActorSleep(int ScriptId, long ActorId, int Signalnumber,
										bool asynchronus = false){}

	//! used by lua to move an actor or player
	//! the actor change rotation
	void SetActorRotation(int ScriptId, long ActorId, float Angle){}


	//! used by lua to rotate an actor
	//! the actor will rotate until it reach "Angle" with speed "RotationSpeedPerSec"
	//! if RotationSpeedPerSec> 1 it will take the shortest rotation path else the longest
	//! if ManageAnimation is true then the animation will be changed to suit the rotation
	virtual void InternalActorRotateFromPoint(int ScriptId, long ActorId, float Angle, const LbaVec3 &Position,
												float RotationSpeedPerSec, bool asynchronus = false){}


	//! used by lua to make actor follow waypoint
	virtual void InternalActorFollowWaypoint(int ScriptId, long ActorId, int waypointindex1,
												int waypointindex2, bool asynchronus = false){}


	//! used by lua to make actor follow waypoint
	virtual void InternalActorFollowGivenWaypoint(int ScriptId, long ActorId, 
											const LbaVec3 & Pm1, const LbaVec3 & P0,
											const LbaVec3 & P1, const LbaVec3 & P2, 
											const LbaVec3 & P3, const LbaVec3 & P4, 
											bool asynchronus = false){}

	//! used by lua to move an actor or player
	//! the actor show/hide
	virtual void ActorShowHide(int ScriptId, long ActorId, bool Show){}

	//! add/remove item from player inventory
	virtual void AddOrRemoveItem(long PlayerId, long ItemId, int number, int InformClientType){}


	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void HurtActor(int ObjectType, long ObjectId, float HurtValue, bool HurtLife,
								int PlayedAnimation){}


	// display text to client window
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void KillActor(int ObjectType, long ObjectId){}


	//! switch the model of an actor
	virtual void SwitchActorModel(long ActorId, const std::string & newmodelname){}

	//! revert the switch the model of an actor
	virtual void RevertActorModel(long ActorId){}


	//! open dialog with player
	virtual void StartDialog(long PlayerId, long NpcId, long npcnametextid, bool simpledialog,
												boost::shared_ptr<DialogPart> dialogroot){}

	//! stop target player
	virtual void NpcUntargetPlayer(long NpcId, long PlayerId){}

	//! start quest
	virtual void StartQuest(long PlayerId, long Questid){}

	//! end quest
	virtual void TriggerQuestEnd(long PlayerId, long Questid){}

	//! condition
	virtual bool QuestStarted(long PlayerId, long Questid){return false;}

	//! condition
	virtual bool QuestFinished(long PlayerId, long Questid){return false;}

	//! condition
	virtual bool QuestAvailable(long PlayerId, long Questid){return false;}


	//! condition
	virtual bool ChapterStarted(long PlayerId, int Chapter){return false;}

	//! open shop
	virtual void OpenShop(long PlayerId, const LbaNet::ItemsMap &items,
									const LbaNet::ItemInfo & currencyitem){}

	//! open mailbox
	virtual void OpenMailbox(long PlayerId){}


	//! get actor info
	virtual boost::shared_ptr<DynamicObject> GetActor(int ObjectType, long ObjectId)
	{return boost::shared_ptr<DynamicObject>();}


	// AttachActor
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void AttachActor(long ActorId, int AttachedObjectType, long AttachedObjectId){}

	// DettachActor
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual void DettachActor(long ActorId, long AttachedObjectId){}


	//! send event to player
	virtual void SendEvents(long PlayerId, const LbaNet::EventsSeq & evts){}

	//! send event to player
	virtual int GetInventoryItemCount(long PlayerId, long Itemid){return 0;}

	//! inform that map Finished Loaded();
	void MapFinishedLoaded();

	// check if actor can play animation
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual bool CanPlayAnimation(int ObjectType, long ObjectId, const std::string & anim){ return false;}

	// check if actor is moving
	// ObjectType ==>
	//! 1 -> npc object
	//! 2 -> player object
	//! 3 -> movable object
	virtual bool ActorMoving(int ObjectType, long ObjectId){ return false;}

	// run attack script
	virtual void RunAttackScript(long ActorId, const std::string & FunctionName){}

	// set DB flag
	virtual void SetDBFlag(long PlayerId, const std::string & flagid, int value){}

	// get DB flag
	virtual int GetDBFlag(long PlayerId, const std::string & flagid){return -1;}


	// PlayClientVideo
	virtual void PlayClientVideo(long ClientId,	const std::string & VideoPath){}

	// DisplayImage
	virtual void DisplayImage(int ScriptId, const std::string & imagepath, long NbSecondDisplay,
								bool FadeIn, const LbaColor &FadeInColor,
								bool FadeOut, const LbaColor &FadeOutColor,
								const std::string & OptionalMusicPath){}

	// DisplayScrollingText
	virtual void DisplayScrollingText(int ScriptId, const std::string & imagepath,
										const LbaVecLong &textIds,
										const std::string & OptionalMusicPath){}



	// reset to game screen after displaying extra gl stuff
	virtual void StartDisplayExtraScreen(int ScriptId){}

	// reset to game screen after displaying extra gl stuff
	virtual void EndDisplayExtraScreen(){}


	//! used by lua to make an actor play a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorStartSound(int ScriptId, long ActorId, int SoundChannel,
							const std::string & soundpath, int numberTime, bool randomPitch){}

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorStopSound(int ScriptId, long ActorId, int SoundChannel){}

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorPauseSound(int ScriptId, long ActorId, int SoundChannel){}

	//! used by lua to make an actor stop a sound
	//! there is 5 available channels (0 to 5)
	virtual void ActorResumeSound(int ScriptId, long ActorId, int SoundChannel){}

	//! TeleportPlayerAtEndScript
	virtual void TeleportPlayerAtEndScript(int ScriptId, const std::string &newmap, long newspawn){}
	
	//! AddHolomap
	virtual void AddHolomap(boost::shared_ptr<Holomap> holo){}

	//! AddHolomap
	virtual void AddHolomapLoc(boost::shared_ptr<HolomapLocation> holo){}

	//! AddHolomap
	virtual void AddHolomapPath(boost::shared_ptr<HolomapTravelPath> holo){}

	//! display holomap
	virtual void DisplayHolomap(int ScriptId, long PlayerId, int mode, long holoid){}

	//! add a managed ghost to the map
	virtual void ExecuteDelayedAction(const std::string & fctname, long ms, int ObjectType, long actorId, ActionArgumentBase* args) {}

	//! play a sound
	void PlaySound(const std::string & soundpath, bool Use3d, float  PosX, float  PosY, float  PosZ) {}

	//! wait movement script
	void ActorWaitForResume(int ScriptId, long ActorId) {}

private:
	enum ObjectEditType {EditObjCopy, EditObjCut, EditObjDelete};

public slots:
	 //! ui button clicked
     void addtrigger_button_clicked();

	 //! ui button clicked
     void removetrigger_button_clicked();

	 //! ui button clicked
     void selecttrigger_button_clicked();


	 //! dialog accepted
     void addtrigger_accepted();


	 //! quit editor
     void quit();

	 //! on new world action
     void NewWorldAction();

	 //! on open world action
     void OpenWorldAction();

	 //! on save world action
	 void SaveWorldAction(std::string mapname = "");

	 //! on open world
     void OpenWorld();

	 //! on go tp clicked
     void goto_tp_button_clicked();

	 //! on go map clicked
     void goto_map_button_clicked();

	 //! on go tp clicked
     void goto_tp_double_clicked(const QModelIndex & itm);

	 //! on go map clicked
     void goto_map_double_clicked(const QModelIndex & itm);

	//! on addspawning clicked
	void addspawning_button_clicked();

	//! on removespawning clicked
	void removespawning_button_clicked();

	//! on selectspawning clicked
	void selectspawning_button_clicked();

	 //! on selectspawning clicked
     void selectspawning_double_clicked(const QModelIndex & itm);

	//! on AddSpawning clicked
	void AddSpawning_clicked();

	//! when go button clicked on info part
	void info_go_clicked();

	//! camera type toggled in info
	void info_camera_toggled(bool checked);

	//! data changed in object view
	void objectdatachanged(const QModelIndex &index1, const QModelIndex &index2);


	 //! on selecttrigger_double_clicked
     void selecttrigger_double_clicked(const QModelIndex & itm);

	 //! world description changed
	 void WorldDescriptionChanged();

	 //! worlds news changed
	 void WorldNewsChanged();

	 //! WorldStartingScriptChanged
	 void WorldStartingScriptChanged(const QString &);

	 //! starting map modified
	 void StartingMapModified(int index);

	 //! starting spawn modified
	 void StartingSpawnModified(int index);

	 //! starting life modified
	 void StartingLifeModified(int value);

	 //! starting Outfit modified
	 void StartingOutfitModified(int index);

	 //! starting Weapon modified
	 void StartingWeaponModified(int index);

	 //! starting Mode modified
	 void StartingModeModified(int index);


	 //! ui button clicked
     void addmap_button_clicked();

	 //! ui button clicked
     void removemap_button_clicked();

	 //! dialog accepted
     void addmap_accepted();

	 //! map description changed
	 void MapDescriptionChanged();

	 //! map type changed
	 void MapTypeChanged(int flag);

	 //! map instance changed
	 void MapInstanceChanged(int flag);

	 //! map cam type changed
	 void MapCameraTypeChanged(bool flag);


	//! tp add button push
	 void TpAdd_button();

	//! tp remove button push
	 void TpRemove_button();

	//! tp edit button push
	 void TpEdit_button();

	//! tp add button push
	 void TpAdd_button_accepted();

	//! map changed in tp dialog
	void TpDialogMapChanged(int flag);

	//! add world accepted
	void addworld_accepted();


	//! Actor add button push
	 void ActorAdd_button();

	//! Actor remove button push
	 void ActorRemove_button();

	//! Actor edit button push
	 void ActorSelect_button();

	//! Actor add button push
	 void ActorAdd_button_accepted();

	 //! on selectactor_double_clicked
     void selectactor_double_clicked(const QModelIndex & itm);

	//! text type modified
	 void TextTypeModified(int index);

	//! text lang modified
	 void TextLangModified(int index);



	//! TextAdd_button
	void TextAdd_button();

	//! TextRemove_button
	void TextRemove_button();


	//! VoicemAdd_button
	void VoicemAdd_button();

	//! VoicemRem_button
	void VoicemRem_button();


	//! TextEdit_button
	void TextEdit_button();

	//! TextPlay_button
	void TextPlay_button();



	//! TextAdd_button_accepted
	void TextAdd_button_accepted();

	//! OpenCustomServerLua
	void OpenCustomServerLua();

	//! OpenCustomClientLua
	void OpenCustomClientLua();

	//! RefreshClientScript
	void RefreshClientScript();

	//! TestLuaConsole
	void TestLuaConsole();

	 //! on selectactor_double_clicked
     void selecttext_double_clicked(const QModelIndex & itm);


	 //! on selectitem_double_clicked
     void selectitem_double_clicked(const QModelIndex & itm);


	//! ItemAdd_button_accepted
	void ItemAdd_button_accepted();

	//! ItemAdd_button
	void ItemAdd_button();

	//! ItemRemove_button
	void ItemRemove_button();

	//! ItemSelect_button
	void ItemSelect_button();


	//! StartItemAdd_button
	void StartItemAdd_button();

	//! StartItemRemove_button
	void StartItemRemove_button();

	//! StartItemAdd_button_accepted
	void StartItemAdd_button_accepted();


	//! player colors modified
	void colorModified(int v);


	//! QuestAdd_button
	void QuestAdd_button();

	//! QuestRemove_button
	void QuestRemove_button();

	//! QuestSelect_button
	void QuestSelect_button();


	//! HoloAdd_button
	void HoloAdd_button();

	//! HoloRemove_button
	void HoloRemove_button();

	//! HoloSelect_button
	void HoloSelect_button();

	//! HoloAdd_button
	void HoloLocAdd_button();

	//! HoloRemove_button
	void HoloLocRemove_button();

	//! HoloSelect_button
	void HoloLocSelect_button();

	//! HoloAdd_button
	void HoloPathAdd_button();

	//! HoloRemove_button
	void HoloPathRemove_button();

	//! HoloSelect_button
	void HoloPathSelect_button();



	//! on selectitem_double_clicked
	void selectquest_double_clicked(const QModelIndex & itm);

	//! on selectholomap_double_clicked
	void selectholomap_double_clicked(const QModelIndex & itm);
	void selectholomaploc_double_clicked(const QModelIndex & itm);
	void selectholomappath_double_clicked(const QModelIndex & itm);




	//! map music changed
	void MapMusicChanged(const QString & text);

	//! map music repeat changed
	void MapMusicRepeatChanged(int newvalue);

	//! map music file clicked
	void MapMusicFile_clicked();

	//! Notice_accepted
	void Notice_accepted();

	//! Notice_rejected
	void Notice_rejected();

	//! Notice_closed
	void Notice_closed(int v);


	//! GenerateNavimesh
	void GenerateNavimesh();

	//! ToggleNavimeshDisplay
	void ToggleNavimeshDisplay();

	//! option navimesh
	void OptionNavimesh();

	//! test find path
	void TestFindPath();

	//! TestNPCAttack
	void TestNPCAttack();

	//! TestNPCStopAttack
	void TestNPCStopAttack();

	//! edition slots
	void CopyObjectClicked();
	void PasteObjectClicked();
	void CutObjectClicked();
	void DeleteObjectClicked();

	//! add template object
	void AddTemplateObject();

	//! template object clicked
	void TemplateMenuClicked(int templateid);

	//! edit templates
	void EditTemplates();

	//! TemplateRemove_button
	void TemplateRemove_button();

	//! run lua console
	void RunLuaConsole();

	//! execute an action on a given zone
	virtual void ExecuteActionOnZone(ActionBasePtr action, const LbaSphere & zone, ActionArgumentBase* args) {}
	
	//! generate an id to be used for a dynamic actor creation
	virtual long GenerateDynamicActorId() {return -1;}

	//! add a managed ghost to the map
	virtual long AddManagedGhost(long ManagingPlayerid, const ActorObjectInfo& ainfo, bool UseAsDecoy, bool moving) {return -1;}

	//! remove managed ghost from the map
	virtual void RemoveManagedGhost(long id) {}

	//!  used by lua to get player position
	virtual LbaNet::PlayerPosition GetPlayerPosition(long clientid)
	{return LbaNet::PlayerPosition();}

protected:
	//! override close event
	virtual void closeEvent(QCloseEvent* event);

	//! Check if we need to save before quit
	bool SaveBeforeQuit();

	//! return true if some changes are not saved
	bool Modified();

	//! set the work as modified
	void SetModified();

	//! set the work as saved
	void SetSaved();

	//! set the work as modified
	void SetMapModified();

	//! Check if we need to save before quit map
	bool SaveMapBeforeQuit(bool alreadychangedmap = false);

	//! refresh gui with world info
	void SetWorldInfo(const std::string & worldname);


	//! reset world display in editor
	void ResetWorld();

	//! reset map display in editor
	void ResetMap();

	//! reset object display in editor
	void ResetObject();


	//! add a spawning to the map
	long AddOrModSpawning(const std::string &mapname,
							const std::string &spawningname,
							float PosX, float PosY, float PosZ,
							float Rotation, bool forcedrotation,
							ActionBasePtr actionatarrival,
							long modifyid = -1);

	//! remove a spawning from the map
	void RemoveSpawning(const std::string &mapname, long spawningid);


	//! set spawning in the object
	void SelectSpawning(long id, const QModelIndex &parent = QModelIndex());


	//! clear the object displayed if it is the selected one
	void ClearObjectIfSelected(const std::string objecttype, long id);


	//! called when spawning object changed
	void SpawningObjectChanged(long id, const QModelIndex &parentIdx);


	//! check if a world is opened
	bool IsWorldOpened();

	//! check if a map is opened
	bool IsMapOpened();


	//! select an action and display it in object view
	void SelectAction(ActionBase* action, const QModelIndex &parent = QModelIndex());

	//! called when action object changed
	void ActionObjectChanged(const std::string & category, const QModelIndex &parentIdx);




	//! add an spawning name to the name list
	void AddSpawningName(const std::string & mapname, const std::string & name);

	//! remove an spawning name to the name list
	void RemoveSpawningName(const std::string & mapname, const std::string & name);

	//! replace an spawning name to the name list
	void ReplaceSpawningName(const std::string & mapname, const std::string & oldname,
															const std::string & newname);



	//! add an new trigger to the list
	void AddNewTrigger(boost::shared_ptr<TriggerBase> trigger);

	//! remove trigger
	void RemoveTrigger(long id);

	//! select trigger
	void SelectTrigger(long id, const QModelIndex &parent = QModelIndex());

	//! called when trigger object changed
	void TriggerObjectChanged(long id, const std::string & category, const QModelIndex &parentIdx);

	//! change current map to new map
	void ChangeMap(const std::string & mapname, long spawningid);

	//! change current map to new map
	void ChangeMap(const std::string & mapname, const std::string SpawnName);

	//! save current map to file
	void SaveMap(const std::string & filename);

	//! save holomap to file
	void SaveHoloMap(const std::string & filename);


	// refresh starting info tab
	void RefreshStartingInfo();

	// refresh starting map
	void RefreshStartingMap();

	// refresh starting spawning
	void RefreshStartingSpawning();

	// refresh starting outfit
	void RefreshStartingModelOutfit();

	// refresh starting weapon
	void RefreshStartingModelWeapon();

	// refresh starting mode
	void RefreshStartingModelMode();

	// generate list of possible name
	void UpdateModelName(boost::shared_ptr<CustomStringListModel> toupdate);

	// generate list of possible outfit
	void UpdateModelOutfit(const std::string & modelname, boost::shared_ptr<CustomStringListModel> toupdate);

	// generate list of possible weapon
	void UpdateModelWeapon(const std::string & modelname, const std::string & outfit,
											boost::shared_ptr<CustomStringListModel> toupdate);

	// generate list of possible mode
	void UpdateModelMode(const std::string & modelname, const std::string & outfit, const std::string & weapon,
											boost::shared_ptr<CustomStringListModel> toupdate);

	// refresh Actor Model Name
	void RefreshActorModelName(int index, QModelIndex parentIdx, bool resize,
									boost::shared_ptr<ActorHandler> actor);

	// refresh Actor Model Outfit
	void RefreshActorModelOutfit(int index, QModelIndex parentIdx, const std::string & modelname, bool resize,
									boost::shared_ptr<ActorHandler> actor);

	// refresh Actor Model Weapon
	void RefreshActorModelWeapon(int index, QModelIndex parentIdx,
									const std::string & modelname, const std::string & outfit, bool resize,
									boost::shared_ptr<ActorHandler> actor);

	// refresh Actor Model Mode
	void RefreshActorModelMode(int index, QModelIndex parentIdx,
									const std::string & modelname, const std::string & outfit,
									const std::string & weapon, bool resize,
									boost::shared_ptr<ActorHandler> actor);




	// refresh Actor Model Name
	void RefreshSimpleActorModelName(LbaNet::ModelInfo & dinfo);
	void RefreshSimpleActorModelOutfit(LbaNet::ModelInfo & dinfo);
	void RefreshSimpleActorModelWeapon(LbaNet::ModelInfo & dinfo);
	void RefreshSimpleActorModelMode(LbaNet::ModelInfo & dinfo);


	// tp to default spawning of map
	void CreateDefaultSpawningAndTp(const std::string & mapname);

	// update tp list with new span name
	void UpdateTpSpanName(const std::string & mapname, long spwid, const std::string &  spname);

	// remove actor
	void RemoveActor(long id);

	// select actor
	void SelectActor(long id, const QModelIndex &parent = QModelIndex());

	// called when actor object changed
	void ActorObjectChanged(long id, const QModelIndex &parentIdx, int updatedrow);

	//! update editor selected ector display
	void UpdateSelectedActorDisplay(LbaNet::ObjectPhysicDesc desc,
											   boost::shared_ptr<ActorHandler> actor);

	//! remove current selected display
	void RemoveSelectedActorDislay();

	//! update editor selected trigger display
	void UpdateSelectedZoneTriggerDisplay(float PosX, float PosY, float PosZ,
												float SizeX, float SizeY, float SizeZ);

	//! update editor selected trigger display
	void UpdateSelectedDistanceTriggerDisplay(float PosX, float PosY, float PosZ, float distance);

	//! draw arrow on selected object
	void DrawArrows(float PosX, float PosY, float PosZ);

	//! remove arrow from display
	void RemoveArrows();

	//! select script and put info in object list
	void SelectCScript( ClientScriptBasePtr script, const QModelIndex &parent = QModelIndex());


	//! draw ladder on the map
	void UpdateSelectedGoUpLadderScriptDisplay( float posX, float posY, float posZ, float Height, int Direction );


	//! called when CScript object changed
	void CScriptObjectChanged(const std::string & category, const QModelIndex &parentIdx);


	//! remove current selected display
	void RemoveSelectedScriptDislay();


	//! select a condition
	void SelectCondition(ConditionBasePtr cond, const QModelIndex &parent = QModelIndex());

	//! create a new condition
	ConditionBasePtr CreateCondition(const std::string & type);

	//! get type of condition
	std::string GetConditionType(ConditionBasePtr ptr);

	//! called when Condition changed
	void ConditionChanged(const std::string & category, const QModelIndex &parentIdx);


	//! create a new cscript
	ClientScriptBasePtr CreateCscript(const std::string & type);

	//! get type of cscript
	std::string GetCScriptType(ClientScriptBasePtr ptr);


	//! create a new action
	ActionBasePtr CreateAction(const std::string & type);

	//! get type of action
	std::string GetActionType(ActionBasePtr ptr);

	//! set item in the object
	void SelectItem(boost::shared_ptr<InventoryItemDef> item, const QModelIndex &parent = QModelIndex());

	//! item object changed
	void ItemChanged(long id, const std::string & category, const QModelIndex &parentIdx);

	//! select dialog and put info in object list
	void SelectDialog(DialogPart* dialog, const QModelIndex &parent = QModelIndex());

	//! called when dialog changed
	void DialogChanged(const QModelIndex &parentIdx);


	//! set Teleport in the object
	void SelectTeleport(long id, const QModelIndex &parent = QModelIndex());

	//! called when Teleport object changed
	void TeleportChanged(long id, const QModelIndex &parentIdx);

	//! set Quest in the object
	void SelectQuest(long id, const QModelIndex &parent = QModelIndex());

	//! called when Quest object changed
	void QuestChanged(long id, const QModelIndex &parentIdx);

	//! display conditions
	void DisplayConditions(std::vector<ConditionBasePtr> conds, const QModelIndex &parent);

	//! refresh navimeshdisplay
	void RefreshNaviMeshDisplay();

	//! display start path
	void DisplayStartPath(const LbaVec3 & position);

	//! display end path
	void DisplayEndPath(const LbaVec3 & position);

	//! hide start path
	void HideStartPath();

	//! hide end path
	void HideEndPath();



	//! select a Projectile
	void SelectProjectile(ProjectileObjectDef* cond, const QModelIndex &parent = QModelIndex());

	//! called when Projectile changed
	void ProjectileChanged(const QModelIndex &parentIdx);

	//! edit selected object
	void EditSelectedObject(ObjectEditType edittype);


	//! add new tempalte object
	void AddNewTemplate(const EditorTemplateObject & templ);


	//! paste spawn
	void PasteSpawn(std::string content);

	//! paste actor
	void PasteActor(std::string content);

	//! paste trigger
	void PasteTrigger(std::string content);

	//! remove template
	void RemoveTemplate(int id);


	//! show hide map info
	void ShowHideMapInfo(bool Show);

	//! select display info
	void SelectDisplayInfo(const LbaNet::ModelInfo &mdisinfo, const QModelIndex &parent = QModelIndex());

	//! select coordinate info
	void SelectHoloCoordinate(const HoloCoordinate &coordinate, const QModelIndex &parent = QModelIndex());

	//! set Holomap in the object
	void SelectHolomap(long id, const QModelIndex &parent = QModelIndex());

	//! set Holomap in the object
	void SelectHolomapLoc(long id, const QModelIndex &parent = QModelIndex());

	//! set Holomap in the object
	void SelectHolomapPath(long id, const QModelIndex &parent = QModelIndex());

	//! called when Holomap object changed
	void HolomapChanged(long id, const QModelIndex &parentIdx);

	//! called when Holomap object changed
	void HolomapLocChanged(long id, const QModelIndex &parentIdx);

	//! called when Holomap object changed
	void HolomapPathChanged(long id, const QModelIndex &parentIdx);

	//! called when DisplayInfoChanged
	void DisplayInfoChanged(const QModelIndex &parentIdx, LbaNet::ModelInfo & dmodel);

	//! select coordinate info
	void HoloCoordinateChanged(HoloCoordinate &coordinate, const QModelIndex &parentIdx);

private:
	Ui::EditorClass										_uieditor;

	Ui::AddTriggerDialog								_uiaddtriggerdialog;
	QDialog *											_addtriggerdialog;

	Ui::DialogOpenWorld									_uiopenworlddialog;
	QDialog *											_openworlddialog;

	Ui::DialogNewSpawning								_uiaddspawningdialog;
	QDialog *											_addspawningdialog;


	Ui::DialogAddMap									_ui_addmapdialog;
	QDialog *											_addmapdialog;

	Ui::DialogAddTeleport								_ui_addtpdialog;
	QDialog *											_addtpdialog;

	Ui::Dialog_NewWorld									_ui_addworlddialog;
	QDialog *											_addworlddialog;

	Ui::DialogAddActor									_ui_addactordialog;
	QDialog *											_addactordialog;

	Ui::DialogAddText									_ui_addtextdialog;
	QDialog *											_addtextdialog;

	Ui::DialogAddItem									_ui_additemdialog;
	QDialog *											_additemdialog;

	Ui::DialogAddStartItem								_ui_addstartitemdialog;
	QDialog *											_addstartitemdialog;

	Ui::NoticeDialog									_ui_NoticeDialog;
	QDialog *											_NoticeDialogdialog;

	Ui::NavMeshOptionD									_ui_NavMeshDialog;
	QDialog *											_NavMeshdialog;

	Ui::Dialog_addtemplate								_ui_addtemplateDialog;
	QDialog *											_addtemplatedialog;

	Ui::EditTemplatesDialog								_ui_edittemplateDialog;
	QDialog *											_edittemplatedialog;

	Ui::DialogLuaConsole								_ui_luaconsoledialog;
	QDialog *											_luaconsoledialog;

	StringTableModel *									_maplistmodel;
	StringTableModel *									_tplistmodel;
	StringTableModel *									_mapspawninglistmodel;
	StringTableModel *									_triggerlistmodel;
	StringTableModel *									_actorlistmodel;

	StringTableModel *									_text_maplistmodel;
	StringTableModel *									_text_questlistmodel;
	StringTableModel *									_text_inventorylistmodel;
	StringTableModel *									_text_namelistmodel;
	Localizer::LocalizeType								_currentchoosentext;
	std::string											_currentchoosenlang;

	StringTableModel *									_itemlistmodel;
	StringTableModel *									_startitemlistmodel;

	StringTableModel *									_questlistmodel;
	StringTableModel *									_templatelistmodel;

	StringTableModel *									_holomaplistmodel;
	StringTableModel *									_holomaploclistmodel;
	StringTableModel *									_holomappathlistmodel;

	TreeModel *											_objectmodel;
	CustomDelegate *									_objectcustomdelegate;

	QSignalMapper *										_signalMapper;

	boost::shared_ptr<CustomStringListModel>							_mapNameList;
	boost::shared_ptr<CustomStringListModel>							_triggerNameList;
	std::map<std::string, boost::shared_ptr<CustomStringListModel> >	_mapSpawningList;
	boost::shared_ptr<CustomStringListModel>							_actortypeList;
	boost::shared_ptr<CustomStringListModel>							_actordtypeList;
	boost::shared_ptr<CustomStringListModel>							_actorptypeList;
	boost::shared_ptr<CustomStringListModel>							_cscriptList;
	boost::shared_ptr<CustomStringListModel>							_actormodeList;
	boost::shared_ptr<CustomStringListModel>							_conditiontypeList;
	boost::shared_ptr<CustomStringListModel>							_cscripttypeList;
	boost::shared_ptr<CustomStringListModel>							_actiontypeList;

	boost::shared_ptr<CustomStringListModel>							_actorModelNameList;
	boost::shared_ptr<CustomStringListModel>							_actorModelOutfitList;
	boost::shared_ptr<CustomStringListModel>							_actorModelWeaponList;
	boost::shared_ptr<CustomStringListModel>							_actorModelModeList;

	boost::shared_ptr<CustomStringListModel>							_text_mapNameList;
	boost::shared_ptr<CustomStringListModel>							_text_questNameList;
	boost::shared_ptr<CustomStringListModel>							_text_inventoryNameList;
	boost::shared_ptr<CustomStringListModel>							_text_nameNameList;

	boost::shared_ptr<CustomStringListModel>							_itemNameList;

	boost::shared_ptr<CustomStringListModel>							_consumable_itemlistmodel;
	boost::shared_ptr<CustomStringListModel>							_mount_itemlistmodel;
	boost::shared_ptr<CustomStringListModel>							_special_itemlistmodel;

	boost::shared_ptr<CustomStringListModel>							_actorscriptparttypeList;

	boost::shared_ptr<CustomStringListModel>							_dorropeningtypeList;
	boost::shared_ptr<CustomStringListModel>							_dorropeningdirectionList;

	boost::shared_ptr<CustomStringListModel>							_hurtanimationList;
	boost::shared_ptr<CustomStringListModel>							_iteminformclientList;

	boost::shared_ptr<CustomStringListModel>							_addList;
	boost::shared_ptr<CustomStringListModel>							_removeList;
	boost::shared_ptr<CustomStringListModel>							_materialtypeList;

	boost::shared_ptr<CustomStringListModel>							_weapontypeList;

	boost::shared_ptr<CustomStringListModel>							_particletypeList;

	bool												_modified;
	bool												_mapmodified;
	LbaNet::WorldInformation							_winfo;
	bool												_firstmapofworld;
	bool												_disableModifs;


	boost::shared_ptr<ServerLuaHandler>					_luaH;

	std::map<long, boost::shared_ptr<TriggerBase> >			_triggers;
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >	_Actors;
	std::map<long, boost::shared_ptr<Spawn> >				_spawns;


	long												_currspawningidx;
	long												_currtriggeridx;
	long												_currteleportidx;
	long												_curractoridx;

	long												_selectedid;


	int													_updatetriggerdialogonnewaction;
	int													_updateactiondialogonnewscript;

	float												_posX;
	float												_posY;
	float												_posZ;



	osg::ref_ptr<osg::MatrixTransform>					_actornode;
	osg::ref_ptr<osg::MatrixTransform>					_arrownode;
	osg::ref_ptr<osg::MatrixTransform>					_scriptnode;

	osg::ref_ptr<osg::MatrixTransform>					_startpathO;
	osg::ref_ptr<osg::MatrixTransform>					_endpathO;

	osg::ref_ptr<osgManipulator::Translate1DDragger>	_draggerX;
	osg::ref_ptr<osgManipulator::Translate1DDragger>	_draggerY;
	osg::ref_ptr<osgManipulator::Translate1DDragger>	_draggerZ;

	boost::shared_ptr<DisplayObjectHandlerBase>			_grid;


	// keep selected items in meemory
	std::map<QModelIndex, void *>						_modelidxdatamap;


	std::map<Ice::Long, ActorObjectInfo >				_editorObjects;


	boost::shared_ptr<NaviMeshHandler>					_navimesh;

	bool												_pickfindpathstarted;
	bool												_startpathpicked;
	LbaVec3												_startpath;


	std::map<int, EditorTemplateObject>					_objecttemplates;

};

#endif // EDITORHANDLER_H
