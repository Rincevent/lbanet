#include "MapHandler.h"
#include "SynchronizedTimeHandler.h"
#include "SharedDataHandler.h"

#define _THREAD_WAIT_TIME_	17




/***********************************************************
constructor
***********************************************************/
EventsSender::EventsSender(EventsSeq & events, ClientInterfacePrx proxy)
: _proxy(proxy)
{
	_events.swap(events);
}

/***********************************************************
running function of the thread
***********************************************************/
void EventsSender::run()
{
	try
	{
		if(_proxy)
			_proxy->ServerEvents(_events);
	}
	catch(const IceUtil::Exception& ex)
	{
		std::cout<<"Exception in EventSender: "<<ex.what()<<std::endl;
	}
	catch(...)
	{
		std::cout<<"Unknown exception in EventSender. "<<std::endl;
	}
}




/***********************************************************
constructor
***********************************************************/
EventsSenderToAll::EventsSenderToAll(EventsSeq & events, std::map<Ice::Long, ClientInterfacePrx> &proxies)
{
	_events.swap(events);
	_proxies.swap(proxies);
}


/***********************************************************
running function of the thread
***********************************************************/
void EventsSenderToAll::run()
{
	std::map<Ice::Long, ClientInterfacePrx>::iterator it = _proxies.begin();
	std::map<Ice::Long, ClientInterfacePrx>::iterator end = _proxies.end();
	for(; it != end; ++it)
	{
		try
		{
			if(it->second)
				it->second->ServerEvents(_events);
		}
		catch(const IceUtil::Exception& ex)
		{
			std::cout<<"Exception in EventsSenderToAll: "<<ex.what()<<std::endl;
		}
		catch(...)
		{
			std::cout<<"Unknown exception in EventsSenderToAll. "<<std::endl;
		}
	}

}





/***********************************************************
constructor
***********************************************************/
MapHandler::MapHandler(const MapInfo & mapinfo)
: _Trunning(false), _mapinfo(mapinfo)
{
}

/***********************************************************
destructor
***********************************************************/
MapHandler::~MapHandler(void)
{
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
		_Trunning = false;
		_monitor.notifyAll();
	}

	if(_thread)
	{
		_threadcontrol.join();
		_thread = NULL;
	}
}


/***********************************************************
start run function
***********************************************************/
void MapHandler::StartThread()
{
	// do nothing if already running
	{
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
		if(_Trunning)
			return;
	}

	// clean old thread if needed
	if(_thread)
	{
		_threadcontrol.join();
		_thread = NULL;
	}


	// create thread
	_thread = new RunThread(this);
	_threadcontrol = _thread->start();
}



/***********************************************************
running function of the thread
***********************************************************/
void MapHandler::run()
{
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_monitor);
	_Trunning = true;

	// init time
	double waittime = SynchronizedTimeHandler::GetCurrentTimeDouble();

	// stop thread if running is false
	while(_Trunning)
	{
		// prepare events to send to everybody
		EventsSeq tosend;

		// process events
		std::map<Ice::Long, EventsSeq> evts;
		GetEvents(evts);
		ProcessEvents(evts, tosend);


		// send events to all proxies
		if(tosend.size() > 0)
		{
			IceUtil::ThreadPtr t = new EventsSenderToAll(tosend, GetProxies());
			t->start();	
		}


		// wait for a few milliseconds
		{
			double currwtime = SynchronizedTimeHandler::GetCurrentTimeDouble();
			double wdiff = (currwtime-waittime);
			long timelong = (long)(wdiff);

			if(timelong < _THREAD_WAIT_TIME_)
			{
				IceUtil::Time t = IceUtil::Time::milliSeconds(_THREAD_WAIT_TIME_-timelong);
				_monitor.timedWait(t);
			}

			// mesure the time used to do one cycle
			waittime = SynchronizedTimeHandler::GetCurrentTimeDouble();
		}
	}
}



