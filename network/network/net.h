#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <iterator>
//#include "file-data.h"

using namespace std;

const double ALPHA = 1.8;
const double BETA = 0.25;

void print(int n)
{
	cout << n << '\0';
}

class Node {
public:
	Node() :num(0), k(0) {};
	Node(int num, int k) :num(num), k(k), load(pow(k, ALPHA)), capacity((1 + BETA)*(pow(k, ALPHA))), isIndependent(0), isInGP(0), status(1) {};
public:
	int num;//节点编号
	int k;//节点度
	//double betweenness;//节点介数
	double load;//节点负载
	double capacity;//节点容量
	//double cluster;//聚类系数
	int isIndependent;//是否构成依赖边:1-依赖边，0-无依赖关系
	int isInGP;//是否在最大连通图内：1-在最大连通图内，0-不在
	int status;//节点状态，是否失效：2-等待失效1-没失效，0-失效
};

class Edge {
public:
	Edge() :num(0) {};
	Edge(int num, int nodeNum1, int nodeNum2) :num(num), nodeNum1(nodeNum1), nodeNum2(nodeNum2) {};
public:
	int num;//边编号
	int nodeNum1;//连接节点号1
	int nodeNum2;//连接节点号2
};

class Net {
public:
	Net() = default;
	Net(vector<vector<int> > adjMatrix);//邻接矩阵初始化
	Net(string style, int N, int m0, int m,int flag);//BA网络初始化，flag=0,初始孤立点；flag=1,初始完全连接
	Net(int nNum, int eNum);


public:
	vector<vector<int> > adjMatrix;//邻接矩阵
	//vector<vector<int> > disMatrix;//距离矩阵
	vector<Node> node;
	int nodeNum;//节点数
	vector<Edge> edge;
	int edgeNum;//总边数

public:
	//void initalDistanceMatrix();//计算距离矩阵
	//int distance(int i, int j);//节点i和节点j之间的距离
	//double avgDistance();//网络的平均路径长度
	//void initalCluster();//计算网络中节点聚类系数
	//double avgCluster();//网络平均聚类系数
	double avgDegree();//网络平均度
	//vector<int> degreeDistribution();//网络节点度分布
	double cascade(int style);//级联失效:style=1度最大失效，style=2负载最大失效
	int maxGraph();//最大联通子图

private:
	bool isInVector(int num,vector<int> vec);
};

