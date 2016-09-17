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

// һЩ����
const int STATION_NUM = 300;
const int LINE_NUM = 20;

station st_arr[STATION_NUM];
line_list linelst_arr[LINE_NUM];
string line_arr[LINE_NUM]; // ����·����
int g[STATION_NUM][STATION_NUM];//ͼ���ڽӾ���
int st_num = 0;
int line_num = 0;
queue<int> route;//�����·����վ����±�ֵ
map<string, int> st_map;

// ����վ������������±꣬��û���򷵻�-1
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

//������վ�����·�����飬�Լ�����ص�ͼ�������
void setArray() {

	string lineName;
	string station_str;
	string word;
	ifstream fin;
	int last_index; //��һ����վ����������±�ֵ

	fin.open("beijing-subway.txt");
	while (getline(fin, lineName)) {
		SLinkList p, end, head;
		line_arr[line_num] = lineName;
		linelst_arr[line_num] = line_list(lineName);

		// ������·���͸���·�ϵ�����վ������
		getline(fin, station_str);
		stringstream ss(station_str);

		//��ʼ��һЩ����
		last_index = -1;
		head = NULL;
		end = NULL;
		
		while (ss >> word) {

			//��ʼ������
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

			// ��վ�����������
			if (x >= 0) { // �Ѵ��ڵ�վ��
				st_arr[x].addLine(lineName);
			}
			else { // δ�����վ��
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

		//������
		if (linelst_arr[line_num].linename == "2����" 
			|| linelst_arr[line_num].linename == "10����") {
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

// �õϽ�˹�����㷨��������վ��֮�����̵�·��
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

		// �ҵ�ͨ��u���Ե���ġ���δȷ�����·���ĵ�
		for (int i = 0; i < st_num; ++i) {

			if (s[i] != 0) {
				continue;
			}
			else {

				// ����·������
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

	// ��ʱ��start��end��·���Ѿ��ҳ��ˣ���һ�����н�·��������

	for (int i = 0; i <= pos[index2]; ++i) {
		route.push(path[index2][i]);
	}

}

// ������С���˴����ķ���
void leastChangeSlution(string start, string end) {

	int index1 = getStationIndex(start);
	int index2 = getStationIndex(end);
	int dist[STATION_NUM];
	int path[STATION_NUM][LINE_NUM]; // ��¼���˵�վ��
	int pos[STATION_NUM]; // ��¼��վ����path[] �����е�λ��
	int final_change_time = INT_MAX;//���յĻ��˴���
	int change_time[STATION_NUM]; // ���˴���,����վ���Ѿ��ҵ����յ�վ���ڵ�ʱ��Ż�����趨
	int min_dist = INT_MAX;
	int min_index = -1;
	bool st_flag[STATION_NUM];
	bool line_flag[LINE_NUM];
	bool find = false;
	queue<int> q1;//���Ҫ���ҵĻ���վ
	queue<int> q2;//��Ŷ�Ӧ�Ļ��˴���

	// ��ʼ��line_flag[]
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

	// ͬһ����
	if (st_arr[index1].isInSameLine(st_arr[index2])) {
		find = true;
		path[index2][++pos[index2]] = index2;
		change_time[index2] = 0;
	}
	else {

		//����ʼվ��������·�����л���վ���в���
		for (int i = 0; i < 3; i++) {

			if (st_arr[index1].line[i] == "") {
				break;
			}
			//�ó�һ������
			string line_name = st_arr[index1].line[i];
			
			//�������������еĻ��˵�ӽ�ȥ
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

		//��q1����Ļ���վ���в���,ֱ���ҵ�Ϊֹ
		while (!q1.empty()) {

			int temp_index = q1.front();
			q1.pop();
			int now_time = q2.front();
			q2.pop();

			//����ʱ�Ļ���վ���յ�վ���ߣ���ֹͣѰ��
			if (st_arr[temp_index].isInSameLine(st_arr[index2])) {
				find = true;
				//final_change_time = now_time;
				//path[temp_index][++pos[temp_index]] = temp_index;
				path[temp_index][++pos[temp_index]] = index2;
				change_time[temp_index] = now_time;
			}
			else {

				//���������վ������·��δ����Ѱ����·���в���
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
								// ����վ��path[]��ֵ��temp_index�ı���һ��
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

	//�ҳ����˴�����С�ķ���
	for (int i = 0; i < st_num; ++i) {
		if (change_time[i] < final_change_time) {
			final_change_time = change_time[i];
			//cout << st_arr[i].name;
		}
	}

	//�ҳ������Ѿ��ҵ���·��������������Сֵ(���˴�����С�ķ����о�����С��)
	for (int i = 0; i < st_num; ++i) {
	
		if (path[i][pos[i]] == index2 && change_time[i] == final_change_time ) { // �Ѿ��ҵ�·���ģ�����dist
			dist[i] = 0;
			for (int j = 0; j < pos[i]; ++j) {
				string s = st_arr[path[i][j]].getSameLineName(st_arr[path[i][j + 1]]);//��·������
				string s1 = st_arr[path[i][j]].name;
				string s2 = st_arr[path[i][j+1]].name;
				int d = linelst_arr[getLineIndex(s)].calDistance(s1, s2); //�������
				dist[i] += d;
			}

			if (min_dist > dist[i]) {
				min_dist = dist[i];
				min_index = i;
			}

		}
	}

	if (min_index == -1) {
		cout << "�Ҳ�����̻���·��" << endl;
		return;
	}
	//�ҵ����ʺϵ�·���ˣ���·���浽��������
	//�Ȱ����ӽ�ȥ
	route.push(path[min_index][0]);
	for (int i = 0; i < pos[min_index]; ++i) {

		string s = st_arr[path[min_index][i]].getSameLineName(st_arr[path[min_index][i + 1]]);
		string s1 = st_arr[path[min_index][i]].name;
		string s2 = st_arr[path[min_index][i+1]].name;

		int line_index = getLineIndex(s);

		//����
		//���ҵ�s1,��¼��λ�ã�Ȼ��ֱ������߻��������ߣ��ĸ��ȵ���s2��˵��Ӧ�ô��Ǹ�������
		if (s == "2����" || s == "10����") {
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
		else {//�ǻ���
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
		
			if (p2 == NULL) { // ������p1��ʱ��û�е�p2,Ӧ�ô�p1���Ҷ�

				do {
					p = p->right;
					route.push(getStationIndex(p->station_name));
					p2 = (p->station_name == s2) ? p : p2;
				} while (p2 == NULL);
			}
			else { // ������p1��ʱ���Ѿ�����p2,Ӧ�ô�p1�����
				do {
					p = p->left;
					route.push(getStationIndex(p->station_name));
				} while (p != p2);
			}

		}
	}

}

// ���·������Ϣ
void printRuote() {

	bool gbd = false; //�߱���
	bool dwl = false; //����·
	bool xj = false;//����
	bool llq = false;//������
	bool qlz = false; //����ׯ
	int last_1 = -1;
	int last_2 = -1;
	int now;

	if (route.size() < 2) {
		cout << "���󣡣���" << endl;
		return;
	}

	cout << "��" << route.size() << "վ" << endl;
	now = route.front();
	route.pop();
	cout << st_arr[now].name << "->��վ";

	if (st_arr[now].name == "�߱���") {
		gbd = true;
	}
	if (st_arr[now].name == "����·") {
		dwl = true;
	}
	if (st_arr[now].name == "����") {
		xj = true;
	}
	if (st_arr[now].name == "������") {
		llq = true;
	}
	if (st_arr[now].name == "����ׯ") {
		qlz = true;
	}

	last_1 = now;
	last_2 = last_1;

	while (!route.empty()) {
		now = route.front();
		route.pop();

		if (st_arr[now].name == "�߱���") {
			if (dwl == true) {
				cout << "->���˵�����ͨ��";
			}
			gbd = true;
		}

		if (st_arr[now].name == "����·") {
			if (gbd == true) {
				cout << "->���˵���1����";
			}
			dwl = true;
		}

		if (st_arr[now].name == "����") {

			if ( qlz == true && llq == true) {

				if (st_arr[last_1].name == "����ׯ" && st_arr[last_2].name == "������") {
					cout << "->���˵���14��������";
				}
				if (st_arr[last_2].name == "����ׯ" && st_arr[last_1].name == "������") {
					cout << "->���˵���10����";
				}
				
			}
			xj = true;
		}

		if (st_arr[now].name == "������") {

			if (qlz == true && xj == true) {

				if (st_arr[last_1].name == "����ׯ" && st_arr[last_2].name == "����") {
					cout << "->���˵���9����";
				}
				if (st_arr[last_2].name == "����ׯ" && st_arr[last_1].name == "����") {
					cout << "->���˵���10����";
				}

			}
			llq = true;
		}

		if (st_arr[now].name == "����ׯ") {

			if (llq == true && xj == true) {

				if (st_arr[last_1].name == "������" && st_arr[last_2].name == "����") {
					cout << "->���˵���9����";
				}
				if (st_arr[last_2].name == "������" && st_arr[last_1].name == "����") {
					cout << "->���˵���14��������";
				}

			}
			qlz = true;
		}

		if (last_2 != -1) {
			if (st_arr[now].isInSameLine(st_arr[last_2]) != 1) {
				// ����
				cout << "->���˵���" << st_arr[now].getSameLineName(st_arr[last_1]);
			}
		}
		cout << endl;
		cout << st_arr[now].name;
		last_2 = last_1;
		last_1 = now;
	}

	cout << "->��վ"<<endl;
}

//main����
int main( int argc, char* argv[] ) {
//int main() {

	string op;
	string s_name1, s_name2;

    if ( argc != 4 ) {
		cout << "��������" << endl;
		return 1;
    }
    else {
		op = argv[1];
	    s_name1 = argv[2];
	    s_name2 = argv[3];
	}


	//��ʼ��ͼg
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

	//������ȷ�Լ���Լ���������д���
	if (getStationIndex(s_name1) == -1 || getStationIndex(s_name2) == -1) {
		cout << "����s_name1��s_name2����" << endl;
		return 1;
	}
	if ( op == "-b" ) {
		dijkstra(s_name1, s_name2);
	}
	else if (op == "-c") {
		leastChangeSlution(s_name1, s_name2);
	}
	else {
		cout << "����op����" << endl;
		return 1;
	}

	// ��������
	for (int i = 0; i < line_num; ++i) {
		linelst_arr[i].free();
	}
	//����˳�����
	printRuote();
	system("pause");

	return 0;
}
