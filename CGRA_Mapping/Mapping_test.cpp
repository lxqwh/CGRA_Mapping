 #include "argraph.h"
#include "argedit.h"
#include "match.h"
#include  "vf2_mono_state.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//��ӽڵ������
#include "Pe.h"

using namespace std;

#define MAXNODES 60000
#define AL_NUM 30

int matchingCnt = 0;//��¼���ֵ�ƥ����

void getNodeEdge_AL(ARGEdit &ed, string nodeName, string edgeName, vector<int>&row, vector<int>&col, string &name,int numN)
{
	/******************************��ȡ�ڵ�************************************/
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+�ýڵ�Ľ�ɫID
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	nodeFile >> name;
	nodeFile >> numN;
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		row.push_back(r);
		col.push_back(c);
		//cout << nodeID << " " << pe << endl;
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************��ȡ��************************************/
	//���ļ���ȡ�ߣ����ļ�һ������ʼ�ڵ�ID+" "+Ŀ��ڵ�ID
	ifstream edgeFile(edgeName);
	int start, end;
	//cout << edgeName << endl;

	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
		//cout << start << " " << startPe << " " << end << " " << endPe << endl;

		//������
		ed.InsertEdge(start, end, NULL);
	}

	edgeFile.close();
}
void getNodeEdge_AR(ARGEdit &ed,string nodeName,string edgeName)
{
	/******************************��ȡ�ڵ�************************************/
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+�ýڵ�Ľ�ɫID
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		//cout << nodeID << " " << pe << endl;
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************��ȡ��************************************/
	//���ļ���ȡ�ߣ����ļ�һ������ʼ�ڵ�ID+" "+Ŀ��ڵ�ID
	ifstream edgeFile(edgeName);
	int start,end;
	//cout << edgeName << endl;

	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
		//cout << start << " " << startPe << " " << end << " " << endPe << endl;

		//������
		ed.InsertEdge(start, end, NULL);
	}
	
	edgeFile.close();
}

void getData(ARGEdit &ed,int count)
{
	for (int i = 0; i < count; i++)
		ed.InsertNode(NULL);

	for (int i = 0; i < count; i++)
	{
		for (int j = 0; j < count; j++)
		{
			if (i != j)
				ed.InsertEdge(i, j, NULL);
		}
	}
}

bool graph_visitor(int n, node_id ni1[], node_id ni2[], void *usr_data)
{
	/*************** ƥ��Դ�����ni1��ni2�� **************/
	FILE *f = (FILE *)usr_data;
	//��ͳ�Ƶ���ƥ��ڵ�������ļ���
//	fprintf(f,"n:%d",n); 
//	fprintf(f, "\n");
	for (int i = 0; i < n; i++)
		fprintf(f,"%hd %hd | ",ni1[i],ni2[i]);

	fprintf(f,"\n");
	/*************** ,ƥ��Դ�����ni1��ni2�� **************/

	matchingCnt++; //�õ�ƥ�������
//	cout << matchingCnt << endl;
	//����false����������һ��ƥ��
	return false;
}

int main()
{
	//����ģʽͼ������ͼ��ed
	ARGEdit pattern_ed,data_ed;
	vector<vector<int>> row(AL_NUM), col(AL_NUM);
	vector<string> al(AL_NUM);
	vector<int> Node_num(AL_NUM);
	getNodeEdge_AR(data_ed,"ARN_G1.txt","ARE_G1.txt");
	getNodeEdge_AL(pattern_ed,"ALN_AES.txt","ALE_AES.txt",row[0],col[0],al[0],Node_num[0]);

	//getData(pattern_ed,4);
	//getData(data_ed,5);
	ARGraph<Pe, void> data_g(&data_ed);
	ARGraph<Pe, void> pattern_g(&pattern_ed);

	//�������������
	data_g.SetNodeDestroyer(new PeDestroyer());
	pattern_g.SetNodeDestroyer(new PeDestroyer());

	//�������ԱȽ���
	data_g.SetNodeComparator(new PeComparator());
	pattern_g.SetNodeComparator(new PeComparator());

	//������ͼ�칹�ĳ�ʼ״̬
	VF2MonoState s0(&pattern_g, &data_g);

	//��������ļ�
	FILE *f = fopen("subIso.txt","w");

	int a = match(&s0,graph_visitor,f);

	fprintf(f, "n: %d", matchingCnt);//���ҵ���ƥ����������ı�

	cout << matchingCnt << endl;
	cout <<"a: "<<a<<endl;

	fclose(f);



	return 0;
}