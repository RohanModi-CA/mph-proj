#include "matrix_operations.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
# include <vector>


// Matrix-Vector Multiplication
float* matrixVectorMultiply(float mat1[][4], float mat2[][1]) {
    const int R1 = 4, C1 = 4, R2 = 4, C2 = 1;
    if (C1 != R2) {
        return nullptr;
    }

    float *result = (float*)malloc(R1 * C2 * sizeof(float));
    if (result == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < R1; i++) {
        result[i] = 0; // Initialize the result
        for (int k = 0; k < C1; k++) {
            result[i] += mat1[i][k] * mat2[k][0];
        }
    }

    return result;
}

// Matrix-Matrix Multiplication
float* matrixMultiply(float mat1[][4], float mat2[][4]) {
    const int R1 = 4, C1 = 4, R2 = 4, C2 = 4;
    if (C1 != R2) {
        return nullptr;
    }

    float *result = (float*)malloc(R1 * C2 * sizeof(float));
    if (result == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            result[i * C2 + j] = 0;
            for (int k = 0; k < C1; k++) {
                result[i * C2 + j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return result;
}

// 3x3 Matrix Multiplication
float* newMatrixMultiply(float mat1[][3], float mat2[][3]) {
    const int R1 = 3, C1 = 3, R2 = 3, C2 = 3;
    if (C1 != R2) {
        return nullptr; // Incompatible matrix sizes
    }

    // Allocate memory for the result (3x3 matrix)
    float *result = (float*)malloc(R1 * C2 * sizeof(float));
    if (result == nullptr) {
        return nullptr; // Memory allocation failed
    }

    // Perform matrix multiplication
    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            result[i * C2 + j] = 0; // Initialize the result entry
            for (int k = 0; k < C1; k++) {
                result[i * C2 + j] += mat1[i][k] * mat2[k][j];
            }
        }
    }

    return result;
}


// Lorentz Matrix
float* getLorentzMatrix(const float* velocity) {
    float beta = std::sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]);

    if (beta >= 1) {
      std::cout << "is this it? Error in getLorentzMatrix: velocity[0] = " << velocity[0] << std::endl;
        return nullptr;
    }

    float gamma = 1.0f / std::sqrt(1.0f - beta * beta);

    float* lorentzMatrix = (float*)malloc(16 * sizeof(float));
    if (lorentzMatrix == nullptr) {
        return nullptr;
    }

    for (int i = 0; i < 16; i++) {
        lorentzMatrix[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }

    lorentzMatrix[0] = gamma;
    lorentzMatrix[1] = -gamma * beta;
    lorentzMatrix[4] = -gamma * beta;
    lorentzMatrix[5] = gamma;



    return lorentzMatrix;
}




