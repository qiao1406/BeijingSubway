#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <queue>
#include <map>
#include <limits.h>
#include <Windows.h>
#include "station.h"
#include "line_list.h"


using namespace std;

// 一些常量
const int STATION_NUM = 300;
const int LINE_NUM = 20;

station st_arr[STATION_NUM];
line_list linelst_arr[LINE_NUM];
string line_arr[LINE_NUM]; // 存线路名称
int g[STATION_NUM][STATION_NUM];//图的邻接矩阵
int st_num = 0;
int line_num = 0;
queue<int> route;//存的是路线上站点的下标值
map<string, int> st_map;

// 返回站点在数组里的下标，若没有则返回-1
int getStationIndex(string name) {
	/*int rst = -1;

	for (int i = 0; i < st_num; ++i) {
		if (st_arr[i].name == name) {
			rst = i;
			break;
		}
	}

	return rst;*/
	map<string, int>::iterator it = st_map.find(name);
	return (it == st_map.end()) ? -1 : st_map[name];

}

int getLineIndex(string name) {
	int rst = -1;

	for (int i = 0; i < line_num; ++i) {
		if (line_arr[i] == name) {
			rst = i;
			break;
		}
	}

	return rst;
}

//建立起站点和线路的数组，以及将相关的图结点连上
void setArray() {

	string lineName;
	string station_str;
	string word;
	ifstream fin;
	int last_index; //上一个车站在数组里的下标值

	fin.open("beijing-subway.txt");
	while (getline(fin, lineName)) {
		SLinkList p, end, head;
		line_arr[line_num] = lineName;
		linelst_arr[line_num] = line_list(lineName);

		// 读入线路名和该线路上的所有站的名字
		getline(fin, station_str);
		stringstream ss(station_str);

		//初始化一些参数
		last_index = -1;
		head = NULL;
		end = NULL;
		
		while (ss >> word) {

			//初始化链表
			if ( head == NULL) {
				head = new SNode;
				head->station_name = word;
				head->left = NULL;
				head->right = NULL;
				end = head;
			}
			else {
				p = new SNode;
				p->station_name = word;
				end->right = p;
				p->left = end;
				p->right = NULL;
				end = p;
			}

			int x = getStationIndex(word);

			// 将站点存入数组中
			if (x >= 0) { // 已存在的站点
				st_arr[x].addLine(lineName);
			}
			else { // 未加入的站点
				x = st_num;
				st_arr[st_num] = station(word, lineName);
				st_map.insert(pair<string,int>(word,st_num));
				st_num++;
			}

			if (last_index >= 0) {
				g[x][last_index] = 1;
				g[last_index][x] = 1;
			}
			last_index = x;

		}

		//处理环线
		if (linelst_arr[line_num].linename == "2号线" 
			|| linelst_arr[line_num].linename == "10号线") {
			end->left->right = head;
			head->left = end->left;
			free(end);
			end = head;
		}

		linelst_arr[line_num].head = head;
		linelst_arr[line_num].end = end;
		line_num++;
	}


	fin.close();
}

int findMinDist(int*s, int*dist) {
	int min = INT_MAX;
	int u = -1;

	for (int i = 0; i < st_num; ++i) {
		if (s[i] != 0) {
			continue;
		}
		else {
			if (min >= dist[i]) {
				min = dist[i];
				u = i;
			}
		}
	}

	return u;
}

