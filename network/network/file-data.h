#pragma once
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "net.h"

using namespace std;

void dataOutPajek(Net net,string fname) {
	fname += ".net";//添加文件后缀
	//const char* name = fname.c_str();
	ofstream netdata;
	netdata.open(fname, ofstream::out | std::ofstream::trunc);
	int nodeNum = net.node.size();
	netdata << "*Vertices " << nodeNum << endl;
	for (int i = 0; i < nodeNum; i++) {
		netdata << (i + 1) << " \"" << net.node[i].num << "\"" << endl;
	}
	netdata << "*Matrix" << endl;
	for (int i = 0; i < nodeNum; i++) {
		for (int j = 0; j < nodeNum; j++) {
			netdata << net.adjMatrix[i][j] << " ";
		}
		netdata << endl;
	}
	netdata.close();
}
void dataOutPajek(CoupledNet net, string fname) {
	fname += ".net";//添加文件后缀
					//const char* name = fname.c_str();
	ofstream netdata;
	netdata.open(fname, ofstream::out | std::ofstream::trunc);

	int sizeA = net.A.node.size();
	int sizeB = net.B.node.size();
	int N = sizeA + sizeB;
	vector<vector<int> > adjMatrix = vector<vector<int> >(N, vector<int>(N, 0));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (i < sizeA && j < sizeB) {
				adjMatrix[i][j] = net.A.adjMatrix[i][j];
			}
			else if (i < sizeA && j >= sizeA) {
				adjMatrix[i][j] = net.coupleMatrix[i][j - sizeA];
			}
			else if (i >= sizeA && j < sizeA) {
				adjMatrix[i][j] = net.coupleMatrix[j][i - sizeA];
			}
			else {
				adjMatrix[i][j] = net.B.adjMatrix[i - sizeA][j - sizeA];
			}
		}
	}

	int nodeNum = N;
	netdata << "*Vertices " << nodeNum << endl;
	for (int i = 0; i < nodeNum; i++) {
		if (i < sizeA) {
			netdata << (i + 1) << " \"" << "A" << net.A.node[i].num << "\"" << endl;
		}
		else {
			netdata << (i + 1) << " \"" << "B" << net.B.node[i - sizeA].num << "\"" << endl;
		}	
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
void dataOutPajek(vector<vector<int> > adjMatrix, vector<Node> node, string fname) {
	fname += ".net";//添加文件后缀
					//const char* name = fname.c_str();
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