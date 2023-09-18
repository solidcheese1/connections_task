#include "JsonParametresReader.h"

void Objects::jsonParametresParser(char* cellName, char* inputName){ //(std::string cells, std::string input) {
	std::ifstream cellsStream(cellName);
	json cellsJson;
	cellsStream >> cellsJson;
	int cell_numb = 0;
	for (auto j : cellsJson.items()) {
		// считываем базовые параметры ячейки
		Cell c;
		std::string cell_type = j.key();
		c.cell_type = cell_type;
		double width;
		std::map<std::string, Pin> pins;
		json l = j.value();
		for (auto lk : l.items()) {
			if (lk.key() == "width") {
				width = lk.value();
				c.width = width;
			}
			if (lk.key() == "pins") {
				json pinss = lk.value();
				for (auto pk : pinss.items()) {
					std::string s = pk.key();
					std::vector<double> v = pk.value();
					Pin pin;
					pin.pinName = s;
					pin.leftDown.first = v[0];
					pin.leftDown.second = v[1];
					pin.rightTop.first = v[2];
					pin.rightTop.second = v[3];
					pins[s] = pin;


				}

			}
		}
		c.pins = pins;
		c.cell_numb = cell_numb;
		cell_numb++;
		cellsType[c.cell_type] = c;
	}
	std::ifstream inputStream(inputName);
	json inputJson;
	inputStream >> inputJson;
	if (inputJson.find("cells") != inputJson.end()) {
		json objects = inputJson["cells"];
		for (auto object : objects.items()) {
			Cell c;
			// имя или номер ячейки в области
			c.cellNumb = object.key();
			json objectStruct = object.value();
			for (auto objectDescriptor : objectStruct.items()) {
				if (objectDescriptor.key() == "type") {
					c.cell_type = objectDescriptor.value();
					c.pins = cellsType[c.cell_type].pins;
					c.width = cellsType[c.cell_type].width;
				}
				if (objectDescriptor.key() == "connections") {
					json connections = objectDescriptor.value();
					for (auto connectionDescriptor : connections.items()) {
						// добавляем контакт к межсоединению
						contactConnections[connectionDescriptor.value()].push_back({c.cellNumb, connectionDescriptor.key()});
					}
				}
			}
			// запоминаем
			cells[c.cellNumb] = c;
		}
	}
}



void Objects::jsonWriteResults(char* outputName)
{
	/*std::ofstream outputStream("output.json");*/
	std::ofstream outputStream(outputName);
	json j;
	j["size"] = { areaSize.first, areaSize.second };

	for (auto it = cells.begin(); it != cells.end(); ++it) {
		j["cells"][cells[it->first].cellNumb] = { cells[it->first].cell_type, static_cast<int>(std::round(cells[it->first].left_corn.first)), static_cast<int>(std::round(cells[it->first].left_corn.second)) };
	}
	/*json::array a;
	for (size_t i = 0; i < horizontal.size(); ++i) {
		j["wires0"].push_back({ {horizontal[i].first.first, horizontal[i].first.second, horizontal[i].second.first, horizontal[i].second.second });
	}*/
	std::vector<std::vector<double>> wires0;
	wires0.resize(horizontal.size());
	std::vector<double> tmp;
	tmp.resize(4);
	for (size_t i = 0; i < horizontal.size(); ++i) {
		tmp[0] = horizontal[i].first.first;
		tmp[1] = horizontal[i].first.second;
		tmp[2] = horizontal[i].second.first;
		tmp[3] = horizontal[i].second.second;
		wires0[i] = tmp;
	}
	j["wires0"] = wires0;

	std::vector<std::vector<double>> wires1;
	wires1.resize(vertical.size());
	for (size_t i = 0; i < vertical.size(); ++i) {
		tmp[0] = vertical[i].first.first;
		tmp[1] = vertical[i].first.second;
		tmp[2] = vertical[i].second.first;
		tmp[3] = vertical[i].second.second;
		wires1[i] = tmp;
	}
	j["wires1"] = wires1;

	std::vector<std::vector<double>> via0;
	via0.resize(via.size());
	for (size_t i = 0; i < via.size(); ++i) {
		tmp[0] = via[i].first.first;
		tmp[1] = via[i].first.second;
		tmp[2] = via[i].second.first;
		tmp[3] = via[i].second.second;
		via0[i] = tmp;
	}
	j["via"] = via0;
	//for (size_t i = 0; i < horizontal.size(); ++i) {
	//	//j["wires0"] = { {horizontal[i].first.first, horizontal[i].first.second, horizontal[i].second.first, horizontal[i].second.second } };
	//}
	j >> outputStream;
}

//void Objects::jsonSystemParser()
//{
//	std::ifstream inputStream("input.json");
//	std::vector<Cell> cells;
//	json inputJson;
//	inputStream >> inputJson;
//}