//邻接矩阵初始化
Net::Net(vector<vector<int> > adjMatrix) {
	Net::adjMatrix = adjMatrix;
	int k = 0;
	int ksum = 0;

	nodeNum = adjMatrix.size();
	node = vector<Node>(nodeNum);

	for (int i = 0; i < nodeNum; i++ ) {
		for (int j = 0; j < nodeNum; j++) {
			k = k + adjMatrix[i][j];
		}
		node[i] = Node((i+1), k);
		ksum = ksum + k;
		k = 0;
	}

	int n = 0;//计总边数

	for (int i = 0; i < nodeNum; i++) {
		for (int j = i; j < nodeNum; j++) {
			if (adjMatrix[i][j] != 0) {
				//edge[n] = Edge((n + 1), (i + 1), (j + 1));
				n++;
			}
		}
	}

	edgeNum = n;
	edge = vector<Edge>(edgeNum);
	int m = 0;//循环边计数

	for (int i = 0; i < nodeNum; i++) {
				for (int j = i; j < nodeNum; j++) {
			if (adjMatrix[i][j] != 0 && m<n ) {
				edge[m] = Edge((m + 1), (i + 1), (j + 1));
				m++;
			}
		}
	}


}
//BA网络初始化,flag=0,初始孤立点；flag=1,初始完全连接
Net::Net(string style, int N, int m0, int m, int flag){
	for (string::size_type index = 0; index != style.size(); ++index) {
		style[index] = toupper(style[index]);
	}
	if (style.compare("BA") == 0) {
		//初始化
		adjMatrix = vector<vector<int> >(N, vector<int>(N,0));
		nodeNum = N;
		edgeNum = m*(N - m0) + flag*m0*(m0 - 1) / 2;
		node = vector<Node>(nodeNum);
		edge = vector<Edge>(edgeNum);
		int ksum = 0;//网络节点总度
		int fm = 0;//循环边计数
		if (flag == 1) {
			for (int i = 0; i < m0; i++) {
				node[i].num = i + 1;
				node[i].k = m0 - 1;
				for (int j = i + 1; j < m0; j++) {
					adjMatrix[i][j] = 1;
					adjMatrix[j][i] = 1;
					edge[fm] = Edge((fm + 1), (i + 1), (j + 1));
					fm++;
				}
			}
			ksum = m0*(m0 - 1);
		}
		else if (flag == 0) {
			for (int i = 0; i < m0; i++) {
				//node[i] = Node((i + 1), 0);
				node[i].num = i + 1;
				node[i].k = 0;
			}
			ksum = 0;
		}
		//根据优先链接增长
		vector<double> Pn(N, 0.0);
		double Prs = 0.0;
		for (int n = m0; n < N; n++) {//n为节点数
			for (int i = 0; i < n; i++) {
				if (ksum == 0) {
					Pn[i] = 1;
				}
				else {
					Pn[i] = (double) node[i].k / ksum;
				}
			}
			Prs = 1;
			for (int j = 0; j < m; j++) {//m为加入的边数
				double ps = 0;
				double rd =  rand()/(double) RAND_MAX;
				for (int k = 0; k < n; k++) {//k为求累加概率循环数
					ps = ps + Pn[k] / Prs;
					if (rd < ps) {
						adjMatrix[n][k] = 1;
						adjMatrix[k][n] = 1;
						node[n].k = m;
						node[n].num = n + 1;
						node[k].k++;
						edge[fm] = Edge((fm + 1), min(n, k) + 1, max(n, k) + 1);
						fm++;
						Prs = Prs - Pn[k];
						Pn[k] = 0;
						break;
					}
				}
			}
			ksum = ksum + m * 2;
		}
		//cout << (double)ksum / N << endl;//平均度
		for (int i = 0; i < N; i++) {
			node[i] = Node(node[i].num,node[i].k);
		}
	}
}
//初始化
Net::Net(int nNum, int eNum) {

}
//网络平均度
double Net::avgDegree() {
	double sumK = 0;
	size_t len = node.size();
	for (size_t i = 0; i < len; i++) {
		sumK = sumK + node[i].k;
	}
	return sumK / (double)len;
}
//单一网络级联失效，返回鲁棒性指标G=N'/N;
double Net::cascade(int style) {
	queue <Node> Qfail;
	int len = node.size();
	Node temp = node[0];
	//初始化攻击节点
	switch (style) {
	case 1://度最大节点
		for (int i = 0; i < len; i++) {
			if (node[i].k > temp.k) {
				temp = node[i];
			}
		}
		Qfail.push(temp);
		break;
	case 2://负载最大的节点
		for (int i = 0; i < len; i++) {
			if (node[i].load > temp.load) {
				temp = node[i];
			}
		}
		//cout << "初始攻击的节点是：" << temp.num << endl;
		Qfail.push(temp);
		break;
	}
	while (Qfail.size()) {
		while (Qfail.size()) { //对失效节点进行删边
			temp = Qfail.front();
			int neighborNum = 0;
			double neighborCsum = 0;
			for (int i = 0; i < len; i++) {
				if (adjMatrix[temp.num - 1][i] == 1) {
					neighborNum++;
					neighborCsum += node[i].capacity;
				}
			}
			for (int i = 0; i < len; i++) {//节点度减一
				if (adjMatrix[temp.num - 1][i] == 1) {
					adjMatrix[temp.num - 1][i] = 0;
					adjMatrix[i][temp.num - 1] = 0;
					node[i].k--;
					node[i].load += (temp.load)*(node[i].capacity) / neighborCsum;
					node[temp.num - 1].k--;
					node[temp.num - 1].status = 0;//失效
				}
			}
			for (size_t i = 0; i < edge.size(); i++) {//删边
				if (edge[i].nodeNum1 == temp.num || edge[i].nodeNum2 == temp.num) {
					edge.erase(edge.begin() + i);
				}
			}
			Qfail.pop();
		}//对失效节点进行删边结束
		for (int i = 0; i < len; i++) {
			if (node[i].load > node[i].capacity && node[i].status == 1) {
				Qfail.push(node[i]);
				node[i].status = 2;
			}
		}
	}//级联失效结束
	{//级联失效结束的图输出到文件
		string fname = "级联失效.net";
		ofstream netdata;
		netdata.open(fname, ofstream::out | std::ofstream::trunc);
		int nodeNum = node.size();
		netdata << "*Vertices " << nodeNum << endl;
		for (int i = 0; i < nodeNum; i++) {
			netdata << (i + 1) << " \"" << node[i].num << "\"" << endl;
		}
		netdata << "*Matrix" << endl;
		for (int i = 0; i < nodeNum; i++) {
			for (int j = 0; j < nodeNum; j++) {
				netdata << adjMatrix[i][j] << " ";
			}
			netdata << endl;
		}
		netdata.close();
	}
	//找最大联通子图
	return maxGraph() / (double)len;
}

