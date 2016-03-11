//测试代码

#include "argraph.h"
#include "argedit.h"
#include "match.h"
#include  "vf2_mono_state.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
//添加节点的属性
#include "Pe.h"

using namespace std;

#define MAXNODES 60000
#define AL_NUM 30

int matchingCnt = 0;//记录发现的匹配数

void getNodeEdge_AL(ARGEdit &ed, string nodeName, string edgeName, vector<int>&row, vector<int>&col, string &name,int &numN)
{
	/******************************获取节点************************************/
	//从文件读取节点，节点文件一行是节点ID+" "+x坐标+" "+y坐标+" "+属性
	//例如：0  0 0 0_xorlut_
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	nodeFile >> name; //算法名称
	nodeFile >> numN; //结点个数
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		row.push_back(r);
		col.push_back(c);
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************获取边************************************/
	//从文件读取边，边文件一行是起始节点ID+" "+目标节点ID
	ifstream edgeFile(edgeName);
	int start, end;
	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
		//创建边
		ed.InsertEdge(start, end, NULL);
	}

	edgeFile.close();
}


void getNodeEdge_AR(ARGEdit &ed,string nodeName,string edgeName)
{
	/******************************获取节点************************************/
	//从文件读取节点，节点文件一行是节点ID+" "+x坐标+" "+y坐标+" "+属性
	//例如：0  0 0 1_au_xorau_auxor_xorauxor_xorxorau_xorxorauxor_sh_xorsh_shxor_xorshxor_xor_xorxor_xorxorxor_or_and_&&&&_bn_xorbn_bnxor_xorbnxor_
	ifstream nodeFile(nodeName);
	int nodeID, r, c;
	string pe;
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> r >> c >> pe;
		ed.InsertNode(new Pe(pe));
	}

	nodeFile.close();

	/******************************获取边************************************/
	//从文件读取边，边文件一行是起始节点ID+" "+目标节点ID
	ifstream edgeFile(edgeName);
	int start,end;

	while (!edgeFile.eof())
	{
		edgeFile >> start >> end;
		//创建边
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
	/******************************获取映射点对************************************/
	//格式：l r char
	//一行为一个完整映射
	//例如：0 4 | 1 5 | 2 6 | 3 7 | 4 8 | 5 9 | 6 10 | 7 11 | 
	ifstream inFile(fileName);
	int  l, r;
	char sign;
	vector<pair<int, int>> tmp;
	int rn = 0;
	int trm = 0;  //规则1：从候选集中找出所有满足R=max(x)=min(max(x))的映射，这是映射最优选择的第一个条件，它保证选出的候选集的映射占有最少的行数
	int tcm = 0;  //规则2：从候选集中找出所有满足C=max(y)=min(max(y))的映射，它保证选出的候选集的映射占有最少的列数
	int tsy = 0;  //规则3：在最少列数的基础上使映射尽可能靠右
	int tsx = 0;  //规则4：尽量靠上
	int tmix = 0; //规则5：最少交叉
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
			if (trm < brm) {//规则1
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
				if (tcm < bcm) {//规则2
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
					if (tsy < bsy) {//规则3
						best = tmp;
						brm = trm;
						bcm = tcm;
						bsy = tsy;
						bsx = tsx;
						bmix = tmix;
					}
					else if (tsy>bsy) continue;
					else {
						if (tsx < bsx) {//规则4
							best = tmp;
							brm = trm;
							bcm = tcm;
							bsy = tsy;
							bsx = tsx;
							bmix = tmix;
						}
						else if (tsx>bsx) continue;
						else {
							if (tmix < bmix) {//规则5
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
	/*************** 匹配对储存在ni1和ni2中 **************/
	FILE *f = (FILE *)usr_data;
	//将统计到的匹配节点输出到文件中
//	fprintf(f,"n:%d",n); 
//	fprintf(f, "\n");
	for (int i = 0; i < n; i++)
		fprintf(f,"%hd %hd | ",ni1[i],ni2[i]);

	fprintf(f,"\n");
	/*************** ,匹配对储存在ni1和ni2中 **************/

	matchingCnt++; //得到匹配的数量
//	cout << matchingCnt << endl;
	//返回false用于搜索下一个匹配
	return false;
}

int main()
{
	//创建模式图和数据图的ed
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

	//设置属性清除器
	data_g.SetNodeDestroyer(new PeDestroyer());
	pattern_g.SetNodeDestroyer(new PeDestroyer());

	//设置属性比较器
	data_g.SetNodeComparator(new PeComparator());
	pattern_g.SetNodeComparator(new PeComparator());

	//创建子图异构的初始状态
	VF2MonoState s0(&pattern_g, &data_g);

	//创建输出文件
	errno_t er;
	FILE *f;
	er = fopen_s(&f,"subIso.txt","w");

	int a = match(&s0,graph_visitor,f);
	//fprintf(f, "n: %d", matchingCnt);//将找到的匹配数输出至文本
	cout << matchingCnt << endl;
	cout <<"a: "<<a<<endl;
	fclose(f);

	select("subIso.txt", row[0], col[0], result, Node_num[0]);
	for (auto i : result) {
		cout << i.first << " " << i.second << " | ";
	}


	return 0;
}