/*------------------------------------------------------------
* Pe.h
* node�Ķ��壬�������ԺͱȽ���
*
* Author: Li Xiaoquan
* 2016/3/9
*-----------------------------------------------------------------*/



#include "argraph.h"  //AttrComparator��AttrDestroyer��argraph.h��
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
	int peAttr; //pe����

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