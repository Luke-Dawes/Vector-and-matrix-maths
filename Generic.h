#pragma once
#include <array>
#include <initializer_list>
#include <algorithm>
#include <iostream>
#include <iomanip>

//dot and cross product, normals

template <typename T, size_t rows, size_t cols>
class Matrix {
	std::array<T, rows * cols> data{};


public:
	T* operator[](size_t row) {
		return &data[row * cols];
	}

	const T* operator[](size_t row) const {
		return &data[row * cols];
	}

	Matrix() = default;

	Matrix(std::initializer_list<std::initializer_list<T>> list) {
		size_t r = 0;
		for (const auto& row_list : list) {
			if (r < rows) {
				size_t c_count = std::min(row_list.size(), cols);
				std::copy(row_list.begin(), row_list.begin() + c_count, &data[r * cols]);
				r++;
			}
		}
	}

	template<size_t otherRows, size_t otherCols>
	Matrix<T, rows, otherCols> operator*(const Matrix<T, otherRows, otherCols>& other) const {

		static_assert(cols == otherRows, "matrix cant be multipled");

		Matrix<T, rows, otherCols> result;

		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < otherCols; ++j) {
				T sum = 0;
				for (size_t k = 0; k < cols; ++k) {
					sum += (*this)[i][k] * other[k][j];
				}
				result[i][j] = sum;
			}
		}
		return result;
	}

	Matrix<T, rows, cols> operator*(const T& value) const {
		Matrix<T, rows, cols> result;

		for (size_t i = 0; i < rows * cols; ++i) {
			result.data[i] = data[i] * value;
		}
		return result;
	}
	
	T determinate() const {
		static_assert(rows == cols, "determinate requires squre");
		static_assert(rows <= 3, "not implemented");
		return calculateDeterminate(*this);
	}

	Matrix<T, cols, rows> transpose() const {
		Matrix<T, cols, rows> result;

		for (size_t i = 0; i < rows; ++i) {
			for (size_t j = 0; j < cols; ++j) {
				result[j][i] = (*this)[i][j];
			}
		}
		return result;
	}

	Matrix<T, cols, rows> invert() const {
		static_assert(rows == cols, "inverse requires square");
		static_assert(rows <= 3, "not implemented");
		return calculateInvertMatrix(*this);
	}


protected:
	template<size_t N>
	T calculateDeterminate(const Matrix<T, N, N>& mat) const {

		if constexpr (N == 1) {
			return mat[0][0];
		}
		if constexpr (N == 2) {
			return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0]; //[0][1] is rop right, [1][0] is bottom left
		}
		if constexpr (N == 3) {
			
			T term1 = mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1]);

			T term2 = mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0]);

			T term3 = mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);

			return term1 - term2 + term3;

		}
	}

	template<size_t N>
	Matrix<T, N, N> calculateInvertMatrix(const Matrix<T, N, N> mat) const {

		if constexpr (N == 1) {
			return Matrix<T, 1, 1>{{ static_cast<T>(1) / mat[0][0] }};
			
		}
		if constexpr (N == 2) {
			T det = calculateDeterminate(mat);
			Matrix<T, N, N> result = {
				{mat[1][1], -mat[0][1]},
				{-mat[1][0], mat[0][0]}
			};
			return result * (1.f/ det);
		}
		if constexpr (N == 3) {
			T det = calculateDeterminate(mat);

			T a = mat[0][0];
			T b = mat[0][1];
			T c = mat[0][2];
			T d = mat[1][0];
			T e = mat[1][1];
			T f = mat[1][2];
			T g = mat[2][0];
			T h = mat[2][1];
			T i = mat[2][2];

			Matrix<T, 3, 3> result;

			result[0][0] = (e * i - f * h);
			result[0][1] = -(b * i - c * h);
			result[0][2] = (b * f - c * e);

			// Row 1 of Inverse (Cofactors of Col 1)
			result[1][0] = -(d * i - f * g);
			result[1][1] = (a * i - c * g);
			result[1][2] = -(a * f - c * d);

			// Row 2 of Inverse (Cofactors of Col 2)
			result[2][0] = (d * h - e * g);
			result[2][1] = -(a * h - b * g);
			result[2][2] = (a * e - b * d);

			

			return result * (static_cast<T>(1) / det);
		}
	}
};