int Net::maxGraph() {
	vector <int> nodeVisitNum;//访问队列，保存节点编号
	vector<pair<int, int> > subGraphs;//子图（起始节点编号，子图节点数目）
	int maxNum=1;//最大子图节点数
	int maxSubNode = 1;//最大子图搜索起始节点
	queue <int> Qtmp;//子图搜索队列
	int len = node.size();
	for (int i = 0; i < len; i++) {
		if (node[i].status == 1 && !isInVector(node[i].num, nodeVisitNum)) {
			nodeVisitNum.push_back(node[i].num);
			//BFS广度优先搜索,节点i
			int Nsub = 1;//子图节点数
			int key = 0;
			Qtmp.push(node[i].num);//子图邻边队列
			while (Qtmp.size() > 0) {
				key = Qtmp.front();
				for (int j = 0; j < len; j++) {
					if (adjMatrix[key-1][j] == 1 && node[j].status == 1 && !isInVector(node[j].num, nodeVisitNum)) {
						nodeVisitNum.push_back(node[j].num);
						Qtmp.push(node[j].num);
						Nsub++;
					}
				}
				Qtmp.pop();
			}//子图搜索结束
			subGraphs.push_back(pair<int,int> (i+1,Nsub));
			if (Nsub > maxNum) {
				maxNum = Nsub;
				maxSubNode = i + 1;
			}
		}
	}
	//节点在最大子图内赋值
	nodeVisitNum.clear();
	int key = 0;
	nodeVisitNum.push_back(maxSubNode);
	Qtmp.push(maxSubNode);
	node[maxSubNode - 1].isInGP = 1;
	while (Qtmp.size() > 0) {
		key = Qtmp.front();
		for (int j = 0; j < len; j++) {
			if (adjMatrix[key - 1][j] == 1 && node[j].status == 1 && !isInVector(node[j].num, nodeVisitNum)) {
				nodeVisitNum.push_back(node[j].num);
				Qtmp.push(node[j].num);
				node[j].isInGP = 1;
			}
		}
		Qtmp.pop();
	}
	return maxNum;
}

bool Net::isInVector(int num, vector<int> vec) {
	if (!vec.empty()) {
		for (size_t i = 0; i < vec.size(); i++) {
			if (num == vec[i]) {
				return true;
			}
		}
	}
	return false;
}

