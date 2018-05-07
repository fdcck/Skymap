// Skymap_step3_0.cpp: 定义控制台应用程序的入口点。
//采用星对存储星图信息，

#include "stdafx.h"

using namespace std;
//宏常数
#define GuideStarNumber 4000 //导航星数目
#define StarPairNumber 10000 //星对的总数目
#define GroupNumber 200  //对星对分组情况

float magnitude[GuideStarNumber] = { 0 };
//星对存储结构
struct StarPair
{
	int group_nuber;//分组存储星对数据，分组依据为角距大小。首先星对按角距由小到大排序，然后没0.02°分一个组，便于索引
	float angular_distance;//四位有效数字存储，且小于对焦距d（d 可取12°）
	int star1;//存储星对的两颗星的编号
	int star2;

};
//分组索引可以采用链表的数据结构；使用数组模拟。使用一个头指针数组，存储不同角距所应对应的数组起点。
StarPair StarData[StarPairNumber];
int GroupHead[GroupNumber] = { 0 };
int GroupTail[GroupNumber] = { 0 };
int GroupSize[GroupNumber] = { 0 };
//begin_number=GroupHead[f(d)]; f(d)=(int)（d/0.02）;

fstream DataSource;
string Filename="SkyData.txt";
/*将星图数据加载到程序中，用于以后的匹配
（似乎这样对内存占用较大，可以直接操作文件句柄，使用seekg（）等函数实现移动，需要设计文件存储结构）*/
void DataLoad()
{
	
	DataSource.open(Filename, ios::in);
	//int IDcount=0;
	float a_d;
	int id1, id2;
	for (int i = 0; (i < StarPairNumber )&& !DataSource.eof(); i++) {
		DataSource >> a_d >> id1 >> id2;
		StarData[i].angular_distance = a_d;
		StarData[i].star1 = id1;
		StarData[i].star2 = id2;
		int id = a_d / 0.02;
		GroupSize[id]++;
	}
	for (int i = 1; i < 200; i++) {
		GroupHead[i] = GroupHead[i - 1] + GroupSize[i - 1];
		GroupTail[i] = GroupHead[i]+GroupSize[i] ;
	}
	DataSource.close();
}



struct MatchPair {
	int star1;
	int star2;
	int middle_star;
}MatchGroup[200];

bool m_check(int k, float m0, float m1, float m2,float deta) {
	int s0, s1, s2;
	s0 = MatchGroup[k].middle_star;
	s1 = MatchGroup[k].star1;
	s2 = MatchGroup[k].star2;
	if ((s0 - m0 <= deta) && (s1 - m1 <= deta) && (s2 - m2 <= deta)) return true;
	else return false;

}

int check(int k,float m1,float m2,float m3) {
	int s0, s1, s2;
	float alpha = 0.1;
	s0 = MatchGroup[k].middle_star;
	s1 = MatchGroup[k].star1;
	s2 = MatchGroup[k].star2;
	bool b1, b2, b3, b4, b5, b6,m_b;
	//b1 = (magnitude[s0] == m1) && (magnitude[s1] = m2) && (magnitude[s2] = m3);
	//b2 = (magnitude[s0] == m1) && (magnitude[s1] = m3) && (magnitude[s2] = m2);
	//b3 = (magnitude[s0] == m2) && (magnitude[s1] = m1) && (magnitude[s2] = m3);
	//b4 = (magnitude[s0] == m2) && (magnitude[s1] = m3) && (magnitude[s2] = m1);
	//b5 = (magnitude[s0] == m3) && (magnitude[s1] = m2) && (magnitude[s2] = m1);
	//b6 = (magnitude[s0] == m3) && (magnitude[s1] = m1) && (magnitude[s2] = m2);
	b1 = m_check(k, m1, m2, m3, alpha);
	b2 = m_check(k, m1, m3, m2, alpha);
	b3 = m_check(k, m2, m1, m3, alpha);
	b4 = m_check(k, m3, m3, m1, alpha);
	b5 = m_check(k, m3, m2, m1, alpha);
	b6 = m_check(k, m3, m1, m2, alpha);
	m_b = (b1 || b2 || b3 || b4 || b5 || b6);

	if (m_b) return 1;
	return 0;
}

