#include "line_list.h"


line_list::line_list()
{
	head = NULL;
	end = NULL;
}


line_list::line_list( string name )
{
	
	linename = name;
	head = NULL;
	end = NULL;
}


line_list::~line_list()
{
}


int line_list::calDistance(string s1, string s2) {
	
	SLinkList p = this->head;

	//环线的情况
	if (this->linename == "10号线" || this->linename == "2号线") {
		int case1 = 0;//模仿非环行线
		int case2 = 0;//左右开弓
		int d1 = 0;
		int d2 = 0;

		// case1
		int temp = 0;//temp记录的是当前指针所指位置与起点站的距离
		while (p->station_name != s1 && p->station_name != s2 && p != NULL) {
			p = p->right;
			temp++;
		}
		if (p->station_name == s1) {
			d1 = temp;

			do {
				temp++;
				p = p->right;
			} while (p->station_name != s2 && p != head->left);
			d2 = temp;
		}
		else {
			d2 = temp;
			do {
				temp++;
				p = p->right;
			} while (p->station_name != s1 && p != head->left);
			d1 = temp;
		}
		case1 = (d1 - d2 > 0) ? (d1 - d2) : (d2 - d1);

		// case2
		d1 = 0;
		d2 = 0;
		p = head;
		while (p->station_name != s1 && p->station_name != s2 ) {
			p = p->right;
			d1++;
		}

		
		if (p->station_name == s1) {
			p = head;
			while ( p->station_name != s2) {
				p = p->left;
				d2++;
			}
		}
		else {
			p = head;
			while (p->station_name != s1) {
				p = p->left;
				d2++;
			}
		}
		case2 = d1 + d2;

		return case1 < case2 ? case1 : case2;

	} 
	else {//不是环线的情况
		int d1 = 0;
		int d2 = 0;
		int temp = 0;//temp记录的是当前指针所指位置与起点站的距离

		while (p->station_name != s1 && p->station_name != s2 && p != NULL) {
			p = p->right;
			temp++;
		}
		if (p->station_name == s1) {
			d1 = temp;
			while (p->station_name != s2 && p != NULL) {
				temp++;
				p = p->right;
			}
			d2 = temp;
		}
		else {
			d2 = temp;
			while (p->station_name != s1 && p != NULL) {
				temp++;
				p = p->right;
			}
			d1 = temp;
		}

		return (d1 - d2 > 0) ? (d1 - d2) : (d2 - d1);
	}
	
}

// 释放指针
void line_list::free() {
	SLinkList p;

	//环线
	if (linename == "2号线" || linename == "10号线") {
		end = head->left;
		do {
			p = head;
			head = head->right;
			delete p;
		} while (p != end);
		p = head = NULL;
	}
	else { //非环线
		while (head != NULL) {
			p = head;
			head = head->right;
			delete p;
		}
		p = head = NULL;
	}
	
}