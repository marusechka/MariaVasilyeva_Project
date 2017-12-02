#include "fragmentation.h"

/// вектор, содержащий box-ы, являющиеся частью рабочего пространства
std::vector<Box> solution;
/// вектор, содержащий box-ы, не являющиеся частью рабочего пространства
std::vector<Box> not_solution;
/// вектор, содержащий box-ы, находящиеся на границе между "рабочим" и "нерабочим" пространством
std::vector<Box> boundary;
/// вектор, хранящий box-ы, анализируемые на следующей итерации алгоритма
std::vector<Box> temporary_boxes;


/// функции gj()
//------------------------------------------------------------------------------------------
double g1(double x1, double x2)
{
	return (x1*x1 + x2*x2 - g_l1_max*g_l1_max);
}

//------------------------------------------------------------------------------------------
double g2(double x1, double x2)
{
	return (g_l1_min*g_l1_min - x1*x1 - x2*x2);
}

//------------------------------------------------------------------------------------------
double g3(double x1, double x2)
{
	return (x1*x1 + x2*x2 - g_l2_max*g_l2_max);
}

//------------------------------------------------------------------------------------------
double g4(double x1, double x2)
{
	return (g_l2_min*g_l2_min - x1*x1 - x2*x2);
}


//------------------------------------------------------------------------------------------
low_level_fragmentation::low_level_fragmentation(double& min_x, double& min_y, double& x_width, double& y_height )
{
	current_box = Box( min_x, min_y, x_width, y_height );
}

