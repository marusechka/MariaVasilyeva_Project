#include "fragmentation.h"
#include <iostream>
#include <locale.h>
#include <conio.h>
#include <chrono>
#include <iostream>
using namespace std::chrono;

/// параметры начальной прямоугольной области
double g_l1_max = 12.0;
double g_l2_max = g_l1_max;
double g_l1_min = 8.0;
double g_l2_min = g_l1_min;
double g_l0 = 5.0;

/// точность аппроксимации рабочего пространства
const double g_precision = 0.25;


int main()
{
	setlocale(LC_ALL,"Rus");
		
	double a = -10, b = 0, c = 24, d = 16;
	
	high_resolution_clock::time_point t1, t2;
	__cilkrts_end_cilk();
	__cilkrts_set_param("nworkers", "4");
	
	t1 = high_resolution_clock::now(); //засекли время
	high_level_analysis main_object(a,b,c,d);
		
	main_object.GetSolution();

	t2 = high_resolution_clock::now();
	duration<double> duration = (t2 - t1);
	std::cout << "Длительность выполнения программы: " << duration.count() << " секунд" << std::endl;
	std::cout << "Число вычислителей: " << __cilkrts_get_nworkers() << std::endl;

	// Внимание! здесь необходимо определить пути до выходных файлов!
	//const char* out_files[3] = {	"C:/Users/Marusik/Desktop/Project/sol.txt", "C:/Users/Marusik/Desktop/Project/notsol.txt", "C:/Users/Marusik/Desktop/Project/bound.txt" };
	//WriteResults( out_files );
	//_getch();

	return 0;
}
