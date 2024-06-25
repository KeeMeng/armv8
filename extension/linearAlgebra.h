#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include <stdint.h>

// vectors can be accessed using their
// components as x,y,z,w values or as
// elements of a float array
// c stands for component
// a stands for array
typedef struct {
	float x;
	float y;
} Vec2f;

typedef struct {
	float x;
	float y;
	float z;
} Vec3f;

typedef struct {
	float x;
	float y;
	float z;
	float w;
} Vec4f;

typedef struct {
	float data[2][2];
} Mat2x2f;

typedef struct {
	float data[3][3];
} Mat3x3f;

typedef struct {
	float data[4][4];
} Mat4x4f;

// vector scalar multiplication
Vec2f scalarVec2f(float scalar, Vec2f v);
Vec3f scalarVec3f(float scalar, Vec3f v);
Vec4f scalarVec4f(float scalar, Vec4f v);

// matrix multiplication libraries
Mat2x2f matMultiply2x2f(Mat2x2f a, Mat2x2f b);
Mat3x3f matMultiply3x3f(Mat3x3f a, Mat3x3f b);
Mat4x4f matMultiply4x4f(Mat4x4f a, Mat4x4f b);

// vector multiplicaiton functions
Vec2f matVecMultiply2f(Mat2x2f m, Vec2f v);
Vec3f matVecMultiply3f(Mat3x3f m, Vec3f v);
Vec4f matVecMultiply4f(Mat4x4f m, Vec4f v);

// identity matrix
Mat2x2f matId2d(void);
Mat3x3f matId3d(void);
Mat4x4f matId4d(void);

// enlargement matrix
Mat2x2f matEnlarge2d2x2f(float xScale, float yScale);
Mat3x3f matEnlarge2d3x3f(float xScale, float yScale);
Mat3x3f matEnlarge3d3x3f(float xScale, float yScale, float zScale);
Mat4x4f matEnlarge3d4x4f(float xScale, float yScale, float zScale, float wScale);

// rotation matrix
Mat2x2f mat2dRotation2x2f(float angle);
Mat3x3f mat2dRotation3x3f(float angle);
Mat3x3f mat3dRotationX3x3f(float angle);
Mat3x3f mat3dRotationY3x3f(float angle);
Mat3x3f mat3dRotationZ3x3f(float angle);
Mat4x4f mat3dRotationX4x4f(float angle);
Mat4x4f mat3dRotationY4x4f(float angle);
Mat4x4f mat3dRotationZ4x4f(float angle);

// translation matrix
Mat3x3f mat2dTranslation3x3f(float x, float y);
Mat3x3f mat3dTranslation3x3f(float x, float y, float z);
Mat4x4f mat3dTranslation4x4f(float x, float y, float z);

// transpose matrix
Mat2x2f matTranspose2x2f(Mat2x2f mat);
Mat3x3f matTranspose3x3f(Mat3x3f mat);
Mat4x4f matTranspose4x4f(Mat4x4f mat);

// screenProjection2d
// Mat2x2f pixelCoordToNDC2d(uint32_t width, uint32_t height);
Mat3x3f pixelCoordToNDC2d3x3f(uint32_t width, uint32_t height);
Mat4x4f pixelCoordToNDC2d4x4f(uint32_t width, uint32_t height);

#endif
