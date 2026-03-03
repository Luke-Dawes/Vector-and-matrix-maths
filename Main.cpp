#include "generic_allocator.h"
//#include "Generic.h"
#include <iostream>
#include <chrono>
#include <random>

/*
Math::Vector2 first(1,1);
Math::Vector2 second(1, 1);

Math::Mat2x2 mat2(first, second);

Math::Vector2 f(5, 2);
Math::Vector2 s(1, 8);

Math::Mat2x2 mat22(f, s);

int scalar = 5;

int main() {
	std::cout << "addition: " << (first + second) << "\n";
	std::cout << "vec1 * 5: " << (first * scalar) << "\n";

	std::cout << (mat2 * mat22) << "\n\n";

	std::cout << (mat22 * mat22.invert());
}
*/

int main() {
	Matrix<float, 4, 4> A = {
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16}
	};

	Vector<float, 4> B =
	{17, 18, 19, 20};
	
	Matrix<float, 1, 1> C = { 1, 2 };


	std::cout << A * B;


    return 0;
}
