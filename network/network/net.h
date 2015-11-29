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
	int num;//�ڵ���
	int k;//�ڵ��
	//double betweenness;//�ڵ����
	double load;//�ڵ㸺��
	double capacity;//�ڵ�����
	//double cluster;//����ϵ��
	int isIndependent;//�Ƿ񹹳�������:1-�����ߣ�0-��������ϵ
	int isInGP;//�Ƿ��������ͨͼ�ڣ�1-�������ͨͼ�ڣ�0-����
	int status;//�ڵ�״̬���Ƿ�ʧЧ��2-�ȴ�ʧЧ1-ûʧЧ��0-ʧЧ
};

class Edge {
public:
	Edge() :num(0) {};
	Edge(int num, int nodeNum1, int nodeNum2) :num(num), nodeNum1(nodeNum1), nodeNum2(nodeNum2) {};
public:
	int num;//�߱��
	int nodeNum1;//���ӽڵ��1
	int nodeNum2;//���ӽڵ��2
};

class Net {
public:
	Net() = default;
	Net(vector<vector<int> > adjMatrix);//�ڽӾ����ʼ��
	Net(string style, int N, int m0, int m,int flag);//BA�����ʼ����flag=0,��ʼ�����㣻flag=1,��ʼ��ȫ����
	Net(int nNum, int eNum);


public:
	vector<vector<int> > adjMatrix;//�ڽӾ���
	//vector<vector<int> > disMatrix;//�������
	vector<Node> node;
	int nodeNum;//�ڵ���
	vector<Edge> edge;
	int edgeNum;//�ܱ���

public:
	//void initalDistanceMatrix();//����������
	//int distance(int i, int j);//�ڵ�i�ͽڵ�j֮��ľ���
	//double avgDistance();//�����ƽ��·������
	//void initalCluster();//���������нڵ����ϵ��
	//double avgCluster();//����ƽ������ϵ��
	double avgDegree();//����ƽ����
	//vector<int> degreeDistribution();//����ڵ�ȷֲ�
	double cascade(int style);//����ʧЧ:style=1�����ʧЧ��style=2�������ʧЧ
	int maxGraph();//�����ͨ��ͼ

private:
	bool isInVector(int num,vector<int> vec);
};

