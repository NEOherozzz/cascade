#include <stdlib.h>
#include <iostream>
#include <vector>
#include "net.h"
#include "MA.h"
#include "file-data.h"

using namespace std;

void randtest();

int main() {
	int Matrix[10][10] = { { 0,1,0,0,0,1,0,0,0,0 },{ 1,0,1,0,0,0,1,0,0,0 },{ 0,1,0,1,0,0,0,1,0,0 },{ 0,0,1,0,1,0,0,0,1,0 },{ 0,0,0,1,0,0,0,0,0,1 },{ 1,0,0,0,0,0,0,1,0,1 },{ 0,1,0,0,0,0,0,1,0,0 },{ 0,0,1,0,0,1,1,0,1,0 },{ 0,0,0,1,0,0,0,1,0,0 },{ 0,0,0,0,1,1,0,0,0,0 }};
	vector<vector<int> > matrix(10,vector<int>(10));
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			matrix[i][j]=Matrix[i][j];
		}
	}
	//cout << matrix.size() << matrix[1].size() << endl;
	//Net myNet = Net(matrix);
	Net myNet = Net("ba",30,2,2,1);
	//cout << myNet.nodeNum << endl;
	//for (int i = 0; i < myNet.nodeNum; i++) {
	//	cout << "节点" << myNet.node[i].num << "的度为" << myNet.node[i].k << endl;
	//	cout << "\n" << endl;
	//}
	//for (int i = 0; i < myNet.edgeNum; i++) {
	//	cout << "边" << myNet.edge[i].num << "为" << myNet.edge[i].nodeNum1 << "->" << myNet.edge[i].nodeNum2 << endl;
	//	cout << "\n" << endl;
	//}
	//randtest();
	Net myNet2 = Net("ba", 30, 2, 2, 1);
	//CoupledNet CNet(myNet, myNet2, 1, 6);
	//cout << CNet.cascade(1) << endl;
	//MA算法求解最优连边
	//optSolution opt=MA(myNet, myNet2, 1, MApara{50,100,0.8,0.5,0.9});
	//cout << opt.robustness << endl;
	dataOutPajek(myNet,"A网络");
	dataOutPajek(myNet2, "B网络");
	system("pause");
	return 0;
}

void randtest() {
	double rd[100];

	for (int i = 0; i < 100; i++) {
		//rd[i] = (rand() + 1.0) / (RAND_MAX + 1.1);
		rd[i] = rand() / (double) RAND_MAX;
		//cout << rd[i] << " " << endl;
	}
	/*for (int i = 0; i < 100; i++) {
		for (int j = 0; j < 100 - i; j++) {
			if (rd[j] > rd[j + 1])
			{
				double tmp = rd[j]; rd[j] = rd[j + 1];  rd[j + 1] = tmp;
			}
		}
	}*/
	for (int i = 0; i < 100; i++) {
		cout << i << " " << rd[i] << " " << endl;
	}

	//system("pause");
	//return 0;
}