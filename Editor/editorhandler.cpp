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

#include "editorhandler.h"
#include "EventsQueue.h"
#include "ClientExtendedEvents.h"
#include "SynchronizedTimeHandler.h"
#include "DataLoader.h"
#include "SharedDataHandler.h"
#include "ServerLuaHandler.h"
#include "Triggers.h"
#include "StringHelperFuncs.h"
#include "ClientScript.h"

#include <QMessageBox>
#include <boost/filesystem.hpp>

#include <fstream>
#include <math.h>

#include "OSGHandler.h"
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osgUtil/SmoothingVisitor>
#include <osg/LineWidth>
#include <osg/PolygonMode>
#include <osgManipulator/TranslateAxisDragger>
#include <osgManipulator/Command>



class DraggerCustomCallback : public osgManipulator::DraggerCallback
{
public:
	//! constructor 
	DraggerCustomCallback(int pickedarrow)
		: _pickedarrow(pickedarrow)
	{}


	virtual bool receive(const osgManipulator::MotionCommand& command) 
	{ 
		EventsQueue::getReceiverQueue()->AddEvent(new PickedArrowMovedEvent(_pickedarrow));		
		return true; 
	}


private:
	int					_pickedarrow;
};




/***********************************************************
add data to the list
***********************************************************/
void CustomStringListModel::AddData(const QString & Data)
{
	int row = rowCount();
	insertRows(row, 1);
	setData(createIndex(row, 0), Data);
}


/***********************************************************
clear the list
***********************************************************/	
void CustomStringListModel::Clear()
{
	setStringList(QStringList());
}


/***********************************************************
remove data of the list if exist
***********************************************************/
void CustomStringListModel::RemoveData(const QString & Data)
{
	int idx = stringList().indexOf(Data);
	if(idx >= 0)
		removeRows(idx, 1);
}


/***********************************************************
remove data of the list if exist
***********************************************************/
void CustomStringListModel::ReplaceData(const QString & OldData, const QString & NewData)
{
	int idx = stringList().indexOf(OldData);
	if(idx >= 0)
		setData(createIndex(idx, 0), NewData);
}





/***********************************************************
rowCount
***********************************************************/
int StringTableModel::rowCount(const QModelIndex &parent) const
{
	if(_stringMatrix.size() > 0)
		return _stringMatrix[0].size();

	return 0;
}

/***********************************************************
columnCount
***********************************************************/
int StringTableModel::columnCount(const QModelIndex &parent) const
{
	return _stringMatrix.size();
}

/***********************************************************
data
***********************************************************/
QVariant StringTableModel::data(const QModelIndex &index, int role) const
{
     if (!index.isValid())
         return QVariant();

     if (index.column() >= _stringMatrix.size())
         return QVariant();

     if (index.row() >= _stringMatrix[0].size())
         return QVariant();

     if (role == Qt::DisplayRole/* || role == Qt::EditRole*/)
         return _stringMatrix[index.column()][index.row()];
     else
         return QVariant();

}

/***********************************************************
headerData
***********************************************************/
QVariant StringTableModel::headerData(int section, Qt::Orientation orientation,
											 int role) const
{
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal)
	 {
		 if(section < _header.size())
			return _header[section];
	 }
	 else
	 {
		if(section < _ids.size())
			return _ids[section];	
	 }

	 return QVariant();
}

/***********************************************************
flags
***********************************************************/
Qt::ItemFlags StringTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index);
}

/***********************************************************
setData
***********************************************************/
bool StringTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole) 
	{
		_stringMatrix[index.column()].replace(index.row(), value.toString());
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

/***********************************************************
get a string
***********************************************************/
QString StringTableModel::GetString(const QModelIndex &index)
{
	if (index.isValid()) 
		return _stringMatrix[index.column()][index.row()];

	return QString();
}

/***********************************************************
get the corresponding id
***********************************************************/
long StringTableModel::GetId(const QModelIndex &index)
{
	if (index.isValid()) 
		return _ids[index.row()];

	return -1;
}


/***********************************************************
get next free id
***********************************************************/
long StringTableModel::GetNextId()
{
	if(_ids.size() == 0)
		return 1;

	return _ids[_ids.size()-1] + 1;
}


/***********************************************************
insertRows
***********************************************************/
bool StringTableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	beginInsertRows(QModelIndex(), position, position+rows-1);

	for (int row = 0; row < rows; ++row) 
	{
		for(size_t i=0; i<_stringMatrix.size(); ++i)
			_stringMatrix[i].insert(position, "");
	}

	endInsertRows();
	return true;
}

/***********************************************************
removeRows
***********************************************************/
bool StringTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	beginRemoveRows(QModelIndex(), position, position+rows-1);

	for (int row = 0; row < rows; ++row) 
	{
		_ids.erase(_ids.begin()+position);

		for(size_t i=0; i<_stringMatrix.size(); ++i)
			_stringMatrix[i].removeAt(position);
	}

	endRemoveRows();
	return true;
}


/***********************************************************
clear the table content
***********************************************************/
void StringTableModel::Clear()
{
	if(_ids.size() > 0)
	{
		_ids.clear();

		if(_stringMatrix.size() > 0)
		{
			beginRemoveRows(QModelIndex(), 0, _stringMatrix[0].size()-1);

			for(size_t i=0; i<_stringMatrix.size(); ++i)
				_stringMatrix[i].clear();

			endRemoveRows();
		}

		reset();
	}
}


/***********************************************************
add a row
***********************************************************/
void StringTableModel::AddOrUpdateRow(long id, const QStringList &data)
{
	if(_stringMatrix.size() == data.size())
	{
		for(size_t i=0; i< _ids.size(); ++i)
		{
			if(_ids[i] == id)
			{
				// object already existing - update it
				for(size_t j=0; j< data.size(); ++j)
					_stringMatrix[j].replace(i, data[j]);

				QModelIndex index1 = createIndex(i, 0);
				QModelIndex index2 = createIndex(i, data.size()-1);
				emit dataChanged(index1, index2);
				return;
			}
		}

		// object does not exist - insert it
		beginInsertRows(QModelIndex(), _stringMatrix[0].size(), _stringMatrix[0].size());

		for(size_t i=0; i<_stringMatrix.size(); ++i)
			_stringMatrix[i].push_back(data[i]);

		_ids.push_back(id);

		endInsertRows();
	}
}









/***********************************************************
rowCount
***********************************************************/
int MixedTableModel::rowCount(const QModelIndex &parent) const
{
	if(_objMatrix.size() > 0)
		return _objMatrix[0].size();

	return 0;
}

/***********************************************************
columnCount
***********************************************************/
int MixedTableModel::columnCount(const QModelIndex &parent) const
{
	return _objMatrix.size();
}

/***********************************************************
data
***********************************************************/
QVariant MixedTableModel::data(const QModelIndex &index, int role) const
{
     if (!index.isValid())
         return QVariant();

     if (index.column() >= _objMatrix.size())
         return QVariant();

     if (index.row() >= _objMatrix[0].size())
         return QVariant();

     if (role == Qt::DisplayRole || role == Qt::EditRole)
         return _objMatrix[index.column()][index.row()];
     else
         return QVariant();

}

/***********************************************************
headerData
***********************************************************/
QVariant MixedTableModel::headerData(int section, Qt::Orientation orientation,
											 int role) const
{
     if (role != Qt::DisplayRole)
         return QVariant();

     if (orientation == Qt::Horizontal)
	 {
		 if(section < _header.size())
			return _header[section];
	 }

	 return QVariant();
}

/***********************************************************
flags
***********************************************************/
Qt::ItemFlags MixedTableModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	if(index.column() > 0 && index.row() > 2)
		return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
	else
		return QAbstractItemModel::flags(index);
}

/***********************************************************
setData
***********************************************************/
bool MixedTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.isValid() && role == Qt::EditRole) 
	{
		_objMatrix[index.column()][index.row()] = value;
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

/***********************************************************
get a string
***********************************************************/
QVariant MixedTableModel::GetData(const QModelIndex &index)
{
	if (index.isValid()) 
		return _objMatrix[index.column()][index.row()];

	return QVariant();
}

/***********************************************************
get a string
***********************************************************/
QVariant MixedTableModel::GetData(int col, int row)
{
	return _objMatrix[col][row];
}


/***********************************************************
set a string
***********************************************************/
void MixedTableModel::SetData(int col, int row, QVariant v)
{
	_objMatrix[col][row] = v;
	QModelIndex index = createIndex(row, col);
	emit dataChanged(index, index);
}


/***********************************************************
insertRows
***********************************************************/
bool MixedTableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	beginInsertRows(QModelIndex(), position, position+rows-1);

	for (int row = 0; row < rows; ++row) 
	{
		for(size_t i=0; i<_objMatrix.size(); ++i)
			_objMatrix[i].insert(position, "");
	}

	endInsertRows();
	return true;
}

/***********************************************************
removeRows
***********************************************************/
bool MixedTableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	beginRemoveRows(QModelIndex(), position, position+rows-1);

	for (int row = 0; row < rows; ++row) 
	{
		for(size_t i=0; i<_objMatrix.size(); ++i)
			_objMatrix[i].removeAt(position);
	}

	endRemoveRows();
	return true;
}


/***********************************************************
clear the table content
***********************************************************/
void MixedTableModel::Clear()
{
	if(_objMatrix.size() > 0 && _objMatrix[0].size() > 0)
	{
		beginRemoveRows(QModelIndex(), 0, _objMatrix[0].size()-1);

		for(size_t i=0; i<_objMatrix.size(); ++i)
			_objMatrix[i].clear();

		endRemoveRows();
	}

	reset();
}


/***********************************************************
add a row
***********************************************************/
void MixedTableModel::AddRow(const QVariantList &data)
{
	if(_objMatrix.size() == data.size())
	{
		// object does not exist - insert it
		beginInsertRows(QModelIndex(), _objMatrix[0].size(), _objMatrix[0].size());

		for(size_t i=0; i<_objMatrix.size(); ++i)
			_objMatrix[i].push_back(data[i]);

		endInsertRows();
	}
}





/***********************************************************
Constructor
***********************************************************/
CustomDelegate::CustomDelegate(QObject *parent, QAbstractItemModel * model)
     : QStyledItemDelegate(parent), _model(model)
{
}


/***********************************************************
Constructor
***********************************************************/
QWidget *CustomDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
											const QModelIndex &index) const
{
	std::map<QModelIndex, boost::shared_ptr<CustomStringListModel> >::const_iterator it = _customs.find(index);
	if(it != _customs.end())
	{
		QComboBox *editor = new QComboBox(parent);
		editor->setModel(it->second.get());
		editor->setEditable(false);

		connect(editor, SIGNAL(	activated(int)) , this, SLOT(objmodified(int)));
		return editor;
	}

	QVariant data = _model->data(index, Qt::DisplayRole);
	if(data.type() == QVariant::Double)
	{
		QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
		editor->setMinimum(-100000);
		editor->setMaximum(100000);
		editor->setSingleStep(0.1);
		connect(editor, SIGNAL(	valueChanged(double)) , this, SLOT(objmodified2(double)));
		return editor;
	}


	return QStyledItemDelegate::createEditor(parent, option, index);
}


/***********************************************************
Constructor
***********************************************************/
void CustomDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	std::map<QModelIndex, boost::shared_ptr<CustomStringListModel> >::const_iterator it = _customs.find(index);
	if(it != _customs.end())
	{
		 QString value = index.model()->data(index, Qt::DisplayRole).toString();

		 QComboBox *comboBox = static_cast<QComboBox*>(editor);
		 int index = comboBox->findText(value);
		 if(index >= 0)
			comboBox->setCurrentIndex(index);
	}

	QVariant data = _model->data(index, Qt::DisplayRole);
	if(data.type() == QVariant::Double)
	{
		 double value = index.model()->data(index, Qt::DisplayRole).toDouble();

		 QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		 spinBox->setValue(value);
	}



	QStyledItemDelegate::setEditorData(editor, index);
}


/***********************************************************
Constructor
***********************************************************/
void CustomDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                       const QModelIndex &index) const
{
	std::map<QModelIndex, boost::shared_ptr<CustomStringListModel> >::const_iterator it = _customs.find(index);
	if(it != _customs.end())
	{
		 QComboBox *comboBox = static_cast<QComboBox*>(editor);
		 QString value = comboBox->currentText();
		 model->setData(index, value);
	}

	QVariant data = _model->data(index, Qt::DisplayRole);
	if(data.type() == QVariant::Double)
	{
		 QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
		 spinBox->interpretText();
		 double value = spinBox->value();

		 model->setData(index, value);
	}
	
	QStyledItemDelegate::setModelData(editor, model, index);
}


/***********************************************************
Constructor
***********************************************************/
void CustomDelegate::updateEditorGeometry(QWidget *editor,
         const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QStyledItemDelegate::updateEditorGeometry(editor, option, index);
}



/***********************************************************
Constructor
***********************************************************/
void CustomDelegate::Clear()
{
	_customs.clear();
}

/***********************************************************
Constructor
***********************************************************/
void CustomDelegate::SetCustomIndex(QModelIndex index, boost::shared_ptr<CustomStringListModel> list)
{
	_customs[index] = list;
}


/***********************************************************
data changed in object view
***********************************************************/
void CustomDelegate::objmodified(int flag)
{
	QWidget *editor = static_cast<QWidget *>(sender());
	emit commitData(editor);
}


/***********************************************************
data changed in object view
***********************************************************/
void CustomDelegate::objmodified2(double flag)
{
	QWidget *editor = static_cast<QWidget *>(sender());
	emit commitData(editor);
}







/***********************************************************
Constructor
***********************************************************/
EditorHandler::EditorHandler(QWidget *parent, Qt::WindowFlags flags)
 : QMainWindow(parent, flags), _modified(false), 
 _actionNameList(new CustomStringListModel()), _mapNameList(new CustomStringListModel()),
 _curractionidx(0), _currspawningidx(0), _currtriggeridx(0),
	_updatetriggerdialogonnewaction(-1), _triggerNameList(new CustomStringListModel()),
	_actortypeList(new CustomStringListModel()), _actordtypeList(new CustomStringListModel()),
	_actorptypeList(new CustomStringListModel()), _cscriptList(new CustomStringListModel()),
	_updateactiondialogonnewscript(-1), _actormodeList(new CustomStringListModel())

