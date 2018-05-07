#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>

#include "cimg.h"
#include "console.h"
#include "point.h"
#include "random.h"

using namespace cimg_library;

class Shepard
{
  private:
	double ave_err;

  public:
	Shepard() {}
	~Shepard() {}

  public:
	bool run(const int nb_points,
			 const double maxpower,
			 const char *pInput,
			 const char *pOutput)
	{
		std::ofstream file;
		// read input image
		std::cout << "read input image...";
		CImg<unsigned char> input(pInput);

		// verify image size
		const int width = input.width();
		const int height = input.height();
		if (width * height == 0)
		{
			std::cerr << red << "failed" << white << std::endl;
			return false;
		}
		std::cout << "done" << std::endl;

		// generate initial points to interpolate
		// at the four image corners
		std::list<Point> points;
		// points.push_back(Point(0.0, 0.0));
		// points.push_back(Point(width - 1.0, 0.0));
		// points.push_back(Point(0.0, height - 1.0));
		// points.push_back(Point(width - 1.0, height - 1.0));

		// interpolation
		std::cout << "interpolating...";
		CImg<unsigned char> output(width, height, 1, 3, 0); // color image with pixels set to
		CImg<unsigned char> err(width, height, 1, 1, 0);	//error image
		file.open("data0.csv", std::ios::app | std::ios::in | std::ios::out);
		for (int j = 0; j != 100; ++j)
		{
			for (int i = 0; i != nb_points; ++i)
			{
				const int x = random_int(0, width - 1);
				const int y = random_int(0, height - 1);
				points.push_back(Point(x, y));
			}

			// interpolate RGB colors at data points
			double r = 0.0;
			double g = 0.0;
			double b = 0.0;
			std::list<Point>::iterator pit = points.end();
			std::prev(pit, nb_points);
			std::list<double>::iterator wit = weights.begin();
			for (/**/; pit != points.end(); ++pit, ++wit)
			{
				const Point &p = *pit;
				const double w = *wit;
				const int xp = (int)p.x();
				const int yp = (int)p.y();
				r += w * (double)input(xp, yp, 0);
				g += w * (double)input(xp, yp, 1);
				b += w * (double)input(xp, yp, 2);
			}

			double pave_err = 1000.0;
			double power = 4.0;
			ave_err = 0.0;
			while (ave_err < pave_err && power != maxpower)
			{
				// simple ASCII progress bar
				std::cout << ".";

				if (ave_err)
					pave_err = ave_err;

				for (int x = 0; x != width; ++x)
				{
					for (int y = 0; y != height; ++y)
					{
						// compute weights for each data points
						std::list<double> weights;

						const double sum = this->compute_weights(x, y,
																 pit, points.end(), power,
																 std::back_inserter(weights));
						r /= sum; // note normalization
						g /= sum;
						b /= sum;

						// write pixel color
						output.atXY(x, y, 0) = (unsigned char)r;
						output.atXY(x, y, 1) = (unsigned char)g;
						output.atXY(x, y, 2) = (unsigned char)b;

						double er = (double)abs(input(x, y, 0) - r);
						double eg = (double)abs(input(x, y, 1) - g);
						double eb = (double)abs(input(x, y, 2) - b);
						double gray = (er + eg + eb) / 3;
						ave_err += gray;

						// write pixel color
						err.atXY(x, y, 0) = (unsigned char)gray;
					}
				}
				ave_err /= (double)(width * height);
				power++;
			}
			ave_err = pave_err;
			file << ave_err << ',';
			file << (power - 1) << "\n";
		}
		file.close();
		std::cout << "done" << std::endl;

		std::cout << "write image...";
		output.save_bmp(pOutput);
		err.save_bmp("err0.bmp");
		std::cout << "done" << std::endl;

		return true;
	}

	// compute average error
	double average_err()
	{
		return ave_err;
	}

	// compute barycentric coordinates of a query point (x, y)
	// with respect to all data points
	// return sum of weights
	template <class InputIterator, class OutputIterator>
	double compute_weights(const int x,
						   const int y,
						   InputIterator begin,
						   InputIterator end,
						   const double power,
						   OutputIterator out)
	{
		const Point query((double)x, (double)y);

		// verify whether query coincides with one data point
		InputIterator itc = end; // will store location of equal data point
		for (InputIterator it = begin; it != end; ++it)
		{
			const Point &p = *it;
			if (p.coincide(query))
				itc = it;
		}
		if (itc != end) // yes, set all weights to 0.0 except one to 1.0
		{
			for (InputIterator it = begin; it != end; ++it)
			{
				if (it == itc)
					*out++ = 1.0; // write to output iterator
				else
					*out++ = 0.0; // write to output iterator
			}
			return 1.0; // return sum of weights
		}

		// compute general weights
		double sum = 0.0;
		for (InputIterator it = begin; it != end; ++it)
		{
			const Point &p = *it;
			const double w = p.inverse_distance(query, power);

			*out++ = w;
			sum += w;
		}
		return sum; // sum of weights
	}				// end compute_weights
};
