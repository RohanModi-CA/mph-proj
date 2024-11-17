#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>

#ifndef MATRIX_OPERATIONS_H
#define MATRIX_OPERATIONS_H

float* matrixVectorMultiply(float mat1[][4], float mat2[][1]);
float* matrixMultiply(float mat1[][4], float mat2[][4]);
float* newMatrixMultiply(float mat1[][3], float mat2[][3]);
float* getLorentzMatrix(const float* velocity);
float** getRotMatrices(const float* velocity);
float* getFinalMatrix(float* velocity);

float* transformation(const float* inputArrays, const float* velocity);

#endif
