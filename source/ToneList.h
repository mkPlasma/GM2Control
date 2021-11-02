#pragma once

#include<string>
#include<vector>
#include"json.hpp"

using std::string;
using std::vector;


struct Tone{
	string name;
	int pc;
	int msb;
	int lsb;
};

struct ToneCategory{
	string name;
	vector<Tone> tones;
};


class ToneList{
	vector<ToneCategory> _tones;

public:
	bool load(const string& path);

	const vector<ToneCategory>& ToneList::getTones() const;

	vector<string> ToneList::getCategoryNames() const;
	vector<string> ToneList::getToneNames(int cat) const;
};