// Rotation Matrices
float** getRotMatrices(const float* velocity) {
    float** matrices = (float**)malloc(2 * sizeof(float*));
    matrices[0] = (float*)malloc(16 * sizeof(float)); // Rotation matrix
    matrices[1] = (float*)malloc(16 * sizeof(float)); // Inverse rotation matrix
    // Ensure the velocity vector is normalized (magnitude == 1)
    float magnitude = std::sqrt(velocity[0] * velocity[0] + velocity[1] * velocity[1] + velocity[2] * velocity[2]);
    if (magnitude == 0) {
        matrices[0][0] = 1.0f; matrices[0][1] = 0.0f; matrices[0][2] = 0.0f; matrices[0][3] = 0.0f;
        matrices[0][4] = 0.0f; matrices[0][5] = 1.0f; matrices[0][6] = 0.0f; matrices[0][7] = 0.0f;
        matrices[0][8] = 0.0f; matrices[0][9] = 0.0f; matrices[0][10] = 1.0f; matrices[0][11] = 0.0f;
        matrices[0][12] = 0.0f; matrices[0][13] = 0.0f; matrices[0][14] = 0.0f; matrices[0][15] = 1.0f;
        for (int i = 0; i < 16; i++) {
            matrices[1][i] = matrices[0][i];
        }
        return matrices; // Invalid input: zero vector
    }

    // Normalize the velocity vector
    float v_x = velocity[0] / magnitude;
    float v_y = velocity[1] / magnitude;
    float v_z = velocity[2] / magnitude;

    // Axis of rotation: perpendicular to the velocity (cross product with x-axis)
    float axis_x = 0;
    float axis_y = v_z;
    float axis_z = -v_y;

    // Normalize the axis vector
    float axis_magnitude = std::sqrt(axis_x * axis_x + axis_y * axis_y + axis_z * axis_z);
    axis_x /= axis_magnitude;
    axis_y /= axis_magnitude;
    axis_z /= axis_magnitude;

    // Angle of rotation (theta) between the x-axis and the velocity

    float cos_theta = v_x; // since the x-axis is (1, 0, 0)
    if (cos_theta == 1.0f) {
        matrices[0][0] = 1.0f; matrices[0][1] = 0.0f; matrices[0][2] = 0.0f; matrices[0][3] = 0.0f;
        matrices[0][4] = 0.0f; matrices[0][5] = 1.0f; matrices[0][6] = 0.0f; matrices[0][7] = 0.0f;
        matrices[0][8] = 0.0f; matrices[0][9] = 0.0f; matrices[0][10] = 1.0f; matrices[0][11] = 0.0f;
        matrices[0][12] = 0.0f; matrices[0][13] = 0.0f; matrices[0][14] = 0.0f; matrices[0][15] = 1.0f;
        for (int i = 0; i < 16; i++) {
            matrices[1][i] = matrices[0][i];
        }
        return matrices;
    }

    if (cos_theta == -1.0f) {
        matrices[0][0] = 1.0f; matrices[0][1] = 0.0f; matrices[0][2] = 0.0f; matrices[0][3] = 0.0f;
        matrices[0][4] = 0.0f; matrices[0][5] = -1.0f; matrices[0][6] = 0.0f; matrices[0][7] = 0.0f;
        matrices[0][8] = 0.0f; matrices[0][9] = 0.0f; matrices[0][10] = 1.0f; matrices[0][11] = 0.0f;
        matrices[0][12] = 0.0f; matrices[0][13] = 0.0f; matrices[0][14] = 0.0f; matrices[0][15] = 1.0f;
        for (int i = 0; i < 16; i++) {
            matrices[1][i] = matrices[0][i];
        }
        return matrices;
    }


    float sin_theta = std::sqrt(1 - cos_theta * cos_theta);

    // Cross product matrix (K)
    float K[3][3] = {
        {0, -axis_z, axis_y},
        {axis_z, 0, -axis_x},
        {-axis_y, axis_x, 0}
    };

    // K^2 (K matrix multiplied by itself)
    float* K_squared = newMatrixMultiply(K, K);

    // Rotation matrix R (using Rodrigues' formula)
    float R[3][3] = {0};

    // Identity matrix contribution
    for (int i = 0; i < 3; i++) {
        R[i][i] = 1.0f;
    }

    // Adding sin(theta) * K
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] += sin_theta * K[i][j];
        }
    }

    // Adding (1 - cos(theta)) * K^2
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            R[i][j] += (1 - cos_theta) * K_squared[i * 3 + j];
        }
    }

    // Now, we need to convert R (a 3x3 matrix) into a 4x4 matrix for homogeneous coordinates
    matrices[0] = (float*)malloc(16 * sizeof(float)); // Rotation matrix
    matrices[1] = (float*)malloc(16 * sizeof(float)); // Inverse rotation matrix

    // Initialize as identity matrices
    for (int i = 0; i < 16; i++) {
        matrices[0][i] = matrices[1][i] = (i % 5 == 0) ? 1.0f : 0.0f;
    }

    // Fill in the rotation matrix (top-left 3x3 block)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            matrices[0][(i + 1) * 4 + (j + 1)] = R[i][j];
            matrices[1][(i + 1) * 4 + (j + 1)] = R[j][i];  // Inverse rotation (transpose)
        }
    }

    return matrices;
}

