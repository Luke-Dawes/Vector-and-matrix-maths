#pragma once
#include <iostream>

#define startNamespace namespace Math { 
#define endNamespace }
#define a2x2 [0][0]
#define b2x2 [0][1]
#define c2x2 [1][0]
#define d2x2 [1][1]
#define matrix1 m
#define matrix2 other.m

startNamespace

	struct Vector2 {

	float x;
	float y;


	public:
		Vector2(float x, float y)
			: x(x), y(y)
		{}

		Vector2 operator+(const Vector2& other) {
			return Vector2(this->x + other.x, this->y + other.y);
		}

		Vector2 operator*(const float value) {
			return Vector2(this->x * value, this->y * value);
		}


	};
	
struct Vector3 {
	float x;
	float y; 
	float z;

	Vector3(float x, float y, float z)
		: x(x), y(y), z(z) {}

	
	Vector3 operator+(const Vector3& other) {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 operator*(const float value) {
		return Vector3(x * value, y * value, z * value);
	}

	};


struct Mat2x2 {
	float m[2][2];

	Mat2x2(Math::Vector2 row1, Math::Vector2 row2) {
		m[0][0] = row1.x; m[0][1] = row1.y;
		m[1][0] = row2.x; m[1][1] = row2.y;
	}

	Vector2 operator*(const Vector2& other) const {
		float newX = (m[0][0] * other.x) + (m[0][1] * other.y);
		float newY = (m[1][0] * other.x) + (m[1][1] * other.y);
		return Vector2(newX, newY);
	}

	/*
		m[0][0] m[0][1]
		m[1][0] m[1][1]
	
	*/


	Mat2x2 operator*(const Mat2x2& other) const {
		float newX1 = (matrix1 a2x2 * matrix2 a2x2) + (matrix1 b2x2 * matrix2 c2x2);
		float newX2 = (matrix1 a2x2 * matrix2 b2x2) + (matrix1 b2x2 * matrix2 d2x2);
		float newY1 = (matrix1 c2x2 * matrix2 a2x2) + (matrix1 d2x2 * matrix2 c2x2);
		float newY2 = (matrix1 c2x2 * matrix2 b2x2) + (matrix1 d2x2 * matrix2 d2x2);
		return Mat2x2(
			Vector2(newX1, newX2), 
			Vector2(newY1, newY2));
		
	}

	Mat2x2 operator*(const float value) const {
		return Mat2x2(
			Vector2(matrix1 a2x2 * value, matrix1 b2x2 * value), 
			Vector2(matrix1 c2x2 * value, matrix1 d2x2 * value));
	}	

	float determinant() const {
		return (matrix1 a2x2 * matrix1 d2x2) - (matrix1 b2x2 * matrix1 c2x2);
	}


	Mat2x2 invert() const {
		float det = this->determinant();

		if (det == 0) {
			return Mat2x2(
				Vector2(0, 0),
				Vector2(0, 0)
			);
		}

		float a = matrix1 a2x2;
		float b = matrix1 b2x2;
		float c = matrix1 c2x2;
		float d = matrix1 d2x2;

		Mat2x2 newMat(
			Vector2(d, -b), 
			Vector2(-c, a));

		return newMat * (1.f / det);

	}

};




endNamespace


std::ostream& operator<<(std::ostream& os, const Math::Vector2& vec) {
	os << "(" << vec.x << ", " << vec.y << ")";
	return os;
}
std::ostream& operator<<(std::ostream& os, const Math::Vector3& vec) {
	os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	return os;
}
std::ostream& operator<<(std::ostream& os, const Math::Mat2x2& vec) {
	os << "(" << vec.m[0][0] << ", " << vec.m[0][1] << ")\n(" << vec.m[1][0] << ", " << vec.m[1][1] << ")";
	return os;
}