{
	QStringList actlist;
	actlist << "Static" << "Scripted" << "Movable";
	_actortypeList->setStringList(actlist);
	QStringList acttypelist;
	acttypelist << "Osg Model" << "Sprite" << "Lba1 Model" << "Lba2 Model";
	_actordtypeList->setStringList(acttypelist);
	QStringList actptypelist;
	actptypelist << "No Shape" << "Box" << "Capsule" << "Sphere" << "Triangle Mesh";
	_actorptypeList->setStringList(actptypelist);
	QStringList actmodelist;
	actmodelist << "None" << "Normal" << "Sport" << "Angry" << "Discrete" << "Protopack" << "Horse" << "Dinofly";
	_actormodeList->setStringList(actmodelist);


	_uieditor.setupUi(this);

	_addtriggerdialog = new QDialog(this);
	_uiaddtriggerdialog.setupUi(_addtriggerdialog);

	_openworlddialog = new QDialog(this);
	_uiopenworlddialog.setupUi(_openworlddialog);

	_addspawningdialog = new QDialog(this);
	_uiaddspawningdialog.setupUi(_addspawningdialog);

	_addactiondialog = new QDialog(this);
	_ui_addactiondialog.setupUi(_addactiondialog);

	_addmapdialog = new QDialog(this);
	_ui_addmapdialog.setupUi(_addmapdialog);

	_addtpdialog = new QDialog(this);
	_ui_addtpdialog.setupUi(_addtpdialog);

	_addworlddialog = new QDialog(this);
	_ui_addworlddialog.setupUi(_addworlddialog);

	_addactordialog = new QDialog(this);
	_ui_addactordialog.setupUi(_addactordialog);

	_addcscriptdialog = new QDialog(this);
	_ui_addcscriptdialog.setupUi(_addcscriptdialog);


	// read the file and get data
	std::ifstream file((Lba1ModelDataPath+"lba1_model_animation.csv").c_str());
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

			_lba1Mdata[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].modelnumber = atoi(tokens[4].c_str());
				_lba1Mdata[tokens[0]].outfits[tokens[1]].weapons[tokens[2]].modes[tokens[3]].bodynumber = atoi(tokens[5].c_str());
		}
	}


	


	// set model for map list
	{
		 QStringList header;
		 header << "Name" << "Description";
		_maplistmodel = new StringTableModel(header);
		_uieditor.tableView_MapList->setModel(_maplistmodel);
		QHeaderView * mpheaders = _uieditor.tableView_MapList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}

	// set model for tp list
	{
		 QStringList header;
		 header << "Name" << "Map" << "Spawn";
		_tplistmodel = new StringTableModel(header);
		_uieditor.tableView_TeleportList->setModel(_tplistmodel);
		QHeaderView * mpheaders = _uieditor.tableView_TeleportList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}

	// set model for spawninglist
	{
		 QStringList header;
		 header << "Name" << "X" << "Y" << "Z";
		_mapspawninglistmodel = new StringTableModel(header);
		_uieditor.tableView_SpawningList->setModel(_mapspawninglistmodel);
		QHeaderView * mpheaders = _uieditor.tableView_SpawningList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}

	// set model for objectmap
	{
		 QVector<QVariant> header;
		 header << "Property" << "Value";
		_objectmodel = new TreeModel(header);
		_uieditor.treeView_object->setModel(_objectmodel);
		QHeaderView * mpheaders = _uieditor.treeView_object->header();
		mpheaders->setResizeMode(QHeaderView::Stretch);

		_objectcustomdelegate = new CustomDelegate(0, _objectmodel);
		_uieditor.treeView_object->setItemDelegate(_objectcustomdelegate);

		_uieditor.treeView_object->setEditTriggers(QAbstractItemView::CurrentChanged | 
													QAbstractItemView::DoubleClicked | 
													QAbstractItemView::SelectedClicked);
	}


	// set model for actionlist
	{
		 QStringList header;
		 header << "Name" << "Type";
		_actionlistmodel = new StringTableModel(header);
		_uieditor.tableViewActionList->setModel(_actionlistmodel);
		QHeaderView * mpheaders = _uieditor.tableViewActionList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}
	


	// set model for triggerlist
	{
		 QStringList header;
		 header << "Name" << "Type";
		_triggerlistmodel = new StringTableModel(header);
		_uieditor.tableViewTriggerList->setModel(_triggerlistmodel);
		QHeaderView * mpheaders = _uieditor.tableViewTriggerList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}
		
	
	// set model for spawninglist
	{
		 QStringList header;
		 header << "Name" << "X" << "Y" << "Z";
		_mapspawninglistmodel = new StringTableModel(header);
		_uieditor.tableView_SpawningList->setModel(_mapspawninglistmodel);
		QHeaderView * mpheaders = _uieditor.tableView_SpawningList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}


	// set model for actorlist
	{
		 QStringList header;
		 header << "Name" << "Type" << "DisplayType" << "PhysicalType";
		_actorlistmodel = new StringTableModel(header);
		_uieditor.tableView_ActorList->setModel(_actorlistmodel);
		QHeaderView * mpheaders = _uieditor.tableView_ActorList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}
	

	// set model for client script
	{
		 QStringList header;
		 header << "Name" << "Type";
		_cscriptlistmodel = new StringTableModel(header);
		_uieditor.tableViewCScriptsList->setModel(_cscriptlistmodel);
		QHeaderView * mpheaders = _uieditor.tableViewCScriptsList->horizontalHeader();
		mpheaders->setResizeMode(QHeaderView::Stretch);
	}
	




	// reset world info
	ResetWorld();


	// show myself
	showMaximized();

	// connect signals and slots
	connect(_uieditor.actionQuit, SIGNAL(triggered()), this, SLOT(quit())); 
	connect(_uieditor.actionNew_World, SIGNAL(triggered()), this, SLOT(NewWorldAction())); 
	connect(_uieditor.actionLoad_World, SIGNAL(triggered()), this, SLOT(OpenWorldAction())); 
	connect(_uieditor.actionSave, SIGNAL(triggered()), this, SLOT(SaveWorldAction())); 
	connect(_uieditor.pushButton_info_go, SIGNAL(clicked()) , this, SLOT(info_go_clicked()));
	connect(_uieditor.radioButton_info_player, SIGNAL(toggled(bool)) , this, SLOT(info_camera_toggled(bool)));

	connect(_uieditor.pushButton_addTrigger, SIGNAL(clicked()) , this, SLOT(addtrigger_button_clicked()));
	connect(_uieditor.pushButton_removeTrigger, SIGNAL(clicked()) , this, SLOT(removetrigger_button_clicked()));	
	connect(_uieditor.pushButton_selectTrigger, SIGNAL(clicked()) , this, SLOT(selecttrigger_button_clicked()));		

	connect(_uieditor.pushButton_addSpwaning, SIGNAL(clicked()) , this, SLOT(addspawning_button_clicked()));
	connect(_uieditor.pushButton_removeSpwaning, SIGNAL(clicked()) , this, SLOT(removespawning_button_clicked()));	
	connect(_uieditor.pushButton_selectSpwaning, SIGNAL(clicked()) , this, SLOT(selectspawning_button_clicked()));		

	connect(_uieditor.pushButton_addAction, SIGNAL(clicked()) , this, SLOT(addAction_button_clicked()));
	connect(_uieditor.pushButton_removeAction, SIGNAL(clicked()) , this, SLOT(removeAction_button_clicked()));	
	connect(_uieditor.pushButton_selectAction, SIGNAL(clicked()) , this, SLOT(selectAction_button_clicked()));	

	connect(_uieditor.pushButton_addmap, SIGNAL(clicked()) , this, SLOT(addmap_button_clicked()));
	connect(_uieditor.pushButton_removemap, SIGNAL(clicked()) , this, SLOT(removemap_button_clicked()));	

	connect(_uieditor.pushButton_addtp, SIGNAL(clicked()) , this, SLOT(TpAdd_button()));
	connect(_uieditor.pushButton_removetp, SIGNAL(clicked()) , this, SLOT(TpRemove_button()));	
	connect(_uieditor.pushButton_edittp, SIGNAL(clicked()) , this, SLOT(TpEdit_button()));	


	connect(_uieditor.pushButton_addActor, SIGNAL(clicked()) , this, SLOT(ActorAdd_button()));
	connect(_uieditor.pushButton_removeActor, SIGNAL(clicked()) , this, SLOT(ActorRemove_button()));	
	connect(_uieditor.pushButton_selectActor, SIGNAL(clicked()) , this, SLOT(ActorSelect_button()));


	connect(_uieditor.pushButton_addCScript, SIGNAL(clicked()) , this, SLOT(addcscript_button_clicked()));
	connect(_uieditor.pushButton_removeCScript, SIGNAL(clicked()) , this, SLOT(removecscript_button_clicked()));	
	connect(_uieditor.pushButton_selectCScript, SIGNAL(clicked()) , this, SLOT(selectcscript_button_clicked()));



	//! Actor add button push
	 void ActorAdd_button_accepted();


	connect(_uieditor.textEdit_worlddescription, SIGNAL(textChanged()) , this, SLOT(WorldDescriptionChanged()));	
	connect(_uieditor.textEdit_worldnews, SIGNAL(textChanged()) , this, SLOT(WorldNewsChanged()));	
	

	connect(_uiaddtriggerdialog.pushButton_addact1, SIGNAL(clicked()) , this, SLOT(addAction1_button_clicked()));
	connect(_uiaddtriggerdialog.pushButton_addact2, SIGNAL(clicked()) , this, SLOT(addAction2_button_clicked()));
	connect(_uiaddtriggerdialog.pushButton_addact3, SIGNAL(clicked()) , this, SLOT(addAction3_button_clicked()));
	
	connect(_uieditor.pushButton_goto_tp, SIGNAL(clicked()) , this, SLOT(goto_tp_button_clicked()));
	connect(_uieditor.pushButton_goto_map, SIGNAL(clicked()) , this, SLOT(goto_map_button_clicked()));

	connect(_uieditor.tableView_TeleportList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(goto_tp_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableView_MapList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(goto_map_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableView_SpawningList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(selectspawning_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableViewActionList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(selectaction_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableViewTriggerList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(selecttrigger_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableView_ActorList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(selectactor_double_clicked(const QModelIndex&)));
	connect(_uieditor.tableViewCScriptsList, SIGNAL(doubleClicked(const QModelIndex&)) , this, SLOT(selectcscript_double_clicked(const QModelIndex&)));



	connect(_objectmodel, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)) , 
								this, SLOT(objectdatachanged(const QModelIndex &, const QModelIndex &)));


	connect(_uiaddtriggerdialog.buttonBox, SIGNAL(accepted()) , this, SLOT(addtrigger_accepted()));
	connect(_uiopenworlddialog.buttonBox, SIGNAL(accepted()) , this, SLOT(OpenWorld()));
	connect(_uiaddspawningdialog.buttonBox, SIGNAL(accepted()) , this, SLOT(AddSpawning_clicked()));
	connect(_ui_addactiondialog.buttonBox, SIGNAL(accepted()) , this, SLOT(AddActionAccepted()));
	connect(_ui_addmapdialog.buttonBox, SIGNAL(accepted()) , this, SLOT(addmap_accepted()));
	connect(_ui_addtpdialog.buttonBox, SIGNAL(accepted()) , this, SLOT(TpAdd_button_accepted()));
	connect(_ui_addworlddialog.buttonBox, SIGNAL(accepted()) , this, SLOT(addworld_accepted()));
	connect(_ui_addactordialog.buttonBox, SIGNAL(accepted()) , this, SLOT(ActorAdd_button_accepted()));
	connect(_ui_addcscriptdialog.buttonBox, SIGNAL(accepted()) , this, SLOT(addcscript_accepted()));


	connect(_uieditor.comboBox_startingmap, SIGNAL(activated(int)) , this, SLOT(StartingMapModified(int)));		
	connect(_uieditor.comboBox_startingspawning, SIGNAL(activated(int)) , this, SLOT(StartingSpawnModified(int)));	
	connect(_uieditor.comboBox_modeloutfit, SIGNAL(activated(int)) , this, SLOT(StartingOutfitModified(int)));		
	connect(_uieditor.comboBox_modelweapon, SIGNAL(activated(int)) , this, SLOT(StartingWeaponModified(int)));	
	connect(_uieditor.comboBox_modelmode, SIGNAL(activated(int)) , this, SLOT(StartingModeModified(int)));		
	connect(_uieditor.spinBox_startinglife, SIGNAL(valueChanged(int)) , this, SLOT(StartingLifeModified(int)));	
	connect(_uieditor.spinBox_startingMana, SIGNAL(valueChanged(int)) , this, SLOT(StartingLifeModified(int)));	
	connect(_uieditor.spinBox_startingarmor, SIGNAL(valueChanged(int)) , this, SLOT(StartingLifeModified(int)));

	connect(_ui_addtpdialog.comboBox_map, SIGNAL(activated(int)) , this, SLOT(TpDialogMapChanged(int)));

	connect(_uieditor.textEdit_map_description, SIGNAL(textChanged()) , this, SLOT(MapDescriptionChanged()));
	connect(_uieditor.comboBox_maptype, SIGNAL(activated(int)) , this, SLOT(MapTypeChanged(int)));
	connect(_uieditor.checkBox_map_instancied, SIGNAL(stateChanged(int)) , this, SLOT(MapInstanceChanged(int)));
	connect(_uieditor.radioButton_camtype_ortho, SIGNAL(toggled(bool)) , this, SLOT(MapCameraTypeChanged(bool)));
	connect(_uieditor.radioButton_camtype_persp, SIGNAL(toggled(bool)) , this, SLOT(MapCameraTypeChanged(bool)));
	connect(_uieditor.radioButton_camtype_3d, SIGNAL(toggled(bool)) , this, SLOT(MapCameraTypeChanged(bool)));

	connect(_ui_addactiondialog.comboBox_actiontype, SIGNAL(activated(int)) , this, SLOT(SetActionDialogType(int)));
	connect(_ui_addactiondialog.comboBox_tp_map, SIGNAL(activated(int)) , this, SLOT(actiond_tpmap_changed(int)));
	connect(_ui_addactiondialog.pushButton_script_add, SIGNAL(clicked()) , this, SLOT(actiond_scriptadd_clicked()));
}

/***********************************************************
Destructor
***********************************************************/
EditorHandler::~EditorHandler()
{
	delete _addtriggerdialog;
}



/***********************************************************
set the osg window
***********************************************************/
void EditorHandler::SetOsgWindow(GraphicsWindowQt *osgwindow)
{
	_osgwindow = osgwindow;
	_uieditor.groupBox_game->layout()->addWidget(_osgwindow->getGraphWidget());
	_osgwindow->getGraphWidget()->show();
}



/***********************************************************
ui button clicked
***********************************************************/
void EditorHandler::addtrigger_button_clicked()
{
	if(!IsMapOpened())
		return;

	//clear data
	_uiaddtriggerdialog.lineEdit_triggername->setText("");
	_uiaddtriggerdialog.comboBox_action1->setModel(_actionNameList.get());
	_uiaddtriggerdialog.comboBox_action2->setModel(_actionNameList.get());
	_uiaddtriggerdialog.comboBox_action3->setModel(_actionNameList.get());
	_addtriggerdialog->show();
}



/***********************************************************
ui button clicked
***********************************************************/
void EditorHandler::removetrigger_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewTriggerList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		// inform server
		long id = _triggerlistmodel->GetId(indexes[0]);
		RemoveTrigger(id);

		// remove row from table
		_triggerlistmodel->removeRows(indexes[0].row(), 1);

		// clear object
		ClearObjectIfSelected("Trigger", id);
	}
}


/***********************************************************
on selecttrigger_double_clicked
***********************************************************/
void EditorHandler::selecttrigger_double_clicked(const QModelIndex & itm)
{
	selecttrigger_button_clicked();
}

/***********************************************************
ui button clicked
***********************************************************/
void EditorHandler::selecttrigger_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewTriggerList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		long id = _triggerlistmodel->GetId(indexes[0]);
		SelectTrigger(id);
	}
}



/***********************************************************
dialog accepted
***********************************************************/
void EditorHandler::addtrigger_accepted()
{
	QString triggername = _uiaddtriggerdialog.lineEdit_triggername->text();

	// user forgot to set a name
	if(triggername == "")
		return;

	if(_triggerNameList->stringList().contains(triggername))
	{
		QMessageBox::warning(this, tr("Name already used"),
			tr("The name you entered for the trigger is already used. Please enter a unique name."),
			QMessageBox::Ok);
		return;
	}

	_addtriggerdialog->hide();



	long id = _currtriggeridx+1;
	TriggerInfo triggerinf;
	triggerinf.id = id;
	triggerinf.CheckNpcs = true;
	triggerinf.CheckPlayers = true;
	triggerinf.CheckMovableObjects = false;
	triggerinf.name = triggername.toAscii().data();

	switch(_uiaddtriggerdialog.comboBox_type->currentIndex())
	{
		case 0: // zone trigger
		{
			boost::shared_ptr<TriggerBase> trig(new ZoneTrigger(triggerinf, 1, 1, 1, true));
			trig->SetPosition(_posX, _posY, _posZ);
			trig->SetAction1(GetActionId(_uiaddtriggerdialog.comboBox_action1->currentText().toAscii().data()));
			trig->SetAction2(GetActionId(_uiaddtriggerdialog.comboBox_action2->currentText().toAscii().data()));
			trig->SetAction3(GetActionId(_uiaddtriggerdialog.comboBox_action3->currentText().toAscii().data()));
			AddNewTrigger(trig);
		}
		break;

		case 1: // activation trigger
		{
			boost::shared_ptr<TriggerBase> trig(new ActivationTrigger(triggerinf, 1.5, "Normal", "None"));
			trig->SetPosition(_posX, _posY, _posZ);
			trig->SetAction1(GetActionId(_uiaddtriggerdialog.comboBox_action1->currentText().toAscii().data()));
			trig->SetAction2(GetActionId(_uiaddtriggerdialog.comboBox_action2->currentText().toAscii().data()));
			trig->SetAction3(GetActionId(_uiaddtriggerdialog.comboBox_action3->currentText().toAscii().data()));
			AddNewTrigger(trig);
		}
		break;

		case 2: // ZoneActionTrigger
		{
			boost::shared_ptr<TriggerBase> trig(new ZoneActionTrigger(triggerinf, 1, 1, 1, "Normal", "None"));
			trig->SetPosition(_posX, _posY, _posZ);
			trig->SetAction1(GetActionId(_uiaddtriggerdialog.comboBox_action1->currentText().toAscii().data()));
			trig->SetAction2(GetActionId(_uiaddtriggerdialog.comboBox_action2->currentText().toAscii().data()));
			trig->SetAction3(GetActionId(_uiaddtriggerdialog.comboBox_action3->currentText().toAscii().data()));
			AddNewTrigger(trig);
		}
		break;

		case 3: // TimerTrigger
		{
			boost::shared_ptr<TriggerBase> trig(new TimerTrigger(triggerinf, 1000));
			trig->SetPosition(_posX, _posY, _posZ);
			trig->SetAction1(GetActionId(_uiaddtriggerdialog.comboBox_action1->currentText().toAscii().data()));
			trig->SetAction2(GetActionId(_uiaddtriggerdialog.comboBox_action2->currentText().toAscii().data()));
			trig->SetAction3(GetActionId(_uiaddtriggerdialog.comboBox_action3->currentText().toAscii().data()));
			AddNewTrigger(trig);
		}
		break;

	}

	SelectTrigger(id);
}




/***********************************************************
override close event
***********************************************************/
void EditorHandler::closeEvent(QCloseEvent* event)
{
	if(SaveBeforeQuit())
	{
		EventsQueue::getReceiverQueue()->AddEvent(new QuitGameEvent());
	}
	else
	{
		event->ignore();
	}
}


/***********************************************************
refresh display
***********************************************************/
void EditorHandler::RefreshDisplay()
{
	_uieditor.groupBox_game->setMinimumHeight(600);
}

/***********************************************************
quit editor
***********************************************************/
void EditorHandler::quit()
{
	if(SaveBeforeQuit())
	{
		EventsQueue::getReceiverQueue()->AddEvent(new QuitGameEvent());
	}
}


/***********************************************************
NewWorldAction
***********************************************************/
void EditorHandler::NewWorldAction()
{
	if(SaveBeforeQuit())
	{
		_ui_addworlddialog.lineEdit_worldname->setText("");
		_addworlddialog->show();
	}
}


/***********************************************************
OpenWorldAction
***********************************************************/
void EditorHandler::OpenWorldAction()
{
	if(SaveBeforeQuit())
	{
		std::vector<LbaNet::WorldDesc> list;
		DataLoader::getInstance()->GetAvailableWorlds(list);

		_uiopenworlddialog.comboBoxWorldToOpen->clear();

		for(size_t i=0; i<list.size(); ++i)
			_uiopenworlddialog.comboBoxWorldToOpen->addItem(list[i].WorldName.c_str());

		_openworlddialog->show();
	}
}


/***********************************************************
SaveWorldAction
***********************************************************/
void EditorHandler::SaveWorldAction()
{
	if(!IsWorldOpened())
		return;

	if(_modified)
	{
		// save xml file
		DataLoader::getInstance()->SaveWorldInformation(_winfo.Description.WorldName, _winfo);

		// save global lua file
		SaveGlobalLua("./Data/Worlds/" + _winfo.Description.WorldName + "/Lua/global_server.lua");

		// save current map
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		if(mapname != "")
			SaveMap("./Data/Worlds/" + _winfo.Description.WorldName + "/Lua/" + mapname + "_server.lua");

		// save editor file
		SaveEditorLua("./Data/Worlds/" + _winfo.Description.WorldName + "/Lua/global_editor.lua");

		// save client file
		SaveGlobalClientLua("./Data/Worlds/" + _winfo.Description.WorldName + "/Lua/global_client.lua");


		SetSaved();
	}
}



/***********************************************************
Check if we need to save before quit
***********************************************************/
bool EditorHandler::SaveBeforeQuit()
{
	if(Modified())
	{
		int r = QMessageBox::warning(this, tr("Save changes?"),
		tr("Do you want to save your changes before quitting?"),
		QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

		if(r == QMessageBox::Yes)
		{
			SaveWorldAction();
		}
		else if (r == QMessageBox::Cancel)
		{
			return false;
		}
	}

	return true;
}


/***********************************************************
Check if we need to save before quit map
***********************************************************/
bool EditorHandler::SaveMapBeforeQuit(bool alreadychangedmap)
{
	if(_mapmodified)
	{
		int r = 0;
		if(alreadychangedmap)
		{
			r = QMessageBox::warning(this, tr("Save changes?"),
			tr("Do you want to save your changes for the current map? Else all changes will be lost."),
			QMessageBox::Yes, QMessageBox::No);
		}
		else
		{
			r = QMessageBox::warning(this, tr("Save changes?"),
			tr("Do you want to save your changes for the current map? Else all changes will be lost."),
			QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		}

		if(r == QMessageBox::Yes)
		{
			SaveWorldAction();
		}
		else if (r == QMessageBox::No)
		{
			_mapmodified = false;
		}
		else if (r == QMessageBox::Cancel)
		{
			return false;
		}
	}

	return true;
}



/***********************************************************
return true if some changes are not saved
***********************************************************/
bool EditorHandler::Modified()
{
	return _modified;
}


/***********************************************************
set the work as modified
***********************************************************/
void EditorHandler::SetModified()
{
	if(!_modified)
	{
		setWindowTitle(windowTitle () + " *");
		_modified = true;
	}
}

/***********************************************************
set the work as saved
***********************************************************/
void EditorHandler::SetSaved()
{
	if(_modified)
	{
		setWindowTitle(windowTitle().remove(windowTitle().size()-2, 2));
		_modified = false;
	}

	_mapmodified = false;
}


/***********************************************************
set the work as modified
***********************************************************/
void EditorHandler::SetMapModified()
{
	SetModified();
	_mapmodified = true;
}



/***********************************************************
on open world
***********************************************************/
void EditorHandler::OpenWorld()
{
	std::string choosenworld = _uiopenworlddialog.comboBoxWorldToOpen->currentText().toAscii().data();
	EventsQueue::getReceiverQueue()->AddEvent(new ChangeWorldEvent(choosenworld));

	ResetWorld();
	SetWorldInfo(choosenworld);

	_openworlddialog->hide();
	SetSaved();
}


/***********************************************************
refresh gui with world info
***********************************************************/
void EditorHandler::SetWorldInfo(const std::string & worldname)
{
	LbaNet::WorldInformation tmpinfo;
	DataLoader::getInstance()->GetWorldInformation(worldname, tmpinfo);
	_winfo = tmpinfo;

	setWindowTitle(("LBANet Editor - " + worldname).c_str());

	_uieditor.label_worldname->setText(_winfo.Description.WorldName.c_str());

	QString descs = QString::fromUtf8(_winfo.Description.Description.c_str());
	descs.replace(QString(" @ "), QString("\n"));
	_uieditor.textEdit_worlddescription->setText(descs);

	QString newss = QString::fromUtf8(_winfo.Description.News.c_str());
	newss.replace(QString(" @ "), QString("\n"));
	_uieditor.textEdit_worldnews->setText(newss);


	// add maps
	{
		_maplistmodel->Clear();
		LbaNet::MapsSeq::const_iterator itm = _winfo.Maps.begin();
		LbaNet::MapsSeq::const_iterator endm = _winfo.Maps.end();
		for(long cc=1; itm != endm; ++itm, ++cc)
		{
			QStringList data;
			data << itm->first.c_str() << itm->second.Description.c_str();
			_maplistmodel->AddOrUpdateRow(cc, data);


			_mapNameList->AddData(itm->first.c_str());

			// add spawning to the list
			_mapSpawningList[itm->first] = boost::shared_ptr<CustomStringListModel>(new CustomStringListModel());
			LbaNet::SpawningsSeq::const_iterator itsp = itm->second.Spawnings.begin();
			LbaNet::SpawningsSeq::const_iterator endsp = itm->second.Spawnings.end();
			for(; itsp != endsp; ++itsp)
				_mapSpawningList[itm->first]->AddData(itsp->second.Name.c_str());
		}
	}

	// add teleport
	{
		_tplistmodel->Clear();
		LbaNet::ServerTeleportsSeq::const_iterator ittp = _winfo.TeleportInfo.begin();
		LbaNet::ServerTeleportsSeq::const_iterator endtp = _winfo.TeleportInfo.end();
		for(long cc=1; ittp != endtp; ++ittp, ++cc)
		{
			std::string spawningname;
			LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(ittp->second.MapName);
			if(itmap != _winfo.Maps.end())
			{
				LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.find(ittp->second.SpawningId);
				if(it != itmap->second.Spawnings.end())
					spawningname = it->second.Name;
			}

			QStringList data;
			data << ittp->second.Name.c_str() << ittp->second.MapName.c_str() << spawningname.c_str();
			_tplistmodel->AddOrUpdateRow(ittp->first, data);

			_currteleportidx = ittp->first;
		}
	}



	// add lua stuff
	std::string luafile = "Worlds/" + _winfo.Description.WorldName + "/Lua/";
	std::string globalluafile = luafile + "global_server.lua";
	std::string globaleditorluafile = luafile + "global_editor.lua";
	_luaH = boost::shared_ptr<ServerLuaHandler>(new ServerLuaHandler());
	_luaH->LoadFile(globalluafile);
	_luaH->LoadFile(globaleditorluafile);
	_luaH->CallLua("InitGlobal", this);
	_luaH->CallLua("InitEditor", this);

	// refresh starting info
	RefreshStartingInfo();
}




/***********************************************************
reset world display in editor
***********************************************************/
void EditorHandler::ResetWorld()
{
	_maplistmodel->Clear();
	_tplistmodel->Clear();
	_actionlistmodel->Clear();
	_actionNameList->Clear();
	_actionNameList->AddData("None");
	_mapNameList->Clear();
	_cscriptList->Clear();
	_cscriptlistmodel->Clear();
	_mapSpawningList.clear();

	_uieditor.label_worldname->setText("");
	_uieditor.textEdit_worlddescription->setText("");
	_uieditor.textEdit_worldnews->setText("");

	_curractionidx = 0;
	_currteleportidx = 0;
	_curscriptidx = 0;

	ResetMap();
}


/***********************************************************
reset map display in editor
***********************************************************/
void EditorHandler::ResetMap()
{
	_currspawningidx = 0;
	_currtriggeridx = 0;

	_uieditor.label_mapname->setText("");
	_uieditor.textEdit_map_description->setText("");
	_mapspawninglistmodel->Clear();
	_actorlistmodel->Clear();

	_triggerlistmodel->Clear();
	_triggers.clear();
	_triggerNameList->Clear();
	_Actors.clear();

	ResetObject();
}


/***********************************************************
reset object display in editor
***********************************************************/
void EditorHandler::ResetObject()
{
	_objectcustomdelegate->Clear();
	_objectmodel->Clear();

	RemoveSelectedActorDislay();
	RemoveSelectedScriptDislay();
	RemoveArrows();
}



/***********************************************************
on go tp clicked
***********************************************************/
void EditorHandler::goto_tp_double_clicked(const QModelIndex & itm)
{
	goto_tp_button_clicked();
}


/***********************************************************
on go map clicked
***********************************************************/
void EditorHandler::goto_map_double_clicked(const QModelIndex & itm)
{
	goto_map_button_clicked();
}


/***********************************************************
on go tp clicked
***********************************************************/
void EditorHandler::goto_tp_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_TeleportList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();


	if(indexes.size() > 2)
	{
		std::string mapname = _tplistmodel->GetString(indexes[1]).toAscii().data();
		std::string spawning = _tplistmodel->GetString(indexes[2]).toAscii().data();

		// get id associated to spawning
		long spid = -1;
		if(spawning != "")
		{
			LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(mapname);
			if(itmap != _winfo.Maps.end())
			{
				LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.begin();
				LbaNet::SpawningsSeq::iterator end = itmap->second.Spawnings.end();
				for(;it != end; ++it)
				{
					if(spawning == it->second.Name)
					{
						spid = it->first;
						break;
					}
				}
			}
		}

		if(spid >= 0)
			ChangeMap(mapname, spid);
	}
}

/***********************************************************
on go map clicked
***********************************************************/
void EditorHandler::goto_map_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_MapList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();

	if(indexes.size() > 0)
	{
		std::string mapname = _maplistmodel->GetString(indexes[0]).toAscii().data();
		
		const LbaNet::MapInfo & mapinfo = _winfo.Maps[mapname];
		if(mapinfo.Spawnings.size() > 0)
		{
			ChangeMap(mapname, mapinfo.Spawnings.begin()->first);
		}
		else
		{
			CreateDefaultSpawningAndTp(mapname);
		}
	}
}



