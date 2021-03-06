#include "LuaHandlerBase.h"

#pragma warning (disable : 4251) // disable luabind warnings
extern "C"
{
    #include "lua.h"
	#include "lauxlib.h"
	#include "lualib.h"
}
#include <luabind/luabind.hpp>


#include "LogHandler.h"
#include "LuaThreadHandler.h"
#include "ScriptEnvironmentBase.h"
#include "ActionArguments.h"
#include "ScriptInitHandler.h"
#include "DataDirHandler.h"


/***********************************************************
constructor
***********************************************************/
LuaHandlerBase::LuaHandlerBase()
: m_creatingthread(-1)
{
	try
	{
		// Create a new lua state
		m_LuaState = lua_open();

		// Connect LuaBind to this lua state
		luabind::open(m_LuaState);

		//open standard libs
		luaopen_base(m_LuaState);
		luaopen_table(m_LuaState);
		luaopen_string(m_LuaState);
		luaopen_math(m_LuaState);
		luaopen_debug(m_LuaState);
		//luaopen_package(m_LuaState);

		//luaL_openlibs(m_LuaState);
	}
	catch(const std::exception &error)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception initializing LUA for base lua: ") + error.what(), 0);
	}
}

/***********************************************************
destructor
***********************************************************/
LuaHandlerBase::~LuaHandlerBase(void)
{
	// clear running thread before closing main lua state
	m_RunningThreads.clear();

#ifndef WIN32
	lua_close(m_LuaState); // TODO - this crashes with msvc 2010 ...
#endif
}


/***********************************************************
load a lua file
***********************************************************/
void LuaHandlerBase::LoadFile(const std::string & luafile)
{
	// read lua file
	int error = luaL_dofile(m_LuaState, (DataDirHandler::getInstance()->GetDataDirPath() + "/"+luafile).c_str());
	if(error != 0)
	{
		LogHandler::getInstance()->LogToFile(std::string("Error loading lua file: ") + lua_tostring(m_LuaState, -1));
	}
}


/***********************************************************
call lua function
***********************************************************/
std::string LuaHandlerBase::CallLua(const std::string & functioname, ScriptEnvironmentBase* env)
{
	try
	{
		if(env)
			luabind::call_function<void>(m_LuaState, functioname.c_str(), env);
		else
			luabind::call_function<void>(m_LuaState, functioname.c_str());
	}
	catch(const std::exception &error)
	{
		std::string errorStr("Exception calling lua function ");
		errorStr += functioname + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1);
		LogHandler::getInstance()->LogToFile(errorStr, 0);
		return errorStr;
	}
	return "Success";
}


/***********************************************************
call lua function
***********************************************************/
void LuaHandlerBase::CallLua(const std::string & functioname, ScriptInitHandler* env)
{
	try
	{
		luabind::call_function<void>(m_LuaState, functioname.c_str(), env);
	}
	catch(const std::exception &error)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception calling lua function ") + functioname + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	}
}




/***********************************************************
start script in a new thread
***********************************************************/
void LuaHandlerBase::StartScript(const std::string & FunctionName, bool inlinefunction,
									ScriptEnvironmentBase* env, int &ThreadReference)
{
	ThreadReference = -1;

	//try
	//{
	//	luabind::call_function<void>(m_LuaState, FunctionName.c_str(), 1, env);
	//}
	//catch(const std::exception &error)
	//{
	//	LogHandler::getInstance()->LogToFile(std::string("Exception calling lua function ") + FunctionName + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	//}


	boost::shared_ptr<LuaThreadHandler> Th(new LuaThreadHandler(m_LuaState, FunctionName,
												inlinefunction, env, ThreadReference, m_creatingthread));
	if(Th->IsStarted())
		m_RunningThreads[ThreadReference] = Th;
	else
		ThreadReference = -1;

	m_creatingthread = -1;
}


