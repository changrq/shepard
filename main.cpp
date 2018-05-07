#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

#include "cimg.h"
#include "shepard.h"

using namespace cimg_library;

void usage()
{
	std::cout << "Usage: shepard n p input.bmp output.bmp" << std::endl;
	std::cout << "n: number of interpolation points (integer, >= 1)" << std::endl;
	std::cout << "p: inverse distance power (double, >= 1)" << std::endl;
	std::cout << "input: input file in bmp format (24 bits)" << std::endl;
	std::cout << "output: output file in bmp format (24 bits)" << std::endl;
}

int main(int argc, char **argv)
{
	if (argc < 5)
	{
		usage();
		return 1;
	}

	srand(0);
	// scan parameters
	int nb_points = 1;
	double power = 20.0;
	sscanf(argv[1], "%i", &nb_points);
	sscanf(argv[2], "%lf", &power);

	std::cout << yellow << "SHEPARD INTERPOLATION" << white << std::endl;
	std::cout << "number of points: " << (nb_points * 100) << std::endl;
	std::cout << "maxpower: " << power << std::endl;

	// Shepard interpolation
	Shepard shepard;
	shepard.run(nb_points, power, argv[3], argv[4]);
	std::cout << "Average error: " << shepard.average_err() << std::endl;

	return 0;
}