// 用迪杰斯特拉算法计算两个站点之间的最短的路径
void dijkstra(string start, string end) {

	int index1 = getStationIndex(start);
	int index2 = getStationIndex(end);
	int pos[STATION_NUM];
	int dist[STATION_NUM];
	int path[STATION_NUM][STATION_NUM];
	int s[STATION_NUM];

	for (int i = 0; i < st_num; ++i) {
		s[i] = 0;
		dist[i] = g[index1][i];
		path[i][0] = index1;
		pos[i] = 0;
	}

	s[index1] = 1;

	while (s[index2] == 0) {
		int u = findMinDist(s, dist);
		s[u] = 1;
		path[u][++pos[u]] = u;

		// 找到通过u可以到达的、尚未确定最短路径的点
		for (int i = 0; i < st_num; ++i) {

			if (s[i] != 0) {
				continue;
			}
			else {

				// 更新路径长度
				if (g[u][i] == 1 && dist[u] + g[u][i] < dist[i]) {
					dist[i] = dist[u] + g[u][i];
					for (int j = 0; j <= pos[u]; ++j) {
						path[i][j] = path[u][j];
					}
					pos[i] = pos[u];
				}

			}
		}

	}

	// 此时，start到end的路径已经找出了，用一个队列将路径存起来

	for (int i = 0; i <= pos[index2]; ++i) {
		route.push(path[index2][i]);
	}

}

