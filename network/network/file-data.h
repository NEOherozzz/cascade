#pragma once
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "net.h"

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