/***********************************************************
refresh gui with map info
***********************************************************/
void EditorHandler::SetMapInfo(const std::string & mapname)
{
	SaveMapBeforeQuit(true);

	ResetMap();

	const LbaNet::MapInfo & mapinfo = _winfo.Maps[mapname];

	_uieditor.label_mapname->setText(mapinfo.Name.c_str());


	QString descs = QString::fromUtf8(mapinfo.Description.c_str());
	descs.replace(QString(" @ "), QString("\n"));
	_uieditor.textEdit_map_description->setText(descs);

	switch(mapinfo.AutoCameraType)
	{
		case 1:
			_uieditor.radioButton_camtype_ortho->setChecked(true);
		break;

		case 2:
			_uieditor.radioButton_camtype_persp->setChecked(true);
		break;

		case 3:
			_uieditor.radioButton_camtype_3d->setChecked(true);
		break;
	}

	if(mapinfo.Type == "exterior")
	{
		_uieditor.comboBox_maptype->setCurrentIndex(0);
	}
	else
	{
		_uieditor.comboBox_maptype->setCurrentIndex(1);
	}

	_uieditor.checkBox_map_instancied->setChecked(mapinfo.IsInstance);


	// add the spawnings
	{
		_mapspawninglistmodel->Clear();
		LbaNet::SpawningsSeq::const_iterator ittp = mapinfo.Spawnings.begin();
		LbaNet::SpawningsSeq::const_iterator endtp = mapinfo.Spawnings.end();
		for(; ittp != endtp; ++ittp)
		{
			QString Xs;
			Xs.setNum (ittp->second.PosX, 'f');
			QString Ys;
			Ys.setNum (ittp->second.PosY, 'f');
			QString Zs;
			Zs.setNum (ittp->second.PosZ, 'f');

			QStringList data;
			data << ittp->second.Name.c_str() << Xs << Ys << Zs;
			_mapspawninglistmodel->AddOrUpdateRow(ittp->first, data);

			_currspawningidx = (ittp->first + 1);
		}
	}


	// add lua stuff
	std::string luafile = "Worlds/" + _winfo.Description.WorldName + "/Lua/";
	std::string globalluafile = luafile + "global_server.lua";
	luafile += mapname + "_server.lua";
	_luaH = boost::shared_ptr<ServerLuaHandler>(new ServerLuaHandler());
	_luaH->LoadFile(globalluafile);
	_luaH->LoadFile(luafile);
	_luaH->CallLua("InitMap", this);
}





/***********************************************************
on addspawning clicked
***********************************************************/
void EditorHandler::addspawning_button_clicked()
{
	if(!IsMapOpened())
		return;

	//clear old stuff
	_uiaddspawningdialog.lineEdit_name->setText("");
	_uiaddspawningdialog.doubleSpinBox_posx->setValue(_posX);
	_uiaddspawningdialog.doubleSpinBox_posy->setValue(_posY);
	_uiaddspawningdialog.doubleSpinBox_posz->setValue(_posZ);
	_uiaddspawningdialog.doubleSpinBox_rotation->setValue(0);
	_uiaddspawningdialog.checkBox_forcerotation->setChecked(false);

	_addspawningdialog->show();
}

/***********************************************************
on removespawning clicked
***********************************************************/
void EditorHandler::removespawning_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_SpawningList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 2)
	{
		// inform server
		long spawningid = _mapspawninglistmodel->GetId(indexes[0]);
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		RemoveSpawning(mapname, spawningid);

		// remove row from table
		_mapspawninglistmodel->removeRows(indexes[0].row(), 1);

		RefreshStartingSpawning();
	}
}


/***********************************************************
on selectspawning clicked
***********************************************************/
void EditorHandler::selectspawning_double_clicked(const QModelIndex & itm)
{
	selectspawning_button_clicked();
}

/***********************************************************
on selectspawning clicked
***********************************************************/
void EditorHandler::selectspawning_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_SpawningList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 2)
	{
		long spawningid = _mapspawninglistmodel->GetId(indexes[0]);
		SelectSpawning(spawningid);
	}
}


/***********************************************************
on AddSpawning clicked
***********************************************************/
void EditorHandler::AddSpawning_clicked()
{
	QString spname = _uiaddspawningdialog.lineEdit_name->text();
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();

	// user forgot to set a name
	if(spname == "")
		return;


	if(_mapSpawningList[mapname]->stringList().contains(spname))
	{
		QMessageBox::warning(this, tr("Name already used"),
			tr("The name you entered for the spawning is already used. Please enter a unique name."),
			QMessageBox::Ok);
		return;
	}


	_addspawningdialog->hide();

	long id = AddOrModSpawning(mapname, spname.toAscii().data(),
				(float)_uiaddspawningdialog.doubleSpinBox_posx->value(), 
				(float)_uiaddspawningdialog.doubleSpinBox_posy->value(), 
				(float)_uiaddspawningdialog.doubleSpinBox_posz->value(),
				(float)_uiaddspawningdialog.doubleSpinBox_rotation->value(), 
				_uiaddspawningdialog.checkBox_forcerotation->isChecked());

	// add name to list
	AddSpawningName(mapname, spname.toAscii().data());

	SelectSpawning(id);

	SetModified();
	RefreshStartingSpawning();
}


/***********************************************************
add a spawning to the map
***********************************************************/
long EditorHandler::AddOrModSpawning(const std::string &mapname,
										const std::string &spawningname,
										float PosX, float PosY, float PosZ,
										float Rotation, bool forcedrotation,
										long modifyid)
{
	// first update the internal info
	LbaNet::SpawningInfo spawn;
	spawn.Id = ((modifyid >= 0)?modifyid:_currspawningidx++);
	spawn.Name = spawningname;
	spawn.PosX = PosX;
	spawn.PosY = PosY;
	spawn.PosZ = PosZ;
	spawn.Rotation = Rotation;
	spawn.ForceRotation = forcedrotation;
	_winfo.Maps[mapname].Spawnings[spawn.Id] = spawn;


	//! add spawning to the list
	{
		QString Xs;
		Xs.setNum (PosX, 'f');
		QString Ys;
		Ys.setNum (PosY, 'f');
		QString Zs;
		Zs.setNum (PosZ, 'f');

		QStringList data;
		data << spawningname.c_str() << Xs << Ys << Zs;
		_mapspawninglistmodel->AddOrUpdateRow(spawn.Id, data);
	}


	// then inform the server
	EditorUpdateBasePtr update = new UpdateEditor_AddOrModSpawning(	spawn.Id,
																	spawningname,
																	PosX, PosY, PosZ,
																	Rotation, forcedrotation);

	SharedDataHandler::getInstance()->EditorUpdate(mapname, update);

	return spawn.Id;
}



/***********************************************************
remove a spawning from the map
***********************************************************/
void EditorHandler::RemoveSpawning(const std::string &mapname, long spawningid)
{
	// first update the internal info
	LbaNet::SpawningsSeq::iterator it = _winfo.Maps[mapname].Spawnings.find(spawningid);
	if(it != _winfo.Maps[mapname].Spawnings.end())
	{
		// remove from text list
		RemoveSpawningName(mapname, it->second.Name);

		// erase from data
		_winfo.Maps[mapname].Spawnings.erase(it);

		// then inform the server
		EditorUpdateBasePtr update = new UpdateEditor_RemoveSpawning(spawningid);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);

		ClearObjectIfSelected("Spawn", spawningid);

		SetModified();
	}
}


/***********************************************************
player moved
***********************************************************/
void EditorHandler::PlayerMoved(float posx, float posy, float posz)
{
	_uieditor.doubleSpinBox_info_posx->setValue(posx);
	_uieditor.doubleSpinBox_info_posy->setValue(posy);
	_uieditor.doubleSpinBox_info_posz->setValue(posz);

	_posX = floor(posx*2)/2;
	_posY = floor(posy+0.5);
	_posZ = floor(posz*2)/2;
}



/***********************************************************
when go button clicked on info part
***********************************************************/
void EditorHandler::info_go_clicked()
{
	EventsQueue::getReceiverQueue()->AddEvent(new EditorTeleportEvent(	_uieditor.doubleSpinBox_info_posx->value(),
																		_uieditor.doubleSpinBox_info_posy->value(),
																		_uieditor.doubleSpinBox_info_posz->value()));
}



/***********************************************************
camera type toggled in info
***********************************************************/
void EditorHandler::info_camera_toggled(bool checked)
{
	EventsQueue::getReceiverQueue()->AddEvent(new EditorCameraChangeEvent(!checked));
}



/***********************************************************
set spawning in the object
***********************************************************/
void EditorHandler::SelectSpawning(long id, const QModelIndex &parent)
{
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	LbaNet::SpawningsSeq::const_iterator it = _winfo.Maps[mapname].Spawnings.find(id);
	if(it == _winfo.Maps[mapname].Spawnings.end())
		return;

	std::string spawningname = it->second.Name;
	float PosX = it->second.PosX;
	float PosY = it->second.PosY;
	float PosZ = it->second.PosZ;
	float Rotation = it->second.Rotation;
	bool forcedrotation = it->second.ForceRotation;

	if(parent == QModelIndex())
		ResetObject();


	{
		QVector<QVariant> data;
		data<<"Type"<<"Spawn";
		_objectmodel->AppendRow(data, parent, true);
	}

	{
		QVector<QVariant> data;
		data<<"SubCategory"<<"-";
		_objectmodel->AppendRow(data, parent, true);
	}

	{
		QVector<QVariant> data;
		data<<"Id"<<id;
		_objectmodel->AppendRow(data, parent, true);
	}

	{
		QVector<QVariant> data;
		data<<"Name"<<spawningname.c_str();
		_objectmodel->AppendRow(data, parent);
	}

	{
		QVector<QVariant> data;
		data<<"PosX"<<(double)PosX;
		_objectmodel->AppendRow(data, parent);
	}

	{
		QVector<QVariant> data;
		data<<"PosY"<<(double)PosY;
		_objectmodel->AppendRow(data, parent);
	}

	{
		QVector<QVariant> data;
		data<<"PosZ"<<(double)PosZ;
		_objectmodel->AppendRow(data, parent);
	}

	{
		QVector<QVariant> data;
		data<<"Force Rotation"<<forcedrotation;
		_objectmodel->AppendRow(data, parent);
	}

	{
		QVector<QVariant> data;
		data<<"Rotation"<<(double)Rotation;
		_objectmodel->AppendRow(data, parent);
	}
}


/***********************************************************
clear the object displayed if it is the selected one
***********************************************************/
void EditorHandler::ClearObjectIfSelected(const std::string objecttype, long id)
{
	if(_objectmodel->rowCount() > 2)
	{
		QString type = _objectmodel->data(_objectmodel->GetIndex(1, 0)).toString();
		if(objecttype == type.toAscii().data())
		{
			long objid = _objectmodel->data(_objectmodel->GetIndex(1, 2)).toString().toLong();	
			if(id == objid)
			{
				ResetObject();
			}
		}
	}
}



/***********************************************************
data changed in object view
***********************************************************/
void EditorHandler::objectdatachanged(const QModelIndex &index1, const QModelIndex &index2)
{
	QModelIndex parentIdx = _objectmodel->parent(index1);

	if(_objectmodel->rowCount() > 2)
	{
		QString type = _objectmodel->data(_objectmodel->GetIndex(1, 0, parentIdx)).toString();
		std::string category = _objectmodel->data(_objectmodel->GetIndex(1, 1, parentIdx)).toString().toAscii().data();
		long objid = _objectmodel->data(_objectmodel->GetIndex(1, 2, parentIdx)).toString().toLong();

		if(type == "Spawn")
		{
			SpawningObjectChanged(objid, parentIdx);
			return;
		}

		if(type == "Action")
		{
			ActionObjectChanged(objid, category, parentIdx);
			return;
		}

		if(type == "Trigger")
		{
			TriggerObjectChanged(objid, category, parentIdx);
			return;
		}

		if(type == "Actor")
		{
			ActorObjectChanged(objid, parentIdx);
			return;
		}

		if(type == "ClientScript")
		{
			CScriptObjectChanged(objid, category, parentIdx);
			return;
		}

		
	}
}


/***********************************************************
called when spawning object changed
***********************************************************/
void EditorHandler::SpawningObjectChanged(long id, const QModelIndex &parentIdx)
{
	if(_objectmodel->rowCount() > 8)
	{
		QString name = _objectmodel->data(_objectmodel->GetIndex(1, 3, parentIdx)).toString();
		float PosX = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toFloat();
		float PosY = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float PosZ = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		bool forcedrotation = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toBool();
		float Rotation = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toFloat();
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		std::string oldname = _winfo.Maps[mapname].Spawnings[id].Name;

		// check if name changed already exist
		if(name != oldname.c_str())
		{
			if(name == "" || _mapSpawningList[mapname]->stringList().contains(name))
			{
				QMessageBox::warning(this, tr("Name already used"),
					tr("The name you entered for the spawning is already used. Please enter a unique name."),
					QMessageBox::Ok);

				_objectmodel->setData(_objectmodel->GetIndex(1, 3, parentIdx), oldname.c_str());
				return;
			}
		}


		ReplaceSpawningName(mapname, oldname, name.toAscii().data());


		AddOrModSpawning(mapname, name.toAscii().data(),
									PosX, PosY, PosZ,
									Rotation, forcedrotation,
									id);

		UpdateTpSpanName(mapname, id, name.toAscii().data());

		SetModified();
		RefreshStartingSpawning();
	}
}


/***********************************************************
check if a world is opened
***********************************************************/
bool EditorHandler::IsWorldOpened()
{
	return (_uieditor.label_worldname->text() != "");
}

/***********************************************************
check if a map is opened
***********************************************************/
bool EditorHandler::IsMapOpened()
{
	return (_uieditor.label_mapname->text() != "");
}



/***********************************************************
function used by LUA to add actor
***********************************************************/
void EditorHandler::AddActorObject(boost::shared_ptr<ActorHandler> actor)
{
	_Actors[actor->GetId()] = actor;

	std::string type;
	switch(actor->GetInfo().PhysicDesc.TypePhysO)
	{
		case LbaNet::StaticAType:
			type = "Static";
		break;

		case LbaNet::KynematicAType:
			type = "Scripted";
		break;

		case LbaNet::CharControlAType:
			type = "Movable";
		break;  
	}


	std::string dtype;
	switch(actor->GetInfo().DisplayDesc.TypeRenderer)
	{
		case LbaNet::RenderOsgModel:
			dtype = "Osg Model";
		break;

		case RenderSprite:
			dtype = "Sprite";
		break;

		case RenderLba1M:
			dtype = "Lba1 Model";
		break;

		case RenderLba2M:
			dtype = "Lba2 Model";
		break;

		case RenderCross:
			dtype = "Cross";
		break;

		case RenderBox:
			dtype = "Box";
		break;

		case RenderCapsule:
			dtype = "Capsule";
		break;
	}

	std::string ptype;
	switch(actor->GetInfo().PhysicDesc.TypeShape)
	{
		case LbaNet::NoShape:
			ptype = "No Shape";
		break;
		case BoxShape:
			ptype = "Box";
		break;
		case CapsuleShape:
			ptype = "Capsule";
		break;
		case SphereShape:
			ptype = "Sphere";
		break;
		case TriangleMeshShape:
			ptype = "Triangle Mesh";
		break;
	}

	std::string name = actor->GetInfo().ExtraInfo.Name;
	if(name == "")
		name = "-";

	QStringList data;
	data << name.c_str()<< type.c_str()  << dtype.c_str() << ptype.c_str();
	_actorlistmodel->AddOrUpdateRow(actor->GetId(), data);

	_curractoridx = actor->GetId();
}

/***********************************************************
add a trigger of moving type to the map
***********************************************************/				
void EditorHandler::AddTrigger(boost::shared_ptr<TriggerBase> trigger)
{
	if(trigger)
	{
		_currtriggeridx = trigger->GetId();

		trigger->SetOwner(this);
		_triggers[_currtriggeridx] = trigger;

		QStringList slist;
		slist << trigger->GetName().c_str() << trigger->GetTypeName().c_str();
		_triggerlistmodel->AddOrUpdateRow(_currtriggeridx, slist);

		_triggerNameList->AddData(trigger->GetName().c_str());
	}
}

/***********************************************************
add an action
***********************************************************/				
void EditorHandler::AddAction(boost::shared_ptr<ActionBase> action)
{
	if(action)
	{
		_curractionidx = action->GetId();

		_actions[_curractionidx] = action;

		QStringList slist;
		slist << action->GetName().c_str() << action->GetTypeName().c_str();
		_actionlistmodel->AddOrUpdateRow(_curractionidx, slist);

		AddActionName(action->GetName());
	}
}

/***********************************************************
// teleport an object
// ObjectType ==>
//! 1 -> npc object
//! 2 -> player object
//! 3 -> movable object
***********************************************************/
void EditorHandler::Teleport(int ObjectType, long ObjectId,
								const std::string &NewMapName, 
								long SpawningId,
								float offsetX, float offsetY, float offsetZ)
{
	// nothing to do
}


/***********************************************************
get the action correspondant to the id
***********************************************************/				
boost::shared_ptr<ActionBase> EditorHandler::GetAction(long actionid)
{
	std::map<long, boost::shared_ptr<ActionBase> >::iterator it = _actions.find(actionid);
	if(it != _actions.end())
		return it->second;

	return boost::shared_ptr<ActionBase>();
}



/***********************************************************
add an new action to the list
***********************************************************/	
void EditorHandler::AddNewAction(boost::shared_ptr<ActionBase> action)
{
	if(action)
	{
		// add action internally
		AddAction(action);
		SetModified();

		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModAction(action);
		SharedDataHandler::getInstance()->EditorUpdate("", update);

		// update add trigger dialog if opened
		if(_updatetriggerdialogonnewaction>=0)
		{
			int index1 = _uiaddtriggerdialog.comboBox_action1->currentIndex();
			int index2 = _uiaddtriggerdialog.comboBox_action2->currentIndex();
			int index3 = _uiaddtriggerdialog.comboBox_action3->currentIndex();

			_uiaddtriggerdialog.comboBox_action1->setModel(_actionNameList.get());
			_uiaddtriggerdialog.comboBox_action2->setModel(_actionNameList.get());
			_uiaddtriggerdialog.comboBox_action3->setModel(_actionNameList.get());

			_uiaddtriggerdialog.comboBox_action1->setCurrentIndex(index1);
			_uiaddtriggerdialog.comboBox_action2->setCurrentIndex(index2);
			_uiaddtriggerdialog.comboBox_action3->setCurrentIndex(index3);

			switch(_updatetriggerdialogonnewaction)
			{
				case 1:
				{
					 int index = _uiaddtriggerdialog.comboBox_action1->findText(action->GetName().c_str());
					 if(index >= 0)
						_uiaddtriggerdialog.comboBox_action1->setCurrentIndex(index);
				}
				break;

				case 2:
				{
					 int index = _uiaddtriggerdialog.comboBox_action2->findText(action->GetName().c_str());
					 if(index >= 0)
						_uiaddtriggerdialog.comboBox_action2->setCurrentIndex(index);
				}
				break;

				case 3:
				{
					 int index = _uiaddtriggerdialog.comboBox_action3->findText(action->GetName().c_str());
					 if(index >= 0)
						_uiaddtriggerdialog.comboBox_action3->setCurrentIndex(index);
				}
				break;
			}

			_updatetriggerdialogonnewaction = -1;
		}
	}
}

/***********************************************************
remove an action from the list
***********************************************************/	
void EditorHandler::RemoveAction(long id)
{
	//remove internal action
	std::map<long, boost::shared_ptr<ActionBase> >::iterator it = _actions.find(id);
	if(it != _actions.end())
	{
		RemoveActionName(it->second->GetName());
		_actions.erase(it);
		SetModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_RemoveAction(id);
		SharedDataHandler::getInstance()->EditorUpdate("", update);
	}
}