void MatchAlgorithm(float ad12, float ad23, float ad13,float m1, float m2,float m3) 
{
	int Flag[GuideStarNumber] = { 0 };
	
	int group1 = ad12 / 0.02;
	int group2 = ad23 / 0.02;
	int group3 = ad13 / 0.02;

	map< int, vector<int> > StatStar;
	map< int, vector<int> >::iterator SS_iter;
	
	//扫描和标记组1中的星，把组中出现的星的flag参量标记为1，并记下与之相邻的另外一颗星。
	
	//使用map 数据结构存储毗邻星
	for (int i = GroupHead[group1]; i < GroupHead[group1] + GroupSize[group1]; i++) {
		int s1, s2;
		s1 = StarData[i].star1;
		s2 = StarData[i].star2;
		Flag[s1] = 1;
		Flag[s2] = 1;
		SS_iter = StatStar.find(s1);
		if (SS_iter == StatStar.end()) {
			vector<int> v1;
			v1.push_back(s2);
			StatStar.insert(pair<int, vector<int> >(s1, v1));
		}
		else {
			SS_iter->second.push_back(s2);
		}

		SS_iter=StatStar.find(s2);
		if (SS_iter == StatStar.end()) {
			vector<int> v2;
			v2.push_back(s1);
			StatStar.insert(pair<int, vector<int> >(s2, v2));
		}
		else {
			SS_iter->second.push_back(s1);
		}

	}

	//扫描标记组2中的星，如果该星已被标记为1，则标记为状态2；并记录与之毗邻的另一颗星；
	//（组1和组2为相同组的情况如何处理？是否需要特殊情况考虑呢？）
	int MatchCnt = 0;
	for (int i = GroupHead[group2]; i < GroupHead[group2] + GroupSize[group2]; i++) {
		int b1, b2;
		b1 = StarData[i].star1;
		b2 = StarData[i].star2;

		if (Flag[b1] != 0) {
			Flag[b1] = 2;
			for (int j = 0; j < StatStar[b1].size(); j++) {
				MatchGroup[MatchCnt].star1 = b2;
				MatchGroup[MatchCnt].star2 = StatStar[b1][j];
				MatchGroup[MatchCnt].middle_star = b1;
				MatchCnt++;
			}
		}

		if (Flag[b2] != 0) {
			Flag[b2] = 2;
			for (int j = 0; j < StatStar[b2].size(); j++) {
				MatchGroup[MatchCnt].star1 = b1;
				MatchGroup[MatchCnt].star2 = StatStar[b2][j];
				MatchGroup[MatchCnt].middle_star = b2;
				MatchCnt++;
			}
		}
	}

	//在标记组3中寻找可以符合条件的星对组成匹配三角形。使用栈存储成功匹配的编号；
	stack<int> succes;
	for (int i = GroupHead[group3]; i < GroupHead[group3]+GroupSize[group3]; i++) {
		int c1,c2;
		c1 = StarData[i].star1;
		c2 = StarData[i].star2;
		for (int j = 0; j < MatchCnt; j++) {
			if ((MatchGroup[j].star1 == c1 && MatchGroup[j].star2 == c2) || (MatchGroup[j].star1 == c2 && MatchGroup[j].star2 == c1)) {
				//匹配成功，获得一组匹配三角形。标记j,或者直接输出j中的三个星的编号
				succes.push(j);
				Flag[MatchGroup[j].middle_star] = 3;
			}
		}
	}

	if (succes.empty()) {
		cout << "Matching Failled";
		//扩大组的范围，或换一组三角形匹配。
	}
	else {
		//从匹配成功的三角形中筛选出最终结果，需要结合星等数据选择，或者加入GPS等参考数据
		int result=-1;
		while (!succes.empty()) {
			int k = succes.top();
			//int sign = check(k,m1,m2,m3);
			//if (sign == 1) { 
			//	result = k;
			//	break; 
			//}
			result = k; break;
		}
		if (result == -1) {
			cout << "Error!";
			//exit(0);
		}
		//把三角形中的三颗星确定，使用角距区分。
		else {
			cout << result << endl;
			cout << MatchGroup[result].middle_star << " " << MatchGroup[result].star1 << " " << MatchGroup[result].star2 << endl;
		}

	}

}


int main()
{
	DataLoad();
	MatchAlgorithm(2.8, 3.3, 3.4,1.0,1.0,1.0);
	
}

