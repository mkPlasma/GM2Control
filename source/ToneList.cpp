#include "ToneList.h"

#include<fstream>
#include<streambuf>

using nlohmann::json;


bool ToneList::load(const string& path){

	std::ifstream in(path);

	if(!in.good())
		return false;

	std::string tonelistFile((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	json tonesJson = json::parse(tonelistFile);

	_tones.clear();

	// Counter for ordering categories
	for(int i = 0; i < tonesJson.size(); i++){

		// Iterate categories
		for(auto& j : tonesJson.items()){

			// Check that category order matches
			try{
				if(i != j.value().at(0)["index"])
					continue;
			}
			catch(const nlohmann::json::out_of_range&){
				return false;
			}


			// Create tone list for category
			vector<Tone> catTones;

			// Iterate tones
			for(int k = 1; k < j.value().size(); k++){
				Tone tone;
				auto t = j.value().at(k);

				try{
					tone.name = t.at("name").get<string>();
					tone.pc = t.at("pc").get<int>() - 1;
					tone.msb = t.at("msb").get<int>();
					tone.lsb = t.at("lsb").get<int>();
				}
				catch(const nlohmann::json::out_of_range&){
					return false;
				}

				catTones.push_back(tone);
			}

			// Create category
			ToneCategory c;
			c.name = j.key();
			c.tones = catTones;

			_tones.push_back(c);
		}
	}

	// Blank tone category
	ToneCategory c;
	c.name = "-";
	c.tones.push_back({"-", 0, 0, 0});
	_tones.push_back(c);

	return true;
}

const vector<ToneCategory>& ToneList::getTones() const{
	return _tones;
}

vector<string> ToneList::getCategoryNames() const{

	vector<string> names;

	for(auto i = _tones.begin(); i != _tones.end(); i++)
		names.push_back((*i).name);

	return names;
}

vector<string> ToneList::getToneNames(int cat) const{

	vector<string> names;

	vector<Tone> tns = _tones[cat].tones;

	for(auto i = tns.begin(); i != tns.end(); i++)
		names.push_back((*i).name);

	return names;
}