/***********************************************************
add an action name to the name list
***********************************************************/
void EditorHandler::AddActionName(const std::string & name)
{
	if(!_actionNameList->stringList().contains(name.c_str()))
		_actionNameList->AddData(name.c_str());
}


/***********************************************************
remove an action name to the name list
***********************************************************/
void EditorHandler::RemoveActionName(const std::string & name)
{
	_actionNameList->RemoveData(name.c_str());
}


/***********************************************************
replace an action name to the name list
***********************************************************/
void EditorHandler::ReplaceActionName(const std::string & oldname, const std::string & newname)
{
	_actionNameList->ReplaceData(oldname.c_str(), newname.c_str());
}


/***********************************************************
addAction_button_clicked
***********************************************************/
void EditorHandler::addAction1_button_clicked()
{
	_updatetriggerdialogonnewaction = 1;
	addAction_button_clicked();
}

/***********************************************************
addAction_button_clicked
***********************************************************/
void EditorHandler::addAction2_button_clicked()
{
	_updatetriggerdialogonnewaction = 2;
	addAction_button_clicked();
}

/***********************************************************
addAction_button_clicked
***********************************************************/
void EditorHandler::addAction3_button_clicked()
{
	_updatetriggerdialogonnewaction = 3;
	addAction_button_clicked();
}

/***********************************************************
addAction_button_clicked
***********************************************************/
void EditorHandler::addAction_button_clicked()
{
	if(!IsWorldOpened())
		return;

	// clear data
	_ui_addactiondialog.lineEdit_actionname->setText("");
	_ui_addactiondialog.comboBox_actiontype->setCurrentIndex(0);
	SetActionDialogType(0);

	_addactiondialog->show();
}



/***********************************************************
removeAction_button_clicked
***********************************************************/
void EditorHandler::removeAction_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewActionList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		// inform server
		long id = _actionlistmodel->GetId(indexes[0]);
		RemoveAction(id);

		// remove row from table
		_actionlistmodel->removeRows(indexes[0].row(), 1);

		// clear object
		ClearObjectIfSelected("Action", id);
	}
}


/***********************************************************
selectAction_button_clicked
***********************************************************/
void EditorHandler::selectaction_double_clicked(const QModelIndex & itm)
{
	selectAction_button_clicked();
}

/***********************************************************
selectAction_button_clicked
***********************************************************/
void EditorHandler::selectAction_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewActionList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		long id = _actionlistmodel->GetId(indexes[0]);
		SelectAction(id);
	}
}


/***********************************************************
add action accepted
***********************************************************/
void EditorHandler::AddActionAccepted()
{
	QString actname = _ui_addactiondialog.lineEdit_actionname->text();

	if(actname == "" || actname == "None")
		return;

	if(_actionNameList->stringList().contains(actname))
	{
		QMessageBox::warning(this, tr("Name already used"),
			tr("The name you entered for the action is already used. Please enter a unique name."),
			QMessageBox::Ok);
		return;
	}

	_addactiondialog->hide();
	long id = _curractionidx+1;

	switch(_ui_addactiondialog.comboBox_actiontype->currentIndex())
	{
		case 0: // teleport
		{
			std::string mapname = _ui_addactiondialog.comboBox_tp_map->currentText().toAscii().data();
			std::string spawning = _ui_addactiondialog.comboBox_tp_spawn->currentText().toAscii().data();

			long spid = -1;
			if(spawning != "")
			{
				LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(mapname);
				if(itmap != _winfo.Maps.end())
				{
					LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.begin();
					LbaNet::SpawningsSeq::iterator end = itmap->second.Spawnings.end();
					for(;it != end; ++it)
					{
						if(spawning == it->second.Name)
						{
							spid = it->first;
							break;
						}
					}
				}
			}

			AddNewAction(boost::shared_ptr<ActionBase>
				(new TeleportAction(id,	actname.toAscii().data(), mapname, spid)));
		}
		break;
	
		case 1: // client script
		{
			std::string scriptname = _ui_addactiondialog.comboBox_script_name->currentText().toAscii().data();

			long scid = -1;
			if(scriptname != "")
			{
				std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator it = _cscripts.begin();
				std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator end = _cscripts.end();
				for(;it != end; ++it)
				{
					if(scriptname == it->second->GetName())
					{
						scid = it->first;
						break;
					}
				}
			}

			AddNewAction(boost::shared_ptr<ActionBase>
				(new ClientScriptAction(id,	actname.toAscii().data(), scid)));
		}
		break;
	
		case 2: // custom action
		{
			std::string functionname = _ui_addactiondialog.lineEdit_customaction->text().toAscii().data();

			AddNewAction(boost::shared_ptr<ActionBase>
				(new CustomAction(id,	actname.toAscii().data(), functionname)));
		}
		break;
	
		case 3: // display text action
		{
			long textid = _ui_addactiondialog.spinBox_displaytext->value();

			AddNewAction(boost::shared_ptr<ActionBase>
				(new DisplayTextAction(id,	actname.toAscii().data(), textid)));
		}
		break;
	}

	SelectAction(id);
}

/***********************************************************
select an action and display it in object view
***********************************************************/
void EditorHandler::SelectAction(long id, const QModelIndex &parent)
{
	std::map<long, boost::shared_ptr<ActionBase> >::iterator it = _actions.find(id);
	if(it != _actions.end())
	{
		if(parent == QModelIndex())
			ResetObject();

		{
			QVector<QVariant> data;
			data<<"Type"<<"Action";
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"SubCategory"<<it->second->GetTypeName().c_str();
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Id"<<id;
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Name"<<it->second->GetName().c_str();
			_objectmodel->AppendRow(data, parent);
		}

		std::string actiontype = it->second->GetTypeName();
		if(actiontype == "TeleportAction")
		{
			TeleportAction* ptr = static_cast<TeleportAction*>(it->second.get());
			{
				QVector<QVariant> data;
				data << "NewMap" << ptr->GetMapName().c_str();
				_objectmodel->AppendRow(data, parent);
			}

			{	
				std::string spawningname;
				LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(ptr->GetMapName());
				if(itmap != _winfo.Maps.end())
				{
					LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.find(ptr->GetSpawning());
					if(it != itmap->second.Spawnings.end())
						spawningname = it->second.Name;
				}

				QVector<QVariant> data;
				data << "Spawn" << spawningname.c_str();
				_objectmodel->AppendRow(data, parent);
			}

			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _mapNameList);

			std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator it = 
														_mapSpawningList.find(ptr->GetMapName());
			if(it != _mapSpawningList.end())
				_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 5, parent), it->second);

			return;
		}

		if(actiontype == "ClientScriptAction")
		{
			ClientScriptAction* ptr = static_cast<ClientScriptAction*>(it->second.get());
			{
				std::string csname;
				std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator itsc = _cscripts.find(ptr->GetScriptId());
				if(itsc != _cscripts.end())
					csname = itsc->second->GetName();

				QVector<QVariant> data;
				data << "Script name" << csname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);

				if(csname != "")
					SelectCScript(ptr->GetScriptId(), idx);
			}

			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _cscriptList);

			return;
		}


		if(actiontype == "CustomAction")
		{
			CustomAction* ptr = static_cast<CustomAction*>(it->second.get());
			{
				QVector<QVariant> data;
				data << "Function name" << ptr->GetLuaFunctionName().c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);
			}

			return;
		}

		if(actiontype == "DisplayTextAction")
		{
			DisplayTextAction* ptr = static_cast<DisplayTextAction*>(it->second.get());
			{
				QVector<QVariant> data;
				data << "Text id" << ptr->GetTextId(); //todo - check if need cast to int to display as spinbox
				QModelIndex idx = _objectmodel->AppendRow(data, parent);
			}

			return;
		}

	}
}




/***********************************************************
add an spawning name to the name list
***********************************************************/
void EditorHandler::AddSpawningName(const std::string & mapname, const std::string & name)
{
	if(!_mapSpawningList[mapname]->stringList().contains(name.c_str()))
		_mapSpawningList[mapname]->AddData(name.c_str());
}


/***********************************************************
remove an spawning name to the name list
***********************************************************/
void EditorHandler::RemoveSpawningName(const std::string & mapname, const std::string & name)
{
	_mapSpawningList[mapname]->RemoveData(name.c_str());
}


/***********************************************************
replace an spawning name to the name list
***********************************************************/
void EditorHandler::ReplaceSpawningName(const std::string & mapname, const std::string & oldname, 
															const std::string & newname)
{
	_mapSpawningList[mapname]->ReplaceData(oldname.c_str(), newname.c_str());
}


/***********************************************************
called when action object changed
***********************************************************/
void EditorHandler::ActionObjectChanged(long id, const std::string & category, const QModelIndex &parentIdx)
{
	std::string oldname = _actions[id]->GetName();
	QString name = _objectmodel->data(_objectmodel->GetIndex(1, 3, parentIdx)).toString();

	// check if name changed already exist
	if(name != oldname.c_str())
	{
		if(name == "" || _actionNameList->stringList().contains(name))
		{
			QMessageBox::warning(this, tr("Name already used"),
				tr("The name you entered for the action is already used. Please enter a unique name."),
				QMessageBox::Ok);

			_objectmodel->setData(_objectmodel->GetIndex(1, 3, parentIdx), oldname.c_str());
			return;
		}
	}


	if(category == "TeleportAction")
	{
		// get info
		std::string mapname = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		std::string spawning = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toString().toAscii().data();

		std::string oldmapname = static_cast<TeleportAction*>(_actions[id].get())->GetMapName();

		// get id associated to spawning
		long spid = -1;
		if(spawning != "")
		{
			LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(mapname);
			if(itmap != _winfo.Maps.end())
			{
				LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.begin();
				LbaNet::SpawningsSeq::iterator end = itmap->second.Spawnings.end();
				for(;it != end; ++it)
				{
					if(spawning == it->second.Name)
					{
						spid = it->first;
						break;
					}
				}
			}
		}


		// created modified action and replace old one
		boost::shared_ptr<ActionBase> modifiedact(new TeleportAction(id,
											name.toAscii().data(), mapname, spid));


		_actions[id] = modifiedact;

		if(name != oldname.c_str())
		{
			ReplaceActionName(oldname, modifiedact->GetName());

			// update action name on parent
			if(parentIdx != QModelIndex())
				_objectmodel->setData(_objectmodel->GetIndex(1, parentIdx.row(), 
											_objectmodel->parent(parentIdx)), name);	
		}

		// update action list display
		QStringList slist;
		slist << name << modifiedact->GetTypeName().c_str();
		_actionlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModAction(modifiedact);
		SharedDataHandler::getInstance()->EditorUpdate("", update);

		// refresh spawning list for action
		if(oldmapname != mapname)
		{
			std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator it = 
				_mapSpawningList.find(mapname);
			if(it != _mapSpawningList.end())
			{
				_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 5, parentIdx), it->second);

				QStringList lst = it->second->stringList();
				if(lst.size() > 0)
					_objectmodel->setData(_objectmodel->GetIndex(1, 5, parentIdx), lst[0]);	
			}
		}


		return;
	}


	if(category == "ClientScriptAction")
	{
		// get info
		std::string scriptname = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();

		// get id associated to spawning
		long scid = -1;
		if(scriptname != "")
		{
			std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator it = _cscripts.begin();
			std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator end = _cscripts.end();
			for(;it != end; ++it)
			{
				if(scriptname == it->second->GetName())
				{
					scid = it->first;
					break;
				}
			}
		}


		// created modified action and replace old one
		boost::shared_ptr<ActionBase> modifiedact(new ClientScriptAction(id,
													name.toAscii().data(), scid));


		_actions[id] = modifiedact;

		if(name != oldname.c_str())
		{
			ReplaceActionName(oldname, modifiedact->GetName());

			// update action name on parent
			if(parentIdx != QModelIndex())
				_objectmodel->setData(_objectmodel->GetIndex(1, parentIdx.row(), 
				_objectmodel->parent(parentIdx)), name);	
		}

		// update action list display
		QStringList slist;
		slist << name << modifiedact->GetTypeName().c_str();
		_actionlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
		if(scid >= 0)
			SelectCScript(scid, _objectmodel->GetIndex(0, 4, parentIdx));



		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModAction(modifiedact);
		SharedDataHandler::getInstance()->EditorUpdate("", update);

		return;
	}



	if(category == "CustomAction")
	{
		// get info
		std::string scriptname = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();


		// created modified action and replace old one
		boost::shared_ptr<ActionBase> modifiedact(new CustomAction(id,
													name.toAscii().data(), scriptname));


		_actions[id] = modifiedact;

		if(name != oldname.c_str())
		{
			ReplaceActionName(oldname, modifiedact->GetName());

			// update action name on parent
			if(parentIdx != QModelIndex())
				_objectmodel->setData(_objectmodel->GetIndex(1, parentIdx.row(), 
				_objectmodel->parent(parentIdx)), name);	
		}

		// update action list display
		QStringList slist;
		slist << name << modifiedact->GetTypeName().c_str();
		_actionlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModAction(modifiedact);
		SharedDataHandler::getInstance()->EditorUpdate("", update);

		return;
	}



	if(category == "DisplayTextAction")
	{
		// get info
		long textid = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toLong();


		// created modified action and replace old one
		boost::shared_ptr<ActionBase> modifiedact(new DisplayTextAction(id,
													name.toAscii().data(), textid));


		_actions[id] = modifiedact;

		if(name != oldname.c_str())
		{
			ReplaceActionName(oldname, modifiedact->GetName());

			// update action name on parent
			if(parentIdx != QModelIndex())
				_objectmodel->setData(_objectmodel->GetIndex(1, parentIdx.row(), 
				_objectmodel->parent(parentIdx)), name);	
		}

		// update action list display
		QStringList slist;
		slist << name << modifiedact->GetTypeName().c_str();
		_actionlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModAction(modifiedact);
		SharedDataHandler::getInstance()->EditorUpdate("", update);

		return;
	}

}


/***********************************************************
save global lus file for current world
***********************************************************/
void EditorHandler::SaveGlobalLua(const std::string & filename)
{
	std::ofstream file(filename.c_str());
	file<<"function InitGlobal(environment)"<<std::endl;

	std::map<long, boost::shared_ptr<ActionBase> >::const_iterator it = _actions.begin();
	std::map<long, boost::shared_ptr<ActionBase> >::const_iterator end = _actions.end();
	for(; it != end; ++it)
		it->second->SaveToLuaFile(file);


	file<<"end"<<std::endl;
}


/***********************************************************
add an new trigger to the list
***********************************************************/
void EditorHandler::AddNewTrigger(boost::shared_ptr<TriggerBase> trigger)
{
	if(trigger)
	{
		// add action internally
		AddTrigger(trigger);
		SetMapModified();

		//inform server
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModTrigger(trigger);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);
	}
}

/***********************************************************
get action id from name
***********************************************************/
long EditorHandler::GetActionId(const std::string & name)
{
	long id = -1;
	if(name != "" && name != "None")
	{
		std::map<long, boost::shared_ptr<ActionBase> >::iterator it = _actions.begin();
		std::map<long, boost::shared_ptr<ActionBase> >::iterator end = _actions.end();
		for(;it != end; ++it)
		{
			if(name == it->second->GetName())
			{
				id = it->first;
				break;
			}
		}
	}

	return id;
}



/***********************************************************
remove trigger
***********************************************************/
void EditorHandler::RemoveTrigger(long id)
{
	//remove internal trigger
	std::map<long, boost::shared_ptr<TriggerBase> >::iterator it = _triggers.find(id);
	if(it != _triggers.end())
	{
		_triggers.erase(it);
		SetMapModified();

		//inform server
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		EditorUpdateBasePtr update = new UpdateEditor_RemoveTrigger(id);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);
	}
}


/***********************************************************
select trigger
***********************************************************/
void EditorHandler::SelectTrigger(long id, const QModelIndex &parent)
{
	std::map<long, boost::shared_ptr<TriggerBase> >::iterator it = _triggers.find(id);
	if(it != _triggers.end())
	{
		if(parent == QModelIndex())
			ResetObject();

		{
			QVector<QVariant> data;
			data<<"Type"<<"Trigger";
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"SubCategory"<<it->second->GetTypeName().c_str();
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Id"<<id;
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Name"<<it->second->GetName().c_str();
			_objectmodel->AppendRow(data, parent);
		}

		std::string actiontype = it->second->GetTypeName();
		if(actiontype == "ZoneTrigger")
		{
			ZoneTrigger* ptr = static_cast<ZoneTrigger*>(it->second.get());
			{
				std::string actname = GetActionName(ptr->GetAction1());
				QVector<QVariant> data;
				data << "Action On Enter" << actname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);
				
				if(actname != "" && actname != "None")
					SelectAction(ptr->GetAction1(), idx);
			}
			{
				std::string actname = GetActionName(ptr->GetAction2());
				QVector<QVariant> data;
				data << "Action On Leave" << actname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);
				
				if(actname != "" && actname != "None")
					SelectAction(ptr->GetAction1(), idx);
			}

			{
				QVector<QVariant> data;
				data << "Trigger on Player" << ptr->CheckPlayer();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Trigger on Npc" << ptr->CheckNpcs();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Trigger on Movable Objects" << ptr->CheckMovableObjects();
				_objectmodel->AppendRow(data, parent);
			}


			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetPosX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetPosY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetPosZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size X" << (double)ptr->GetSizeX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size Y" << (double)ptr->GetSizeY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size Z" << (double)ptr->GetSizeZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Multi Activation" << ptr->MultiActivation();
				_objectmodel->AppendRow(data, parent);
			}


			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _actionNameList);
			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 5, parent), _actionNameList);

			UpdateSelectedZoneTriggerDisplay(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ(),
											ptr->GetSizeX(), ptr->GetSizeY(), ptr->GetSizeZ());

			DrawArrows(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ());

			return;
		}

		if(actiontype == "ZoneActionTrigger")
		{
			ZoneActionTrigger* ptr = static_cast<ZoneActionTrigger*>(it->second.get());
			{
				std::string actname = GetActionName(ptr->GetAction1());
				QVector<QVariant> data;
				data << "Action On Activation" << actname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);

				if(actname != "" && actname != "None")
					SelectAction(ptr->GetAction1(), idx);
			}

			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetPosX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetPosY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetPosZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size X" << (double)ptr->GetSizeX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size Y" << (double)ptr->GetSizeY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Zone size Z" << (double)ptr->GetSizeZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Accepted Mode 1" << ptr->GetAcceptedMode1().c_str();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Accepted Mode 2" << ptr->GetAcceptedMode2().c_str();
				_objectmodel->AppendRow(data, parent);
			}

			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _actionNameList);
			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 11, parent), _actormodeList);
			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 12, parent), _actormodeList);

			UpdateSelectedZoneTriggerDisplay(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ(),
				ptr->GetSizeX(), ptr->GetSizeY(), ptr->GetSizeZ());

			DrawArrows(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ());

			return;
		}


		if(actiontype == "ActivationTrigger")
		{
			ActivationTrigger* ptr = static_cast<ActivationTrigger*>(it->second.get());
			{
				std::string actname = GetActionName(ptr->GetAction1());
				QVector<QVariant> data;
				data << "Action On Activation" << actname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);

				if(actname != "" && actname != "None")
					SelectAction(ptr->GetAction1(), idx);
			}

			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetPosX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetPosY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetPosZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Activation distance" << (double)ptr->GetDistance();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Accepted Mode 1" << ptr->GetAcceptedMode1().c_str();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Accepted Mode 2" << ptr->GetAcceptedMode2().c_str();
				_objectmodel->AppendRow(data, parent);
			}

			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _actionNameList);
			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 9, parent), _actormodeList);
			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 10, parent), _actormodeList);

			UpdateSelectedDistanceTriggerDisplay(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ(),
																					ptr->GetDistance());

			DrawArrows(ptr->GetPosX(), ptr->GetPosY(), ptr->GetPosZ());

			return;
		}



		if(actiontype == "TimerTrigger")
		{
			TimerTrigger* ptr = static_cast<TimerTrigger*>(it->second.get());
			{
				std::string actname = GetActionName(ptr->GetAction1());
				QVector<QVariant> data;
				data << "Action On Activation" << actname.c_str();
				QModelIndex idx = _objectmodel->AppendRow(data, parent);

				if(actname != "" && actname != "None")
					SelectAction(ptr->GetAction1(), idx);
			}

			{
				QVector<QVariant> data;
				data << "Time in ms" << (int)ptr->GetTimeinMs();
				_objectmodel->AppendRow(data, parent);
			}


			_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _actionNameList);

			return;
		}
	}
}


/***********************************************************
get action name from id
***********************************************************/
std::string  EditorHandler::GetActionName(long id)
{
	std::string name = "None";

	std::map<long, boost::shared_ptr<ActionBase> >::iterator it = _actions.find(id);
	if(it != _actions.end())
		return it->second->GetName();

	return name;
}


