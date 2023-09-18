#pragma once

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

// структура контакта
struct Pin {
	// имя контакта
	std::string pinName;
	// координаты левого нижнего угла контакта
	std::pair<double, double> leftDown;
	// координаты правого верхнего угла контакта
	std::pair<double, double> rightTop;
};

// структура ячейки
struct Cell {
	int cell_numb;
	// название типа ячейки
	std::string cell_type;
	// имя или номер ячейки
	std::string cellNumb;
	// ширина ячейки в у.е.
	int width;
	// координаты левого нижнего угла ячейки
	std::pair<double, double> left_corn;
	// контакты на ячейке
	std::map<std::string, Pin> pins;
	// номер строки ячейки в структуре расположения ячеек в области
	int row;
	// номер столбца ячейки в структуре расположения ячеек в области
	int col;
	

};

class Objects {
public:
	// ячейки в области(имя ячейки, структура ячейки)
	std::unordered_map<std::string, Cell> cells;
	// типы ячеек(тип ячейки, структура ячейки)
	std::unordered_map<std::string, Cell> cellsType;
	// имя межсоединения(набор имя ячейки имя контакта)
	std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>>> contactConnections;
	// размер области
	std::pair<int, int> areaSize;
	// горизонтальные проводники(координаты левого нижнего угла и правого верхнего)
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> horizontal;
	// вертикальные проводники
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> vertical;
	// связи между 2 уровнями проводников
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> via;
	// считывание типов объектов, их количества и межсоединений
	void jsonParametresParser(char* cellName, char* inputName);
	// вывод результатов в json-файл
	void jsonWriteResults(char* outputName);
};
 