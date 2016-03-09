/*------------------------------------------------------------
* Pe.h
* node的定义，包括属性和比较器
*
* Author: Li Xiaoquan
* 2016/3/9
*-----------------------------------------------------------------*/



#include "argraph.h"  //AttrComparator和AttrDestroyer在argraph.h中
#include <iostream>

using namespace std;

class Pe
{
public:
	Pe(int thePe) :peAttr(thePe)
	{}

	int getPeAttr()
	{
		return peAttr;
	}

	void setPeAttr(int thePe)
	{
		this->peAttr = thePe;
	}

private:
	int peAttr; //pe属性

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
		return a->getPeAttr() == b->getPeAttr();
	}
};