/*********************************
公司：寰景信息
版本：V1.0
作者：FSB
日期：2016.12.20
功能：
**********************************/
#include <iostream>
#include <list>
#include "boost/thread.hpp"
template<typename Data>  
class SyncQueue  
{
public:  
	SyncQueue()  {  m_bclose = false;} 
	virtual ~SyncQueue()
	{
	}

	void push(Data const& data, bool bBack = true)
	{  
		do 
		{
			boost::mutex::scoped_lock lock(m_mtx);  
			if(m_bclose)
				return;
			if(bBack)
				m_objList.push_back(data); 
			else
				m_objList.push_front(data); 
		} while (0);
	}  
	bool empty() const
	{  
		boost::mutex::scoped_lock lock(m_mtx);  
		return m_objList.empty();  
	}  
	bool pull(Data& datavalue)
	{ 
		boost::mutex::scoped_lock lock(m_mtx);  
		while (m_objList.empty())
			return false;
		datavalue=m_objList.front();  
		m_objList.pop_front();  
		return true;
	}  

	unsigned int getSize()
	{
		boost::mutex::scoped_lock lock(m_mtx);
		return m_objList.size();
	}

	void clear()
	{
		boost::mutex::scoped_lock lock(m_mtx);
		m_objList.clear();
	}
	bool close()
	{
		boost::mutex::scoped_lock lock(m_mtx);
		m_bclose = true;
		m_objList.clear();
		return true;
	}
	bool closed()
	{
		return m_bclose;
	}

protected:
	std::list<Data> m_objList;  
	boost::mutex m_mtx;  

private:
	bool m_bclose;
};