/***********************************************************
called when trigger object changed
***********************************************************/
void EditorHandler::TriggerObjectChanged(long id, const std::string & category, const QModelIndex &parentIdx)
{
	TriggerInfo triggerinfo;
	triggerinfo.id = id;
	triggerinfo.name = _objectmodel->data(_objectmodel->GetIndex(1, 3, parentIdx)).toString().toAscii().data();
	std::string oldname = _triggers[id]->GetName();
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();

	// check if name changed already exist
	if(triggerinfo.name != oldname)
	{
		if(triggerinfo.name == "" || _triggerNameList->stringList().contains(triggerinfo.name.c_str()))
		{
			QMessageBox::warning(this, tr("Name already used"),
				tr("The name you entered for the trigger is already used. Please enter a unique name."),
				QMessageBox::Ok);

			_objectmodel->setData(_objectmodel->GetIndex(1, 3, parentIdx), oldname.c_str());
			return;
		}
	}


	if(category == "ZoneTrigger")
	{
		// get info

		std::string action1 = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		std::string action2 = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toString().toAscii().data();
		triggerinfo.CheckPlayers = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toBool();
		triggerinfo.CheckNpcs = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toBool();
		triggerinfo.CheckMovableObjects = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toBool();
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 9, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 10, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 11, parentIdx)).toFloat();
		float sizeX = _objectmodel->data(_objectmodel->GetIndex(1, 12, parentIdx)).toFloat();
		float sizeY = _objectmodel->data(_objectmodel->GetIndex(1, 13, parentIdx)).toFloat();
		float sizeZ = _objectmodel->data(_objectmodel->GetIndex(1, 14, parentIdx)).toFloat();
		if(sizeX < 0.0001)
			sizeX = 0.0001;
		if(sizeY < 0.0001)
			sizeY = 0.0001;
		if(sizeZ < 0.0001)
			sizeZ = 0.0001;

		bool multicactiv = _objectmodel->data(_objectmodel->GetIndex(1, 15, parentIdx)).toBool();

		// get id associated to actions
		long act1id = GetActionId(action1);
		long act2id = GetActionId(action2);


		// created modified action and replace old one

		boost::shared_ptr<TriggerBase> modifiedtrig(new ZoneTrigger(triggerinfo, sizeX, sizeY, sizeZ, multicactiv));
		modifiedtrig->SetPosition(posX, posY, posZ);

		long lastact1id = modifiedtrig->GetAction1();
		long lastact2id = modifiedtrig->GetAction2();

		modifiedtrig->SetAction1(act1id);
		modifiedtrig->SetAction2(act2id);



		// update trigger name list
		_triggerNameList->ReplaceData(oldname.c_str(), triggerinfo.name.c_str());


		_triggers[id] = modifiedtrig;


		// update trigger list display
		QStringList slist;
		slist << triggerinfo.name.c_str() << modifiedtrig->GetTypeName().c_str();
		_triggerlistmodel->AddOrUpdateRow(id, slist);



		// need to save as something changed
		SetMapModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModTrigger(modifiedtrig);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);


		UpdateSelectedZoneTriggerDisplay(posX, posY, posZ, sizeX, sizeY, sizeZ);

		if(act1id < 0)
		{
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
		}
		else if(lastact1id != act1id)
		{
			// refresh sub tree
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
			SelectAction(act1id, _objectmodel->GetIndex(0, 4, parentIdx));
		}

		if(act2id < 0)
		{
			_objectmodel->Clear(_objectmodel->GetIndex(0, 5, parentIdx));
		}
		else if(lastact2id != act2id)
		{
			// refresh sub tree
			_objectmodel->Clear(_objectmodel->GetIndex(0, 5, parentIdx));
			SelectAction(act2id, _objectmodel->GetIndex(0, 5, parentIdx));
		}
		return;
	}


	if(category == "ZoneActionTrigger")
	{
		// get info

		std::string action1 = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toFloat();
		float sizeX = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toFloat();
		float sizeY = _objectmodel->data(_objectmodel->GetIndex(1, 9, parentIdx)).toFloat();
		float sizeZ = _objectmodel->data(_objectmodel->GetIndex(1, 10, parentIdx)).toFloat();
		triggerinfo.CheckPlayers = true;
		triggerinfo.CheckNpcs = true;
		triggerinfo.CheckMovableObjects = false;

		if(sizeX < 0.0001)
			sizeX = 0.0001;
		if(sizeY < 0.0001)
			sizeY = 0.0001;
		if(sizeZ < 0.0001)
			sizeZ = 0.0001;

		std::string mode1 = _objectmodel->data(_objectmodel->GetIndex(1, 11, parentIdx)).toString().toAscii().data();
		std::string mode2 = _objectmodel->data(_objectmodel->GetIndex(1, 12, parentIdx)).toString().toAscii().data();


		// get id associated to actions
		long act1id = GetActionId(action1);


		// created modified action and replace old one

		boost::shared_ptr<TriggerBase> modifiedtrig(new ZoneActionTrigger(triggerinfo, sizeX, sizeY, sizeZ, mode1, mode2));
		modifiedtrig->SetPosition(posX, posY, posZ);

		long lastact1id = modifiedtrig->GetAction1();
		modifiedtrig->SetAction1(act1id);


		// update trigger name list
		_triggerNameList->ReplaceData(oldname.c_str(), triggerinfo.name.c_str());


		_triggers[id] = modifiedtrig;


		// update trigger list display
		QStringList slist;
		slist << triggerinfo.name.c_str() << modifiedtrig->GetTypeName().c_str();
		_triggerlistmodel->AddOrUpdateRow(id, slist);



		// need to save as something changed
		SetMapModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModTrigger(modifiedtrig);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);


		UpdateSelectedZoneTriggerDisplay(posX, posY, posZ, sizeX, sizeY, sizeZ);

		if(act1id < 0)
		{
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
		}
		else if(lastact1id != act1id)
		{
			// refresh sub tree
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
			SelectAction(act1id, _objectmodel->GetIndex(0, 4, parentIdx));
		}

		return;
	}


	if(category == "ActivationTrigger")
	{
		// get info

		std::string action1 = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toFloat();
		float distance = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toFloat();
		triggerinfo.CheckPlayers = true;
		triggerinfo.CheckNpcs = true;
		triggerinfo.CheckMovableObjects = false;

		if(distance < 0.0001)
			distance = 0.0001;

		std::string mode1 = _objectmodel->data(_objectmodel->GetIndex(1, 9, parentIdx)).toString().toAscii().data();
		std::string mode2 = _objectmodel->data(_objectmodel->GetIndex(1, 10, parentIdx)).toString().toAscii().data();


		// get id associated to actions
		long act1id = GetActionId(action1);


		// created modified action and replace old one

		boost::shared_ptr<TriggerBase> modifiedtrig(new ActivationTrigger(triggerinfo, distance, mode1, mode2));
		modifiedtrig->SetPosition(posX, posY, posZ);

		long lastact1id = modifiedtrig->GetAction1();
		modifiedtrig->SetAction1(act1id);


		// update trigger name list
		_triggerNameList->ReplaceData(oldname.c_str(), triggerinfo.name.c_str());


		_triggers[id] = modifiedtrig;


		// update trigger list display
		QStringList slist;
		slist << triggerinfo.name.c_str() << modifiedtrig->GetTypeName().c_str();
		_triggerlistmodel->AddOrUpdateRow(id, slist);



		// need to save as something changed
		SetMapModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModTrigger(modifiedtrig);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);


		UpdateSelectedDistanceTriggerDisplay(posX, posY, posZ, distance);

		if(act1id < 0)
		{
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
		}
		else if(lastact1id != act1id)
		{
			// refresh sub tree
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
			SelectAction(act1id, _objectmodel->GetIndex(0, 4, parentIdx));
		}

		return;
	}



	if(category == "TimerTrigger")
	{
		// get info

		std::string action1 = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		long time = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toInt();
		triggerinfo.CheckPlayers = true;
		triggerinfo.CheckNpcs = true;
		triggerinfo.CheckMovableObjects = false;


		// get id associated to actions
		long act1id = GetActionId(action1);


		// created modified action and replace old one
		boost::shared_ptr<TriggerBase> modifiedtrig(new TimerTrigger(triggerinfo, time));

		long lastact1id = modifiedtrig->GetAction1();
		modifiedtrig->SetAction1(act1id);


		// update trigger name list
		_triggerNameList->ReplaceData(oldname.c_str(), triggerinfo.name.c_str());


		_triggers[id] = modifiedtrig;


		// update trigger list display
		QStringList slist;
		slist << triggerinfo.name.c_str() << modifiedtrig->GetTypeName().c_str();
		_triggerlistmodel->AddOrUpdateRow(id, slist);



		// need to save as something changed
		SetMapModified();


		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModTrigger(modifiedtrig);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);


		if(act1id < 0)
		{
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
		}
		else if(lastact1id != act1id)
		{
			// refresh sub tree
			_objectmodel->Clear(_objectmodel->GetIndex(0, 4, parentIdx));
			SelectAction(act1id, _objectmodel->GetIndex(0, 4, parentIdx));
		}

		return;
	}


}



/***********************************************************
change current map to new map
***********************************************************/
void EditorHandler::ChangeMap(const std::string & mapname, long spawningid)
{
	if(SaveMapBeforeQuit())
		SharedDataHandler::getInstance()->ChangeMapPlayer(1, mapname, spawningid);
}



/***********************************************************
save current map to file
***********************************************************/
void EditorHandler::SaveMap(const std::string & filename)
{
	std::ofstream file(filename.c_str());
	file<<"function InitMap(environment)"<<std::endl;

	// save actors
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >::iterator ita = _Actors.begin();
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >::iterator enda = _Actors.end();
	for(;ita != enda; ++ita)
		ita->second->SaveToLuaFile(file);

	// save triggers
	std::map<long, boost::shared_ptr<TriggerBase> >::iterator itt = _triggers.begin();
	std::map<long, boost::shared_ptr<TriggerBase> >::iterator endt = _triggers.end();
	for(;itt != endt; ++itt)
		itt->second->SaveToLuaFile(file);

	file<<"end"<<std::endl;
}


/***********************************************************
world description changed
***********************************************************/
void EditorHandler::WorldDescriptionChanged()
{
	QString descs = _uieditor.textEdit_worlddescription->toPlainText();
	descs.replace(QString("\n"), QString(" @ "));
	_winfo.Description.Description = descs.toUtf8().data();
	SetModified();
}

/***********************************************************
worlds news changed
***********************************************************/
void EditorHandler::WorldNewsChanged()
{
	QString descs = _uieditor.textEdit_worldnews->toPlainText();
	descs.replace(QString("\n"), QString(" @ "));
	_winfo.Description.News = descs.toUtf8().data();
	SetModified();
}


/***********************************************************
refresh starting info tab
***********************************************************/
void EditorHandler::RefreshStartingInfo()
{
	// map list part
	RefreshStartingMap();

	// spawning part
	RefreshStartingSpawning();

	_uieditor.spinBox_startinglife->setValue(_winfo.StartingInfo.StartingLife);
	_uieditor.spinBox_startingMana->setValue(_winfo.StartingInfo.StartingMana);
	_uieditor.spinBox_startingarmor->setValue(_winfo.StartingInfo.StartingArmor);

	RefreshStartingModelOutfit();
}


/***********************************************************
refresh starting map
***********************************************************/
void EditorHandler::RefreshStartingMap()
{
	_uieditor.comboBox_startingmap->setModel(_mapNameList.get());
	int index = _uieditor.comboBox_startingmap->findText(_winfo.StartingInfo.StartingMap.c_str());
	if(index >= 0)
		_uieditor.comboBox_startingmap->setCurrentIndex(index);
	else
	{
		if(_winfo.Maps.size() > 0)
		{
			_winfo.StartingInfo.StartingMap = _winfo.Maps.begin()->first;
			RefreshStartingMap();
			return;
		}
	}
}

/***********************************************************
refresh starting spawning
***********************************************************/
void EditorHandler::RefreshStartingSpawning()
{
	LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(_winfo.StartingInfo.StartingMap);
	if(itmap != _winfo.Maps.end())
	{
		std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator itsp = 
													_mapSpawningList.find(_winfo.StartingInfo.StartingMap);
		if(itsp != _mapSpawningList.end())
		{
			_uieditor.comboBox_startingspawning->setModel(itsp->second.get());	

			LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.find(_winfo.StartingInfo.SpawningId);
			if(it != itmap->second.Spawnings.end())
			{
				int index = _uieditor.comboBox_startingspawning->findText(it->second.Name.c_str());
				if(index >= 0)
					_uieditor.comboBox_startingspawning->setCurrentIndex(index);
			}	
			else
			{
				if(itmap->second.Spawnings.size() > 0)
				{
					_winfo.StartingInfo.SpawningId = itmap->second.Spawnings.begin()->first;
					RefreshStartingSpawning();
					return;
				}
			}
		}
	}
}

/***********************************************************
refresh starting outfit
***********************************************************/
void EditorHandler::RefreshStartingModelOutfit()
{
	_uieditor.comboBox_modeloutfit->clear();

	std::map<std::string, OutfitData>::iterator it = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].outfits.begin();
	std::map<std::string, OutfitData>::iterator end = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].outfits.end();
	for(; it != end; ++it)
		_uieditor.comboBox_modeloutfit->addItem(it->first.c_str());

	int index = _uieditor.comboBox_modeloutfit->findText(_winfo.StartingInfo.StartingModel.Outfit.c_str());
	if(index >= 0)
		_uieditor.comboBox_modeloutfit->setCurrentIndex(index);
	else
	{
		if(_lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].outfits.size() > 0)
		{
			_winfo.StartingInfo.StartingModel.Outfit = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].outfits.begin()->first;
		
			RefreshStartingModelOutfit();
			return;
		}
	}

	RefreshStartingModelWeapon();
}

/***********************************************************

***********************************************************/
void EditorHandler::RefreshStartingModelWeapon()
{
	_uieditor.comboBox_modelweapon->clear();

	std::map<std::string, WeaponData>::iterator it = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].weapons.begin();
	std::map<std::string, WeaponData>::iterator end = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].weapons.end();
	for(; it != end; ++it)
		_uieditor.comboBox_modelweapon->addItem(it->first.c_str());

	int index = _uieditor.comboBox_modelweapon->findText(_winfo.StartingInfo.StartingModel.Weapon.c_str());
	if(index >= 0)
		_uieditor.comboBox_modelweapon->setCurrentIndex(index);
	else
	{
		if(_lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
			outfits[_winfo.StartingInfo.StartingModel.Outfit].weapons.size() > 0)
		{
			_winfo.StartingInfo.StartingModel.Weapon = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].weapons.begin()->first;
		
			RefreshStartingModelWeapon();
			return;
		}
	}

	RefreshStartingModelMode();
}

/***********************************************************
refresh starting mode
***********************************************************/
void EditorHandler::RefreshStartingModelMode()
{
	_uieditor.comboBox_modelmode->clear();

	std::map<std::string, ModeData>::iterator it = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].
											weapons[_winfo.StartingInfo.StartingModel.Weapon].modes.begin();
	std::map<std::string, ModeData>::iterator end = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].
											weapons[_winfo.StartingInfo.StartingModel.Weapon].modes.end();
	for(; it != end; ++it)
		_uieditor.comboBox_modelmode->addItem(it->first.c_str());

	int index = _uieditor.comboBox_modelmode->findText(_winfo.StartingInfo.StartingModel.Mode.c_str());
	if(index >= 0)
		_uieditor.comboBox_modelmode->setCurrentIndex(index);
	else
	{
		if(_lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
			outfits[_winfo.StartingInfo.StartingModel.Outfit].
			weapons[_winfo.StartingInfo.StartingModel.Weapon].modes.size() > 0)
		{
			_winfo.StartingInfo.StartingModel.Mode = _lba1Mdata[_winfo.StartingInfo.StartingModel.ModelName].
											outfits[_winfo.StartingInfo.StartingModel.Outfit].
											weapons[_winfo.StartingInfo.StartingModel.Weapon].modes.begin()->first;
		
			RefreshStartingModelMode();
			return;
		}
	}
}



/***********************************************************
starting map modified
***********************************************************/
void EditorHandler::StartingMapModified(int index)
{
	_winfo.StartingInfo.StartingMap = _uieditor.comboBox_startingmap->currentText().toAscii().data();

	SetModified();
	RefreshStartingSpawning();
}


/***********************************************************
starting spawn modified
***********************************************************/
void EditorHandler::StartingSpawnModified(int index)
{
	std::string spname = _uieditor.comboBox_startingspawning->currentText().toAscii().data();
	long spid = -1;
	if(spname != "")
	{
		LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(_winfo.StartingInfo.StartingMap);
		if(itmap != _winfo.Maps.end())
		{
			LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.begin();
			LbaNet::SpawningsSeq::iterator end = itmap->second.Spawnings.end();
			for(;it != end; ++it)
			{
				if(spname == it->second.Name)
				{
					spid = it->first;
					break;
				}
			}
		}
	}

	_winfo.StartingInfo.SpawningId = spid;
	SetModified();
}

/***********************************************************
starting life modified
***********************************************************/
void EditorHandler::StartingLifeModified(int value)
{
	_winfo.StartingInfo.StartingLife = _uieditor.spinBox_startinglife->value();
	_winfo.StartingInfo.StartingMana = _uieditor.spinBox_startingMana->value();
	_winfo.StartingInfo.StartingArmor = _uieditor.spinBox_startingarmor->value();

	SetModified();
}

/***********************************************************
starting Outfit modified
***********************************************************/
void EditorHandler::StartingOutfitModified(int index)
{
	_winfo.StartingInfo.StartingModel.Outfit = _uieditor.comboBox_modeloutfit->currentText().toAscii().data();

	SetModified();
	RefreshStartingModelWeapon();
}

/***********************************************************
starting Weapon modified
***********************************************************/
void EditorHandler::StartingWeaponModified(int index)
{
	_winfo.StartingInfo.StartingModel.Weapon = _uieditor.comboBox_modelweapon->currentText().toAscii().data();

	SetModified();
	RefreshStartingModelMode();
}

/***********************************************************
starting Mode modified
***********************************************************/
void EditorHandler::StartingModeModified(int index)
{
	_winfo.StartingInfo.StartingModel.Mode = _uieditor.comboBox_modelmode->currentText().toAscii().data();

	SetModified();
}



/***********************************************************
ui button clicked
***********************************************************/
void EditorHandler::addmap_button_clicked()
{
	if(!IsWorldOpened())
		return;

	// clear data
	_ui_addmapdialog.lineEdit_mapname->setText("");
	_ui_addmapdialog.textEdit_map_description->setText("");
	_ui_addmapdialog.radioButton_camtype_ortho->setChecked (true);
	_ui_addmapdialog.checkBox_map_instancied->setChecked (false);
	_addmapdialog->show();
}

/***********************************************************
ui button clicked
***********************************************************/
void EditorHandler::removemap_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_MapList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();

	if(indexes.size() > 0)
	{
		std::string mapname = _maplistmodel->GetString(indexes[0]).toAscii().data();


		LbaNet::MapsSeq::iterator itm = _winfo.Maps.find(mapname);
		if(itm != _winfo.Maps.end())
		{
			// remove from internal memory
			_winfo.Maps.erase(itm);

			// remove row from table
			_maplistmodel->removeRows(indexes[0].row(), 1);

			// update map list name
			_mapNameList->RemoveData(mapname.c_str());

			//inform server
			EditorUpdateBasePtr update = new UpdateEditor_RemoveMap(mapname);
			SharedDataHandler::getInstance()->EditorUpdate("-", update);

			SetModified();
			RefreshStartingMap();
		}
	}
}


/***********************************************************
dialog accepted
***********************************************************/
void EditorHandler::addmap_accepted()
{
	QString mapname = _ui_addmapdialog.lineEdit_mapname->text();

	// user forgot to set a name
	if(mapname == "")
		return;

	if(_mapNameList->stringList().contains(mapname))
	{
		QMessageBox::warning(this, tr("Name already used"),
			tr("The name you entered for the map is already used. Please enter a unique name."),
			QMessageBox::Ok);
		return;
	}

	_addmapdialog->hide();

	LbaNet::MapInfo newmapinfo;
	newmapinfo.Name = mapname.toAscii().data();

	QString descs = _ui_addmapdialog.textEdit_map_description->toPlainText();
	descs.replace(QString("\n"), QString(" @ "));
	newmapinfo.Description = descs.toUtf8().data();

	newmapinfo.Type = (_ui_addmapdialog.comboBox_maptype->currentIndex() == 0) ? "exterior" : "interior";

	newmapinfo.IsInstance = _ui_addmapdialog.checkBox_map_instancied->isChecked();

	newmapinfo.AutoCameraType = 1;
	if(_ui_addmapdialog.radioButton_camtype_persp->isChecked())
		newmapinfo.AutoCameraType = 2;
	if(_ui_addmapdialog.radioButton_camtype_3d->isChecked())
		newmapinfo.AutoCameraType = 3;

	_winfo.Maps[newmapinfo.Name] = newmapinfo;


	// refresh lists
	QStringList data;
	data << mapname << newmapinfo.Description.c_str();
	_maplistmodel->AddOrUpdateRow(_maplistmodel->GetNextId(), data);

	_mapNameList->AddData(mapname);
	_mapSpawningList[newmapinfo.Name] = boost::shared_ptr<CustomStringListModel>(new CustomStringListModel());


	//inform server
	EditorUpdateBasePtr update = new UpdateEditor_AddOrModMap(newmapinfo);
	SharedDataHandler::getInstance()->EditorUpdate("-", update);
	

	SetModified();
	RefreshStartingMap();
	CreateDefaultSpawningAndTp(newmapinfo.Name);
}



/***********************************************************
tp to default spawning of map
***********************************************************/
void EditorHandler::CreateDefaultSpawningAndTp(const std::string & mapname)
{
	// add a default spawning
	long spid = AddOrModSpawning(mapname, "DefaultSpawning", 0, 0, 0, 0, true, 0);
	AddSpawningName(mapname, "DefaultSpawning");
	ChangeMap(mapname, spid);
	SetModified();
	RefreshStartingSpawning();
}


