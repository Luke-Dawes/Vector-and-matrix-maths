#pragma once
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <malloc.h>
#include <array>
#include <initializer_list>
#include <new>
#include <cassert>
#include <immintrin.h>

template<typename T>
struct AlignedAllocator {
	using value_type = T;

	T* allocate(size_t n) {

		size_t allignment = 16;
		size_t size = n * sizeof(T);

		size_t remainder = size % allignment;
		if (remainder != 0) {
			size += (allignment - remainder);
		}

		void* ptr = _aligned_malloc(size, allignment);
		if (!ptr) throw std::bad_alloc();
		return static_cast<T*>(ptr);
	}

	void deallocate(T* p, size_t n) {
		_aligned_free(p);
	}
};

template<typename T, size_t Cols, typename Alloc = AlignedAllocator<T>>
class alignas(16) Vector {
	Alloc allocator;
	T* data;

public:

	Vector() {
		data = allocator.allocate(Cols);
	}

	Vector(const Vector& other) {
		data = other.data;
	}

	Vector(Vector&& other)
		: data(other.data) 
	{
		other.data = nullptr;
	}

	void zero() {
		for (size_t i = 0; i < Cols; ++i) {
			data[i] = static_cast<T>(0);
		}
	}

	T& operator[](size_t row) {
		assert(row < Cols && "row out of bounds");
		return data[row];
	}

	const T& operator[](size_t row) const {
		assert(row < Cols && "row out of bounds");
		return data[row];
	}

	Vector(std::initializer_list<T> list) {
		data = allocator.allocate(Cols);
		size_t c = 0;
		for (const auto& val : list) {
			if (c > Cols) break;
			data[c] = val;
			++c;
		}
	}

};


template<typename T, size_t Rows, size_t Cols, typename Alloc = AlignedAllocator<T>>
class alignas(16) Matrix {
	Alloc allocator;
	T* data;

public:
	Matrix() {
		data = allocator.allocate(Rows * Cols);
	}
	
	Matrix(const Matrix& other) {
		data = allocator.allocate(Rows * Cols);

		for (size_t i = 0; i < Rows * Cols; ++ i) {
			data[i] = other.data[i];
		}
	}

	Matrix(Matrix&& other)
		: data(other.data)
	{
		other.data = nullptr;
	}

	Matrix& operator=(const Matrix& other) {
		if (&other != this) { //assignment to itself
			allocator.deallocate(data, Rows * Cols);
			data = other.data;
			other.data = nullptr;
		}
		return *this;
	}

	Matrix(std::initializer_list<T> list) {
		data = allocator.allocate(Rows * Cols);
		size_t c = 0;
		for (const auto& val : list) {
			data[c] = val;
			++c;
			if (c > Rows * Cols) break; //find a way to print a message
		}
	}

	Matrix(std::initializer_list<std::initializer_list<T>> list) {
		data = allocator.allocate(Rows * Cols);
		size_t r = 0;
		for (const auto& row_list : list) {
			if (r >= Rows) break;
			size_t c = 0;
			for (const auto& val : row_list){
				if (c >= Cols) break;
				data[r * Cols + c] = val;
				++c;
			}
			++r;
		}
	}

	Matrix(const __m128 row0, const __m128 row1, const __m128 row2, const __m128 row3) {
		data = allocator.allocate(Rows * Cols);

		_mm_store_ps(&data[0], row0);
		_mm_store_ps(&data[4], row1);
		_mm_store_ps(&data[8], row2);
		_mm_store_ps(&data[12], row3);

	}

	Matrix<T, Rows, Cols>& zero() {
		for (size_t i = 0; i < Rows * Cols; ++i) {
			data[i] = static_cast<T>(0);
		}
		return *this;
	}


	~Matrix() {
		if (data) {
			allocator.deallocate(data, Rows * Cols);
		}
	}

	Matrix<T, Cols, Rows> transpose() const {
		Matrix<T, Cols, Rows> result;

		if constexpr (Cols == 4 && Rows == 4) {

			__m128 row0 = _mm_load_ps(&data[0]);
			__m128 row1 = _mm_load_ps(&data[4]);
			__m128 row2 = _mm_load_ps(&data[8]);
			__m128 row3 = _mm_load_ps(&data[12]);

			_MM_TRANSPOSE4_PS(row0, row1, row2, row3);

			Matrix<T, Cols, Rows> temp = { row0, row1, row2, row3 };

			return temp;

		}
		else {
			for (size_t i = 0; i < Rows; ++i) {
				for (size_t j = 0; j < Cols; ++j) {
					result[j][i] = (*this)[i][j];
				}
			}
		}
		

		return result;
	}


	T& operator()(size_t r, size_t c) {
		assert(r < Rows && "row out of bounds");
		assert(c < Cols && "col out of bounds");
		return data[r * Cols + c];
	}