template<typename T, size_t cols>
class Vec {
	std::array<T, cols> data{};

public:
	T& operator[](size_t num) {
		return data[num];
	}

	const T& operator[](size_t num) const {
		return data[num];
	}

	Vec() = default;

	Vec(std::initializer_list<T> list) {
		if (list.size() != cols) std::cout << "error on size of vec";
		size_t count = std::min(list.size(), cols);
		std::copy(list.begin(), list.begin() + count, data.begin());
	}

	template<size_t otherCols, size_t otherRows>
	Vec<T, otherCols> operator*(const Matrix<T, otherRows, otherCols> mat) const {
		static_assert(cols == otherRows, "matrix x vector cant happen - rows are uneven");

		Vec<T, otherCols> result;

		for (size_t i = 0; i < otherCols; ++i) {
			T temp = 0;
			for (size_t j = 0; j < cols; ++j) {
				temp += data[j] * mat[j][i] ;
			}
			result.data[i] = temp;
		}
		return result;

	}

	int length() {
		return data.size();
	}

		//technically dont exist
	Vec<T, cols> operator-(const T value) const {
		Vec<T, cols> result;
		for (size_t i = 0; i < cols; ++i) {
			result.data[i] = data[i] - value;
		}
		return result;
	}

	Vec<T, cols> operator+(const T value) const {
		Vec<T, cols> result;
		for (size_t i = 0; i < cols; ++i) {
			result.data[i] = data[i] + value;
		}
		return result;
	}
	
	template<size_t otherCols>
	Vec<T, cols> operator+(const Vec<T, otherCols> otherVec) {
		static_assert(otherCols == cols, "trying to add vectors which arent the same");

		Vec<T, cols> result; 
		for (size_t i = 0; i < data.size(); ++i) {
			result.data[i] = data[i] + otherVec.data[i];
		}
		return result;
	}

	template<size_t otherCols>
	Vec<T, cols> operator-(const Vec<T, otherCols> otherVec) {
		static_assert(otherCols == cols, "trying to add vectors which arent the same");

		Vec<T, cols> result;
		for (size_t i = 0; i < data.size(); ++i) {
			result.data[i] = data[i] - otherVec.data[i];
		}
		return result;
	}

	Vec<T, cols> operator*(const T value) const {
		Vec<T, cols> result;
		for (size_t i = 0; i < cols; ++i) {
			result.data[i] = data[i] * value;
		}
		return result;
	}

	Vec<T, cols> operator/(const T value) const {
		Vec<T, cols> result;
		for (size_t i = 0; i < cols; ++i) {
			result.data[i] = data[i] / value;
		}
		return result;
	}

	
	T magnitude() const {
		T temp = 0;
		for (size_t i = 0; i < cols; ++i) {
			temp += data[i] * data[i];
		}
		return std::sqrt(temp);
	}

	Vec<T, cols> normalise() const {
		T norm = magnitude();
		return (*this) / norm;
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
std::ostream& operator<<(std::ostream& os, const Vec<T, numOfCols> vec) {
	for (size_t i = 0; i < numOfCols; ++i) {
		std::cout << vec[i] << "\n";
	}
	return os;
}

template<typename T, size_t Cols>
T dotProduct(const Vec<T, Cols>& vec1, const Vec<T, Cols>& vec2) {

	T sum = 0;
	for (size_t i = 0; i < Cols; ++i) {
		sum = sum + (vec1[i] * vec2[i]);
	}
	return sum;
}

template<typename T, size_t Cols>
Vec< T, 3> cross(const Vec<T, 3>& vec1, const Vec<T, 3>& vec2) {
	static_assert(Cols == 3, "cant be done unless in 3d");

	Vec<T, 3> result;
	result[0] = vec1[1] * vec2[2] - vec1[2] * vec2[1];
	result[1] = vec1[2] * vec2[0] - vec1[0] * vec2[2];
	result[2] = vec1[0] * vec2[1] - vec1[1] * vec2[0];

	return result;
}

template<typename T, size_t N> //has to be square
Vec<T, N> solve(const Matrix<T, N, N>& mat, const Vec<T, N>& vec) {
	return mat.invert() * vec; //has to have a max size of 3
}