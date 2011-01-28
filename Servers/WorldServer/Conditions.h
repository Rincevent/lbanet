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


#if !defined(__LbaNetModel_1_ConditionBase_h)
#define __LbaNetModel_1_ConditionBase_h


#include <boost/shared_ptr.hpp>
#include <LbaTypes.h>

class ConditionBase;
class ScriptEnvironmentBase;
typedef boost::shared_ptr<ConditionBase> ConditionBasePtr;


/***********************************************************************
 * Module:  Conditions.h
 * Author:  vivien
 * Modified: lundi 27 juillet 2009 14:53:50
 * Purpose: Declaration of the class ConditionBase
 *
 *
  *********************************************************************/
class ConditionBase
{
public:
	//! constructor
	ConditionBase(){}

	//! destructor
	virtual ~ConditionBase(){}

	//! check if the condition is true or not
	virtual bool Passed(ScriptEnvironmentBase * owner, 
		int ObjectType, Ice::Long ObjectId){return false;}

	// save action to lua file
	virtual void SaveToLuaFile(std::ofstream & file, const std::string & conditionname){}
	
};


//! condition is always true
class AlwaysTrueCondition : public ConditionBase
{
public:
	//! constructor
	AlwaysTrueCondition(){}

	//! check if the condition is true or not
	virtual bool Passed(ScriptEnvironmentBase * owner, 
							int ObjectType, Ice::Long ObjectId)
	{ return true;}
	
	// save action to lua file
	virtual void SaveToLuaFile(std::ofstream & file, const std::string & conditionname);
};


//! take the negation of a condition
class NegateCondition : public ConditionBase
{
public:
	//! constructor
	NegateCondition()
	{}

	//! destructor
	virtual ~NegateCondition(){}

	//! check if the condition is true or not
	virtual bool Passed(ScriptEnvironmentBase * owner, 
							int ObjectType, Ice::Long ObjectId)
	{ return !(_tonegate->Passed(owner, ObjectType, ObjectId));}
	
	// save action to lua file
	virtual void SaveToLuaFile(std::ofstream & file, const std::string & conditionname);	

	// set condition to negate
	void SetCondition(ConditionBasePtr tonegate)
	{_tonegate = tonegate;}

	// accessor
	ConditionBasePtr GetCondition()
	{ return _tonegate;}

private:
	ConditionBasePtr _tonegate;
};


//! bind two condition with and logic
class AndCondition : public ConditionBase
{
public:
	//! constructor
	AndCondition()
	{}

	//! destructor
	virtual ~AndCondition(){}

	//! check if the condition is true or not
	virtual bool Passed(ScriptEnvironmentBase * owner, 
							int ObjectType, Ice::Long ObjectId)
	{ 
		return _cond1->Passed(owner, ObjectType, ObjectId) 
			&& _cond2->Passed(owner, ObjectType, ObjectId);
	}
	
	// save action to lua file
	virtual void SaveToLuaFile(std::ofstream & file, const std::string & conditionname);	

	// set conditions
	void SetCondition1(ConditionBasePtr cond1)
	{_cond1 = cond1;}

	// set conditions
	void SetCondition2(ConditionBasePtr cond2)
	{_cond2 = cond2;}

	// accessor
	ConditionBasePtr GetCondition1()
	{ return _cond1;}

	// accessor
	ConditionBasePtr GetCondition2()
	{ return _cond2;}

private:
	ConditionBasePtr _cond1;
	ConditionBasePtr _cond2;
};



//! bind two condition with or logic
class OrCondition : public ConditionBase
{
public:
	//! constructor
	OrCondition()
	{}

	//! destructor
	virtual ~OrCondition(){}

	//! check if the condition is true or not
	virtual bool Passed(ScriptEnvironmentBase * owner, 
							int ObjectType, Ice::Long ObjectId)
	{ 
		return _cond1->Passed(owner, ObjectType, ObjectId) 
			|| _cond2->Passed(owner, ObjectType, ObjectId);
	}
		
	// save action to lua file
	virtual void SaveToLuaFile(std::ofstream & file, const std::string & conditionname);


	// set conditions
	void SetCondition1(ConditionBasePtr cond1)
	{_cond1 = cond1;}

	// set conditions
	void SetCondition2(ConditionBasePtr cond2)
	{_cond2 = cond2;}

	// accessor
	ConditionBasePtr GetCondition1()
	{ return _cond1;}

	// accessor
	ConditionBasePtr GetCondition2()
	{ return _cond2;}

private:
	ConditionBasePtr _cond1;
	ConditionBasePtr _cond2;
};


#endif