// New function to get the resulting matrix A_f
float* getFinalMatrix(float* velocity) {
    // Get the rotation matrices
    float** matrices = getRotMatrices(velocity);
    if (matrices == nullptr) {
      std::cout << "Error in getFinalMatrix()" << std::endl;
        return nullptr; // Handle the case when rotation matrices are invalid
    }

    // Get Lorentz matrix
    float* lorentzMatrix = getLorentzMatrix(velocity);
    if (lorentzMatrix == nullptr) {
        free(matrices[0]);
        free(matrices[1]);
        free(matrices);
        std::cout << "hiError in getFinalMatrix()" << std::endl;
        return nullptr; // Handle the case when Lorentz matrix is invalid
    }

    // Perform the multiplications: A_f = R * A_o * R^T
    // First, multiply lorentzMatrix * matrices[0] (A_o * R)
    float* intermediate = matrixMultiply(reinterpret_cast<float(*)[4]>(lorentzMatrix), reinterpret_cast<float(*)[4]>(matrices[0]));
    if (intermediate == nullptr) {
        free(lorentzMatrix);
        free(matrices[0]);
        free(matrices[1]);
        free(matrices);
        std::cout << "byeError in getFinalMatrix()" << std::endl;
        return nullptr; // Handle multiplication failure
    }

    // Then, multiply matrices[1] * intermediate (R^T * (A_o * R))
    float* final = matrixMultiply(reinterpret_cast<float(*)[4]>(matrices[1]), reinterpret_cast<float(*)[4]>(intermediate));
    if (final == nullptr) {
        free(lorentzMatrix);
        free(matrices[0]);
        free(matrices[1]);
        free(matrices);
        free(intermediate);
        std::cout << "rohanError in getFinalMatrix()" << std::endl;
        return nullptr; // Handle multiplication failure
    }

    // Free memory for intermediate matrices
    free(lorentzMatrix);
    free(matrices[0]);
    free(matrices[1]);
    free(matrices);
    free(intermediate);

    return final; // Return the final matrix A_f
}

// Function to reshape a 1D array into a 2D vector
std::vector<std::vector<float> > reshapeTo2D(const float* inputArray, int rows, int cols) {
    std::vector<std::vector<float> > result(rows, std::vector<float>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[i][j] = inputArray[i * cols + j];
        }
    }
    return result;
}

// Transformation function
float* transformation(const float* inputArray, float* velocity, int size_of_input_array) {
    int cols = 4; // Fixed column size
    int rows = size_of_input_array / cols;
    //int size = sizeof(inputArray) / sizeof(inputArray[0]);

    // Reshape into 2D array
    std::vector<std::vector<float> > reshapedArray = reshapeTo2D(inputArray, rows, cols);

    // Get the final transformation matrix
    float* finalMatrix = getFinalMatrix(velocity);
    if (finalMatrix == nullptr) {
        std::cerr << "Error: Failed to get the final transformation matrix." << std::endl;
    }

    // 1D vector to store the results
    std::vector<float> finalResults;

    // Loop through each row of the reshaped array
    for (int i = 0; i < rows; ++i) {
        // Convert the current row to a column vector
        float rowVector[4] = {reshapedArray[i][0], reshapedArray[i][1], reshapedArray[i][2], reshapedArray[i][3]};
        float* rowResult = matrixVectorMultiply(reinterpret_cast<float(*)[4]>(finalMatrix), reinterpret_cast<float(*)[1]>(&rowVector));

        // Check for multiplication success
        if (rowResult == nullptr) {
            std::cerr << "Error: Failed to multiply matrix with row vector " << i << "." << std::endl;
            free(finalMatrix);
        }

        // Append results to the final 1D array
        for (int j = 0; j < 4; ++j) {
            finalResults.push_back(rowResult[j]);
        }

        // Free the memory allocated for the row result
        free(rowResult);
    }

    float* resultant_events = new float[finalResults.size()];
    std::copy(finalResults.begin(), finalResults.end(), resultant_events);
    return resultant_events;


}