/***********************************************************
map description changed
***********************************************************/
void EditorHandler::MapDescriptionChanged()
{
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	if(mapname != "")
	{
		QString descs = _uieditor.textEdit_map_description->toPlainText();
		descs.replace(QString("\n"), QString(" @ "));
		std::string descstring	= descs.toUtf8().data();

		if(_winfo.Maps[mapname].Description != descstring)
		{
			SetModified();

			//inform server
			EditorUpdateBasePtr update = new UpdateEditor_AddOrModMap(_winfo.Maps[mapname]);
			SharedDataHandler::getInstance()->EditorUpdate("-", update);
		}
	}
}

/***********************************************************
map type changed
***********************************************************/
void EditorHandler::MapTypeChanged(int flag)
{
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	if(mapname != "")
	{
		_winfo.Maps[mapname].Type = (_uieditor.comboBox_maptype->currentIndex() == 0) ? "exterior" : "interior";

		SetModified();

		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModMap(_winfo.Maps[mapname]);
		SharedDataHandler::getInstance()->EditorUpdate("-", update);
	}
}

/***********************************************************
map instance changed
***********************************************************/
void EditorHandler::MapInstanceChanged(int flag)
{
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	if(mapname != "")
	{
		_winfo.Maps[mapname].IsInstance = _uieditor.checkBox_map_instancied->isChecked();

		SetModified();

		//inform server
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModMap(_winfo.Maps[mapname]);
		SharedDataHandler::getInstance()->EditorUpdate("-", update);
	}
}

/***********************************************************
map cam type changed
***********************************************************/
void EditorHandler::MapCameraTypeChanged(bool flag)
{
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	if(mapname != "")
	{
		if(flag == true)
		{
			QWidget *sendero = static_cast<QWidget *>(sender());
			if(sendero)
			{
				bool changed = false;

				if(sendero->objectName() == "radioButton_camtype_ortho")
				{
					if(_winfo.Maps[mapname].AutoCameraType != 1)
					{
						_winfo.Maps[mapname].AutoCameraType = 1;
						changed = true;
					}
				}

				if(sendero->objectName() == "radioButton_camtype_persp")
				{
					if(_winfo.Maps[mapname].AutoCameraType != 2)
					{
						_winfo.Maps[mapname].AutoCameraType = 2;
						changed = true;
					}
				}

				if(sendero->objectName() == "radioButton_camtype_3d")
				{
					if(_winfo.Maps[mapname].AutoCameraType != 3)
					{
						_winfo.Maps[mapname].AutoCameraType = 3;
						changed = true;
					}
				}

				if(changed)
				{
					SetModified();

					//inform server
					EditorUpdateBasePtr update = new UpdateEditor_AddOrModMap(_winfo.Maps[mapname]);
					SharedDataHandler::getInstance()->EditorUpdate("-", update);
				}
			}
		}
	}
}



/***********************************************************
tp add button push
***********************************************************/
void EditorHandler::TpAdd_button()
{
	_ui_addtpdialog.lineEdit_tpname->setText("");
	_ui_addtpdialog.comboBox_map->setModel(_mapNameList.get());
	_addtpdialog->setWindowTitle("Add Teleport");
	_edited_tp = -1;

	_addtpdialog->show();
}


/***********************************************************
tp remove button push
***********************************************************/
void EditorHandler::TpRemove_button()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_TeleportList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();

	if(indexes.size() > 0)
	{
		long tpid = _tplistmodel->GetId(indexes[0]);

		LbaNet::ServerTeleportsSeq::iterator it = _winfo.TeleportInfo.find(tpid);
		if( it != _winfo.TeleportInfo.end())
		{
			// remove from internal memory
			_winfo.TeleportInfo.erase(it);

			// remove row from table
			_tplistmodel->removeRows(indexes[0].row(), 1);


			//inform server
			EditorUpdateBasePtr update = new UpdateEditor_TeleportListChanged(_winfo.TeleportInfo);
			SharedDataHandler::getInstance()->EditorUpdate("-", update);

			SetModified();
		}
	}
}


/***********************************************************
tp edit button push
***********************************************************/
void EditorHandler::TpEdit_button()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_TeleportList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();

	if(indexes.size() > 0)
	{
		long id = _tplistmodel->GetId(indexes[0]);
		std::string name = _tplistmodel->GetString(indexes[0]).toAscii().data();
		_edited_tp = id;

		LbaNet::ServerTeleportsSeq::iterator it = _winfo.TeleportInfo.find(id);
		if( it != _winfo.TeleportInfo.end())
		{
			_ui_addtpdialog.lineEdit_tpname->setText(name.c_str());


			// add map list to combo box
			_ui_addtpdialog.comboBox_map->setModel(_mapNameList.get());

			int index = _ui_addtpdialog.comboBox_map->findText(it->second.MapName.c_str());
			if(index >= 0)
				_ui_addtpdialog.comboBox_map->setCurrentIndex(index);



			// add spawning list to combo box
			std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator itsp = 
													_mapSpawningList.find(it->second.MapName.c_str());
			if(itsp != _mapSpawningList.end())
			{
				_ui_addtpdialog.comboBox_spawn->setModel(itsp->second.get());

				// select span in combo box if exist
				LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(it->second.MapName);
				if(itmap != _winfo.Maps.end())
				{
					LbaNet::SpawningsSeq::iterator itmapsp = itmap->second.Spawnings.find(it->second.SpawningId);
					if(itmapsp != itmap->second.Spawnings.end())
					{
						std::string spname = itmapsp->second.Name;
						int indexsp = _ui_addtpdialog.comboBox_spawn->findText(spname.c_str());
						if(indexsp >= 0)
							_ui_addtpdialog.comboBox_spawn->setCurrentIndex(indexsp);
					}
				}
			}

			std::stringstream strs;
			strs<<"Edit Teleport #"<<id;
			_addtpdialog->setWindowTitle(strs.str().c_str());
			_addtpdialog->show();
		}
	}
}

/***********************************************************
tp add button push
***********************************************************/
void EditorHandler::TpAdd_button_accepted()
{
	std::string tpname = _ui_addtpdialog.lineEdit_tpname->text().toAscii().data();
	if(tpname == "")
		return;

	_addtpdialog->hide();

	std::string mapname = _ui_addtpdialog.comboBox_map->currentText().toAscii().data();
	std::string spawnname = _ui_addtpdialog.comboBox_spawn->currentText().toAscii().data();

	long spid = -1;
	if(spawnname != "")
	{
		LbaNet::MapsSeq::iterator itmap = _winfo.Maps.find(mapname);
		if(itmap != _winfo.Maps.end())
		{
			LbaNet::SpawningsSeq::iterator it = itmap->second.Spawnings.begin();
			LbaNet::SpawningsSeq::iterator end = itmap->second.Spawnings.end();
			for(;it != end; ++it)
			{
				if(spawnname == it->second.Name)
				{
					spid = it->first;
					break;
				}
			}
		}
	}

	long tpid = 0;
	if(_edited_tp >= 0)
		tpid = _edited_tp;
	else
	{
		tpid = (++_currteleportidx);
	}

	QStringList data;
	data << tpname.c_str() << mapname.c_str() << spawnname.c_str();
	_tplistmodel->AddOrUpdateRow(tpid, data);


	_winfo.TeleportInfo[tpid].Name = tpname;
	_winfo.TeleportInfo[tpid].MapName = mapname;
	_winfo.TeleportInfo[tpid].SpawningId = spid;

	//inform server
	EditorUpdateBasePtr update = new UpdateEditor_TeleportListChanged(_winfo.TeleportInfo);
	SharedDataHandler::getInstance()->EditorUpdate("-", update);

	SetModified();
}


/***********************************************************
tp add button push
***********************************************************/
void EditorHandler::TpDialogMapChanged(int flag)
{
	std::string mapname = _ui_addtpdialog.comboBox_map->currentText().toAscii().data();

	std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator itsp = 
											_mapSpawningList.find(mapname.c_str());
	if(itsp != _mapSpawningList.end())
	{
		_ui_addtpdialog.comboBox_spawn->setModel(itsp->second.get());

		if(itsp->second->stringList().size() > 0)
			_ui_addtpdialog.comboBox_spawn->setCurrentIndex(0);
	}
}


/***********************************************************
update tp list with new span name
***********************************************************/
void EditorHandler::UpdateTpSpanName(const std::string & mapname, 
									 long spwid, 
									 const std::string &  spname)
{
	LbaNet::ServerTeleportsSeq::iterator it = _winfo.TeleportInfo.begin();
	LbaNet::ServerTeleportsSeq::iterator end = _winfo.TeleportInfo.end();
	for(; it != end; ++it)
	{
		if(it->second.MapName == mapname)
		{
			if(it->second.SpawningId = spwid)
			{
				QStringList data;
				data << it->second.Name.c_str() << mapname.c_str() << spname.c_str();
				_tplistmodel->AddOrUpdateRow(it->first, data);
			}
		}
	}
}


/***********************************************************
add world accepted
***********************************************************/
void EditorHandler::addworld_accepted()
{
	std::string wname = _ui_addworlddialog.lineEdit_worldname->text().toAscii().data();

	if(wname == "")
		return;

	_addworlddialog->hide();
	ResetWorld();

	LbaNet::WorldInformation winfo;
	winfo.Description.WorldName = wname;
	QString descs = _ui_addworlddialog.textEdit_worlddesc->toPlainText();
	descs.replace(QString("\n"), QString(" @ "));
	winfo.Description.Description = descs.toUtf8().data();
	winfo.StartingInfo.InventorySize = 30;
	winfo.StartingInfo.StartingLife = 50;
	winfo.StartingInfo.StartingMana = 50;
	winfo.StartingInfo.StartingArmor = 0;
	winfo.StartingInfo.StartingModel.TypeRenderer = LbaNet::RenderLba1M;
	winfo.StartingInfo.StartingModel.ModelName = "Twinsen";
	winfo.StartingInfo.StartingModel.Outfit = "Nurse";
	winfo.StartingInfo.StartingModel.Weapon = "No";
	winfo.StartingInfo.StartingModel.Mode = "Normal";
	winfo.StartingInfo.SpawningId = -1;


	//create directories
	try	{boost::filesystem::create_directory( "./Data/Worlds/" + wname );}catch(...){}
	try	{boost::filesystem::create_directory( "./Data/Worlds/" + wname + "/Lua");}catch(...){}

	// save new world
	DataLoader::getInstance()->SaveWorldInformation(wname, winfo);

	// reset gui to new world
	ResetWorld();
	SetWorldInfo(wname);

	EventsQueue::getReceiverQueue()->AddEvent(new ChangeWorldEvent(wname));

	_modified = false;
	_mapmodified = false;
}



/***********************************************************
on selectactor_double_clicked
***********************************************************/
void EditorHandler::selectactor_double_clicked(const QModelIndex & itm)
{
	ActorSelect_button();
}

/***********************************************************
Actor add button push
***********************************************************/
void EditorHandler::ActorAdd_button()
{
	_ui_addactordialog.lineEdit_name->setText("");
	_addactordialog->show();
}


/***********************************************************
Actor remove button push
***********************************************************/
void EditorHandler::ActorRemove_button()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_ActorList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		// inform server
		long id = _actorlistmodel->GetId(indexes[0]);
		RemoveActor(id);

		// remove row from table
		_actorlistmodel->removeRows(indexes[0].row(), 1);

		// clear object
		ClearObjectIfSelected("Actor", id);
	}
}


/***********************************************************
Actor edit button push
***********************************************************/
void EditorHandler::ActorSelect_button()
{
	QItemSelectionModel *selectionModel = _uieditor.tableView_ActorList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		long id = _actorlistmodel->GetId(indexes[0]);	
		SelectActor(id);
	}
}


/***********************************************************
Actor add button push
***********************************************************/
void EditorHandler::ActorAdd_button_accepted()
{
	_addactordialog->hide();


	// add the actor to internal
	ActorObjectInfo ainfo(_curractoridx+1);
	ainfo.ExtraInfo.Name = _ui_addactordialog.lineEdit_name->text().toAscii().data();
	ainfo.ExtraInfo.NameColorR = 1.0;
	ainfo.ExtraInfo.NameColorG = 1.0;
	ainfo.ExtraInfo.NameColorB = 1.0;

	switch(_ui_addactordialog.comboBox_dtype->currentIndex())
	{
		case 0:
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderOsgModel;
		break;
		case 1:
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderSprite;
		break;
		case 2:
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderLba1M;
		break;
		case 3:
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderLba2M;
		break;
	}

	switch(_ui_addactordialog.comboBox_atype->currentIndex())
	{
		case 0:
			ainfo.PhysicDesc.TypePhysO = LbaNet::StaticAType;
		break;
		case 1:
			ainfo.PhysicDesc.TypePhysO = LbaNet::KynematicAType;
		break;
		case 2:
			ainfo.PhysicDesc.TypePhysO = LbaNet::CharControlAType;
		break;
	}

	switch(_ui_addactordialog.comboBox_ptype->currentIndex())
	{
		case 0:
			ainfo.PhysicDesc.TypeShape = LbaNet::NoShape;
		break;
		case 1:
			ainfo.PhysicDesc.TypeShape = LbaNet::BoxShape;
		break;
		case 2:
			ainfo.PhysicDesc.TypeShape = LbaNet::CapsuleShape;
		break;
		case 3:
			ainfo.PhysicDesc.TypeShape = LbaNet::SphereShape;
		break;
		case 4:
			ainfo.PhysicDesc.TypeShape = LbaNet::TriangleMeshShape;
		break;
	}

	ainfo.DisplayDesc.ModelId = 0;
	ainfo.DisplayDesc.UseLight = true;
	ainfo.DisplayDesc.CastShadow = true;
	ainfo.DisplayDesc.ColorR = 1;
	ainfo.DisplayDesc.ColorG = 1;
	ainfo.DisplayDesc.ColorB = 1;
	ainfo.DisplayDesc.ColorA = 1;
	ainfo.DisplayDesc.TransX = 0;
	ainfo.DisplayDesc.TransY = 0;
	ainfo.DisplayDesc.TransZ = 0;
	ainfo.DisplayDesc.ScaleX = 1;
	ainfo.DisplayDesc.ScaleY = 1;
	ainfo.DisplayDesc.ScaleZ = 1;
	ainfo.DisplayDesc.RotX = 0;
	ainfo.DisplayDesc.RotY = 0;
	ainfo.DisplayDesc.RotZ = 0;
	ainfo.DisplayDesc.State = LbaNet::StNormal;

	ainfo.PhysicDesc.Pos.X = _posX;
	ainfo.PhysicDesc.Pos.Y = _posY;
	ainfo.PhysicDesc.Pos.Z = _posZ;
	ainfo.PhysicDesc.Pos.Rotation = 0;
	ainfo.PhysicDesc.Density = 1;
	ainfo.PhysicDesc.Collidable = true;
	ainfo.PhysicDesc.SizeX = 1;
	ainfo.PhysicDesc.SizeY = 1;
	ainfo.PhysicDesc.SizeZ = 1;


	boost::shared_ptr<ActorHandler> act(new ActorHandler(ainfo));
	AddActorObject(act);

	SetMapModified();

	//inform server
	std::string mapname = _uieditor.label_mapname->text().toAscii().data();
	EditorUpdateBasePtr update = new UpdateEditor_AddOrModActor(act);
	SharedDataHandler::getInstance()->EditorUpdate(mapname, update);

	SelectActor(ainfo.ObjectId);
}




/***********************************************************
remove actor
***********************************************************/
void EditorHandler::RemoveActor(long id)
{
	//remove internal trigger
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >::iterator it = _Actors.find(id);
	if(it != _Actors.end())
	{
		_Actors.erase(it);
		SetMapModified();

		//inform server
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		EditorUpdateBasePtr update = new UpdateEditor_RemoveActor(id);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);

		ClearObjectIfSelected("Actor", id);
	}
}



/***********************************************************
select actor
***********************************************************/
void EditorHandler::SelectActor(long id, const QModelIndex &parent)
{
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >::iterator it = _Actors.find(id);
	if(it != _Actors.end())
	{
		if(parent == QModelIndex())
			ResetObject();

		const ActorObjectInfo & ainfo = it->second->GetInfo();
		

		{
			QVector<QVariant> data;
			data<<"Type"<<"Actor";
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"SubCategory"<<"-";
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Id"<<id;
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Name"<<ainfo.ExtraInfo.Name.c_str();
			_objectmodel->AppendRow(data, parent);
		}



		std::string type;
		switch(ainfo.PhysicDesc.TypePhysO)
		{
			case LbaNet::StaticAType:
				type = "Static";
			break;

			case LbaNet::KynematicAType:
				type = "Scripted";
			break;

			case LbaNet::CharControlAType:
				type = "Movable";
			break;  
		}
		{
			QVector<QVariant> data;
			data<<"Type"<<type.c_str();
			_objectmodel->AppendRow(data, parent);
		}

		std::string dtype;
		switch(ainfo.DisplayDesc.TypeRenderer)
		{
			case LbaNet::RenderOsgModel:
				dtype = "Osg Model";
			break;

			case RenderSprite:
				dtype = "Sprite";
			break;

			case RenderLba1M:
				dtype = "Lba1 Model";
			break;

			case RenderLba2M:
				dtype = "Lba2 Model";
			break;

			case RenderCross:
				dtype = "Cross";
			break;

			case RenderBox:
				dtype = "Box";
			break;

			case RenderCapsule:
				dtype = "Capsule";
			break;
		}
		{
			QVector<QVariant> data;
			data<<"Display Type"<<dtype.c_str();
			_objectmodel->AppendRow(data, parent);
		}

		std::string ptype;
		switch(ainfo.PhysicDesc.TypeShape)
		{
			case LbaNet::NoShape:
				ptype = "No Shape";
			break;
			case BoxShape:
				ptype = "Box";
			break;
			case CapsuleShape:
				ptype = "Capsule";
			break;
			case SphereShape:
				ptype = "Sphere";
			break;
			case TriangleMeshShape:
				ptype = "Triangle Mesh";
			break;
		}
		{
			QVector<QVariant> data;
			data<<"Physical Type"<<ptype.c_str();
			_objectmodel->AppendRow(data, parent);
		}

		_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 4, parent), _actortypeList);
		_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 5, parent), _actordtypeList);
		_objectcustomdelegate->SetCustomIndex(_objectmodel->GetIndex(1, 6, parent), _actorptypeList);		


		{
			QVector<QVariant> data;
			data<<"Position X"<<(double)ainfo.PhysicDesc.Pos.X;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Position Y"<<(double)ainfo.PhysicDesc.Pos.Y;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Position Z"<<(double)ainfo.PhysicDesc.Pos.Z;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Rotation"<<(double)ainfo.PhysicDesc.Pos.Rotation;
			_objectmodel->AppendRow(data, parent);
		}

		if(ainfo.PhysicDesc.TypeShape != LbaNet::NoShape)
		{
			{
				QVector<QVariant> data;
				data<<"Collidable"<<ainfo.PhysicDesc.Collidable;
				_objectmodel->AppendRow(data, parent);
			}

			if(ainfo.PhysicDesc.TypeShape != LbaNet::TriangleMeshShape)
			{
				{
					QVector<QVariant> data;
					data<<"Size X"<<(double)ainfo.PhysicDesc.SizeX;
					_objectmodel->AppendRow(data, parent);
				}
				{
					QVector<QVariant> data;
					data<<"Size Y"<<(double)ainfo.PhysicDesc.SizeY;
					_objectmodel->AppendRow(data, parent);
				}
				{
					QVector<QVariant> data;
					data<<"Size Z"<<(double)ainfo.PhysicDesc.SizeZ;
					_objectmodel->AppendRow(data, parent);
				}
			}
			else
			{
				{
					QVector<QVariant> data;
					data<<"Physic filename"<<ainfo.PhysicDesc.Filename.c_str();
					_objectmodel->AppendRow(data, parent);
				}
			}
		}

		{
			QVector<QVariant> data;
			data<<"Use Light"<<ainfo.DisplayDesc.UseLight;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Cast shadow"<<ainfo.DisplayDesc.CastShadow;
			_objectmodel->AppendRow(data, parent);
		}

		{
			QVector<QVariant> data;
			data<<"Display translation X"<<(double)ainfo.DisplayDesc.TransX;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display translation Y"<<(double)ainfo.DisplayDesc.TransY;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display translation Z"<<(double)ainfo.DisplayDesc.TransZ;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display rotation X"<<(double)ainfo.DisplayDesc.RotX;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display rotation Y"<<(double)ainfo.DisplayDesc.RotY;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display rotation Z"<<(double)ainfo.DisplayDesc.RotZ;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display scale X"<<(double)ainfo.DisplayDesc.ScaleX;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display scale Y"<<(double)ainfo.DisplayDesc.ScaleY;
			_objectmodel->AppendRow(data, parent);
		}
		{
			QVector<QVariant> data;
			data<<"Display scale Z"<<(double)ainfo.DisplayDesc.ScaleZ;
			_objectmodel->AppendRow(data, parent);
		}

		{
			QVector<QVariant> data;
			data<<"Display model/file name"<<ainfo.DisplayDesc.ModelName.c_str();
			_objectmodel->AppendRow(data, parent);
		}
		// TODO Outfit;
		// TODO Weapon;
		// TODO - add file choose dialog

		UpdateSelectedActorDisplay(ainfo.PhysicDesc);
		osg::Vec3 center = _actornode->computeBound().center();
		DrawArrows(center.x(), center.y(), center.z());
	}
}



