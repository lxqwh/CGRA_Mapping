//���Դ���

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

//��ӽڵ������
#include "Pe.h"

using namespace std;

std::ofstream out("output/output");

#define AL_NUM_NOW 30 //��ǰ�㷨����
#define MAX_G 5 //������ӳ����������
#define MAXNODES 60000

int matchingCnt = 0;//��¼���ֵ�ƥ����

void getNodeEdge_AL(ARGEdit &ed, string nodeName, string edgeName, vector<string> &func, vector<int>&col, string &name,int &numN, int &G, int &C)
{
	/******************************��ȡ�ڵ�************************************/
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+������+" "+����
	//���磺0  0 0 0_xorlut_
	ifstream nodeFile(nodeName);
	int nodeID, c;
	string pe;
	string note;
	nodeFile >> note;
	nodeFile >> name; //�㷨����
	nodeFile >> numN; //������
	nodeFile >> G; //�����ʼ��Լ��
	nodeFile >> C; //�����ʼ��Լ��
	while (!nodeFile.eof())
	{
		nodeFile >> nodeID >> c >> pe;
//		row.push_back(r);
		col.push_back(c);
		func.push_back(pe.substr(2, pe.size() - 3));
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
	//���ļ���ȡ�ڵ㣬�ڵ��ļ�һ���ǽڵ�ID+" "+������+" "+������+" "+����
	//���磺0  0 0 1_au_xorau_auxor_xorauxor_xorxorau_xorxorauxor_sh_xorsh_shxor_xorshxor_xor_xorxor_xorxorxor_or_and_&&&&_bn_xorbn_bnxor_xorbnxor_
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

void select(string fileName, vector<int>&col, vector<pair<int,int>>& best,int &brm, int &bcm, int nodeNum)
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
	int tsy = 0;  //����3�������������Ļ�����ʹӳ�価���ܿ���ӳ��
	int tsx = 0;  //����4����������ӳ��
	int tmix = 0; //����5�������������ٽ���
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
	if (matchingCnt >= 10000) return true;//���������ʱ�����ܵĽ���÷ǳ���>100w����˴���1��ʱ�Ͳ��ڼ���
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
	//�������ݴ洢
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
								 };//�㷨ͼ�������
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
								 };//�㷨ͼ������
	vector<vector<int>> /*row(AL_NUM),*/ col(AL_NUM_NOW);//�ڵ�����Ϣ����
	vector<string> al(AL_NUM_NOW);//�㷨����
	vector<int> Node_num(AL_NUM_NOW);//�㷨�ڵ���
	vector<vector<pair<int, int>>> result(AL_NUM_NOW);//ӳ��������
	vector<vector<string>> al_pe_func(AL_NUM_NOW);//��ͬ�㷨�Ľڵ㹦�ܱ���
	vector<int> m_row(AL_NUM_NOW), m_col(AL_NUM_NOW);//ӳ����С��н��
	vector<map<string, vector<string>>> pe_util(12);//pe�Ĺ���ʹ��ͳ��
	vector<string> ARN(MAX_G), ARE(MAX_G);
	vector<int> G(AL_NUM_NOW, 0), C(AL_NUM_NOW, 1);
	for (int i = 1; i <= MAX_G; i++){//�ܹ������ļ�
		ARN[i - 1] = string("AR/ARN_G") + to_string(i);
		ARE[i - 1] = string("AR/ARE_G") + to_string(i) + string(".txt");
	}



	//ӳ��ÿһ���㷨
	for (int i = 0; i < AL_NUM_NOW; i++) {
	//	if (i !=8) continue;//����SEED�㷨
		int k = 0; //�ڼ�˳λ
		//����ģʽͼ������ͼ��ed
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
				//�������������
				data_g.SetNodeDestroyer(new PeDestroyer());
				//�������ԱȽ���
				data_g.SetNodeComparator(new PeComparator());
				//������ͼ�칹�ĳ�ʼ״̬
				VF2MonoState s0(&pattern_g, &data_g);
				//��������ļ�
				errno_t er;
				FILE *f;
				er = fopen_s(&f, "output/subIso.txt", "w");
				a = match(&s0, graph_visitor, f);
				//fprintf(f, "n: %d", matchingCnt);//���ҵ���ƥ����������ı�
				//cout << "a: " << a << endl;
				fclose(f);
				if (a >= 1) break;
			}
			if (a >= 1) break;
		}

		select("output/subIso.txt", col[i], result[i], m_row[i], m_col[i],Node_num[i]);
		out << "******************" << al[i] << "*******************" << endl;
		out << al[i] << "��ƥ����: " << matchingCnt << endl;
		out << "ʹ�õ�������" <<m_row[i]<< "   ������" <<m_col[i]<< endl;
		out << "��ʼλ�ã�" << k << endl;
		out << "��Ӧ��ӳ��ƥ�䣺" << endl;
		cout << "******************" << al[i] << "*******************" << endl;
		cout << al[i] << "��ƥ����: " << matchingCnt << endl;
		cout << "ʹ�õ�������" <<m_row[i]<< "   ������" <<m_col[i]<< endl;
		cout << "��ʼλ�ã�" << k << endl;
		cout << "��Ӧ��ӳ��ƥ�䣺" << endl;
		for (auto ai : result[i]) {
			out << ai.first << " " << ai.second << " | ";
			cout << ai.first << " " << ai.second << " | ";
			//ͳ��pe����ʹ�����
			//ai.second%12(��λpe*)   al_pe_func[i][ai.first]��i���㷨��ai.first�ڵ��Ӧ�Ĺ���
			pe_util[(ai.second+(k-1)*4) % 12][al_pe_func[i][ai.first]].push_back(al[i]);
		}
		matchingCnt = 0;
		out << endl<<endl<<endl;
		cout << endl<<endl<<endl;
	}



	//��ӡpeʹ�����
	for (int i = 0; i < 12; i++) {
		out << "��" << setw(2)<<i << "��PE����ʹ��ͳ�ƣ�";
		cout << "��" << setw(2)<<i << "��PE����ʹ��ͳ�ƣ�";
		map<string, vector<string>>::iterator im = pe_util[i].begin();
		if (im != pe_util[i].end()) {//������ӡ��һ�����ܣ����ں������
			out << "���ܣ�" << setw(12) << (*im).first << "��ʹ�ô�����" << setw(2) << (*im).second.size();
			out << "���㷨��";
			cout << "���ܣ�" << setw(12) << (*im).first << "��ʹ�ô�����" << setw(2) << (*im).second.size();
			cout << "���㷨��";
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
			out << setw(28)<<"���ܣ�" <<setw(12)<< (*im).first << "��ʹ�ô�����" << setw(2)<<(*im).second.size();
			out << "���㷨��";
			cout << setw(28)<<"���ܣ�" <<setw(12)<< (*im).first << "��ʹ�ô�����" << setw(2)<<(*im).second.size();
			cout << "���㷨��";
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