#include "station.h"
station::station() { }

station::station(string name, string lineName) {
	this->name = name;
	line[0] = lineName;
}

station::~station() {
	//dtor
}

bool station::isInSameLine(station s) {

	if (this->name == s.name) {
		return true;
	}
	else {

		int i = 0;
		int j = 0;
		bool temp = false;

		while (this->line[i] != "" && i <3) {

			while (s.line[j] != "" && j < 3) {

				if (this->line[i] == s.line[j]) {
					temp = true;
					break;
				}
				j++;
			}

			if (temp == true) {
				break;
			}
			i++;
			j = 0;
		}

		return temp;
	}

}

void station::addLine(string lineName) {
	int i = 0;

	for (; i < 3; ++i) {
		if (line[i] == lineName) {
			return;
		}
		if (line[i] == "")
			break;
	}

	if (i < 3) {
		line[i] = lineName;
	}

}

//�ж��Ƿ�Ϊ���˳�վ
bool station::isChangeStation() {
	return (line[1] != "") ? true : false;
}

// �ж����վ�Ƿ���ĳ������
bool station::isInLine(string lineName) {
	bool rst = false;

	for (int i = 0; i < 3; ++i) {
		if (this->line[i] == "") {
			break;
		}

		if (this->line[i] == lineName) {
			rst = true;
		}

	}

	return rst;
}

//�ҳ�����վ��ͬ����������·
string station::getSameLineName(station s) {
	string sameline = "";
	int i = 0;
	int j = 0;

	while (this->line[i] != "" && i <3) {

		while (s.line[j] != "" && j < 3) {

			if (this->line[i] == s.line[j]) {
				sameline = this->line[i];
				break;
			}
			j++;
		}

		if (sameline != "") {
			break;
		}
		i++;
		j = 0;
	}

	return sameline;

}
