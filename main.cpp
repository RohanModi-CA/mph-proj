#include <iostream>
#include <vector>
#include "matrix_operations.h" // Include header for matrix operations
#include <iostream>
#include "matrix_operations.h"

int main() {
    // Example input array and velocity vector
    float inputArray[] = {1, 5, 2, 1, 1, 5, 2, 1}; // Example input
    float velocity[] = {0.2, 0.1, 0.5};  // Example velocity vector

    // Call the transformation function
    int size_of_input_array = sizeof(inputArray) / sizeof(inputArray[0]);
    float* result = transformation(inputArray, velocity, size_of_input_array);

    // Check if the result is not null
    if (result != nullptr) {
        std::cout << "Transformation Result:" << std::endl;

        // Print the resulting matrix (assuming it's a 4x4 matrix)
        for (int i = 0; i < 8; i++) {
            std::cout << result[i] << " ";
            if ((i + 1) % 4 == 0) std::cout << std::endl;  // New line after every 4 elements (for 4x4 matrix)
        }

        // Free the allocated memory
        free(result);
    } else {
        std::cout << "Transformation failed. Null result." << std::endl;
    }

    return 0;
}