/***********************************************************
start script in a new thread
***********************************************************/
void LuaHandlerBase::StartScript(const std::string & FunctionName, long ActorId, bool inlinefunction,
									ScriptEnvironmentBase* env, int &ThreadReference)
{
	ThreadReference = -1;

	//try
	//{
	//	luabind::call_function<void>(m_LuaState, FunctionName.c_str(), 1, env);
	//}
	//catch(const std::exception &error)
	//{
	//	LogHandler::getInstance()->LogToFile(std::string("Exception calling lua function ") + FunctionName + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	//}


	boost::shared_ptr<LuaThreadHandler> Th(new LuaThreadHandler(m_LuaState, ActorId, FunctionName,
												inlinefunction, env, ThreadReference, m_creatingthread));
	if(Th->IsStarted())
		m_RunningThreads[ThreadReference] = Th;
	else
		ThreadReference = -1;

	m_creatingthread = -1;
}



/***********************************************************
resume yieled thread
***********************************************************/
bool LuaHandlerBase::ResumeThread(int ThreadIdx, std::string & functioname)
{
	// check if thread exist
	std::map<int, boost::shared_ptr<LuaThreadHandler> >::iterator it = m_RunningThreads.find(ThreadIdx);
	if(it != m_RunningThreads.end())
	{
		// run thread and destroy it if finished
		if(it->second->ResumeThread())
		{
			functioname = it->second->GetScriptName();
			m_RunningThreads.erase(it);
			return true;
		}
	}

	return false;
}



/***********************************************************
execute lua script given as a string
***********************************************************/
std::string LuaHandlerBase::ExecuteScriptString( const std::string & ScriptString )
{
	int error = luaL_dostring(m_LuaState, ScriptString.c_str());
	if(error != 0)
	{
		std::string erroStr = std::string("Exception calling executing script string: ") + lua_tostring(m_LuaState, -1);
		LogHandler::getInstance()->LogToFile(erroStr);
		return erroStr;
	}
	return "Success";
}


/***********************************************************
call lua function
***********************************************************/
void LuaHandlerBase::ExecuteCustomAction(int ObjectType, long ObjectId,
											const std::string & FunctionName,
											ActionArgumentBase * args,
											ScriptEnvironmentBase* env)
{
	try
	{
		luabind::call_function<void>(m_LuaState, FunctionName.c_str(), ObjectType, ObjectId, args, env);
	}
	catch(const std::exception &error)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception calling lua function ") + FunctionName + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	}
}



/***********************************************************
try to stop lua thread
***********************************************************/
void LuaHandlerBase::StopThread(int ThreadIdx)
{
	std::map<int, boost::shared_ptr<LuaThreadHandler> >::iterator it = m_RunningThreads.find(ThreadIdx);
	if(it != m_RunningThreads.end())
		m_RunningThreads.erase(it);
}


/***********************************************************
check if thread is still running
***********************************************************/
bool LuaHandlerBase::ThreadRunning(int ThreadIdx)
{
	if(m_creatingthread == ThreadIdx)
		return true;

	std::map<int, boost::shared_ptr<LuaThreadHandler> >::iterator it = m_RunningThreads.find(ThreadIdx);
	if(it != m_RunningThreads.end())
		return true;

	return false;
}



/***********************************************************
call lua function
***********************************************************/
bool LuaHandlerBase::CallLuaCondition(const std::string & functioname, ScriptEnvironmentBase* env, 
											int objecttype, long objectid)
{
	try
	{
		return luabind::call_function<bool>(m_LuaState, functioname.c_str(), env, objecttype, objectid);
	}
	catch(const std::exception &error)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception calling lua condition ") + functioname + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	}

	return false;
}


/***********************************************************
RunAttackScript
***********************************************************/
void LuaHandlerBase::RunAttackScript(long ActorId,
									const std::string & FunctionName,
									ScriptEnvironmentBase* env)
{
	try
	{
		luabind::call_function<void>(m_LuaState, FunctionName.c_str(), -33, ActorId, env);
	}
	catch(const std::exception &error)
	{
		LogHandler::getInstance()->LogToFile(std::string("Exception calling lua attack script ") + FunctionName + std::string(" : ") + error.what() + std::string(" : ") + lua_tostring(m_LuaState, -1), 0);
	}
}

/***********************************************************
setGlobalEnv
***********************************************************/
void LuaHandlerBase::setGlobalEnv(const std::string& globalName, ScriptEnvironmentBase* var)
{
	luabind::globals(m_LuaState)[globalName.c_str()] = var;
}