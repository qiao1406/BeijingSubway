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

	//���ߵ����
	if (this->linename == "10����" || this->linename == "2����") {
		int case1 = 0;//ģ�·ǻ�����
		int case2 = 0;//���ҿ���
		int d1 = 0;
		int d2 = 0;

		// case1
		int temp = 0;//temp��¼���ǵ�ǰָ����ָλ�������վ�ľ���
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
	else {//���ǻ��ߵ����
		int d1 = 0;
		int d2 = 0;
		int temp = 0;//temp��¼���ǵ�ǰָ����ָλ�������վ�ľ���

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

// �ͷ�ָ��
void line_list::free() {
	SLinkList p;

	//����
	if (linename == "2����" || linename == "10����") {
		end = head->left;
		do {
			p = head;
			head = head->right;
			delete p;
		} while (p != end);
		p = head = NULL;
	}
	else { //�ǻ���
		while (head != NULL) {
			p = head;
			head = head->right;
			delete p;
		}
		p = head = NULL;
	}
	
}