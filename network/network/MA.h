#pragma once
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <queue>
#include "net.h"
#include "file-data.h"

//MA文化基因优化算法

struct optSolution {
	vector<coupleEdge> coupleNodes;
	double robustness;
};

struct MApara {
	int popsize;//种群数量	
	int maxG;//最大迭代次数
	double pc;//交叉概率
	double pl;//局部搜索概率
	double a;//局部搜索参数
};

bool allOne(vector<int> vec) {
	for (size_t i = 0; i < vec.size(); i++) {
		if (vec[i] != 1) {
			return false;
		}
	}
	return true;
}

int searchSec(int num,vector<coupleEdge> couplenodes) {
	for (int i = 0; i < couplenodes.size();i++) {
		if (couplenodes[i].second.num == num) {
			return i;
		}
	}
	return -1;
}

int searchFir(int num, vector<coupleEdge> couplenodes) {
	for (int i = 0; i < couplenodes.size(); i++) {
		if (couplenodes[i].first.num == num) {
			return i;
		}
	}
	return -1;
}

bool isInvec(int num, vector<int> vec) {
	int len = vec.size();
	for (int i = 0; i < len; i++) {
		if (num == vec[i]) {
			return true;
		}
	}
	return false;
}

optSolution MA(Net A, Net B, double p, MApara para) {
	vector<double> maxRob(para.maxG,0.0);//最优值
	vector<coupleEdge> maxCouple;//最优解

	//产生初始种群,随机连接
	vector<vector<coupleEdge> > pop(para.popsize, vector<coupleEdge>());
	vector<double> fitness(para.popsize, 0);
	for (int i = 0; i < para.popsize; i++) {
		pop[i] = CoupledNet(A, B, p, 1).coupleNodes;
	}
	vector<vector<coupleEdge> > popc;//子代
	vector<double> fitnessC(para.popsize, 0);
	int len = pop[0].size();//耦合边数量
	//迭代过程
	for (int i = 0; i < para.maxG; i++) {
		//随机选择两个个体进行交叉
		popc = pop;
		vector<int> selectVec(para.popsize, 0);
		int rd1, rd2 = 0;
		while (!allOne(selectVec)) {
			do {
				rd1 = rand() % para.popsize;
			} while (selectVec[rd1] == 1);
			selectVec[rd1] = 1;
			do {
				rd2 = rand() % para.popsize;
			} while (selectVec[rd2] == 1);
			selectVec[rd2] = 1;
			//rd1和rd2进行交叉
			for (int j = 0; j < len; j++) {
				double rd = rand() / (double)RAND_MAX;
				int num;
				if (rd < para.pc) {
					if (pop[rd1][j].first.num == pop[rd2][j].first.num && pop[rd1][j].second.num != pop[rd2][j].second.num) {
						popc[rd1][j].second = pop[rd2][j].second;
						num = searchSec(pop[rd1][j].second.num, pop[rd1]);
						if (num != -1) {
							popc[rd1][num].second = pop[rd1][j].second;
						}
						popc[rd2][j].second = pop[rd1][j].second;
						num = searchSec(pop[rd2][j].second.num, pop[rd2]);
						if (num != -1) {
							popc[rd2][num].second = pop[rd2][j].second;
						}
					}
					else if (pop[rd1][j].first.num != pop[rd2][j].first.num && pop[rd1][j].second.num == pop[rd2][j].second.num) {
						popc[rd1][j].first = pop[rd2][j].first;
						num = searchFir(pop[rd1][j].first.num, pop[rd1]);
						if (num != -1) {
							popc[rd1][num].first = pop[rd1][j].first;
						}
						popc[rd2][j].first = pop[rd1][j].first;
						num = searchFir(pop[rd2][j].first.num, pop[rd2]);
						if (num != -1) {
							popc[rd2][num].first = pop[rd2][j].first;
						}
					}
					else if (pop[rd1][j].first.num != pop[rd2][j].first.num && pop[rd1][j].second.num != pop[rd2][j].second.num) {
						int num2 = searchFir(pop[rd2][j].first.num, pop[rd1]);
						if (num2 != -1) {
							if (pop[rd1][num2].second.num != pop[rd2][j].second.num) {
								popc[rd1][num2].second = pop[rd2][j].second;
								num = searchSec(pop[rd1][num2].second.num, pop[rd1]);
								if (num != -1) {
									popc[rd1][num].second = pop[rd1][num2].second;
								}
							}
						}
						else {
							int num2 = searchSec(pop[rd2][j].second.num, pop[rd1]);
							if (num2 != -1) {
								popc[rd1][num2].first = pop[rd2][j].first;
							}
						}
						int num3 = searchFir(pop[rd1][j].first.num, pop[rd2]);
						if (num3 != -1) {
							if (pop[rd2][num3].second.num != pop[rd1][j].second.num) {
								popc[rd2][num3].second = pop[rd1][j].second;
								num = searchSec(pop[rd2][num3].second.num, pop[rd2]);
								if (num != -1) {
									popc[rd2][num].second = pop[rd2][j].second;
								}
							}
						}
						else {
							int num3 = searchSec(pop[rd1][j].second.num, pop[rd2]);
							if (num3 != -1) {
								popc[rd2][num3].first = pop[rd1][j].first;
							}
						}
					}
				}
			}//rd1和rd2交叉完毕
		}//交叉过程结束
		//计算适应度值
		for (int j = 0; j < para.popsize; j++) {
			CoupledNet Cnet(A, B, pop[j]);
			fitness[j] = Cnet.cascade(2);
			CoupledNet Cnet2(A, B, popc[j]);
			fitnessC[j] = Cnet2.cascade(2);
		}
		//局部搜索过程
		//轮盘赌法选择局部搜索个体
		double sum = 0.0;
		vector<double> fitnessT;
		fitnessT.clear();
		fitnessT.insert(fitnessT.end(), fitness.begin(), fitness.end());
		fitnessT.insert(fitnessT.end(), fitnessC.begin(), fitnessC.end());
		for (int j = 0; j < fitnessT.size(); j++) {
			sum += fitnessT[j];
		}
		vector<double> fitPn(para.popsize * 2, 0);
		for (int j = 0; j < fitPn.size(); j++) {
			fitPn[j] = fitnessT[j] / sum;
		}
		double prs = 1;
		double ps = 0.0;
		double rd;
		for (int j = 0; j < para.popsize; j++) {
			rd = rand() / (double)RAND_MAX;
			ps = 0.0;
			for (int k = 0; k < fitPn.size(); k++) {
				ps += fitPn[k] / prs;
				if (rd < ps) {
					if (k < para.popsize) {
						//对pop[k]进行局部搜索
						for (int m = 0; m < pop[k].size(); m++) {//对每一对耦合边pop[k][m]
							vector<coupleEdge> popLocal;
							double rd = rand() / (double)RAND_MAX;
							if (rd < para.pl) {//局部搜索概率
								int rdnum;
								do {
									rdnum = rand() % len;
								} while (rdnum == (pop[k][m].second.num - 1));
								int num = searchSec(rdnum + 1, pop[k]);
								if (num == -1) {//无耦合边
									//double diff = para.a*fabs(pop[k][m].first.k - pop[k][m].second.k) - fabs(pop[k][m].first.k - pop[k][num].second.k);
									double diff = para.a*fabs(pop[k][m].first.k - pop[k][m].second.k) - fabs(pop[k][m].first.k - B.node[rdnum].k);
									if (diff > 0) {
										popLocal = pop[k];
										popLocal[m].second = pop[k][rdnum].second;
										CoupledNet net1(A, B, popLocal);
										if (net1.cascade(2) > fitnessT[k]) {
											pop[k] = popLocal;
										}
									}
								}
								else {
									//double diff = para.a*fabs(pop[k][m].first.k - pop[k][m].second.k) - fabs(pop[k][m].first.k - pop[k][num].second.k);
									double diff = para.a*fabs(pop[k][m].first.k - pop[k][m].second.k) - fabs(pop[k][m].first.k - B.node[rdnum].k);
									if (diff > 0) {
										popLocal = pop[k];
										popLocal[m].second = pop[k][rdnum].second;
										popLocal[num].second = pop[k][m].second;
										CoupledNet net1(A, B, popLocal);
										if (net1.cascade(2) > fitnessT[k]) {
											pop[k] = popLocal;
										}
									}
								}
							}
						}

						fitPn[k] = 0;
						prs = prs - fitPn[k];
						break;
					}
					else {
						//对popc[para.popsize*2-k]进行局部搜索	
						int t = para.popsize * 2 - 1 - k;
						for (int m = 0; m < popc[t].size(); m++) {//对每一对耦合边popc[para.popsize*2-k][m]
							vector<coupleEdge> popLocal;
							double rd = rand() / (double)RAND_MAX;
							if (rd < para.pl) {//局部搜索概率
								int rdnum;
								do {
									rdnum = rand() % len;
								} while (rdnum == (popc[t][m].second.num - 1));
								int num = searchSec(rdnum + 1, popc[t]);
								if (num == -1) {//无耦合边
									//double diff = (para.a)*fabs(popc[t][m].first.k - popc[t][m].second.k) - fabs(popc[t][m].first.k - popc[t][num].second.k);
									double diff = (para.a)*fabs(popc[t][m].first.k - popc[t][m].second.k) - fabs(popc[t][m].first.k - B.node[rdnum].k);
									if (diff > 0) {
										popLocal = popc[t];
										popLocal[m].second = popc[t][rdnum].second;
										CoupledNet net1(A, B, popLocal);
										if (net1.cascade(2) > fitnessT[k]) {
											popc[t] = popLocal;
										}
									}
								}
								else {
									//double diff = para.a*fabs(popc[t][m].first.k - popc[t][m].second.k) - fabs(popc[t][m].first.k - popc[t][num].second.k);
									double diff = para.a*fabs(popc[t][m].first.k - popc[t][m].second.k) - fabs(popc[t][m].first.k - B.node[rdnum].k);
									if (diff > 0) {
										popLocal = popc[t];
										popLocal[m].second = popc[t][rdnum].second;
										popLocal[num].second = popc[t][m].second;
										CoupledNet net1(A, B, popLocal);
										if (net1.cascade(2) > fitnessT[k]) {
											popc[t] = popLocal;
										}
									}
								}
							}
						}

						fitPn[k] = 0;
						prs = prs - fitPn[k];
						break;
					}
				}
			}
		}
		//局部搜索结束
		//排序选择
		//计算适应度值
		for (int j = 0; j < para.popsize; j++) {
			CoupledNet Cnet(A, B, pop[j]);
			fitness[j] = Cnet.cascade(2);
			CoupledNet Cnet2(A, B, popc[j]);
			fitnessC[j] = Cnet2.cascade(2);
		}
		fitnessT.clear();
		fitnessT.insert(fitnessT.end(), fitness.begin(), fitness.end());
		fitnessT.insert(fitnessT.end(), fitnessC.begin(), fitnessC.end());
		//锦标赛选择法
		vector<vector<coupleEdge> > popnew(para.popsize, vector<coupleEdge>());
		//随机排列
		vector<int> arrvec(para.popsize,-1);
		for (int j = 0; j < para.popsize; j++) {
			int rd;
			do {
				rd = rand() % para.popsize;
			} while (isInvec(rd,arrvec));
			arrvec[j] = rd;
			if (fitness[j] < fitnessC[rd]) {
				popnew[j] = pop[j];
			}
			else {
				popnew[j] = popc[rd];
			}
		}
		//冒泡排序 有问题
		int size = fitnessT.size();
		vector<double> sortfit(size, 0.0);
		vector<int> sortNum(size, 0);
		for (int j = 0; j < size; j++) {
			sortNum[j] = j;
			sortfit[j] = fitnessT[j];
		}
		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size - 1; k++) {
				if (sortfit[k] < sortfit[k + 1]) {
					int temp = sortNum[k];
					sortNum[k] = sortNum[k + 1];
					sortNum[k + 1] = temp;
					double temp2 = sortfit[k];
					sortfit[k] = sortfit[k + 1];
					sortfit[k + 1] = temp2;
				}
			}
		}//冒泡排序结束
		cout << "第" << i << "代最优值：" << fitnessT[sortNum[0]] << endl;
		maxRob[i] = fitnessT[sortNum[0]];
		if (sortNum[0] < para.popsize) {
			maxCouple = pop[sortNum[0]];
		}
		else {
			maxCouple = popc[sortNum[0] - para.popsize];
		}
		pop = popnew;
	}
	return (optSolution { maxCouple,maxRob[para.maxG - 1] });
}
