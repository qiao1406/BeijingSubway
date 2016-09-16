#pragma once
#include <string>

using namespace std;

typedef struct StationNode{
	string station_name;
	struct StationNode *left;
	struct StationNode *right;

}SNode, *SLinkList;

class line_list
{

	

public:
	string linename;
	SLinkList head;
	SLinkList end;
	line_list();
	line_list(string name);
	~line_list();
	int calDistance(string s1, string s2);
	void free();
};



