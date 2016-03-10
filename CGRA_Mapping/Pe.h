/*------------------------------------------------------------
* Pe.h
* node的定义，包括属性和比较器
*
* Author: Li Xiaoquan
* 2016/3/9
*-----------------------------------------------------------------*/



#include "argraph.h"  //AttrComparator和AttrDestroyer在argraph.h中
#include <iostream>
#include <string>

using namespace std;

class Pe
{
public:
	Pe(string thePe) :peAttr(thePe)
	{}

	string getPeAttr()
	{
		return peAttr;
	}

	void setPeAttr(string thePe)
	{
		this->peAttr = thePe;
	}

private:
	string peAttr; //pe属性

};

class PeDestroyer : public AttrDestroyer
{
public:
	virtual void destroy(void *p)
	{
		delete p;
	}
};

class PeComparator : public AttrComparator
{
public:
	PeComparator()
	{
	}

	virtual bool compatible(void *nodeA, void *nodeB)
	{
		Pe *a = (Pe *)nodeA;
		Pe *b = (Pe *)nodeB;
		//cout << a->getPeAttr() << " a b " << b->getPeAttr();
		//return a->getPeAttr() == b->getPeAttr();
		string as = a->getPeAttr();
		string bs = b->getPeAttr();
		if(as[0]=='0' && bs[0]=='1'){
			if (bs.find(as.substr(1, as.size() - 1)) != string::npos)
				return true;
			else
				return false;
		}
		else if(as[0]=='1' && bs[0]=='0'){
			return compatible(nodeB, nodeA);
		}
		else if(as[0]=='0' && bs[0]=='0'){
			return as == bs;
		}
		else if(as[0]=='1' && bs[0]=='1'){
			return as == bs;
		}
		else 
			return false;
	}
};