/***********************************************************
called when actor object changed
***********************************************************/
void EditorHandler::ActorObjectChanged(long id, const QModelIndex &parentIdx)
{
	std::map<Ice::Long, boost::shared_ptr<ActorHandler> >::iterator it = _Actors.find(id);
	if(it != _Actors.end())
	{
		bool updateobj = false;
		ActorObjectInfo & ainfo = it->second->GetInfo();

		ainfo.ExtraInfo.Name = _objectmodel->data(_objectmodel->GetIndex(1, 3, parentIdx)).toString().toAscii().data();

		ainfo.PhysicDesc.Pos.X = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toFloat();
		ainfo.PhysicDesc.Pos.Y = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toFloat();
		ainfo.PhysicDesc.Pos.Z = _objectmodel->data(_objectmodel->GetIndex(1, 9, parentIdx)).toFloat();
		ainfo.PhysicDesc.Pos.Rotation = _objectmodel->data(_objectmodel->GetIndex(1, 10, parentIdx)).toFloat();

		int index = 11;

		if(ainfo.PhysicDesc.TypeShape != LbaNet::NoShape)
		{
			ainfo.PhysicDesc.Collidable = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toBool();
			++index;

			if(ainfo.PhysicDesc.TypeShape != LbaNet::TriangleMeshShape)
			{
				ainfo.PhysicDesc.SizeX = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
				++index;
				ainfo.PhysicDesc.SizeY = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
				++index;
				ainfo.PhysicDesc.SizeZ = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
				++index;
			}
			else
			{
				ainfo.PhysicDesc.Filename = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toAscii().data();
				++index;
			}
		}

		ainfo.DisplayDesc.UseLight = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toBool();
		++index;

		ainfo.DisplayDesc.CastShadow = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toBool();
		++index;

		ainfo.DisplayDesc.TransX = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.TransY = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.TransZ = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;

		ainfo.DisplayDesc.RotX = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.RotY = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.RotZ = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;

		ainfo.DisplayDesc.ScaleX = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.ScaleY = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;
		ainfo.DisplayDesc.ScaleZ = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toFloat();
		++index;


		ainfo.DisplayDesc.ModelName = _objectmodel->data(_objectmodel->GetIndex(1, index, parentIdx)).toString().toAscii().data();
		++index;

		
		{
		LbaNet::PhysicalActorType before = ainfo.PhysicDesc.TypePhysO;
		std::string type = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();
		if(type == "Static") 
			ainfo.PhysicDesc.TypePhysO = LbaNet::StaticAType;
		if(type == "Scripted") 
			ainfo.PhysicDesc.TypePhysO = LbaNet::KynematicAType;
		if(type == "Movable") 
			ainfo.PhysicDesc.TypePhysO = LbaNet::CharControlAType;
		if(before != ainfo.PhysicDesc.TypePhysO)
			updateobj = true;

		LbaNet::RenderTypeEnum befored = ainfo.DisplayDesc.TypeRenderer;
		std::string dtype = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toString().toAscii().data();
		if(dtype == "Osg Model") 
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderOsgModel;
		if(dtype == "Sprite") 
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderSprite;
		if(dtype == "Lba1 Model") 
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderLba1M;
		if(dtype == "Lba2 Model") 
			ainfo.DisplayDesc.TypeRenderer = LbaNet::RenderLba2M;
		if(befored != ainfo.DisplayDesc.TypeRenderer)
			updateobj = true;

		LbaNet::PhysicalShapeEnum beforep = ainfo.PhysicDesc.TypeShape;
		std::string ptype = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toString().toAscii().data();
		if(ptype == "No Shape") 
			ainfo.PhysicDesc.TypeShape = LbaNet::NoShape;
		if(ptype == "Box") 
			ainfo.PhysicDesc.TypeShape = LbaNet::BoxShape;
		if(ptype == "Capsule") 
			ainfo.PhysicDesc.TypeShape = LbaNet::CapsuleShape;
		if(ptype == "Sphere") 
			ainfo.PhysicDesc.TypeShape = LbaNet::SphereShape;
		if(ptype == "Triangle Mesh") 
			ainfo.PhysicDesc.TypeShape = LbaNet::TriangleMeshShape;
		if(beforep != ainfo.PhysicDesc.TypeShape)
			updateobj = true;
		}


		//inform server
		std::string mapname = _uieditor.label_mapname->text().toAscii().data();
		EditorUpdateBasePtr update = new UpdateEditor_AddOrModActor(it->second);
		SharedDataHandler::getInstance()->EditorUpdate(mapname, update);


		// update list
		{
			std::string type;
			switch(ainfo.PhysicDesc.TypePhysO)
			{
				case LbaNet::StaticAType:
					type = "Static";
				break;

				case LbaNet::KynematicAType:
					type = "Scripted";
				break;

				case LbaNet::CharControlAType:
					type = "Movable";
				break;  
			}


			std::string dtype;
			switch(ainfo.DisplayDesc.TypeRenderer)
			{
				case LbaNet::RenderOsgModel:
					dtype = "Osg Model";
				break;

				case RenderSprite:
					dtype = "Sprite";
				break;

				case RenderLba1M:
					dtype = "Lba1 Model";
				break;

				case RenderLba2M:
					dtype = "Lba2 Model";
				break;

				case RenderCross:
					dtype = "Cross";
				break;

				case RenderBox:
					dtype = "Box";
				break;

				case RenderCapsule:
					dtype = "Capsule";
				break;
			}

			std::string ptype;
			switch(ainfo.PhysicDesc.TypeShape)
			{
				case LbaNet::NoShape:
					ptype = "No Shape";
				break;
				case BoxShape:
					ptype = "Box";
				break;
				case CapsuleShape:
					ptype = "Capsule";
				break;
				case SphereShape:
					ptype = "Sphere";
				break;
				case TriangleMeshShape:
					ptype = "Triangle Mesh";
				break;
			}

			std::string name = ainfo.ExtraInfo.Name;
			if(name == "")
				name = "-";

			QStringList data;
			data << name.c_str()<< type.c_str()  << dtype.c_str() << ptype.c_str();
			_actorlistmodel->AddOrUpdateRow(id, data);
		}



		//refresh object
		if(updateobj)
			SelectActor(id);


		SetMapModified();
		UpdateSelectedActorDisplay(ainfo.PhysicDesc);
	}
}


/***********************************************************
update editor selected ector display
***********************************************************/
void EditorHandler::UpdateSelectedActorDisplay(LbaNet::ObjectPhysicDesc desc)
{
	RemoveSelectedActorDislay();

	_actornode = OsgHandler::getInstance()->AddEmptyActorNode(false);

	osg::Matrixd Trans;
	osg::Matrixd Rotation;
	Trans.makeTranslate(desc.Pos.X, desc.Pos.Y, desc.Pos.Z);
	LbaQuaternion Q(desc.Pos.Rotation, LbaVec3(0,1,0));
	Rotation.makeRotate(osg::Quat(Q.X, Q.Y, Q.Z, Q.W));
	_actornode->setMatrix(Rotation * Trans);

	switch(desc.TypeShape)
	{
		case LbaNet::NoShape:
		{
			osg::Geode* lineGeode = new osg::Geode();
			osg::Geometry* lineGeometry = new osg::Geometry();
			lineGeode->addDrawable(lineGeometry); 

			osg::Vec3Array* lineVertices = new osg::Vec3Array();
			lineVertices->push_back( osg::Vec3( -2, 0, 0) );
			lineVertices->push_back( osg::Vec3(2, 0, 0) );
			lineVertices->push_back( osg::Vec3(0 , 0, -2) );
			lineVertices->push_back( osg::Vec3(0 , 0, 2) );
			lineGeometry->setVertexArray( lineVertices ); 

			osg::Vec4Array* linecolor = new osg::Vec4Array();
			linecolor->push_back( osg::Vec4( 0.2, 0.2, 1.0, 0.5) );
			lineGeometry->setColorArray(linecolor);
			lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

			osg::DrawElementsUInt* dunit = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
			dunit->push_back(0);
			dunit->push_back(1);
			dunit->push_back(2);
			dunit->push_back(3);
			lineGeometry->addPrimitiveSet(dunit); 
			_actornode->addChild(lineGeode);

			osg::StateSet* stateset = lineGeometry->getOrCreateStateSet();
			osg::LineWidth* linewidth = new osg::LineWidth();

			linewidth->setWidth(3.0f);
			stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
			stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			stateset->setRenderBinDetails( 8000, "RenderBin");


		}
		break;
		case LbaNet::BoxShape:
		{
			osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
			osg::ref_ptr<osg::Box> caps(new osg::Box(osg::Vec3(0,(desc.SizeY/2),0), desc.SizeX, desc.SizeY, desc.SizeZ));
			osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
			capsdraw->setColor(osg::Vec4(0.2, 0.2, 1.0, 0.4));
			capsuleGeode->addDrawable(capsdraw);
			_actornode->addChild(capsuleGeode);

			osg::StateSet* stateset = capsuleGeode->getOrCreateStateSet();
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			stateset->setRenderBinDetails( 8000, "RenderBin");	
		}
		break;
		case CapsuleShape:
		{
			osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
			osg::ref_ptr<osg::Capsule> caps(new osg::Capsule(osg::Vec3(0,desc.SizeY/2,0),desc.SizeX/2,desc.SizeY-desc.SizeX));
			osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
			capsdraw->setColor(osg::Vec4(0.2, 0.2, 1.0, 0.4));
			capsuleGeode->addDrawable(capsdraw);
			_actornode->addChild(capsuleGeode);

			osg::Matrixd RotationC;
			LbaQuaternion QC(90, LbaVec3(1,0,0));
			RotationC.makeRotate(osg::Quat(QC.X, QC.Y, QC.Z, QC.W));
			osg::Quat quatC;
			quatC.set(RotationC);
			caps->setRotation(quatC);

			osg::StateSet* stateset = capsuleGeode->getOrCreateStateSet();
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			stateset->setRenderBinDetails( 8000, "RenderBin");	
		}
		break;
		case SphereShape:
		{
			osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
			osg::ref_ptr<osg::Sphere> caps(new osg::Sphere(osg::Vec3(0,desc.SizeY/2,0),desc.SizeY/2));
			osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
			capsdraw->setColor(osg::Vec4(0.2, 0.2, 1.0, 0.4));
			capsuleGeode->addDrawable(capsdraw);
			_actornode->addChild(capsuleGeode);

			osg::StateSet* stateset = capsuleGeode->getOrCreateStateSet();
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			stateset->setRenderBinDetails( 8000, "RenderBin");	
		}
		break;
		case TriangleMeshShape:
		{
			std::ifstream file(("Data/"+desc.Filename).c_str(), std::ifstream::binary);
			if(!file.is_open())
				return;

			unsigned int sizevertex;
			unsigned int sizeindices;
			unsigned int sizematerials;

			file.read((char*)&sizevertex, sizeof(unsigned int));
			file.read((char*)&sizeindices, sizeof(unsigned int));
			file.read((char*)&sizematerials, sizeof(unsigned int));

			float *buffervertex = new float[sizevertex];
			unsigned int *bufferindices = new unsigned int[sizeindices];
			file.read((char*)buffervertex, sizevertex*sizeof(float));
			file.read((char*)bufferindices, sizeindices*sizeof(unsigned int));

			osg::ref_ptr<osg::Geode> myGeode = new osg::Geode();
			_actornode->addChild(myGeode);
			osg::ref_ptr<osg::Geometry> m_myGeometry = new osg::Geometry();
			myGeode->addDrawable(m_myGeometry.get());

			osg::Vec3Array* myVerticesPoly = new osg::Vec3Array;
			for(unsigned int i=0; i<sizevertex; i+=3)
				myVerticesPoly->push_back(osg::Vec3(buffervertex[i], buffervertex[i+1], buffervertex[i+2]));
			m_myGeometry->setVertexArray( myVerticesPoly ); 

			osg::DrawElementsUInt* myprimitive = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
			for(unsigned int i=0; i<sizeindices; ++i)
				myprimitive->push_back(bufferindices[i]);

			m_myGeometry->addPrimitiveSet(myprimitive);

			osgUtil::SmoothingVisitor::smooth(*(m_myGeometry.get()));

			osg::Vec4Array* colors = new osg::Vec4Array;
			colors->push_back(osg::Vec4(0.2, 0.2, 1.0, 0.4));
			m_myGeometry->setColorArray(colors);
			m_myGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

			delete[] buffervertex;
			delete[] bufferindices;

			osg::StateSet* stateset = myGeode->getOrCreateStateSet();
			stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			stateset->setRenderBinDetails( 8000, "RenderBin");	

		}
		break;
	}


}


/***********************************************************
remove current selected display
***********************************************************/
void EditorHandler::RemoveSelectedActorDislay()
{
	if(_actornode)
		OsgHandler::getInstance()->RemoveActorNode(_actornode, false);

	_actornode = NULL;
}


/***********************************************************
called when an object is picked
***********************************************************/
void EditorHandler::PickedObject(const std::string & name)
{
	if(_draggerX && _draggerX->getDraggerActive())
		return;

	if(_draggerY && _draggerY->getDraggerActive())
		return;

	if(_draggerZ && _draggerZ->getDraggerActive())
		return;

	if(name == "")
	{
		ResetObject();
		return;
	}

	long id = atol(name.substr(2).c_str());


	switch(name[0])
	{
		case 'A':
			SelectActor(id);
		break;
		case 'E':
			if(id > 2000000)
				SelectTrigger(id-2000000);
			else if(id > 1000000)
				SelectSpawning(id-1000000);
		break;
		case 'M':
			ResetObject();
		break;
	}
}


/***********************************************************
update editor selected trigger display
***********************************************************/
void EditorHandler::UpdateSelectedZoneTriggerDisplay(float PosX, float PosY, float PosZ,
													 float SizeX, float SizeY, float SizeZ)
{
	RemoveSelectedActorDislay();

	_actornode = OsgHandler::getInstance()->AddEmptyActorNode(false);

	osg::Matrixd Trans;
	Trans.makeTranslate(PosX, PosY, PosZ);
	_actornode->setMatrix(Trans);


	osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
	osg::ref_ptr<osg::Box> caps(new osg::Box(osg::Vec3(0,SizeY/2,0), SizeX, SizeY, SizeZ));
	osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
	capsdraw->setColor(osg::Vec4(0.5, 0.0, 0.0, 1.0));
	capsuleGeode->addDrawable(capsdraw);
	_actornode->addChild(capsuleGeode);

	osg::StateSet* stateset = capsuleGeode->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setRenderingHint( osg::StateSet::OPAQUE_BIN );
	stateset->setRenderBinDetails( 9100, "RenderBin");	

	osg::PolygonMode* polymode = new osg::PolygonMode;
	polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
	stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
}


/***********************************************************
update editor selected trigger display
***********************************************************/
void EditorHandler::UpdateSelectedDistanceTriggerDisplay(float PosX, float PosY, float PosZ, float distance)
{
	RemoveSelectedActorDislay();

	_actornode = OsgHandler::getInstance()->AddEmptyActorNode(false);

	osg::Matrixd Trans;
	Trans.makeTranslate(PosX, PosY, PosZ);
	_actornode->setMatrix(Trans);


	osg::ref_ptr<osg::Geode> capsuleGeode(new osg::Geode());
	osg::ref_ptr<osg::Sphere> caps(new osg::Sphere(osg::Vec3(0,0,0), distance));
	osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
	capsdraw->setColor(osg::Vec4(0.5, 0.0, 0.0, 0.2));
	capsuleGeode->addDrawable(capsdraw);
	_actornode->addChild(capsuleGeode);

	osg::StateSet* stateset = capsuleGeode->getOrCreateStateSet();
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setRenderBinDetails( 9100, "RenderBin");	
}





/***********************************************************
draw arrow on selected object
***********************************************************/
void EditorHandler::DrawArrows(float PosX, float PosY, float PosZ)
{
	RemoveArrows();

	_arrownode = OsgHandler::getInstance()->AddEmptyActorNode(false);

	osg::ref_ptr<osg::Camera> PostRenderCam = new osg::Camera;
	PostRenderCam->setClearMask(GL_DEPTH_BUFFER_BIT);
	PostRenderCam->setRenderOrder(osg::Camera::POST_RENDER, 90000);
	_arrownode->addChild(PostRenderCam);

	{
		_draggerX = new osgManipulator::Translate1DDragger(osg::Vec3d(0,0,0), osg::Vec3d(1,0,0));
		_draggerX->setCheckForNodeInNodePath(false);

		osg::Matrixd Trans;
		Trans.makeTranslate(PosX, PosY, PosZ);
		osg::Matrixd Scale;
		Scale.makeScale(1, 2, 1);
		_draggerX->setMatrix(Scale*Trans);
		_draggerX->addDraggerCallback(new DraggerCustomCallback(1));
		_draggerX->setHandleEvents(true);
		_draggerX->setColor(osg::Vec4(0.0, 0.8, 0.0, 1.0));
		PostRenderCam->addChild(_draggerX);

		osg::ref_ptr<osg::Geode> myGeode(new osg::Geode());

		osg::ref_ptr<osg::Cylinder> caps(new osg::Cylinder(osg::Vec3(1,0,0), 0.1, 2));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
		myGeode->addDrawable(capsdraw);

		osg::ref_ptr<osg::Cone> caps2(new osg::Cone(osg::Vec3(2,0,0), 0.2, 0.5));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw2 = new osg::ShapeDrawable(caps2);
		myGeode->addDrawable(capsdraw2);

		_draggerX->addChild(myGeode);

		osg::Matrixd RotationC;
		LbaQuaternion QC(90, LbaVec3(0,1,0));
		RotationC.makeRotate(osg::Quat(QC.X, QC.Y, QC.Z, QC.W));
		osg::Quat quatC;
		quatC.set(RotationC);
		caps->setRotation(quatC);
		caps2->setRotation(quatC);
	}


	{
		_draggerY = new osgManipulator::Translate1DDragger(osg::Vec3d(0,0,0), osg::Vec3d(0,1,0));
		_draggerY->setCheckForNodeInNodePath(false);

		osg::Matrixd Trans;
		Trans.makeTranslate(PosX, PosY, PosZ);
		osg::Matrixd Scale;
		Scale.makeScale(1, 2, 1);
		_draggerY->setMatrix(Scale*Trans);
		_draggerY->addDraggerCallback(new DraggerCustomCallback(2));
		_draggerY->setHandleEvents(true);
		_draggerY->setColor(osg::Vec4(0.8, 0.0, 0.0, 1.0));

		PostRenderCam->addChild(_draggerY);


		osg::ref_ptr<osg::Geode> myGeode(new osg::Geode());
		osg::ref_ptr<osg::Cylinder> caps(new osg::Cylinder(osg::Vec3(0,1,0), 0.1, 2));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
		myGeode->addDrawable(capsdraw);

		osg::ref_ptr<osg::Cone> caps2(new osg::Cone(osg::Vec3(0,2,0), 0.2, 0.5));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw2 = new osg::ShapeDrawable(caps2);
		myGeode->addDrawable(capsdraw2);

		_draggerY->addChild(myGeode);


		osg::Matrixd RotationC;
		LbaQuaternion QC(270, LbaVec3(1,0,0));
		RotationC.makeRotate(osg::Quat(QC.X, QC.Y, QC.Z, QC.W));
		osg::Quat quatC;
		quatC.set(RotationC);
		caps->setRotation(quatC);
		caps2->setRotation(quatC);
	}

	{
		_draggerZ = new osgManipulator::Translate1DDragger(osg::Vec3d(0,0,0), osg::Vec3d(0,0,1));
		_draggerZ->setCheckForNodeInNodePath(false);

		osg::Matrixd Trans;
		Trans.makeTranslate(PosX, PosY, PosZ);
		osg::Matrixd Scale;
		Scale.makeScale(1, 2, 1);
		_draggerZ->setMatrix(Scale*Trans);
		_draggerZ->addDraggerCallback(new DraggerCustomCallback(3));
		_draggerZ->setHandleEvents(true);
		_draggerZ->setColor(osg::Vec4(0.0, 0.0, 0.8, 1.0));
		PostRenderCam->addChild(_draggerZ);


		osg::ref_ptr<osg::Geode> myGeode(new osg::Geode());

		osg::ref_ptr<osg::Cylinder> caps(new osg::Cylinder(osg::Vec3(0,0,1), 0.1, 2));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw = new osg::ShapeDrawable(caps);
		myGeode->addDrawable(capsdraw);

		osg::ref_ptr<osg::Cone> caps2(new osg::Cone(osg::Vec3(0,0,2), 0.2, 0.5));
		osg::ref_ptr<osg::ShapeDrawable> capsdraw2 = new osg::ShapeDrawable(caps2);
		myGeode->addDrawable(capsdraw2);

		_draggerZ->addChild(myGeode);
	}

	return;
}



