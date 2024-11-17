#include <iostream>
#include <vector>
#include "matrix_operations.h" // Include your header file for matrix operations

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

int main() {
    // Example 1D array input
    float inputArray[] = {1, 5, 2, 1, 1, 5, 2, 1};
    int cols = 4; // Fixed column size
    int size = sizeof(inputArray) / sizeof(inputArray[0]);
    int rows = size / cols;

    // Reshape into 2D array
    std::vector<std::vector<float> > reshapedArray = reshapeTo2D(inputArray, rows, cols);

    // Example velocity vector
    float velocity[3] = {0.2, 0.1, 0.5};

    // Get the final transformation matrix
    float* finalMatrix = getFinalMatrix(velocity);
    if (finalMatrix == nullptr) {
        std::cerr << "Error: Failed to get the final transformation matrix." << std::endl;
        return -1;
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
            return -1;
        }

        // Append results to the final 1D array
        for (int j = 0; j < 4; ++j) {
            finalResults.push_back(rowResult[j]);
        }

        // Free the memory allocated for the row result
        free(rowResult);
    }

    // Free the memory allocated for the final matrix
    free(finalMatrix);

    // Print the final 1D array
    std::cout << "Final 1D Array:" << std::endl;
    for (size_t i = 0; i < finalResults.size(); ++i) {
        std::cout << finalResults[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