/***********************************************************
process events
***********************************************************/
void MapHandler::ProcessEvents(const std::map<Ice::Long, EventsSeq> & evts, 
									EventsSeq &tosendevts)
{
	std::map<Ice::Long, EventsSeq>::const_iterator it = evts.begin();
	std::map<Ice::Long, EventsSeq>::const_iterator end = evts.end();
	for(;it != end; ++it)
	{
		LbaNet::EventsSeq::const_iterator itevt = it->second.begin();
		LbaNet::EventsSeq::const_iterator endevt = it->second.end();
		for(;itevt != endevt; ++itevt)
		{
			LbaNet::ClientServerEventBasePtr ptr = *itevt;
			LbaNet::ClientServerEventBase & obj = *ptr;
			const std::type_info& info = typeid(obj);

			// client entered
			if(info == typeid(LbaNet::PlayerEnterEvent))
			{
				PlayerEntered(it->first, tosendevts);
			}

			// client left
			if(info == typeid(LbaNet::PlayerLeaveEvent))
			{
				PlayerLeft(it->first, tosendevts);
			}

			// player updated position
			if(info == typeid(LbaNet::PlayerMovedEvent))
			{
				LbaNet::PlayerMovedEvent* castedptr = 
					dynamic_cast<LbaNet::PlayerMovedEvent *>(&obj);

				//TODO first check if the info is correct

				//TODO then do a interpolation and check for triggers

				if(castedptr->info.ForcedChange)
				{
					// update player position
					PlayerPosition pos;
					pos.MapName = _mapinfo.Name;
					pos.X = castedptr->info.CurrentPosX;
					pos.Y = castedptr->info.CurrentPosY;
					pos.Z = castedptr->info.CurrentPosZ;
					pos.Rotation = castedptr->info.CurrentRotation;
					SharedDataHandler::getInstance()->UpdatePlayerPosition(it->first, pos);

					// inform all of player move
					tosendevts.push_back(new LbaNet::PlayerMovedEvent(castedptr->Time, it->first, castedptr->info));
				}
			}

		}
	}
}




/***********************************************************
add events
***********************************************************/
void MapHandler::AddEvents(Ice::Long clientid, const EventsSeq &evts)
{
	IceUtil::Mutex::Lock sync(_mutex_events);
	EventsSeq &evtseq = _events[clientid];
	for(size_t i=0; i<evts.size(); ++i)
		evtseq.push_back(evts[i]);
}

/***********************************************************
add 1 event
***********************************************************/
void MapHandler::AddEvent(Ice::Long clientid, ClientServerEventBasePtr evt)
{
	IceUtil::Mutex::Lock sync(_mutex_events);
	_events[clientid].push_back(evt);
}

/***********************************************************
get all events for a specific map
***********************************************************/
void MapHandler::GetEvents(std::map<Ice::Long, EventsSeq> & evts)
{
	IceUtil::Mutex::Lock sync(_mutex_events);
	_events.swap(evts);
}

/***********************************************************
add player proxy
***********************************************************/
void MapHandler::AddProxy(Ice::Long clientid, ClientInterfacePrx proxy)
{
	IceUtil::Mutex::Lock sync(_mutex_proxies);
	_playerproxies[clientid] = proxy;

}

/***********************************************************
remove player proxy
***********************************************************/
void MapHandler::RemoveProxy(Ice::Long clientid)
{
	IceUtil::Mutex::Lock sync(_mutex_proxies);

	std::map<Ice::Long, ClientInterfacePrx>::iterator it =	_playerproxies.find(clientid);	
	if(it != _playerproxies.end())
		_playerproxies.erase(it);
}

/***********************************************************
get player proxy
***********************************************************/
ClientInterfacePrx MapHandler::GetProxy(Ice::Long clientid)
{
	IceUtil::Mutex::Lock sync(_mutex_proxies);

	std::map<Ice::Long, ClientInterfacePrx>::iterator it =	_playerproxies.find(clientid);	
	if(it != _playerproxies.end())
		return it->second;

	return NULL;
}

