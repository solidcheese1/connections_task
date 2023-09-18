// microprocessorsCmake.cpp: определяет точку входа для приложения.
//

#include "JsonParametresReader.h"
#include "math.h"
#include <unordered_set>
#include <algorithm>
#include <iostream>

using namespace std;

// происходит поиск свободной вертикали для прокладки вертикального провода
double find_x_coordinate(double x, std::map<std::string, Pin>& pins, std::unordered_set<double>& plate_x_coordinates) {
	for (auto it = pins.begin(); it != pins.end(); ++it) {
		// координата x левого нижнего угла контакта в области
		double z = x + it->second.leftDown.first;
		if ((plate_x_coordinates.count(z)) || (plate_x_coordinates.count(z + 0.1)) || (plate_x_coordinates.count(z - 0.1))) {
			x = find_x_coordinate(x + 1, pins, plate_x_coordinates);
			break;
		}
	}
	return x;
}

// меняем координаты местами в случае необходимости для записи в файл
pair<pair<double, double>, pair<double, double>> correctOutput(pair<double, double> pair1, pair<double, double> pair2) {
	pair<pair<double, double>, pair<double, double>> result;
	// меняем координаты местами в случае необходимости для записи в файл
	if (pair1.first > pair2.first) {
		swap(pair1.first,pair2.first);
	}
	if (pair1.second > pair2.second) {
		swap(pair1.second, pair2.second);
	}
	result = make_pair(pair1, pair2);
	return result;
}