	const T& operator()(size_t r, size_t c) const {
		assert(r < Rows && "row out of bounds");
		assert(c < Cols && "col out of bounds");
		return data[r * Cols + c];
	}

	T* operator[](size_t row) {
		assert(row < Rows && "row out of bounds");
		return &data[row * Cols];
	}

	const T* operator[](size_t row) const {
		assert(row < Rows && "row out of bounds");
		return &data[row * Cols];
	}


	template<size_t otherCols>
	Matrix<float, Rows, otherCols> operator*(const Matrix<float, Cols, otherCols>& other) const { //only works with float

		Matrix<float, Rows, otherCols> result;

		result.zero();

		static float buffer1[4];
		static float buffer2[4];
		static float buffer3[4];


		for (size_t i = 0; i < Rows; ++i) {
			for (size_t k = 0; k < Cols; ++k) {

				__m128 valA = _mm_set1_ps((*this)[i][k]); //val A is a single value

				size_t j = 0;

				for (; j + 4 <= otherCols; j += 4) {

					__m128 currentResult = _mm_load_ps(&result[i][j]); //load the results, starting with element [i][j] 

					//_mm_store_ps(&buffer1[0], currentResult);

					//for (size_t l = 0; l < 4; ++l) {
						//std::cout << buffer1[l] << " ";
					//}
					//std::cout << "\n";

					__m128 rowB = _mm_load_ps(&other[k][j]); //loads the row starting with [k][j], with j being the first value and the next four memory values. 

					//_mm_store_ps(&buffer2[0], rowB);
					//for (size_t l = 0; l < 4; ++l) {
					//	std::cout << buffer2[l] << " ";
					//}
					//std::cout << "\n";

					__m128 updatedResult = _mm_fmadd_ps(valA, rowB, currentResult); //multipplies the row by the col and then adds to result

					//val A is the row of the 

					//_mm_store_ps(&buffer3[0], updatedResult);
					//for (size_t l = 0; l < 4; ++l) {
					//	std::cout << buffer2[l] << " ";
					//}
					//std::cout << "\n";

					_mm_store_ps(&result[i][j], updatedResult); //store back in result[i][j]	
				}

				for (; j < otherCols; ++j) {
					// Standard scalar math for the leftovers
					result[i][j] += (*this)[i][k] * other[k][j];
				}

			}
		}
		return result;
	}

	template<size_t otherCols>
	Vector<T, otherCols> operator*(const Vector<T, otherCols>& other) const {

		Vector<T, otherCols> result;
		result.zero();

		auto test = (*this);

		//std::cout << test;

		Matrix<T, Rows, Cols> temp = test.transpose();

		//std::cout << temp;
		
		//static float buffer1[4];
		//static float buffer2[4];
		//static float buffer3[4];
		//static float buffer4[4];

		for (size_t k = 0; k < Rows; ++k) {

			size_t i = 0;

			for (; i + 4 <= otherCols; i += 4) { //only runs once

				__m128 valA = _mm_set1_ps(other[k]);

				//_mm_store_ps(&buffer1[0], valA);

				//for (size_t l = 0; l < 4; ++l) {
				//	std::cout << buffer1[l] << " ";
				//}
				//std::cout << "\n";

				__m128 rowB = _mm_load_ps(&temp[k][i]); 

				//_mm_store_ps(&buffer2[0], rowB);

				//for (size_t l = 0; l < 4; ++l) {
				//	std::cout << buffer2[l] << " ";
				//}
				//std::cout << "\n";


				__m128 currentResults = _mm_load_ps(&result[i]);

				//_mm_store_ps(&buffer3[0], currentResults);

				//for (size_t l = 0; l < 4; ++l) {
				//	std::cout << buffer3[l] << " ";
				//}
				//std::cout << "\n";

				__m128 updatedResults = _mm_fmadd_ps(valA, rowB, currentResults);

				//_mm_store_ps(&buffer4[0], updatedResults);

				//for (size_t l = 0; l < 4; ++l) {
				//	std::cout << buffer4[l] << " ";
				//}
				//std::cout << "\n";

				_mm_store_ps(&result[i], updatedResults);

			}

			for (; i <= otherCols; ++i) {
				//scalr to do
			}
		}
		
		return result;
	}
};

template<typename T, size_t sizeOfRows, size_t sizeOfCols>
std::ostream& operator<<(std::ostream& os, const Matrix<T, sizeOfRows, sizeOfCols>& mat) {
	for (size_t i = 0; i < sizeOfRows; ++i) {
		for (size_t j = 0; j < sizeOfCols; ++j) {
			std::cout << std::fixed << std::setprecision(5);
			std::cout << mat[i][j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	return os;
}

template<typename T, size_t numOfCols>
std::ostream& operator<<(std::ostream& os, const Vector<T, numOfCols> vec) {
	for (size_t i = 0; i < numOfCols; ++i) {
		std::cout << vec[i] << "\n";
	}
	return os;
}
