#pragma once
#include <string>

using namespace std;

class station {

public:
	string name;
	string line[3];
	station();
	station(string name, string lineName);
	virtual ~station();
	bool isInSameLine(station s);
	void addLine(string lineName);
	string getSameLineName(station s);
	bool isChangeStation();
	bool isInLine(string lineName);

protected:
private:

};

