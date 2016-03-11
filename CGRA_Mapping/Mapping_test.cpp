//���Դ���

#include "argraph.h"
#include "argedit.h"
#include "match.h"
#include  "vf2_mono_state.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
//��ӽڵ������
#include "Pe.h"

using namespace std;

#define MAXNODES 60000
#define AL_NUM 30

int matchingCnt = 0;//��¼���ֵ�ƥ����

void getNodeEdge_AL(ARGEdit &ed, string nodeName, string edgeName, vector<int>&row, vector<int>&col, string &name,int &numN)
{
	/******************************��ȡ�ڵ�************************************/
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+x����+" "+y����+" "+����
	//���磺0  0 0 0_xorlut_
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	nodeFile >> name; //�㷨����
	nodeFile >> numN; //������
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		row.push_back(r);
		col.push_back(c);
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************��ȡ��************************************/
	//���ļ���ȡ�ߣ����ļ�һ������ʼ�ڵ�ID+" "+Ŀ��ڵ�ID
	ifstream edgeFile(edgeName);
	int start, end;
	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
		//������
		ed.InsertEdge(start, end, NULL);
	}

	edgeFile.close();
}


void getNodeEdge_AR(ARGEdit &ed,string nodeName,string edgeName)
{
	/******************************��ȡ�ڵ�************************************/
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+x����+" "+y����+" "+����
	//���磺0  0 0 1_au_xorau_auxor_xorauxor_xorxorau_xorxorauxor_sh_xorsh_shxor_xorshxor_xor_xorxor_xorxorxor_or_and_&&&&_bn_xorbn_bnxor_xorbnxor_
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************��ȡ��************************************/
	//���ļ���ȡ�ߣ����ļ�һ������ʼ�ڵ�ID+" "+Ŀ��ڵ�ID
	ifstream edgeFile(edgeName);
	int start,end;

	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
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

void select(string fileName, vector<int>&row, vector<int>&col, vector<pair<int,int>>& best, int nodeNum)
{
	/******************************��ȡӳ����************************************/
	//��ʽ��l r char
	//һ��Ϊһ������ӳ��
	//���磺0 4 | 1 5 | 2 6 | 3 7 | 4 8 | 5 9 | 6 10 | 7 11 | 
	ifstream inFile(fileName);
	int  l, r;
	char sign;
	vector<pair<int, int>> tmp;
	int rn = 0;
	int trm = 0;  //����1���Ӻ�ѡ�����ҳ���������R=max(x)=min(max(x))��ӳ�䣬����ӳ������ѡ��ĵ�һ������������֤ѡ���ĺ�ѡ����ӳ��ռ�����ٵ�����
	int tcm = 0;  //����2���Ӻ�ѡ�����ҳ���������C=max(y)=min(max(y))��ӳ�䣬����֤ѡ���ĺ�ѡ����ӳ��ռ�����ٵ�����
	int tsy = 0;  //����3�������������Ļ�����ʹӳ�価���ܿ���
	int tsx = 0;  //����4����������
	int tmix = 0; //����5�����ٽ���
	int brm = 0, bcm = 0, bsy = 0, bsx = 0, bmix = 0;
	int rx = 0, ry = 0;
	while (!inFile.eof())
	{
		for (int i = 0; i < nodeNum; i++) {
			inFile >> l >> r >> sign;
			rx = (r + 4) / 4;
			ry = r - ((r + 4) / 4 - 1) * 4 + 1;
			trm = max(brm, rx);
			tcm = max(bcm, ry);
			tsy += ry;
			tsx += rx;
			tmix += abs(ry - col[l]);
			tmp.push_back(make_pair(l, r));
		}
		rn++;
		if (rn == 1) {
			best = tmp;
			brm = trm;
			bcm = tcm;
			bsy = tsy;
			bsx = tsx;
			bmix = tmix;
		}
		else {
			if (trm < brm) {//����1
				best = tmp;
				brm = trm;
				bcm = tcm;
				bsy = tsy;
				bsx = tsx;
				bmix = tmix;
			}
			else if (trm>brm) {
				continue;
			}
			else {
				if (tcm < bcm) {//����2
					best = tmp;
					brm = trm;
					bcm = tcm;
					bsy = tsy;
					bsx = tsx;
					bmix = tmix;
				}
				else if (tcm>bcm) {
					continue;
				}
				else {
					if (tsy < bsy) {//����3
						best = tmp;
						brm = trm;
						bcm = tcm;
						bsy = tsy;
						bsx = tsx;
						bmix = tmix;
					}
					else if (tsy>bsy) continue;
					else {
						if (tsx < bsx) {//����4
							best = tmp;
							brm = trm;
							bcm = tcm;
							bsy = tsy;
							bsx = tsx;
							bmix = tmix;
						}
						else if (tsx>bsx) continue;
						else {
							if (tmix < bmix) {//����5
								best = tmp;
								brm = trm;
								bcm = tcm;
								bsy = tsy;
								bsx = tsx;
								bmix = tmix;
							}
							else continue;
						}
					}
				}
			
			}
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
	vector<pair<int, int>> result;

	getNodeEdge_AR(data_ed,"ARN_G1.txt","ARE_G1.txt");
	getNodeEdge_AL(pattern_ed,"ALN_DES.txt","ALE_DES.txt",row[0],col[0],al[0],Node_num[0]);

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
	errno_t er;
	FILE *f;
	er = fopen_s(&f,"subIso.txt","w");

	int a = match(&s0,graph_visitor,f);
	//fprintf(f, "n: %d", matchingCnt);//���ҵ���ƥ����������ı�
	cout << matchingCnt << endl;
	cout <<"a: "<<a<<endl;
	fclose(f);

	select("subIso.txt", row[0], col[0], result, Node_num[0]);
	for (auto i : result) {
		cout << i.first << " " << i.second << " | ";
	}


	return 0;
}