#pragma once
#include <vector>
#include <iomanip>
#include <iostream>

class Node {
public:
	Node() :num(0), k(0) {};
	Node(int num,int k) :num(num), k(k) {};
public:
	int num;//节点编号
	int k;//节点度
	double betweenness;//节点介数
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
	Net(int **adjMatrix);
	Net(int nNum, int eNum);


private:
	vector<vector<int> > adjMatrix;//网络邻接矩阵
	Node *node;//网络节点
	int nodeNum;//节点数
	Edge *edge;//网络边
	int edgeNum;//总边数
};
Net::Net(int **adjMatrix) {
	int k = 0;
	int ksum = 0;

	nodeNum = (sizeof(adjMatrix) / sizeof(int)) / (sizeof(adjMatrix[0]) / sizeof(int));
	node = new Node[nodeNum];

	for (int i = 0; i < nodeNum; i++ ) {
		for (int j = i; j < nodeNum; j++) {
			k = k + adjMatrix[i][j];
		}
		node[i] = Node((i+1), k);
		ksum = ksum + k;
		k = 0;
	}

	edgeNum = ksum / 2;
	edge = new Edge[edgeNum];

	for (int i = 0; i < nodeNum; i++) {
		int n = 0;//边计数
		for (int j = i; j < nodeNum; j++) {
			if (adjMatrix[i][j] != 0) {
				edge[n] = Edge((n + 1), (i + 1), (j + 1));
			}
		}
	}


}
Net::Net(int nNum, int eNum) {

}