int main(int argc, char* argv[])
{
	Objects tmp;
	//принимаем указатели на путь до файлов cells.json и input.json 
	tmp.jsonParametresParser(argv[1], argv[2]);

	int contactNumber = 0; 
	size_t size = tmp.cells.size();
	//максимальное количество ячеек в одном ряду
	int row = sqrt(size);
	//число рядов
	int k;
	//число рядов
	int row_col;
	if (size % row == 0) {
		k = size / row;
	}
	else {
		k = size / row + 1;
	}
	row_col = k;
	// номер столбца в матрице расположения ячеек
	int col = 0;
	//координаты левого нижнего угла левой нижней ячейки
	int x = 2;
	int y = 2;
	// количество контактов в рассматриваемом ряду ячеек
	int contact_col = 0;
	// координаты правого верхнего угла области
	int x_max = 0;
	int y_max = 0;
	// номер вертикального зазора между рядами
	int gap_numb = 0;
	// номер строки в матрице расположения ячеек
	k = 0;
	// для каждого ряда ячеек будем хранить координату "y" в 
	// соответствующем зазоре между рядами ячеек, вдоль которой будет проходить горизонтальная часть соединения
	// контакты будут соединяться от самого нижнего ряда, к самому верхнему.  
	std::vector<double> hor_wire_y_coord(row_col, 0);
	// множество, хранящее координаты х всех контактов.
	std::unordered_set<double> plate_x_coordinates;
	//цикл по всем ячейкам. Подбираем координаты ячеек в области, 
	// c целью избежать совпадения координат х контактов для того, чтобы
	// одно вертикальное соединение не проходило через несколько контактов сразу,
	// а также определяем максимальную необходимую ширину зазора между рядами по количеству контактов
	for (auto it = tmp.cells.begin(); it != tmp.cells.end(); ++it) {
		// функция, которая подбирает координату х левого нижнего угла ячейки так,
		// чтобы координаты х имеющихся контактов не совпадали
		x = find_x_coordinate(x, it->second.pins, plate_x_coordinates);
		it->second.left_corn.first = x;
		// дополняем множество координат х контактов 
		for (auto gt = it->second.pins.begin(); gt!= it->second.pins.end(); ++gt) {
			plate_x_coordinates.insert(x + gt->second.leftDown.first);
		}
		// корректировка максимального размера области
		if (x + it->second.width > x_max) x_max = x + it->second.width;
		// заполняем координату у и номера столбца и ряда для текущей ячейки
		it->second.left_corn.second = y;
		it->second.row = k;
		it->second.col = col;
		//определяем предположительную координату х следующей ячейки в ряду
		x = x + it->second.width + 2;
		contact_col = contact_col + it->second.pins.size();
		
		col++;
		// проверка заполнен ли ряд до конца
		if (col == row) {
			//переход на новый ряд
			// координата первого горизонтального контакта(0.2- отступ от повехности ячейки)
			hor_wire_y_coord[gap_numb] = y + 1 + 0.2;
			gap_numb++;
			// увеличение номера ряда, расчет координаты y для следующего ряда с учетом необходимого размера зазора
			//(зависит от количества контактов на уровне)
			col = 0;
			k = k++;
			x = 2;
			y = y + ceil((contact_col + 1) * 0.2) + 1;
			contact_col = 0;
			// 
			if (gap_numb < hor_wire_y_coord.size()) {
				// координата первого горизонтального контакта(0.2- отступ от повехности ячейки)
				hor_wire_y_coord[gap_numb] = y + 1 +0.1;
			}
		}
		
	}
	// запоминаем границы области
	x_max = x_max + 1;
	y_max = y + 1;
	tmp.areaSize = {x_max,y_max};
	// массивы координат соединений
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> horizontal;
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> vertical;
	std::vector< std::pair < std::pair<double, double>, std::pair<double, double>>> via;

// цикл по всем соединениям
	for (auto it = tmp.contactConnections.begin(); it != tmp.contactConnections.end(); ++it) {
		// вектор пар из имени ячейки и контакта на ней, через которые проходит рассматриваемое соединение
		std::vector<std::pair<std::string, std::string>>  cell_and_cont = it->second;
		// ключ- номер ряда. значение- координаты левого нижнего угла точки, через которую проходит соединение
		std::multimap<int, std::pair<double, double>> row_cont;
		// цикл по всем ячейкам, через которые проходит соединение
		//запоняем row_cont
		for (auto& t : cell_and_cont) {
			//сама ячейка
			Cell cell = tmp.cells[t.first];
			int row = cell.row;
			//координаты контакта на ней
			double x_c = cell.pins[t.second].leftDown.first +cell.left_corn.first;
			double y_c = cell.pins[t.second].leftDown.second + cell.left_corn.second;
			std::pair<double, double > point(x_c, y_c);
			row_cont.insert({ row, point });
		}
		int u = 0; 
		auto g = row_cont.begin();
		size_t size_row_cont = row_cont.size();
		//построение самих соединений
		while (u < size_row_cont - 1) {
			//tmp.horizontal = horizontal;
			//tmp.vertical = vertical;
			//tmp.via = via;
			//tmp.jsonWriteResults(argv[3]);
			
			//вначале ведем от контакта вертикально вверх до текущей координаты в зазоре между рядами(y_coord)
			// затем ведем горизонтально до нужной координаты x точки, с которой хотим соединиться
			// затем вертикально вверх до координаты y точки, с которой хотим соединиться
			// при этом горизонтальные и вертикальные соединения на разных слоях, для избежания пересечений
			
				double y_coord = hor_wire_y_coord[g->first];
				hor_wire_y_coord[g->first] = 
					hor_wire_y_coord[g->first] + 0.2;
				//построение вертикального контакта
				std::pair<double, double > first_point = g->second;
				std::pair<double, double> via_right_top(first_point.first + 0.1, first_point.second + 0.1);
				std::pair < std::pair<double, double>, std::pair<double, double>> A(first_point, via_right_top);
				via.push_back(A);
				std::pair<double, double> vert_small_right_top(first_point.first + 0.1, y_coord);
				pair<pair<double, double>, pair<double, double>> tmphor;
				//функция, преобразующая координаты углов прямоугольника к виду, необходимомому для выгрузки в выходной файл
				tmphor = correctOutput(first_point, vert_small_right_top);
				// добавление вертикального соединения
				vertical.push_back(tmphor);
				// добавление via переход на горизонтальный уровень
				std::pair<double, double> via_left_corn(first_point.first, y_coord - 0.1);
					A.first = via_left_corn;
					A.second = vert_small_right_top;
					via.push_back(A);

					g++;
					// следующпя точка, через которую проходит соединение
					std::pair<double, double > second_point = g->second;
					//функция, преобразующая координаты углов прямоугольника к виду, необходимомому для выгрузки в выходной файл
					tmphor = correctOutput(via_left_corn, make_pair(second_point.first + 0.1, via_left_corn.second + 0.1));
					// добавление горизонтального соединения
					horizontal.push_back(tmphor);
					via.push_back(make_pair(make_pair(second_point.first, via_left_corn.second),
						make_pair(second_point.first + 0.1, via_left_corn.second + 0.1)));
				tmphor = correctOutput(second_point, make_pair(second_point.first + 0.1, via_left_corn.second + 0.1));
				//добавление последнего вертикального соединения, точки соединены
				vertical.push_back(tmphor);
				via.push_back(make_pair(second_point, make_pair(second_point.first + 0.1, second_point.second + 0.1)));


				u++;


			
		}
		
		

	}


	

	tmp.horizontal = horizontal;
	tmp.vertical = vertical;
	tmp.via = via;
	// запись результатов в файл
	tmp.jsonWriteResults(argv[3]);
	return 0;
}