/***********************************************************
get players proxies
***********************************************************/
std::map<Ice::Long, ClientInterfacePrx> MapHandler::GetProxies()
{
	IceUtil::Mutex::Lock sync(_mutex_proxies);
	return _playerproxies;
}


/***********************************************************
player entered map
***********************************************************/
void MapHandler::PlayerEntered(Ice::Long id, EventsSeq &tosendevts)
{
	// first give info to the player about current map state
	EventsSeq toplayer;


	//current objects in map
	//TODO
	{
	ModelInfo		DisplayDesc;
	DisplayDesc.RendererType = 0;
	DisplayDesc.ModelName = "Worlds/Lba1Original/Grids/map0.osgb";

	ObjectPhysicDesc	PhysicDesc;
	PhysicDesc.Pos.X = 0;
	PhysicDesc.Pos.Y = 0;
	PhysicDesc.Pos.Z = 0;
	PhysicDesc.Pos.Rotation = 0;
	PhysicDesc.Type = 4;	
	PhysicDesc.Collidable = true;
	PhysicDesc.Filename = "Worlds/Lba1Original/Grids/map0.phy";

	toplayer.push_back(new AddObjectEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
												1, 1, DisplayDesc, PhysicDesc));
	}

	
	

	// current players in map
	//TODO
	for(size_t cc=0; cc<_currentplayers.size(); ++cc)
	{
		SavedWorldInfo pinfo = SharedDataHandler::getInstance()->GetInfo(_currentplayers[cc]);

		ObjectPhysicDesc	PhysicDesc;
		PhysicDesc.Pos = pinfo.ppos;
		PhysicDesc.Type = 2;	
		PhysicDesc.ShapeType = 2;
		PhysicDesc.Collidable = false;
		PhysicDesc.Density = 1;
		PhysicDesc.SizeX = 0.5;
		PhysicDesc.SizeY = 5;

		toplayer.push_back(new AddObjectEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
													4, id, pinfo.model, PhysicDesc));
	}

	// player inventory
	{
	GuiParamsSeq seq;
	seq.push_back(new InventoryGuiParameter(SharedDataHandler::getInstance()->GetInventorySize(id), 
								SharedDataHandler::getInstance()->GetInventory(id)));
	toplayer.push_back(
		new RefreshGameGUIEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), "InventoryBox", seq, false, false));
	}

	// player shortcut
	{
	GuiParamsSeq seq;
	seq.push_back(new ShortcutGuiParameter(SharedDataHandler::getInstance()->GetShorcuts(id)));
	toplayer.push_back(
		new RefreshGameGUIEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), "ShortcutBox", seq, false, false));
	}




	IceUtil::ThreadPtr t = new EventsSender(toplayer, GetProxy(id));
	t->start();	


	// then inform all players that player entered
	//TODO
	{
		SavedWorldInfo pinfo = SharedDataHandler::getInstance()->GetInfo(id);

		ObjectPhysicDesc	PhysicDesc;
		PhysicDesc.Pos = pinfo.ppos;
		PhysicDesc.Type = 2;	
		PhysicDesc.ShapeType = 2;
		PhysicDesc.Collidable = false;
		PhysicDesc.Density = 1;
		PhysicDesc.SizeX = 0.5;
		PhysicDesc.SizeY = 5;

		tosendevts.push_back(new AddObjectEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
													4, id, pinfo.model, PhysicDesc));
	}


	// add player to list
	_currentplayers.push_back(id);
}

/***********************************************************
player left map
***********************************************************/
void MapHandler::PlayerLeft(Ice::Long id, EventsSeq &tosendevts)
{
	// inform all players that player left
	tosendevts.push_back(new RemoveObjectEvent(SynchronizedTimeHandler::GetCurrentTimeDouble(), 
												4, id));
	
	// remove player from list
	std::vector<Ice::Long>::iterator it = std::find(_currentplayers.begin(), _currentplayers.end(), id);
	if(it != _currentplayers.end())
		_currentplayers.erase(it);
	
}