// 计算最小换乘次数的方案
void leastChangeSlution(string start, string end) {

	int index1 = getStationIndex(start);
	int index2 = getStationIndex(end);
	int dist[STATION_NUM];
	int path[STATION_NUM][LINE_NUM]; // 记录换乘的站点
	int pos[STATION_NUM]; // 记录该站点在path[] 数组中的位置
	int final_change_time = INT_MAX;//最终的换乘次数
	int change_time[STATION_NUM]; // 换乘次数,当该站点已经找到了终点站所在的时候才会进行设定
	int min_dist = INT_MAX;
	int min_index = -1;
	bool st_flag[STATION_NUM];
	bool line_flag[LINE_NUM];
	bool find = false;
	queue<int> q1;//存放要查找的换乘站
	queue<int> q2;//存放对应的换乘次数

	// 初始化line_flag[]
	for (int i = 0; i < line_num; ++i) {
		line_flag[i] = false;
	}
	for (int i = 0; i < st_num; ++i) {
		dist[i] = INT_MAX;
		change_time[i] = INT_MAX;
		path[i][0] = index1;
		pos[i] = 0;
		st_flag[i] = false;
	}

	st_flag[index1] = true;

	// 同一条线
	if (st_arr[index1].isInSameLine(st_arr[index2])) {
		find = true;
		path[index2][++pos[index2]] = index2;
		change_time[index2] = 0;
	}
	else {

		//对起始站的所在线路的所有换乘站进行查找
		for (int i = 0; i < 3; i++) {

			if (st_arr[index1].line[i] == "") {
				break;
			}
			//拿出一条线来
			string line_name = st_arr[index1].line[i];
			
			//把这条线上所有的换乘点加进去
			for (int j = 0; j < st_num; ++j) {
				if (st_flag[j] == false && st_arr[j].isInLine(line_name)
					&& st_arr[j].isChangeStation()) {
					st_flag[j] = true;
					q1.push(j);
					q2.push(1);
					path[j][++pos[j]] = j;
				}
			}
			line_flag[getLineIndex(line_name)] = true;

		}

		//对q1里面的换乘站进行查找,直到找到为止
		while (!q1.empty()) {

			int temp_index = q1.front();
			q1.pop();
			int now_time = q2.front();
			q2.pop();

			//若此时的换乘站和终点站共线，则停止寻找
			if (st_arr[temp_index].isInSameLine(st_arr[index2])) {
				find = true;
				//final_change_time = now_time;
				//path[temp_index][++pos[temp_index]] = temp_index;
				path[temp_index][++pos[temp_index]] = index2;
				change_time[temp_index] = now_time;
			}
			else {

				//将这个换乘站所在线路中未被搜寻的线路进行查找
				for (int i = 0; i < 3; ++i) {
					string line_name = st_arr[temp_index].line[i];

					if (line_name == "") {
						break;
					}

					if (line_flag[getLineIndex(line_name)] == false) {

						line_flag[getLineIndex(line_name)] = true;
	
						for (int j = 0; j < st_num; ++j) {

							if (st_flag[j] == false
								&& st_arr[j].isInLine(line_name)
								&& st_arr[j].isChangeStation() ) {

								st_flag[j] = true;
								q1.push(j);
								q2.push(now_time + 1);
								// 将该站的path[]的值与temp_index的保持一致
								for (int k = 1; k <= pos[temp_index]; ++k) {
									path[j][k] = path[temp_index][k];
								}
								pos[j] = pos[temp_index];
								path[j][++pos[j]] = j;
							}
						}
					}


				}

			}

		}


	}

	//找出换乘次数最小的方案
	for (int i = 0; i < st_num; ++i) {
		if (change_time[i] < final_change_time) {
			final_change_time = change_time[i];
			//cout << st_arr[i].name;
		}
	}

	//找出所有已经找到的路径，求出距离的最小值(换乘次数最小的方案中距离最小的)
	for (int i = 0; i < st_num; ++i) {
	
		if (path[i][pos[i]] == index2 && change_time[i] == final_change_time ) { // 已经找到路径的，计算dist
			dist[i] = 0;
			for (int j = 0; j < pos[i]; ++j) {
				string s = st_arr[path[i][j]].getSameLineName(st_arr[path[i][j + 1]]);//线路的名字
				string s1 = st_arr[path[i][j]].name;
				string s2 = st_arr[path[i][j+1]].name;
				int d = linelst_arr[getLineIndex(s)].calDistance(s1, s2); //计算距离
				dist[i] += d;
			}

			if (min_dist > dist[i]) {
				min_dist = dist[i];
				min_index = i;
			}

		}
	}

	if (min_index == -1) {
		cout << "找不到最短换乘路径" << endl;
		return;
	}
	//找到最适合的路径了，把路径存到队列里面
	//先把起点加进去
	route.push(path[min_index][0]);
	for (int i = 0; i < pos[min_index]; ++i) {

		string s = st_arr[path[min_index][i]].getSameLineName(st_arr[path[min_index][i + 1]]);
		string s1 = st_arr[path[min_index][i]].name;
		string s2 = st_arr[path[min_index][i+1]].name;

		int line_index = getLineIndex(s);

		//环线
		//先找到s1,记录其位置，然后分别向左走或者向右走，哪个先到达s2就说明应该从那个方向走
		if (s == "2号线" || s == "10号线") {
			SLinkList p, pl, pr, target;
			p = linelst_arr[line_index].head;
			pl = NULL;
			pr = NULL;

			while (p->station_name != s1) {
				p = p->right;
			}
			pl = p;
			pr = p;
			while (pl->station_name != s2 && pr->station_name != s2) {
				pl = pl->left;
				pr = pr->right;
			}
			
			target = (pl->station_name == s2) ? pl : pr;
			do {
				p = (pl->station_name == s2) ? p->left : p->right;
				route.push(getStationIndex(p->station_name));
			} while (p != target);
			
		}
		else {//非环线
			SLinkList p, p1, p2;
			p1 = NULL;
			p2 = NULL;

			p = linelst_arr[line_index].head;
			do {
				p2 = (p->station_name == s2) ? p : p2;
				p1 = (p->station_name == s1) ? p : p1;
				if (p1 != NULL) {
					break;
				}
				p = p->right;
			} while (p != NULL );
		
			if (p2 == NULL) { // 读到点p1的时候还没有到p2,应该从p1往右读

				do {
					p = p->right;
					route.push(getStationIndex(p->station_name));
					p2 = (p->station_name == s2) ? p : p2;
				} while (p2 == NULL);
			}
			else { // 读到点p1的时候已经读到p2,应该从p1往左读
				do {
					p = p->left;
					route.push(getStationIndex(p->station_name));
				} while (p != p2);
			}

		}
	}

}

