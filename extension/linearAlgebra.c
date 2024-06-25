#include "linearAlgebra.h"
#include <math.h>
#include <string.h>

#define MATRIX_PROD(a, b, result, n) { \
	for (int i = 0; i < n; i++) { \
		for (int j = 0; j < n; j++) { \
			float sum = 0; \
			for (int k = 0; k < n; k++) { \
				sum += a.data[i][k] * b.data[k][j]; \
			} \
			result.data[i][j] = sum; \
		} \
	} \
}

Vec2f scalarVec2f(float scalar, Vec2f v) {
	Vec2f result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	return result;
}
Vec3f scalarVec3f(float scalar, Vec3f v) {
	Vec3f result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	return result;
}
Vec4f scalarVec4f(float scalar, Vec4f v) {
	Vec4f result;
	result.x = scalar * v.x;
	result.y = scalar * v.y;
	result.z = scalar * v.z;
	result.w = scalar * v.w;
	return result;
}

// matrix-matrix multiplication
Mat2x2f matMultiply2x2f(Mat2x2f a, Mat2x2f b) {
	Mat2x2f result;
	MATRIX_PROD(a, b, result, 2);
	return result;
}

Mat3x3f matMultiply3x3f(Mat3x3f a, Mat3x3f b) {
	Mat3x3f result;
	MATRIX_PROD(a, b, result, 3);
	return result;
}

Mat4x4f matMultiply4x4f(Mat4x4f a, Mat4x4f b) {
	Mat4x4f result;
	MATRIX_PROD(a, b, result, 4);
	return result;
}

// matrix-vector multiplication
Vec2f matVecMultiply2f(Mat2x2f m, Vec2f v) {
	Vec2f result;
	result.x = m.data[0][0] * v.x + m.data[0][1] * v.y;
	result.y = m.data[1][0] * v.x + m.data[1][1] * v.y;
	return result;
}

Vec3f matVecMultiply3f(Mat3x3f m, Vec3f v) {
	Vec3f result;
	result.x = m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z;
	result.y = m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z;
	result.z = m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z;
	return result;
}

Vec4f matVecMultiply4f(Mat4x4f m, Vec4f v) {
	Vec4f result;
	result.x = m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z + m.data[0][3] * v.w;
	result.y = m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z + m.data[1][3] * v.w;
	result.z = m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z + m.data[2][3] * v.w;
	result.w = m.data[3][0] * v.x + m.data[3][1] * v.y + m.data[3][2] * v.z + m.data[3][3] * v.w;
	return result;
}

// identity matrix
Mat2x2f matId2d(void) {
	Mat2x2f result;
	memset(result.data, 0, sizeof(result.data));

	for (int i = 0; i < 2; i++) {
		result.data[i][i] = 1.0;
	}

	return result;
}

Mat3x3f matId3d(void) {
	Mat3x3f result;
	memset(result.data, 0, sizeof(result.data));

	for (int i = 0; i < 3; i++) {
		result.data[i][i] = 1.0;
	}

	return result;
}

Mat4x4f matId4d(void) {
	Mat4x4f result;
	memset(result.data, 0, sizeof(result.data));

	for (int i = 0; i < 4; i++) {
		result.data[i][i] = 1.0;
	}

	return result;
}

// enlargement matrix
Mat2x2f matEnlarge2d2x2f(float xScale, float yScale) {
	Mat2x2f result;
	memset(result.data, 0, sizeof(result.data));

	result.data[0][0] = xScale;
	result.data[1][1] = yScale;
	return result;
}

Mat3x3f matEnlarge2d3x3f(float xScale, float yScale) {
	Mat3x3f result;
	memset(result.data, 0, sizeof(result.data));

	result.data[0][0] = xScale;
	result.data[1][1] = yScale;
	result.data[2][2] = 1.0;
	return result;
}

Mat3x3f matEnlarge3d3x3f(float xScale, float yScale, float zScale) {
	Mat3x3f result;
	memset(result.data, 0, sizeof(result.data));

	result.data[0][0] = xScale;
	result.data[1][1] = yScale;
	result.data[2][2] = zScale;
	return result;
}

Mat4x4f matEnlarge3d4x4f(float xScale, float yScale, float zScale, float wScale) {
	Mat4x4f result;
	memset(result.data, 0, sizeof(result.data));

	result.data[0][0] = xScale;
	result.data[1][1] = yScale;
	result.data[2][2] = zScale;
	result.data[3][3] = wScale;
	return result;
}