//------------------------------------------------------------------------------------------
low_level_fragmentation::low_level_fragmentation(const Box& box)
{
	current_box = box;
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::VerticalSplitter(const Box& box, boxes_pair& vertical_splitter_pair)
{
	double x, y, w, h;
	box.GetParameters(x, y, w, h);
	vertical_splitter_pair.first = Box(x,y,w/2,h);
	vertical_splitter_pair.second = Box(x+w/2, y, w/2, h);
	// необходимо определить функцию
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::HorizontalSplitter(const Box& box, boxes_pair& horizontal_splitter_pair)
{
	double x, y, w, h;
	box.GetParameters(x, y, w, h);
	horizontal_splitter_pair.first = Box(x, y, w, h/2);
	horizontal_splitter_pair.second = Box(x, y+h/2, w, h/2);
	// необходимо определить функцию
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::GetNewBoxes(const Box& box, boxes_pair& new_pair_of_boxes)
{
	double x, y, w, h;
	box.GetParameters(x, y, w, h);
	if (w >= h)
		VerticalSplitter(box, new_pair_of_boxes);
	else
		HorizontalSplitter(box, new_pair_of_boxes);
	// необходимо определить функцию
}

//------------------------------------------------------------------------------------------
unsigned int low_level_fragmentation::FindTreeDepth()
{
	double box_diagonal = current_box.GetDiagonal();

	if (box_diagonal <= g_precision)
	{
		return 0;
	}
	else
	{
		boxes_pair new_boxes;
		// допустим, разобьем начальную область по ширине
		VerticalSplitter(current_box, new_boxes);
		unsigned int tree_depth = 1;

		box_diagonal = new_boxes.first.GetDiagonal();

		if (box_diagonal <= g_precision)
		{
			return tree_depth;
		}
		else
		{
			for (;;)
			{
				GetNewBoxes(new_boxes.first, new_boxes);
				++tree_depth;
				box_diagonal = new_boxes.first.GetDiagonal();

				if (box_diagonal <= g_precision)
				{
					break;
				}
			}
			return tree_depth;
		}
	}
}

//------------------------------------------------------------------------------------------
int low_level_fragmentation::ClasifyBox(const min_max_vectors& vects)
{
	int cnt_max = 0;
	int cnt_min = 0;
	int length_f=vects.first.size();
	int length_s = vects.second.size();

	if (vects.first[0] == 0) return 3; // граница

	for (int i = 0; i < 4; i++)
	{
		if (vects.first[i] < 0) cnt_max++; // условие 4
		if (vects.second[i] > 0) return 0; // условие 5
	}

	if (cnt_max == 4) return 1; // 4
	else return 2; // деление
	// необходимо определить функцию
}

//------------------------------------------------------------------------------------------
void low_level_fragmentation::GetBoxType(const Box& box)
{
	/// Функция GetBoxType() добавляет классифицированный ранее box во множество решений, 
	/// или удаляет его из анализа, или добавляет его к граничной области, 
	/// или относит его к тем, что подлежат дальнейшему анализу
	// необходимо определить функцию

	int number;
	min_max_vectors vecs;
	GetMinMax(box, vecs);
	boxes_pair pair;

	number = ClasifyBox(vecs);

	if (number == 0) // shit
		not_solution.push_back(box);
	if (number == 1) // work
		solution.push_back(box);
	if (number == 2) { // split
		GetNewBoxes(box, pair);
		temporary_boxes.push_back(pair.first);
		temporary_boxes.push_back(pair.second);
	}
	if (number == 3) // boundary
		boundary.push_back(box);
}


//------------------------------------------------------------------------------------------
high_level_analysis::high_level_analysis( double& min_x, double& min_y, double& x_width, double& y_height ) :
					low_level_fragmentation(min_x, min_y, x_width, y_height) {}

//------------------------------------------------------------------------------------------
high_level_analysis::high_level_analysis( Box& box ) : low_level_fragmentation( box ) {}

//------------------------------------------------------------------------------------------
void high_level_analysis::GetMinMax( const Box& box, min_max_vectors& min_max_vecs )
{
	std::vector<double> g_min;
	std::vector<double> g_max;

	double a1min, a2min, a1max, a2max;
	double xmin, xmax, ymin, ymax;

	box.GetParameters(xmin, ymin, xmax, ymax);

	xmax = xmin + xmax;
	ymax = ymin + ymax;

	double curr_box_diagonal = box.GetDiagonal();

	if (curr_box_diagonal <= g_precision)
	{
		g_min.push_back(0);
		g_max.push_back(0);

		min_max_vecs.first = g_max;
		min_max_vecs.second = g_min;

		return;
	}

	// MIN
	// функция g1(x1,x2)
	a1min = __min(abs(xmin), abs(xmax));
	a2min = __min(abs(ymin), abs(ymax));
	g_min.push_back(g1(a1min, a2min));

	// функция g2(x1,x2)
	a1min = __max(abs(xmin), abs(xmax));
	a2min = __max(abs(ymin), abs(ymax));
	g_min.push_back(g2(a1min, a2min));

	// функция g3(x1,x2)
	a1min = __min(abs(xmin - g_l0), abs(xmax - g_l0));
	a2min = __min(abs(ymin), abs(ymax));
	g_min.push_back(g3(a1min, a2min));

	// функция g4(x1,x2)
	a1min = __max(abs(xmin - g_l0), abs(xmax - g_l0));
	a2min = __max(abs(ymin), abs(ymax));
	g_min.push_back(g4(a1min, a2min));

	// MAX
	// функция g1(x1,x2)
	a1max = __max(abs(xmin), abs(xmax));
	a2max = __max(abs(ymin), abs(ymax));
	g_max.push_back(g1(a1max, a2max));

	// функция g2(x1,x2)
	a1max = __min(abs(xmin), abs(xmax));
	a2max = __min(abs(ymin), abs(ymax));
	g_max.push_back(g2(a1max, a2max));

	// функция g3(x1,x2)
	a1max = __max(abs(xmin - g_l0), abs(xmax - g_l0));
	a2max = __max(abs(ymin), abs(ymax));
	g_max.push_back(g3(a1max, a2max));

	// функция g4(x1,x2)
	a1max = __min(abs(xmin - g_l0), abs(xmax - g_l0));
	a2max = __min(abs(ymin), abs(ymax));
	g_max.push_back(g4(a1max, a2max));

	min_max_vecs.first = g_max;
	min_max_vecs.second = g_min;
}

//------------------------------------------------------------------------------------------
void high_level_analysis::GetSolution()
{
	
	//должны фигурировать два вложенных цикла. Внешний цикл проходит по всем уровням двоичного дерева разбиения. 
	//В рамках внутреннего цикла происходит перебор всех box-ов текущего уровня разбиения и определение типа box-а 
	//(является он частью рабочего пространства либо не является, лежит он на границе или подлежит дальнейшему анализу). 

	int length = FindTreeDepth()+1;	
	boxes_pair pair;
	temporary_boxes.push_back(current_box);

	for (int i = 0; i < length; i++) {
		int number_of_box_on_level = temporary_boxes.size();
		std::vector<Box> curr_boxes(temporary_boxes);
		temporary_boxes.clear();
		for (int j = 0; j < number_of_box_on_level; j++){
			GetBoxType(curr_boxes[j]);
		}
	}

}


//------------------------------------------------------------------------------------------
void WriteResults( const char* file_names[] )
{
	// необходимо определить функцию
	double _xmin, _xmax, _w, _h;
	std::ofstream out(file_names[0]);
	for (int i = 0; i < solution.size(); i++){
		solution[i].GetParameters(_xmin, _xmax, _w, _h);
		out << _xmin << " " << _xmax << " " << _w << " " << _h << std::endl;
	}
	out.close();

	std::ofstream out1(file_names[1]);
	for (int i = 0; i < not_solution.size(); i++){
		not_solution[i].GetParameters(_xmin, _xmax, _w, _h);
		out1 << _xmin << " " << _xmax << " " << _w << " " << _h << std::endl;
	}
	out1.close();

	std::ofstream out2(file_names[2]);
	for (int i = 0; i < boundary.size(); i++){
		boundary[i].GetParameters(_xmin, _xmax, _w, _h);
		out2 << _xmin << " " << _xmax << " " << _w << " " << _h << std::endl;
	}
	out2.close();
}