// 输出路径的信息
void printRuote() {

	bool gbd = false; //高碑店
	bool dwl = false; //大望路
	bool xj = false;//西局
	bool llq = false;//六里桥
	bool qlz = false; //七里庄
	int last_1 = -1;
	int last_2 = -1;
	int now;

	if (route.size() < 2) {
		cout << "错误！！！" << endl;
		return;
	}

	cout << "共" << route.size() << "站" << endl;
	now = route.front();
	route.pop();
	cout << st_arr[now].name << "->进站";

	if (st_arr[now].name == "高碑店") {
		gbd = true;
	}
	if (st_arr[now].name == "大望路") {
		dwl = true;
	}
	if (st_arr[now].name == "西局") {
		xj = true;
	}
	if (st_arr[now].name == "六里桥") {
		llq = true;
	}
	if (st_arr[now].name == "七里庄") {
		qlz = true;
	}

	last_1 = now;
	last_2 = last_1;

	while (!route.empty()) {
		now = route.front();
		route.pop();

		if (st_arr[now].name == "高碑店") {
			if (dwl == true) {
				cout << "->换乘地铁八通线";
			}
			gbd = true;
		}

		if (st_arr[now].name == "大望路") {
			if (gbd == true) {
				cout << "->换乘地铁1号线";
			}
			dwl = true;
		}

		if (st_arr[now].name == "西局") {

			if ( qlz == true && llq == true) {

				if (st_arr[last_1].name == "七里庄" && st_arr[last_2].name == "六里桥") {
					cout << "->换乘地铁14号线西段";
				}
				if (st_arr[last_2].name == "七里庄" && st_arr[last_1].name == "六里桥") {
					cout << "->换乘地铁10号线";
				}
				
			}
			xj = true;
		}

		if (st_arr[now].name == "六里桥") {

			if (qlz == true && xj == true) {

				if (st_arr[last_1].name == "七里庄" && st_arr[last_2].name == "西局") {
					cout << "->换乘地铁9号线";
				}
				if (st_arr[last_2].name == "七里庄" && st_arr[last_1].name == "西局") {
					cout << "->换乘地铁10号线";
				}

			}
			llq = true;
		}

		if (st_arr[now].name == "七里庄") {

			if (llq == true && xj == true) {

				if (st_arr[last_1].name == "六里桥" && st_arr[last_2].name == "西局") {
					cout << "->换乘地铁9号线";
				}
				if (st_arr[last_2].name == "六里桥" && st_arr[last_1].name == "西局") {
					cout << "->换乘地铁14号线西段";
				}

			}
			qlz = true;
		}

		if (last_2 != -1) {
			if (st_arr[now].isInSameLine(st_arr[last_2]) != 1) {
				// 换乘
				cout << "->换乘地铁" << st_arr[now].getSameLineName(st_arr[last_1]);
			}
		}
		cout << endl;
		cout << st_arr[now].name;
		last_2 = last_1;
		last_1 = now;
	}

	cout << "->出站"<<endl;
}

//main函数
int main( int argc, char* argv[] ) {
//int main() {

	string op;
	string s_name1, s_name2;

    if ( argc != 4 ) {
		cout << "参数错误" << endl;
		return 1;
    }
    else {
		op = argv[1];
	    s_name1 = argv[2];
	    s_name2 = argv[3];
	}


	//初始化图g
	for (int i = 0; i < STATION_NUM; ++i) {
		for (int j = 0; j < STATION_NUM; ++j) {

			if (i == j) {
				g[i][j] = 0;
			}
			else {
				g[i][j] = INT_MAX;
			}


		}
	}

	setArray();

	//参数正确性检查以及分情况进行处理
	if (getStationIndex(s_name1) == -1 || getStationIndex(s_name2) == -1) {
		cout << "参数s_name1或s_name2错误" << endl;
		return 1;
	}
	if ( op == "-b" ) {
		dijkstra(s_name1, s_name2);
	}
	else if (op == "-c") {
		leastChangeSlution(s_name1, s_name2);
	}
	else {
		cout << "参数op错误" << endl;
		return 1;
	}

	// 销毁链表
	for (int i = 0; i < line_num; ++i) {
		linelst_arr[i].free();
	}
	//输出乘车方案
	printRuote();
	system("pause");

	return 0;
}