//耦合边结构
struct coupleEdge {
	Node first;
	Node second;
	int style;//耦合边方式，1：互连边；2：耦合边
};
//耦合网络类
class CoupledNet {
public:
	CoupledNet(Net A, Net B, double p, int flag);
	CoupledNet(Net A, Net B, vector<vector<int> > coupleMatrix, vector<coupleEdge> coupleNodes) : A(A), B(B), coupleMatrix(coupleMatrix), coupleNodes(coupleNodes) {};
	CoupledNet(Net A, Net B, vector<vector<int> > coupleMatrix);
	CoupledNet(Net A, Net B, vector<coupleEdge> coupleNodes);
public:
	Net A;
	Net B;
	vector<vector<int> > coupleMatrix;
	vector<coupleEdge> coupleNodes;
public:
	double cascade(int style);

private:
	vector<int> bubbleSort(vector<Node> node,int flag);
	bool isInVector(int num, vector<int> vec);

};
//两个网络A1和B1，耦合强度为p
//flag=1,随机连接；flag=2,同配连接（大到小）；flag=3，同配连接（小到大）；
//falg=4，异配连接（A从大到小）；flag=5，异配连接（A从小到大）；flag=6，异配连接（对称模式）
CoupledNet::CoupledNet(Net A1, Net B1, double p, int flag) {
	//耦合连边后节点度没有变化，不对
	A = A1;
	B = B1;
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	int style = 1;//耦合边方式
	coupleMatrix = vector<vector<int> >(sizeA, vector<int>(sizeB, 0));//耦合矩阵初始化
	int edgeNum = (int) (sizeA*p);
	vector<int> sortA1 = bubbleSort(A.node, 1);
	//for_each(sortA1.begin(), sortA1.end(), print);
	//cout << sortA1 << endl;
	vector<int> sortA2 = bubbleSort(A.node, 2);
	//for_each(sortA2.begin(), sortA2.end(), print);
	vector<int> sortB1 = bubbleSort(B.node, 1);
	vector<int> sortB2 = bubbleSort(B.node, 2);
	vector<int> ANode;
	vector<int> BNode;

	switch (flag) {
	case 1://随机连接
		for (int i = 0; i < edgeNum; i++) {
			//srand((unsigned)time(NULL));
			int rand1, rand2;
			do {
				rand1 = rand() % sizeA;
			} while (isInVector(rand1, ANode));
			do {
				rand2 = rand() % sizeB;
			} while (isInVector(rand2, BNode));
			ANode.push_back(rand1);
			BNode.push_back(rand2);
			A.node[rand1].isIndependent = 1;
			B.node[rand2].isIndependent = 1;
			coupleMatrix[rand1][rand2] = 1;
			coupleNodes.push_back({ A.node[rand1],B.node[rand2],style });
		}
		break;
	case 2://同配连接（大到小）
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA1[i] - 1].isIndependent = 1;
			B.node[sortB1[i] - 1].isIndependent = 1;
			coupleMatrix[sortA1[i] - 1][sortB1[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA1[i] - 1],B.node[sortB1[i] - 1],style });
		}
		break;
	case 3://同配连接（小到大）
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA2[i] - 1].isIndependent = 1;
			B.node[sortB2[i] - 1].isIndependent = 1;
			coupleMatrix[sortA2[i] - 1][sortB2[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA2[i] - 1],B.node[sortB2[i] - 1],style });
		}
		break;
	case 4://异配连接（A从大到小）
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA1[i] - 1].isIndependent = 1;
			B.node[sortB2[i] - 1].isIndependent = 1;
			coupleMatrix[sortA1[i] - 1][sortB2[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA1[i] - 1],B.node[sortB2[i] - 1],style });
		}
		break;
	case 5://异配连接（A从小到大）
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA2[i] - 1].isIndependent = 1;
			B.node[sortB1[i] - 1].isIndependent = 1;
			coupleMatrix[sortA2[i] - 1][sortB1[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA2[i] - 1],B.node[sortB1[i] - 1],style });
		}
		break;
	case 6://异配连接（对称模式）
		int num1 = edgeNum / 2;
		int num2 = edgeNum - num1;
		for (int i = 0; i < num1; i++) {
			A.node[sortA1[i] - 1].isIndependent = 1;
			B.node[sortB2[i] - 1].isIndependent = 1;
			coupleMatrix[sortA1[i] - 1][sortB2[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA1[i] - 1],B.node[sortB2[i] - 1],style });
		}
		for (int i = 0; i < num2; i++) {
			A.node[sortA2[i] - 1].isIndependent = 1;
			B.node[sortB1[i] - 1].isIndependent = 1;
			coupleMatrix[sortA2[i] - 1][sortB1[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA2[i] - 1],B.node[sortB1[i] - 1],style });
		}
		break;
	}
	//测试
	//for (int i = 0; i < coupleNodes.size(); i++) {
	//	cout << "A" << coupleNodes[i].first.num << "B" << coupleNodes[i].second.num << endl;
	//}
}
CoupledNet::CoupledNet(Net A1, Net B1, vector<vector<int> > coupleMatrix1) {
	A = A1;
	B = B1;
	int style = 1;//耦合边方式
	coupleMatrix = coupleMatrix1;
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	for (int i = 0; i < sizeA; i++) {
		for (int j = 0; j < sizeB; j++) {
			if (coupleMatrix[i][j] == 1) {
				coupleNodes.push_back({ A.node[i],B.node[j],style });
			}
		}
	}
}
CoupledNet::CoupledNet(Net A1, Net B1, vector<coupleEdge> coupleNodes) {
	A = A1;
	B = B1;
	int style = 1;//耦合边方式
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	coupleMatrix = vector<vector<int> >(sizeA, vector<int>(sizeB, 0));//耦合矩阵初始化
	for (size_t i = 0; i < coupleNodes.size(); i++) {
		coupleMatrix[coupleNodes[i].first.num - 1][coupleNodes[i].second.num - 1] = 1;
	}
}
//冒泡排序，节点按照度排序，返回节点编号序列；
//flag=1,从大到小；flag=2.从小到大；
vector<int> CoupledNet::bubbleSort(vector<Node> node,int flag) {
	int size = node.size();
	int temp = 0;
	vector<int> sortNum(size,0);
	vector<int> sortK(size, 0);
	for (int i = 0; i < size; i++) {
		sortNum[i] = node[i].num;
		sortK[i] = node[i].k;
	}
	//cout << "节点编号";
	//for_each(sortNum.begin(), sortNum.end(), print);
	switch (flag) {
	case 1://从大到小排序
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size - 1; j++) {
				if (sortK[j] < sortK[j+1]) {
					temp = sortK[j];
					sortK[j] = sortK[j + 1];
					sortK[j + 1] = temp;
					temp = sortNum[j];
					sortNum[j] = sortNum[j + 1];
					sortNum[j + 1] = temp;
				}
			}
		}
		break;
	case 2://从小到大排序
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size-1; j++) {
				if (sortK[j] > sortK[j + 1]) {
					temp = sortK[j];
					sortK[j] = sortK[j + 1];
					sortK[j + 1] = temp;
					temp = sortNum[j];
					sortNum[j] = sortNum[j + 1];
					sortNum[j + 1] = temp;
				}
			}
		}
		break;
	}
	return sortNum;
}
bool CoupledNet::isInVector(int num, vector<int> vec) {
	if (!vec.empty()) {
		for (size_t i = 0; i < vec.size(); i++) {
			if (num == vec[i]) {
				return true;
			}
		}
	}
	return false;
}
//级联失效，style=1度最大失效，style=2负载最大失效
double CoupledNet::cascade(int style) {
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	int N = sizeA + sizeB;
	vector<vector<int> > adjMatrix = vector<vector<int> >(N, vector<int>(N, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i < sizeA && j < sizeB) {
				adjMatrix[i][j] = A.adjMatrix[i][j];
			}
			else if (i < sizeA && j >= sizeA) {
				adjMatrix[i][j] = coupleMatrix[i][j - sizeA];
			}
			else if (i >= sizeA && j < sizeA) {
				adjMatrix[i][j] = coupleMatrix[j][i - sizeA];
			}
			else {
				adjMatrix[i][j] = B.adjMatrix[i-sizeA][j-sizeA];
			}
		}
	}
	Net cNet(adjMatrix);
	return cNet.cascade(style);
}
