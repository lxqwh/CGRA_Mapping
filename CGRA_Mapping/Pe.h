/*------------------------------------------------------------
* Pe.h
* node�Ķ��壬�������ԺͱȽ���
*
* Author: Li Xiaoquan
* 2016/3/9
*-----------------------------------------------------------------*/



#include "argraph.h"  //AttrComparator��AttrDestroyer��argraph.h��
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
	string peAttr; //pe����

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
		string as = a->getPeAttr();
		string bs = b->getPeAttr();
		if(as[0]=='0' && bs[0]=='1'){
			
		}
		else if(as[0]=='1' && bs[0]=='0'){
			
		}
		else if(as[0]=='0' && bs[0]=='0'){
			
		}
		else if(as[0]=='1' && bs[0]=='1'){
			
		}
		else 
			return false;
		
		return a->getPeAttr() == b->getPeAttr();
	}
};