//测试代码

#include "argraph.h"
#include "argedit.h"
#include "match.h"
#include "vf2_state_monomorphism.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <iomanip>

//添加节点的属性
#include "Pe.h"

using namespace std;

std::ofstream out("output/output");

#define AL_NUM_NOW 30 //当前算法个数
#define MAX_G 5 //允许单轮映射的最大组数
#define MAXNODES 60000

int matchingCnt = 0;//记录发现的匹配数

void getNodeEdge_AL(ARGEdit &ed, string nodeName, string edgeName, vector<string> &func, vector<int>&col, string &name,int &numN, int &G, int &C)
{
	/******************************获取节点************************************/
	//从文件读取节点，节点文件一行是节点ID+" "+列坐标+" "+属性
	//例如：0  0 0 0_xorlut_
	ifstream nodeFile(nodeName);
	int nodeID, c;
	string pe;
	string note;
	nodeFile >> note;
	nodeFile >> name; //算法名称
	nodeFile >> numN; //结点个数
	nodeFile >> G; //特殊初始组约定
	nodeFile >> C; //特殊初始行约定
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> c >> pe;
//		row.push_back(r);
		col.push_back(c);
		func.push_back(pe.substr(2, pe.size() - 3));
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
	//从文件读取节点，节点文件一行是节点ID+" "+行坐标+" "+列坐标+" "+属性
	//例如：0  0 0 1_au_xorau_auxor_xorauxor_xorxorau_xorxorauxor_sh_xorsh_shxor_xorshxor_xor_xorxor_xorxorxor_or_and_&&&&_bn_xorbn_bnxor_xorbnxor_
	ifstream nodeFile(nodeName);
	int nodeID;
		//, r, c;
	string pe;
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> pe;
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

void select(string fileName, vector<int>&col, vector<pair<int,int>>& best,int &brm, int &bcm, int nodeNum)
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
	int tsy = 0;  //规则3：在最少列数的基础上使映射尽可能靠左映射
	int tsx = 0;  //规则4：尽量靠上映射
	int tmix = 0; //规则5：互连尽量减少交叉
	int bsy = 0, bsx = 0, bmix = 0;
	brm = 0; bcm = 0;
	int rx = 0, ry = 0;
	while (!inFile.eof())
	{
        trm = 0; 
		tcm = 0; 
		tsy = 0; 
		tsx = 0; 
		tmix = 0;
		tmp.clear();
		for (int i = 0; i < nodeNum; i++) {
			inFile >> l >> r >> sign;
			if (inFile.eof()) return;
			rx = (r + 4) / 4;
			ry = r - ((r + 4) / 4 - 1) * 4 + 1;
			trm = max(trm, rx);
			tcm = max(tcm, ry);
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
	if (matchingCnt >= 10000) return true;//当组数变多时，可能的解会变得非常多>100w；因此大于1万时就不在继续
	else
	return false;
}

#if 0
int main() {
	int group = 5;
	int node_num = 12 * group;
	ofstream outFile("AR/output");
	for (int i = 0; i < node_num - 4; i++) {
		for (int j = (i / 4) * 4 + 4; j < node_num; j++) {
			outFile << setw(2) << i << " " << setw(2) << j << endl;
		}
	}
	return 0;
}
#endif

#if 1
int main()
{
	//基本数据存储
	vector<string> al_n_file = { "AL/ALN_AES.txt","AL/ALN_DES.txt","AL/ALN_TWOFISH.txt","AL/ALN_SM4.txt",
								 "AL/ALN_RC5.txt","AL/ALN_CAST128.txt","AL/ALN_BLOWFISH.txt","AL/ALN_SERPENT.txt",
                                 "AL/ALN_SEED.txt","AL/ALN_CAMELLIA.txt","AL/ALN_GOST.txt","AL/ALN_TEA.txt",
								 "AL/ALN_XTEA.txt",
								 "AL/NEW/ALN_SPECK.txt","AL/NEW/ALN_SIMON.txt","AL/NEW/ALN_LUCIFER.txt",
								 "AL/NEW/ALN_CLEFIA.txt","AL/NEW/ALN_ARIA.txt","AL/NEW/ALN_C2.txt",
								 "AL/NEW/ALN_PRESENT.txt","AL/NEW/ALN_MACGUFFIN.txt","AL/NEW/ALN_SQUARE.txt",
								 "AL/NEW/ALN_M6.txt","AL/NEW/ALN_SHARK.txt","AL/NEW/ALN_NUSH.txt",
								 "AL/NEW/ALN_GRAND_CRU.txt","AL/NEW/ALN_E2.txt","AL/NEW/ALN_KHAZAD.txt",
								 "AL/NEW/ALN_Hierocrypt-L1.txt","AL/NEW/ALN_Hierocrypt-3.txt"
								 };//算法图结点输入
	vector<string> al_e_file = { "AL/ALE_AES.txt","AL/ALE_DES.txt","AL/ALE_TWOFISH.txt","AL/ALE_SM4.txt",
								 "AL/ALE_RC5.txt","AL/ALE_CAST128.txt","AL/ALE_BLOWFISH.txt","AL/ALE_SERPENT.txt",
                                 "AL/ALE_SEED.txt","AL/ALE_CAMELLIA.txt","AL/ALE_GOST.txt","AL/ALE_TEA.txt",
								 "AL/ALE_XTEA.txt",
								 "AL/NEW/ALE_SPECK.txt","AL/NEW/ALE_SIMON.txt","AL/NEW/ALE_LUCIFER.txt",
								 "AL/NEW/ALE_CLEFIA.txt","AL/NEW/ALE_ARIA.txt","AL/NEW/ALE_C2.txt",
								 "AL/NEW/ALE_PRESENT.txt","AL/NEW/ALE_MACGUFFIN.txt","AL/NEW/ALE_SQUARE.txt",
								 "AL/NEW/ALE_M6.txt","AL/NEW/ALE_SHARK.txt","AL/NEW/ALE_NUSH.txt",
								 "AL/NEW/ALE_GRAND_CRU.txt","AL/NEW/ALE_E2.txt","AL/NEW/ALE_KHAZAD.txt",
								 "AL/NEW/ALE_Hierocrypt-L1.txt","AL/NEW/ALE_Hierocrypt-3.txt"
								 };//算法图边输入
	vector<vector<int>> /*row(AL_NUM),*/ col(AL_NUM_NOW);//节点列信息保存
	vector<string> al(AL_NUM_NOW);//算法名称
	vector<int> Node_num(AL_NUM_NOW);//算法节点数
	vector<vector<pair<int, int>>> result(AL_NUM_NOW);//映射结果保存
	vector<vector<string>> al_pe_func(AL_NUM_NOW);//不同算法的节点功能保存
	vector<int> m_row(AL_NUM_NOW), m_col(AL_NUM_NOW);//映射的行、列结果
	vector<map<string, vector<string>>> pe_util(12);//pe的功能使用统计
	vector<string> ARN(MAX_G), ARE(MAX_G);
	vector<int> G(AL_NUM_NOW, 0), C(AL_NUM_NOW, 1);
	for (int i = 1; i <= MAX_G; i++){//架构输入文件
		ARN[i - 1] = string("AR/ARN_G") + to_string(i);
		ARE[i - 1] = string("AR/ARE_G") + to_string(i) + string(".txt");
	}



	//映射每一个算法
	for (int i = 0; i < AL_NUM_NOW; i++) {
	//	if (i !=8) continue;//跳过SEED算法
		int k = 0; //第几顺位
		//创建模式图和数据图的ed
		ARGEdit pattern_ed;
		getNodeEdge_AL(pattern_ed, al_n_file[i], al_e_file[i], al_pe_func[i], col[i], al[i], Node_num[i],G[i],C[i]);
		ARGraph<Pe, void> pattern_g(&pattern_ed);
		pattern_g.SetNodeDestroyer(new PeDestroyer());
		pattern_g.SetNodeComparator(new PeComparator());

		for (int j = G[i]-1; j < MAX_G; j++){
			int a=0;
			for (k = C[i]; k <=3; k++) {
				ARGEdit data_ed;
				string arn=ARN[j] + string("/") + to_string(k) + string(".txt");
				getNodeEdge_AR(data_ed, arn , ARE[j]);
				//getData(pattern_ed,4);
				//getData(data_ed,5);
				ARGraph<Pe, void> data_g(&data_ed);
				//设置属性清除器
				data_g.SetNodeDestroyer(new PeDestroyer());
				//设置属性比较器
				data_g.SetNodeComparator(new PeComparator());
				//创建子图异构的初始状态
				VF2MonoState s0(&pattern_g, &data_g);
				//创建输出文件
				errno_t er;
				FILE *f;
				er = fopen_s(&f, "output/subIso.txt", "w");
				a = match(&s0, graph_visitor, f);
				//fprintf(f, "n: %d", matchingCnt);//将找到的匹配数输出至文本
				//cout << "a: " << a << endl;
				fclose(f);
				if (a >= 1) break;
			}
			if (a >= 1) break;
		}

		select("output/subIso.txt", col[i], result[i], m_row[i], m_col[i],Node_num[i]);
		out << "******************" << al[i] << "*******************" << endl;
		out << al[i] << "的匹配数: " << matchingCnt << endl;
		out << "使用的行数：" <<m_row[i]<< "   列数：" <<m_col[i]<< endl;
		out << "开始位置：" << k << endl;
		out << "对应的映射匹配：" << endl;
		cout << "******************" << al[i] << "*******************" << endl;
		cout << al[i] << "的匹配数: " << matchingCnt << endl;
		cout << "使用的行数：" <<m_row[i]<< "   列数：" <<m_col[i]<< endl;
		cout << "开始位置：" << k << endl;
		cout << "对应的映射匹配：" << endl;
		for (auto ai : result[i]) {
			out << ai.first << " " << ai.second << " | ";
			cout << ai.first << " " << ai.second << " | ";
			//统计pe功能使用情况
			//ai.second%12(定位pe*)   al_pe_func[i][ai.first]第i个算法的ai.first节点对应的功能
			pe_util[(ai.second+(k-1)*4) % 12][al_pe_func[i][ai.first]].push_back(al[i]);
		}
		matchingCnt = 0;
		out << endl<<endl<<endl;
		cout << endl<<endl<<endl;
	}



	//打印pe使用情况
	for (int i = 0; i < 12; i++) {
		out << "第" << setw(2)<<i << "个PE功能使用统计：";
		cout << "第" << setw(2)<<i << "个PE功能使用统计：";
		map<string, vector<string>>::iterator im = pe_util[i].begin();
		if (im != pe_util[i].end()) {//单独打印第一个功能，便于后面对齐
			out << "功能：" << setw(12) << (*im).first << "，使用次数：" << setw(2) << (*im).second.size();
			out << "，算法：";
			cout << "功能：" << setw(12) << (*im).first << "，使用次数：" << setw(2) << (*im).second.size();
			cout << "，算法：";
			for (auto si : (*im).second) 
			{
			out << setw(15) << si << " ";
        	cout << setw(15) << si << " ";
			}
			im++;
			out << endl;
			cout << endl;			
		}
		while (im != pe_util[i].end()) {
			out << setw(28)<<"功能：" <<setw(12)<< (*im).first << "，使用次数：" << setw(2)<<(*im).second.size();
			out << "，算法：";
			cout << setw(28)<<"功能：" <<setw(12)<< (*im).first << "，使用次数：" << setw(2)<<(*im).second.size();
			cout << "，算法：";
			for (auto si : (*im).second) {
				out <<setw(15)<< si << " ";		
				cout <<setw(15)<< si << " ";				
			}
			im++;
			out << endl;
			cout << endl;
		}
		out << endl;
		cout << endl;
	}
	return 0;
}

#endif