//�ڽӾ����ʼ��
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

	int n = 0;//���ܱ���

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
	int m = 0;//ѭ���߼���

	for (int i = 0; i < nodeNum; i++) {
				for (int j = i; j < nodeNum; j++) {
			if (adjMatrix[i][j] != 0 && m<n ) {
				edge[m] = Edge((m + 1), (i + 1), (j + 1));
				m++;
			}
		}
	}


}
//BA�����ʼ��,flag=0,��ʼ�����㣻flag=1,��ʼ��ȫ����
Net::Net(string style, int N, int m0, int m, int flag){
	for (string::size_type index = 0; index != style.size(); ++index) {
		style[index] = toupper(style[index]);
	}
	if (style.compare("BA") == 0) {
		//��ʼ��
		adjMatrix = vector<vector<int> >(N, vector<int>(N,0));
		nodeNum = N;
		edgeNum = m*(N - m0) + flag*m0*(m0 - 1) / 2;
		node = vector<Node>(nodeNum);
		edge = vector<Edge>(edgeNum);
		int ksum = 0;//����ڵ��ܶ�
		int fm = 0;//ѭ���߼���
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
		//����������������
		vector<double> Pn(N, 0.0);
		double Prs = 0.0;
		for (int n = m0; n < N; n++) {//nΪ�ڵ���
			for (int i = 0; i < n; i++) {
				if (ksum == 0) {
					Pn[i] = 1;
				}
				else {
					Pn[i] = (double) node[i].k / ksum;
				}
			}
			Prs = 1;
			for (int j = 0; j < m; j++) {//mΪ����ı���
				double ps = 0;
				double rd =  rand()/(double) RAND_MAX;
				for (int k = 0; k < n; k++) {//kΪ���ۼӸ���ѭ����
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
		//cout << (double)ksum / N << endl;//ƽ����
		for (int i = 0; i < N; i++) {
			node[i] = Node(node[i].num,node[i].k);
		}
	}
}
//��ʼ��
Net::Net(int nNum, int eNum) {

}
//����ƽ����
double Net::avgDegree() {
	double sumK = 0;
	size_t len = node.size();
	for (size_t i = 0; i < len; i++) {
		sumK = sumK + node[i].k;
	}
	return sumK / (double)len;
}
//��һ���缶��ʧЧ������³����ָ��G=N'/N;
double Net::cascade(int style) {
	queue <Node> Qfail;
	int len = node.size();
	Node temp = node[0];
	//��ʼ�������ڵ�
	switch (style) {
	case 1://�����ڵ�
		for (int i = 0; i < len; i++) {
			if (node[i].k > temp.k) {
				temp = node[i];
			}
		}
		Qfail.push(temp);
		break;
	case 2://�������Ľڵ�
		for (int i = 0; i < len; i++) {
			if (node[i].load > temp.load) {
				temp = node[i];
			}
		}
		//cout << "��ʼ�����Ľڵ��ǣ�" << temp.num << endl;
		Qfail.push(temp);
		break;
	}
	while (Qfail.size()) {
		while (Qfail.size()) { //��ʧЧ�ڵ����ɾ��
			temp = Qfail.front();
			int neighborNum = 0;
			double neighborCsum = 0;
			for (int i = 0; i < len; i++) {
				if (adjMatrix[temp.num - 1][i] == 1) {
					neighborNum++;
					neighborCsum += node[i].capacity;
				}
			}
			for (int i = 0; i < len; i++) {//�ڵ�ȼ�һ
				if (adjMatrix[temp.num - 1][i] == 1) {
					adjMatrix[temp.num - 1][i] = 0;
					adjMatrix[i][temp.num - 1] = 0;
					node[i].k--;
					node[i].load += (temp.load)*(node[i].capacity) / neighborCsum;
					node[temp.num - 1].k--;
					node[temp.num - 1].status = 0;//ʧЧ
				}
			}
			for (size_t i = 0; i < edge.size(); i++) {//ɾ��
				if (edge[i].nodeNum1 == temp.num || edge[i].nodeNum2 == temp.num) {
					edge.erase(edge.begin() + i);
				}
			}
			Qfail.pop();
		}//��ʧЧ�ڵ����ɾ�߽���
		for (int i = 0; i < len; i++) {
			if (node[i].load > node[i].capacity && node[i].status == 1) {
				Qfail.push(node[i]);
				node[i].status = 2;
			}
		}
	}//����ʧЧ����
	{//����ʧЧ������ͼ������ļ�
		string fname = "����ʧЧ.net";
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
	//�������ͨ��ͼ
	return maxGraph() / (double)len;
}

int Net::maxGraph() {
	vector <int> nodeVisitNum;//���ʶ��У�����ڵ���
	vector<pair<int, int> > subGraphs;//��ͼ����ʼ�ڵ��ţ���ͼ�ڵ���Ŀ��
	int maxNum=1;//�����ͼ�ڵ���
	int maxSubNode = 1;//�����ͼ������ʼ�ڵ�
	queue <int> Qtmp;//��ͼ��������
	int len = node.size();
	for (int i = 0; i < len; i++) {
		if (node[i].status == 1 && !isInVector(node[i].num, nodeVisitNum)) {
			nodeVisitNum.push_back(node[i].num);
			//BFS�����������,�ڵ�i
			int Nsub = 1;//��ͼ�ڵ���
			int key = 0;
			Qtmp.push(node[i].num);//��ͼ�ڱ߶���
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
			}//��ͼ��������
			subGraphs.push_back(pair<int,int> (i+1,Nsub));
			if (Nsub > maxNum) {
				maxNum = Nsub;
				maxSubNode = i + 1;
			}
		}
	}
	//�ڵ��������ͼ�ڸ�ֵ
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

//��ϱ߽ṹ
struct coupleEdge {
	Node first;
	Node second;
	int style;//��ϱ߷�ʽ��1�������ߣ�2����ϱ�
};
//���������
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
//��������A1��B1�����ǿ��Ϊp
//flag=1,������ӣ�flag=2,ͬ�����ӣ���С����flag=3��ͬ�����ӣ�С���󣩣�
//falg=4���������ӣ�A�Ӵ�С����flag=5���������ӣ�A��С���󣩣�flag=6���������ӣ��Գ�ģʽ��
CoupledNet::CoupledNet(Net A1, Net B1, double p, int flag) {
	//������ߺ�ڵ��û�б仯������
	A = A1;
	B = B1;
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	int style = 1;//��ϱ߷�ʽ
	coupleMatrix = vector<vector<int> >(sizeA, vector<int>(sizeB, 0));//��Ͼ����ʼ��
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
	case 1://�������
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
	case 2://ͬ�����ӣ���С��
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA1[i] - 1].isIndependent = 1;
			B.node[sortB1[i] - 1].isIndependent = 1;
			coupleMatrix[sortA1[i] - 1][sortB1[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA1[i] - 1],B.node[sortB1[i] - 1],style });
		}
		break;
	case 3://ͬ�����ӣ�С����
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA2[i] - 1].isIndependent = 1;
			B.node[sortB2[i] - 1].isIndependent = 1;
			coupleMatrix[sortA2[i] - 1][sortB2[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA2[i] - 1],B.node[sortB2[i] - 1],style });
		}
		break;
	case 4://�������ӣ�A�Ӵ�С��
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA1[i] - 1].isIndependent = 1;
			B.node[sortB2[i] - 1].isIndependent = 1;
			coupleMatrix[sortA1[i] - 1][sortB2[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA1[i] - 1],B.node[sortB2[i] - 1],style });
		}
		break;
	case 5://�������ӣ�A��С����
		for (int i = 0; i < edgeNum; i++) {
			A.node[sortA2[i] - 1].isIndependent = 1;
			B.node[sortB1[i] - 1].isIndependent = 1;
			coupleMatrix[sortA2[i] - 1][sortB1[i] - 1] = 1;
			coupleNodes.push_back({ A.node[sortA2[i] - 1],B.node[sortB1[i] - 1],style });
		}
		break;
	case 6://�������ӣ��Գ�ģʽ��
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
	//����
	//for (int i = 0; i < coupleNodes.size(); i++) {
	//	cout << "A" << coupleNodes[i].first.num << "B" << coupleNodes[i].second.num << endl;
	//}
}
CoupledNet::CoupledNet(Net A1, Net B1, vector<vector<int> > coupleMatrix1) {
	A = A1;
	B = B1;
	int style = 1;//��ϱ߷�ʽ
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
	int style = 1;//��ϱ߷�ʽ
	int sizeA = A.node.size();
	int sizeB = B.node.size();
	coupleMatrix = vector<vector<int> >(sizeA, vector<int>(sizeB, 0));//��Ͼ����ʼ��
	for (size_t i = 0; i < coupleNodes.size(); i++) {
		coupleMatrix[coupleNodes[i].first.num - 1][coupleNodes[i].second.num - 1] = 1;
	}
}
//ð�����򣬽ڵ㰴�ն����򣬷��ؽڵ������У�
//flag=1,�Ӵ�С��flag=2.��С����
vector<int> CoupledNet::bubbleSort(vector<Node> node,int flag) {
	int size = node.size();
	int temp = 0;
	vector<int> sortNum(size,0);
	vector<int> sortK(size, 0);
	for (int i = 0; i < size; i++) {
		sortNum[i] = node[i].num;
		sortK[i] = node[i].k;
	}
	//cout << "�ڵ���";
	//for_each(sortNum.begin(), sortNum.end(), print);
	switch (flag) {
	case 1://�Ӵ�С����
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
	case 2://��С��������
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
//����ʧЧ��style=1�����ʧЧ��style=2�������ʧЧ
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