/***********************************************************
when an editor arrow was dragged by mouse
***********************************************************/
void EditorHandler::PickedArrowMoved(int pickedarrow)
{
	if(_objectmodel->rowCount() > 2)
	{
		QString type = _objectmodel->data(_objectmodel->GetIndex(1, 0)).toString();
		std::string category = _objectmodel->data(_objectmodel->GetIndex(1, 1)).toString().toAscii().data();
		long objid = _objectmodel->data(_objectmodel->GetIndex(1, 2)).toString().toLong();


		if(type == "Trigger")
		{
			if(category == "ZoneTrigger")
			{
				if(pickedarrow == 1)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 9), floor(_draggerX->getMatrix().getTrans().x()*10)/10);
					_draggerY->setMatrix(_draggerX->getMatrix());
					_draggerZ->setMatrix(_draggerX->getMatrix());
				}

				if(pickedarrow == 2)
				{	
					_objectmodel->setData(_objectmodel->GetIndex(1, 10), floor(_draggerY->getMatrix().getTrans().y()*10)/10);
					_draggerX->setMatrix(_draggerY->getMatrix());
					_draggerZ->setMatrix(_draggerY->getMatrix());
				}

				if(pickedarrow == 3)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 11), floor(_draggerZ->getMatrix().getTrans().z()*10)/10);
					_draggerX->setMatrix(_draggerZ->getMatrix());
					_draggerY->setMatrix(_draggerZ->getMatrix());
				}
				
				return;
			}

			if(category == "ActivationTrigger")
			{
				if(pickedarrow == 1)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 5), floor(_draggerX->getMatrix().getTrans().x()*10)/10);
					_draggerY->setMatrix(_draggerX->getMatrix());
					_draggerZ->setMatrix(_draggerX->getMatrix());
				}

				if(pickedarrow == 2)
				{	
					_objectmodel->setData(_objectmodel->GetIndex(1, 6), floor(_draggerY->getMatrix().getTrans().y()*10)/10);
					_draggerX->setMatrix(_draggerY->getMatrix());
					_draggerZ->setMatrix(_draggerY->getMatrix());
				}

				if(pickedarrow == 3)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 7), floor(_draggerZ->getMatrix().getTrans().z()*10)/10);
					_draggerX->setMatrix(_draggerZ->getMatrix());
					_draggerY->setMatrix(_draggerZ->getMatrix());
				}

				return;
			}

			if(category == "ZoneActionTrigger")
			{
				if(pickedarrow == 1)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 5), floor(_draggerX->getMatrix().getTrans().x()*10)/10);
					_draggerY->setMatrix(_draggerX->getMatrix());
					_draggerZ->setMatrix(_draggerX->getMatrix());
				}

				if(pickedarrow == 2)
				{	
					_objectmodel->setData(_objectmodel->GetIndex(1, 6), floor(_draggerY->getMatrix().getTrans().y()*10)/10);
					_draggerX->setMatrix(_draggerY->getMatrix());
					_draggerZ->setMatrix(_draggerY->getMatrix());
				}

				if(pickedarrow == 3)
				{
					_objectmodel->setData(_objectmodel->GetIndex(1, 7), floor(_draggerZ->getMatrix().getTrans().z()*10)/10);
					_draggerX->setMatrix(_draggerZ->getMatrix());
					_draggerY->setMatrix(_draggerZ->getMatrix());
				}

				return;
			}
			return;
		}

		if(type == "Actor")
		{
			osg::Vec3 center = _actornode->computeBound().center();

			if(pickedarrow == 1)
			{
				float nX = _objectmodel->data(_objectmodel->GetIndex(1, 7)).toString().toFloat();
				nX += floor(_draggerX->getMatrix().getTrans().x()*10)/10 - center.x();
				_objectmodel->setData(_objectmodel->GetIndex(1, 7), nX);
				_draggerY->setMatrix(_draggerX->getMatrix());
				_draggerZ->setMatrix(_draggerX->getMatrix());
			}

			if(pickedarrow == 2)
			{	
				float nY = _objectmodel->data(_objectmodel->GetIndex(1, 8)).toString().toFloat();
				nY += floor(_draggerY->getMatrix().getTrans().y()*10)/10 - center.y();
				_objectmodel->setData(_objectmodel->GetIndex(1, 8), nY);
				_draggerX->setMatrix(_draggerY->getMatrix());
				_draggerZ->setMatrix(_draggerY->getMatrix());
			}

			if(pickedarrow == 3)
			{
				float nZ = _objectmodel->data(_objectmodel->GetIndex(1, 9)).toString().toFloat();
				nZ += floor(_draggerZ->getMatrix().getTrans().z()*10)/10 - center.z();
				_objectmodel->setData(_objectmodel->GetIndex(1, 9), nZ);
				_draggerX->setMatrix(_draggerZ->getMatrix());
				_draggerY->setMatrix(_draggerZ->getMatrix());
			}

			return;
		}
	}
}


/***********************************************************
when an editor arrow was dragged by mouse
***********************************************************/
void EditorHandler::RemoveArrows()
{
	if(_arrownode)
	{
		OsgHandler::getInstance()->RemoveActorNode(_arrownode, false);
		_arrownode = NULL;
		_draggerX = NULL;
		_draggerY = NULL;
		_draggerZ = NULL;
	}
}


/***********************************************************
only used by the editor to add client scripts to the list
***********************************************************/
void EditorHandler::EditorAddClientScript(boost::shared_ptr<ClientScriptBase> script)
{
	_cscripts[script->GetId()] = script;
	_curscriptidx = script->GetId();

	QStringList data;
	data << script->GetName().c_str() << script->GetTypeName().c_str();
	_cscriptlistmodel->AddOrUpdateRow(script->GetId(), data);

	_cscriptList->AddData(script->GetName().c_str());

	// refresh client script
	std::stringstream strs;
	script->SaveScriptToLua(strs);
	EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::ClientExecuteScriptStringEvent(
		SynchronizedTimeHandler::GetCurrentTimeDouble(), strs.str()));	
}


/***********************************************************
addcscript_button_clicked
***********************************************************/
void EditorHandler::addcscript_button_clicked()
{
	if(!IsWorldOpened())
		return;

	_ui_addcscriptdialog.lineEdit_name->setText("");
	_addcscriptdialog->show();
}


/***********************************************************
removecscript_button_clicked
***********************************************************/
void EditorHandler::removecscript_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewCScriptsList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		// inform server
		long id = _cscriptlistmodel->GetId(indexes[0]);
		std::string name = _cscriptlistmodel->GetString(indexes[0]).toAscii().data();
		
		//remove from map
		std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator itm = _cscripts.find(id);
		if(itm != _cscripts.end())
			_cscripts.erase(itm);

		// remove row from table
		_cscriptlistmodel->removeRows(indexes[0].row(), 1);

		// remove from list
		_cscriptList->RemoveData(name.c_str());

		// clear object
		ClearObjectIfSelected("ClientScript", id);

		// tell user to save change
		SetModified();
	}
}


/***********************************************************
selectcscript_button_clicked
***********************************************************/
void EditorHandler::selectcscript_button_clicked()
{
	QItemSelectionModel *selectionModel = _uieditor.tableViewCScriptsList->selectionModel();
	QModelIndexList indexes = selectionModel->selectedIndexes();
	if(indexes.size() > 1)
	{
		// inform server
		long id = _cscriptlistmodel->GetId(indexes[0]);

		SelectCScript(id);
	}
}

/***********************************************************
addcscript_accepted
***********************************************************/
void EditorHandler::addcscript_accepted()
{

	QString csname = _ui_addcscriptdialog.lineEdit_name->text();

	// user forgot to set a name
	if(csname == "")
		return;

	if(_cscriptList->stringList().contains(csname))
	{
		QMessageBox::warning(this, tr("Name already used"),
			tr("The name you entered for the script is already used. Please enter a unique name."),
			QMessageBox::Ok);
		return;
	}

	_addcscriptdialog->hide();



	long id = _curscriptidx+1;


	switch(_ui_addcscriptdialog.comboBox_type->currentIndex())
	{
		case 0: // go up ladder
		{
			boost::shared_ptr<ClientScriptBase> script(new GoUpLadderScript(id, csname.toAscii().data(),
																			_posX, _posY, _posZ, 5, 0));

			EditorAddClientScript(script);

		}
		break;

		case 1: // exit up
		{
			boost::shared_ptr<ClientScriptBase> script(new TakeExitUpScript(id, csname.toAscii().data(),
				_posX, _posY, _posZ, 0));

			EditorAddClientScript(script);

		}
		break;

		case 2: // exit down
		{
			boost::shared_ptr<ClientScriptBase> script(new TakeExitDownScript(id, csname.toAscii().data(),
				_posX, _posY, _posZ, 0));

			EditorAddClientScript(script);

		}
		break;

		case 3: // custom script
		{
			boost::shared_ptr<ClientScriptBase> script(new CustomScript(id, csname.toAscii().data(), ""));

			EditorAddClientScript(script);

		}
		break;

	}

	SelectCScript(id);

	// tell user to save change
	SetModified();


	switch(_updateactiondialogonnewscript)
	{
		case 1:
			{
				int index = _ui_addactiondialog.comboBox_script_name->findText(csname);
				if(index >= 0)
					_ui_addactiondialog.comboBox_script_name->setCurrentIndex(index);
			}
		break;
	}

	_updateactiondialogonnewscript = -1;
}

/***********************************************************
selectcscript_double_clicked
***********************************************************/
void EditorHandler::selectcscript_double_clicked( const QModelIndex & itm )
{
	selectcscript_button_clicked();
}

/***********************************************************
SelectCScript
***********************************************************/
void EditorHandler::SelectCScript(long id, const QModelIndex &parent)
{
	std::map<long, boost::shared_ptr<ClientScriptBase> >::iterator it = _cscripts.find(id);
	if(it != _cscripts.end())
	{
		if(parent == QModelIndex())
			ResetObject();

		{
			QVector<QVariant> data;
			data<<"Type"<<"ClientScript";
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"SubCategory"<<it->second->GetTypeName().c_str();
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Id"<<id;
			_objectmodel->AppendRow(data, parent, true);
		}

		{
			QVector<QVariant> data;
			data<<"Name"<<it->second->GetName().c_str();
			_objectmodel->AppendRow(data, parent);
		}

		std::string actiontype = it->second->GetTypeName();
		if(actiontype == "GoUpLadderScript")
		{
			GoUpLadderScript* ptr = static_cast<GoUpLadderScript*>(it->second.get());

			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetLadderPositionX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetLadderPositionY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetLadderPositionZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Ladder Height" << (double)ptr->GetLadderHeight();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Ladder Direction" << ptr->GetLadderDirection();
				_objectmodel->AppendRow(data, parent);
			}

			UpdateSelectedGoUpLadderScriptDisplay(ptr->GetLadderPositionX(), ptr->GetLadderPositionY(), ptr->GetLadderPositionZ(),
													ptr->GetLadderHeight(), ptr->GetLadderDirection());


			return;
		}

		if(actiontype == "TakeExitUpScript")
		{
			TakeExitUpScript* ptr = static_cast<TakeExitUpScript*>(it->second.get());

			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetExitPositionX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetExitPositionY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetExitPositionZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Exit Direction" << ptr->GetExitDirection();
				_objectmodel->AppendRow(data, parent);
			}

			UpdateSelectedGoUpLadderScriptDisplay(ptr->GetExitPositionX(), ptr->GetExitPositionY(), ptr->GetExitPositionZ(),
														0, ptr->GetExitDirection());


			return;
		}

		if(actiontype == "TakeExitDownScript")
		{
			TakeExitDownScript* ptr = static_cast<TakeExitDownScript*>(it->second.get());

			{
				QVector<QVariant> data;
				data << "Position X" << (double)ptr->GetExitPositionX();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Y" << (double)ptr->GetExitPositionY();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Position Z" << (double)ptr->GetExitPositionZ();
				_objectmodel->AppendRow(data, parent);
			}

			{
				QVector<QVariant> data;
				data << "Exit Direction" << ptr->GetExitDirection();
				_objectmodel->AppendRow(data, parent);
			}

			UpdateSelectedGoUpLadderScriptDisplay(ptr->GetExitPositionX(), ptr->GetExitPositionY(), ptr->GetExitPositionZ(),
				0, ptr->GetExitDirection());


			return;
		}

		if(actiontype == "CustomScript")
		{
			CustomScript* ptr = static_cast<CustomScript*>(it->second.get());

			{
				QVector<QVariant> data;
				data << "Lua function name" << ptr->LuaFunctionName().c_str();
				_objectmodel->AppendRow(data, parent);
			}
			return;
		}
	}
}

/***********************************************************
SaveEditorLua
***********************************************************/
void EditorHandler::SaveEditorLua( const std::string & filename )
{
	std::ofstream file(filename.c_str());
	file<<"function InitEditor(environment)"<<std::endl;

	std::map<long, boost::shared_ptr<ClientScriptBase> >::const_iterator it = _cscripts.begin();
	std::map<long, boost::shared_ptr<ClientScriptBase> >::const_iterator end = _cscripts.end();
	for(; it != end; ++it)
		it->second->SaveToLuaFile(file);


	file<<"end"<<std::endl;
}


/***********************************************************
SaveGlobalClientLua
***********************************************************/
void EditorHandler::SaveGlobalClientLua( const std::string & filename )
{
	std::ofstream file(filename.c_str());

	std::map<long, boost::shared_ptr<ClientScriptBase> >::const_iterator it = _cscripts.begin();
	std::map<long, boost::shared_ptr<ClientScriptBase> >::const_iterator end = _cscripts.end();
	for(; it != end; ++it)
		it->second->SaveScriptToLua(file);
}


/***********************************************************
UpdateSelectedGoUpLadderScriptDisplay
***********************************************************/
void EditorHandler::UpdateSelectedGoUpLadderScriptDisplay( float posX, float posY, float posZ, 
															float Height, int Direction )
{
	RemoveSelectedScriptDislay();

	_scriptnode = OsgHandler::getInstance()->AddEmptyActorNode(false);

	osg::Matrixd Trans;
	osg::Matrixd Rotation;
	Trans.makeTranslate(posX, posY, posZ);
	LbaQuaternion Q(Direction+180, LbaVec3(0,1,0));
	Rotation.makeRotate(osg::Quat(Q.X, Q.Y, Q.Z, Q.W));
	_scriptnode->setMatrix(Rotation * Trans);

	osg::Geode* lineGeode = new osg::Geode();
	osg::Geometry* lineGeometry = new osg::Geometry();
	lineGeode->addDrawable(lineGeometry); 

	osg::Vec3Array* lineVertices = new osg::Vec3Array();
	lineVertices->push_back( osg::Vec3( -1, 0, 0) );
	lineVertices->push_back( osg::Vec3(1, 0, 0) );
	lineVertices->push_back( osg::Vec3(0 , 0, -1) );
	lineVertices->push_back( osg::Vec3(0 , 0, 1) );
	lineVertices->push_back( osg::Vec3(0 , 0, 0) );
	lineVertices->push_back( osg::Vec3(0 , Height, 0) );
	lineVertices->push_back( osg::Vec3( -1, Height, 0) );
	lineVertices->push_back( osg::Vec3(1, Height, 0) );
	lineVertices->push_back( osg::Vec3(0 , Height, -1) );
	lineVertices->push_back( osg::Vec3(0 , Height, 1) );

	lineVertices->push_back( osg::Vec3(0 , Height/2, 0) );
	lineVertices->push_back( osg::Vec3(0 , Height/2, 2) );

	lineGeometry->setVertexArray( lineVertices ); 

	osg::Vec4Array* linecolor = new osg::Vec4Array();
	linecolor->push_back( osg::Vec4( 1.0, 1.0, 0.2, 0.8) );
	lineGeometry->setColorArray(linecolor);
	lineGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	osg::DrawElementsUInt* dunit = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	for(int i=0; i<12; ++i)
		dunit->push_back(i);

	lineGeometry->addPrimitiveSet(dunit); 
	_scriptnode->addChild(lineGeode);

	osg::StateSet* stateset = lineGeometry->getOrCreateStateSet();
	osg::LineWidth* linewidth = new osg::LineWidth();

	linewidth->setWidth(3.0f);
	stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	stateset->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	stateset->setRenderBinDetails( 8000, "RenderBin");

}

/***********************************************************
CScriptObjectChanged
***********************************************************/
void EditorHandler::CScriptObjectChanged( long id, const std::string & category, const QModelIndex &parentIdx )
{
	std::string name = _objectmodel->data(_objectmodel->GetIndex(1, 3, parentIdx)).toString().toAscii().data();
	std::string oldname = _cscripts[id]->GetName();


	// check if name changed already exist
	if(name != oldname)
	{
		if(name == "" || _cscriptList->stringList().contains(name.c_str()))
		{
			QMessageBox::warning(this, tr("Name already used"),
				tr("The name you entered for the script is already used. Please enter a unique name."),
				QMessageBox::Ok);

			_objectmodel->setData(_objectmodel->GetIndex(1, 3, parentIdx), oldname.c_str());
			return;
		}

		// update script name list
		_cscriptList->ReplaceData(oldname.c_str(), name.c_str());
	}


	if(category == "GoUpLadderScript")
	{
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		float height = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toFloat();
		int direction = _objectmodel->data(_objectmodel->GetIndex(1, 8, parentIdx)).toInt();


		// created modified script and replace old one
		boost::shared_ptr<ClientScriptBase> modifiedscript(new GoUpLadderScript
										(id, name, posX, posY, posZ, height, direction));


		// update map
		_cscripts[id] = modifiedscript;


		// update script list display
		QStringList slist;
		slist << name.c_str() << modifiedscript->GetTypeName().c_str();
		_cscriptlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		// update display
		UpdateSelectedGoUpLadderScriptDisplay(posX, posY, posZ, height, direction);

		// refresh client script
		std::stringstream strs;
		modifiedscript->SaveScriptToLua(strs);
		EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::ClientExecuteScriptStringEvent(
									SynchronizedTimeHandler::GetCurrentTimeDouble(), strs.str()));	

		return;
	}



	if(category == "TakeExitUpScript")
	{
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		int direction = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toInt();


		// created modified script and replace old one
		boost::shared_ptr<ClientScriptBase> modifiedscript(new TakeExitUpScript
			(id, name, posX, posY, posZ, direction));


		// update map
		_cscripts[id] = modifiedscript;


		// update script list display
		QStringList slist;
		slist << name.c_str() << modifiedscript->GetTypeName().c_str();
		_cscriptlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		// update display
		UpdateSelectedGoUpLadderScriptDisplay(posX, posY, posZ, 0, direction);

		// refresh client script
		std::stringstream strs;
		modifiedscript->SaveScriptToLua(strs);
		EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::ClientExecuteScriptStringEvent(
			SynchronizedTimeHandler::GetCurrentTimeDouble(), strs.str()));	

		return;
	}



	if(category == "TakeExitDownScript")
	{
		float posX = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toFloat();
		float posY = _objectmodel->data(_objectmodel->GetIndex(1, 5, parentIdx)).toFloat();
		float posZ = _objectmodel->data(_objectmodel->GetIndex(1, 6, parentIdx)).toFloat();
		int direction = _objectmodel->data(_objectmodel->GetIndex(1, 7, parentIdx)).toInt();


		// created modified script and replace old one
		boost::shared_ptr<ClientScriptBase> modifiedscript(new TakeExitDownScript
			(id, name, posX, posY, posZ, direction));


		// update map
		_cscripts[id] = modifiedscript;


		// update script list display
		QStringList slist;
		slist << name.c_str() << modifiedscript->GetTypeName().c_str();
		_cscriptlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();


		// update display
		UpdateSelectedGoUpLadderScriptDisplay(posX, posY, posZ, 0, direction);

		// refresh client script
		std::stringstream strs;
		modifiedscript->SaveScriptToLua(strs);
		EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::ClientExecuteScriptStringEvent(
			SynchronizedTimeHandler::GetCurrentTimeDouble(), strs.str()));	

		return;
	}


	if(category == "CustomScript")
	{
		std::string fctname = _objectmodel->data(_objectmodel->GetIndex(1, 4, parentIdx)).toString().toAscii().data();



		// created modified script and replace old one
		boost::shared_ptr<ClientScriptBase> modifiedscript(new CustomScript
			(id, name, fctname));


		// update map
		_cscripts[id] = modifiedscript;


		// update script list display
		QStringList slist;
		slist << name.c_str() << modifiedscript->GetTypeName().c_str();
		_cscriptlistmodel->AddOrUpdateRow(id, slist);


		// need to save as something changed
		SetModified();

		// refresh client script
		std::stringstream strs;
		modifiedscript->SaveScriptToLua(strs);
		EventsQueue::getReceiverQueue()->AddEvent(new LbaNet::ClientExecuteScriptStringEvent(
			SynchronizedTimeHandler::GetCurrentTimeDouble(), strs.str()));	

		return;
	}
}


/***********************************************************
remove current selected display
***********************************************************/
void EditorHandler::RemoveSelectedScriptDislay()
{
	if(_scriptnode)
		OsgHandler::getInstance()->RemoveActorNode(_scriptnode, false);

	_scriptnode = NULL;
}

/***********************************************************
reset action dialog
***********************************************************/
void EditorHandler::ResetActionDialog()
{
	_ui_addactiondialog.frame_TP->hide();
	_ui_addactiondialog.frame_script->hide();
	_ui_addactiondialog.frame_customAct->hide();
	_ui_addactiondialog.frame_displaytext->hide();
	_addactiondialog->resize(300, 100);
}

/***********************************************************
set type of action dialog
***********************************************************/
void EditorHandler::SetActionDialogType( int type )
{
	ResetActionDialog();

	switch(type)
	{
		case 0:
		{
			_ui_addactiondialog.frame_TP->show();
			_ui_addactiondialog.comboBox_tp_map->setModel(_maplistmodel);
			actiond_tpmap_changed(0);
		}
		break;

		case 1:
		{
			_ui_addactiondialog.frame_script->show();
			_ui_addactiondialog.comboBox_script_name->setModel(_cscriptlistmodel);
		}
		break;

		case 2:
		{
			_ui_addactiondialog.frame_customAct->show();
		}
		break;

		case 3:
		{
			_ui_addactiondialog.frame_displaytext->show();
		}
		break;
	}
}

/***********************************************************
actiond_tpmap_changed
***********************************************************/
void EditorHandler::actiond_tpmap_changed( int type )
{
	std::string mapname = _ui_addactiondialog.comboBox_tp_map->currentText().toAscii().data();

	std::map<std::string, boost::shared_ptr<CustomStringListModel> >::iterator it = 
		_mapSpawningList.find(mapname);

	if(it != _mapSpawningList.end())
		_ui_addactiondialog.comboBox_tp_spawn->setModel(it->second.get());
}



/***********************************************************
add script button clicked
***********************************************************/
void EditorHandler::actiond_scriptadd_clicked()
{
	_updateactiondialogonnewscript = 1;
	addcscript_button_clicked();
}