// rotation matrix
Mat2x2f mat2dRotation2x2f(float angle) {
	Mat2x2f result;
	memset(result.data, 0, sizeof(result.data));

	result.data[0][0] = cos(angle);
	result.data[0][1] = -sin(angle);
	result.data[1][0] = sin(angle);
	result.data[1][1] = cos(angle);
	return result;
}

Mat3x3f mat2dRotation3x3f(float angle) {
	Mat3x3f result = matId3d();
	result.data[0][0] = cos(angle);
	result.data[0][1] = -sin(angle);
	result.data[1][0] = sin(angle);
	result.data[1][1] = cos(angle);
	return result;
}

Mat3x3f mat3dRotationX3x3f(float angle) {
	Mat3x3f result = matId3d();
	result.data[1][1] = cos(angle);
	result.data[1][2] = -sin(angle);
	result.data[2][1] = sin(angle);
	result.data[2][2] = cos(angle);
	return result;
}

Mat3x3f mat3dRotationY3x3f(float angle) {
	Mat3x3f result = matId3d();
	result.data[0][0] = cos(angle);
	result.data[2][0] = -sin(angle);
	result.data[0][2] = sin(angle);
	result.data[2][2] = cos(angle);
	return result;
}

Mat3x3f mat3dRotationZ3x3f(float angle) {
	Mat3x3f result = matId3d();
	result.data[0][0] = cos(angle);
	result.data[0][1] = -sin(angle);
	result.data[1][1] = sin(angle);
	result.data[1][2] = cos(angle);
	return result;
}

Mat4x4f mat3dRotationX4x4f(float angle) {
	Mat4x4f result = matId4d();
	result.data[1][1] = cos(angle);
	result.data[1][2] = -sin(angle);
	result.data[2][1] = sin(angle);
	result.data[2][2] = cos(angle);
	return result;
}

Mat4x4f mat3dRotationY4x4f(float angle) {
	Mat4x4f result = matId4d();
	result.data[0][0] = cos(angle);
	result.data[2][0] = -sin(angle);
	result.data[0][2] = sin(angle);
	result.data[2][2] = cos(angle);
	return result;
}

Mat4x4f mat3dRotationZ4x4f(float angle) {
	Mat4x4f result = matId4d();
	result.data[0][0] = cos(angle);
	result.data[0][1] = -sin(angle);
	result.data[1][0] = sin(angle);
	result.data[1][1] = cos(angle);
	return result;
}

// translation matrix
Mat3x3f mat2dTranslation3x3f(float x, float y) {
	Mat3x3f result = matId3d();
	result.data[0][2] = x;
	result.data[1][2] = y;
	return result;
}

Mat3x3f mat3dTranslation3x3f(float x, float y, float z) {
	Mat3x3f result = matId3d();
	result.data[0][2] = x;
	result.data[1][2] = y;
	result.data[2][2] = z;
	return result;
}

Mat4x4f mat3dTranslation4x4f(float x, float y, float z) {
	Mat4x4f result = matId4d();
	result.data[0][3] = x;
	result.data[1][3] = y;
	result.data[2][3] = z;
	result.data[3][3] = 1.0;
	return result;
}

// transpose matrix
Mat2x2f matTranspose2x2f(Mat2x2f mat) {
	Mat2x2f result;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			result.data[i][j] = mat.data[j][i];
		}
	}

	return result;
}

Mat3x3f matTranspose3x3f(Mat3x3f mat) {
	Mat3x3f result;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			result.data[i][j] = mat.data[j][i];
		}
	}

	return result;
}

Mat4x4f matTranspose4x4f(Mat4x4f mat) {
	Mat4x4f result;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.data[i][j] = mat.data[j][i];
		}
	}

	return result;
}

// screenProjection 2d

// create a matrix which projects 3d coordinate
Mat3x3f pixelCoordToNDC2d3x3f(uint32_t width, uint32_t height) {
	// pixel space / world space has dimensions 800x600
	// NDC has space 2x2 (-1 to 1 along each axis)
	// need to shrink by 400 on the x axis
	// and 300 on the y axis
	Mat3x3f result = matEnlarge3d3x3f(1.0 / width, 1.0 / height, 0.0);
	result = matMultiply3x3f(mat3dTranslation3x3f(-1.0, -1.0, 0.0), result);
	return result;
}

Mat4x4f pixelCoordToNDC2d4x4f(uint32_t width, uint32_t height) {
	Mat4x4f result = matEnlarge3d4x4f(2.0 / width, 2.0 / height, 1.0, 1.0);
	result = matMultiply4x4f(mat3dTranslation4x4f(-1.0, -1.0, 0.0), result);
	return result;
}
