/*******************************************************************************************
*
*   raylib [core] example - 3d camera first person
*
*   Example originally created with raylib 1.3, last time updated with raylib 1.3
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2015-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <cmath>
#include "rcamera.h"
#include <cstdlib>
#include <cstring>
#include "matrix_operations.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdio.h>
#include <numeric> // for accumulate
#include <limits> // Required for numeric_limits
#define MAX_COLUMNS 1

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------

std::vector<float> addVectorsCyclically(const std::vector<float>& vector1, const std::vector<float>& vector2) {
  // Error handling: Check for valid input sizes
  if (vector1.size() % 3 != 0) {
    throw std::invalid_argument("vector1 size must be a multiple of 3");
  }
  if (vector2.size() != 3) {
    throw std::invalid_argument("vector2 size must be 3");
  }

  std::vector<float> result = vector1; // Create a copy to avoid modifying the original vector1

  for (size_t i = 0; i < result.size(); ++i) {
    result[i] += vector2[i % 3]; // Cyclically add elements from vector2
  }

  return result;
}

float* vectorToFloatPointer(std::vector<float> vec) {
    float* result = new float[vec.size()]; //Allocate new memory
    std::copy(vec.begin(), vec.end(), result); //Copy data
    return result;
}

std::vector<float> lookup_row(const std::vector<float>& sorted_array, const std::vector<std::vector<float>>& data_matrix, float target_value) {
    if (sorted_array.empty() || data_matrix.empty()) {
        return {}; //Handle empty inputs
    }

    int index = std::upper_bound(sorted_array.begin(), sorted_array.end(), target_value) - sorted_array.begin() -1;

    //Handle boundary conditions
    if (index < 0) index = 0;
    if (index >= sorted_array.size()) index = sorted_array.size() - 1;



    return data_matrix[index];
}

std::vector<float> shift_array(const std::vector<float>& input) {
    if (input.empty()) {
        return {}; // Handle empty input
    }

    //Find the minimum element.  Handle potential NaN values.
    float min_val = std::numeric_limits<float>::infinity();
    for(float val : input){
        if(val < min_val){
            min_val = val;
        }
    }

    float offset = 0.1f - min_val;
    std::vector<float> shifted_array(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        shifted_array[i] = input[i] + offset;
    }

    return shifted_array;
}


std::vector<float> average_start_times(const std::vector<std::vector<std::vector<float>>>& result) {
    int num_timesteps = result[0].size();
    int num_groups = result.size();
    std::vector<float> averages(num_timesteps);

    for (int t = 0; t < num_timesteps; ++t) {
        float sum = 0;
        for (int g = 0; g < num_groups; ++g) {
            sum +=  result[g][t][0];
        }
        averages[t] = sum / num_groups;
    }

    averages = shift_array(averages);
    return averages;
}
// Helper function to check if a vector is all zeros
bool isAllZeros(const std::vector<float>& vec) {
    return std::all_of(vec.begin(), vec.end(), [](float f){ return f == 0.0f; });
}

void removeZeroVectors(std::vector<std::vector<std::vector<float>>>& data) {
    for (auto& middle : data) {
        // Use erase with remove_if for efficient removal
        middle.erase(std::remove_if(middle.begin(), middle.end(), isAllZeros), middle.end());
    }

     // Optionally remove any now-empty middle vectors.  This is only needed if you
     // want to get rid of middle vectors that became empty *after* removing
     // the inner zero vectors.

    data.erase(std::remove_if(data.begin(), data.end(), [](const std::vector<std::vector<float>>& middle){ return middle.empty(); }), data.end());
}


// Function to process events and return the reorganized data
std::vector<std::vector<std::vector<float>>> processEvents(const std::vector<std::vector<float>>& events, int groups=9, int values_per_group=4)
{
    int events_size = events.size();
    if (events.size() != events_size) {
        std::cerr << "Error: Inconsistent events_size and events size." << std::endl;
        //Return an empty std::vector to indicate an error.  Could throw an exception for better error handling
        return {};
    }

    std::vector<std::vector<std::vector<float>>> new_vectors(groups);

    for (int i = 0; i < groups; ++i) {
        new_vectors[i].resize(events_size);
        for (int j = 0; j < events_size; ++j) {
            if (events[j].size() < (i + 1) * values_per_group) {
                std::cerr << "Error: Insufficient elements in events[" << j << "]." << std::endl;
                return {}; //Return an empty vector to indicate an error
            }
            new_vectors[i][j].resize(values_per_group);
            for (int k = 0; k < values_per_group; ++k) {
                new_vectors[i][j][k] = events[j][i * values_per_group + k];
            }
        }
    }



    for (int i = 0; i < groups; ++i) {
        sort(new_vectors[i].begin(), new_vectors[i].end(), [](const std::vector<float>& a, const std::vector<float>& b) {
            return a[0] < b[0];
        });
    }

    // Find the group with the smallest first entry
    int smallest_group_index = 0;
    float smallest_first_entry = new_vectors[0][0][0];

    for (size_t i = 1; i < new_vectors.size(); ++i) {
        if (new_vectors[i][0][0] < smallest_first_entry) {
            smallest_first_entry = new_vectors[i][0][0];
            smallest_group_index = i;
        }
    }

    std::cout << "The group with the smallest first entry (" << smallest_first_entry << ") is group " << smallest_group_index + 1 << std::endl;
    // Find the group with the largest first entry
    int largest_group_index = 0;
    float largest_first_entry = new_vectors[0][0][0];

    for (size_t i = 1; i < new_vectors.size(); ++i) {
        if (new_vectors[i][0][0] > largest_first_entry) {
            largest_first_entry = new_vectors[i][0][0];
            largest_group_index = i;
        }
    }

    std::cout << "The group with the largest first entry (" << largest_first_entry << ") is group " << largest_group_index + 1 << std::endl;

    // Find the group with the largest and smallest first entries of the last row
    size_t lastRowIndex = new_vectors[0].size() > 0 ? new_vectors[0].size() - 1 : 0; //Handle empty case

    if (new_vectors.empty() || new_vectors[0].empty()) {
        std::cout << "Error: new_vectors is empty" << std::endl;
    }

    size_t maxGroupIndex = 0;
    float maxLastEntry = new_vectors[0][lastRowIndex][0];
    size_t minGroupIndex = 0;
    float minLastEntry = new_vectors[0][lastRowIndex][0];

    for (size_t i = 1; i < new_vectors.size(); ++i) {
        if (new_vectors[i][lastRowIndex][0] > maxLastEntry) {
            maxLastEntry = new_vectors[i][lastRowIndex][0];
            maxGroupIndex = i;
        }
        if (new_vectors[i][lastRowIndex][0] < minLastEntry) {
            minLastEntry = new_vectors[i][lastRowIndex][0];
            minGroupIndex = i;
        }
    }

    std::cout << "The group with the largest last entry (" << maxLastEntry << ") is group " << maxGroupIndex + 1 << std::endl;
    std::cout << "The group with the smallest last entry (" << minLastEntry << ") is group " << minGroupIndex + 1 << std::endl;

    std::cout << std::min({maxLastEntry, largest_first_entry}) << std::endl;

    for (auto& data : new_vectors) {
        for (auto& row : data) { // Use a range-based for loop with a reference
            if (!row.empty() && row[0] < largest_first_entry) {
                row = {0.0f, 0.0f, 0.0f, 0.0f}; // Directly assign the new vector
                //std::cout << row[0] << " " << row[1] << " " << row[2] << " " << row[3] << std::endl;
            }
        }
    }

    for (auto& data : new_vectors) {
        for (auto& row : data) { // Use a range-based for loop with a reference
            if (!row.empty() && row[0] > minLastEntry) {
                row = {0.0f, 0.0f, 0.0f, 0.0f}; // Directly assign the new vector
                //std::cout << row[0] << " " << row[1] << " " << row[2] << " " << row[3] << std::endl;
            }
        }
    }

    removeZeroVectors(new_vectors);


    /*
    // assume ascending, so kill the smallest in the front
    for (int i=0; i < new_vectors.size(); ++i) {
        for (int j=0; j < new_vectors[i].size(); ++j) {
            if (new_vectors[i][j][0] < largest_first_entry) {
                std::cout <<  ""  << std::endl;
                std::cout << new_vectors[i][j][0] << std::endl;
                //new_vectors[i][j] = {0,0,0,0};
            }
        }
    }
    for (int i = new_vectors[0].size() - 1; i >= 0; --i) {
        for (int j = new_vectors[i][j].size() - 1; j >= 0; --j) {
            if (new_vectors[i][j][0] > minLastEntry) {
                std::cout << "d" << std::endl;
                std::cout << new_vectors[i][j][0] << std::endl;
                //new_vectors[i][j] = {0, 0, 0, 0};
            }
        }
    } */



    for(int i = 0; i < groups; ++i) {
        //new_vectors[i][0][0];
    }

    /*
    // 1. Print before sorting:
    std::cout << "Before Sorting:" << std::endl;
    for (int i = 0; i < groups; ++i) {
        std::cout << "Group " << i + 1 << ":" << std::endl;
        for (const auto& inner_vec : new_vectors[i]) {
            for (float val : inner_vec) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    */



    return new_vectors;

}

std::vector<std::vector<float>> process_to_points(const std::vector<std::vector<std::vector<float>>>& input) {
    int num_timesteps = input[0].size(); // Assumes all groups have the same number of time steps.
    int num_groups = input.size();
    //std::cout << "num_timesteps: " << num_groups << std::endl;
    int values_per_group = input[0][0].size();

    std::vector<std::vector<float>> points(num_timesteps);

    for (int t = 0; t < num_timesteps; ++t) {
        for (int g = 1; g < num_groups; ++g) {

            std::vector<float> neg_center_pos = {0,0,0};
            for(int i = 1; i < 4; ++i) {
                neg_center_pos[i-1] = -1 * input[0][t][i];
                //std::cout << neg_center_pos[i-1] << std::endl;
            }


            for (int i = 1; i < values_per_group; ++i) { // indices 1,2,3
                points[t].push_back(input[g][t][i] - neg_center_pos[i-1] );
            }
        }
    }

    return points;
}


std::vector<std::vector<float>> get_lorentz_center_pos(const std::vector<std::vector<std::vector<float>>>& input) {
    int num_timesteps = input[0].size(); // Assumes all groups have the same number of time steps.
    int num_groups = input.size();
    //std::cout << "num_timesteps: " << num_groups << std::endl;
    int values_per_group = input[0][0].size();

    std::vector<std::vector<float>> points(num_timesteps);

    for (int t = 0; t < num_timesteps; ++t) {
        for (int g = 0; g < 1; ++g) {

            std::vector<float> neg_center_pos = {0,0,0};
            for(int i = 1; i < 4; ++i) {
                neg_center_pos[i-1] = -1 * input[0][t][i];
                //std::cout << neg_center_pos[i-1] << std::endl;
            }


            for (int i = 1; i < values_per_group; ++i) { // indices 1,2,3
                points[t].push_back(input[0][t][i]);
            }
        }
    }

    return points;
}


void saveVector(const std::vector<std::vector<float>>& vec, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary | std::ios::trunc);
    size_t outerSize = vec.size();
    outFile.write(reinterpret_cast<const char*>(&outerSize), sizeof(outerSize));
    for (const auto& innerVec : vec) {
        size_t innerSize = innerVec.size();
        outFile.write(reinterpret_cast<const char*>(&innerSize), sizeof(innerSize));
        outFile.write(reinterpret_cast<const char*>(innerVec.data()), innerSize * sizeof(float));
    }
}

std::vector<std::vector<float>> loadVector(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    size_t outerSize;
    inFile.read(reinterpret_cast<char*>(&outerSize), sizeof(outerSize));

    std::vector<std::vector<float>> vec(outerSize);

    for (size_t i = 0; i < outerSize; ++i) {
        size_t innerSize;
        inFile.read(reinterpret_cast<char*>(&innerSize), sizeof(innerSize));

        if (inFile.tellg() + static_cast<std::streamoff>(innerSize * sizeof(float)) > fileSize) {
            std::cerr << "Read beyond file size!\n";
            return {};
        }

        vec[i].resize(innerSize);
        inFile.read(reinterpret_cast<char*>(vec[i].data()), innerSize * sizeof(float));
    }
        vec.erase(
        std::remove_if(vec.begin(), vec.end(), [](const std::vector<float>& vec) {
            return vec.empty();
        }),
        vec.end()
    );

    return vec;
}


Vector3 addVector3(Vector3 a, Vector3 b) {
    return (Vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

Vector3 scalarMultiplyVector3(Vector3 a, float b) {
    return (Vector3){ a.x * b, a.y * b, a.z * b  };
}

float* pts_to_vertices(float pts[], int pt_per_face) {
    // Arrays for each face's vertices
    float* vertices = new float[6 * 3 * pt_per_face];  // 6 faces, each having pt_per_face vertices with 3 components each

    // 1. Copy top face (pts[0] to pts[pt_per_face-1])
    std::copy(pts, pts + pt_per_face*3, vertices);

    // 2. Copy bottom face (pts[pt_per_face] to pts[2*pt_per_face-1])
    std::copy(pts + pt_per_face*3, pts + 2 * pt_per_face *3, vertices + pt_per_face *3);


    int front_start_i = 2*pt_per_face*3;
    int front_order[] = {5,3,2,6};
    for (int i = 0; i < 4; ++i) {
        // need the 4,0,1,7 vertices here
        int which_vertex = front_order[i];
        int start_of_vertex = which_vertex*3;

        std::copy( pts + start_of_vertex,   pts + start_of_vertex + 3, vertices +front_start_i  );
        front_start_i += 3;
   }


    int back_start_i = 3*pt_per_face*3;
    int back_order[] = {4,7,1,0};
    for (int i = 0; i < 4; ++i) {
        // need the 4,0,1,7 vertices here
        int which_vertex = back_order[i];
        int start_of_vertex = which_vertex*3;

        std::copy( pts + start_of_vertex,   pts + start_of_vertex + 3, vertices +back_start_i  );
        back_start_i += 3;
   }

    int right_start_i = 4*pt_per_face*3;
    int right_order[] = {7,6,2,1};
    for (int i = 0; i < 4; ++i) {
        // need the 4,0,1,7 vertices here
        int which_vertex = right_order[i];
        int start_of_vertex = which_vertex*3;

        std::copy( pts + start_of_vertex,   pts + start_of_vertex + 3, vertices +right_start_i  );
        right_start_i += 3;
   }

    int left_start_i = 5*pt_per_face*3;
    int left_order[] = {4,0,3,5};
    for (int i = 0; i < 4; ++i) {
        // need the 4,0,1,7 vertices here
        int which_vertex = left_order[i];
        int start_of_vertex = which_vertex*3;

        std::copy( pts + start_of_vertex,   pts + start_of_vertex + 3, vertices +left_start_i  );
        left_start_i += 3;
   } /*


    /*
    const char* faces[] = {"Top", "Bottom", "Front", "Back", "Right", "Left"};
    for (int i = 0; i < 6; ++i) {
        std::cout << faces[i] << " face vertices: ";
        for (int j = 0; j < pt_per_face*3; j+=3) {
            std::cout << "(" << vertices[i * pt_per_face*3 + j] << ", " << vertices[i * pt_per_face*3 + j + 1] << ", " << vertices[i * pt_per_face*3 + j + 2] << ") ";
        }
        std::cout << std::endl;
    }
    */


    return vertices;
}


std::vector<float> cube_vertices(float width, float height, float length, int pt_per_face){
    // pt_per_face should be 4, as of now
    int total_pts = 6 * pt_per_face * 3; // 6 faces then xyz for each point

    float a = width; float b = height; float c = length;
    float corner_vertices[] = {
        // top face
        -a/2, -b/2, c/2,
        a/2, -b/2, c/2,
        a/2, b/2, c/2,
        -a/2, b/2, c/2,
        // bottom face
        -a/2, -b/2, -c/2,
        -a/2, b/2, -c/2,
        a/2, b/2, -c/2,
        a/2, -b/2, -c/2,
    };

    std::vector<float> vertices;
    vertices.reserve(total_pts); // Reserve space for efficiency

    vertices.insert(vertices.end(), corner_vertices, corner_vertices + sizeof(corner_vertices)/sizeof(corner_vertices[0]));

    //Assuming pts_to_vertices modifies the data in place and doesn't allocate new memory.
    //If it does, the below line would need adjustment.
    //If it allocates, you would have to manage that memory appropriately (copying data into vector)
    //pts_to_vertices(vertices.data(), 4); //Modify in place, assuming it modifies the data passed
    return vertices;
};

/*
float* cube_vertices(float width, float height, float length, int pt_per_face){
    // pt_per_face should be 4, as of now
    int total_pts = 6 * pt_per_face * 3; // 6 faces then xyz for each point

    float a = width; float b = height; float c = length;
    float corner_vertices[] = {
        // top face
        -a/2, -b/2, c/2,
        a/2, -b/2, c/2,
        a/2, b/2, c/2,
        -a/2, b/2, c/2,
        // bottom face
        -a/2, -b/2, -c/2,
        -a/2, b/2, -c/2,
        a/2, b/2, -c/2,
        a/2, -b/2, -c/2,
    };

    float* vertices = pts_to_vertices(corner_vertices, 4);
    return vertices;
};
*/

float *insert_t(const float *pts, int num_groups, float t) {
    // Calculate the size of the new array.  Each group of 3 gets a 't' prepended,
    // plus an extra 't' at the very end.

    int new_size = num_groups * 4 + 1; // +1 for the extra t at the end

    // Allocate memory for the new array.  Always check for allocation errors!
    float *new_pts = (float *)malloc(new_size * sizeof(float));
    if (new_pts == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return NULL; // Indicate failure
    }

    // Copy the data, inserting 't' before each group.
    int new_index = 0;
    for (int i = 0; i < num_groups; i++) {
        new_pts[new_index++] = t;
        new_pts[new_index++] = pts[i * 3];
        new_pts[new_index++] = pts[i * 3 + 1];
        new_pts[new_index++] = pts[i * 3 + 2];
    }

    // Add the final 't'.
    new_pts[new_index] = t;


    return new_pts;
}


Mesh GenMeshShape(float* vertices)
{
    Mesh mesh = { 0 };

    float texcoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 0.0f,-1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        0.0f,-1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f
    };

    /*
    for(int i=0; i<73; i++) {
        std::cout << (all_vertices[i]==vertices[i]) << std::endl;
    }

     */


    mesh.vertices = (float *)RL_MALLOC(24*3*sizeof(float));
    memcpy(mesh.vertices, vertices, 24*3*sizeof(float));

    mesh.texcoords = (float *)RL_MALLOC(24*2*sizeof(float));
    memcpy(mesh.texcoords, texcoords, 24*2*sizeof(float));

    mesh.normals = (float *)RL_MALLOC(24*3*sizeof(float));
    memcpy(mesh.normals, normals, 24*3*sizeof(float));

    mesh.indices = (unsigned short *)RL_MALLOC(36*sizeof(unsigned short));

    int k = 0;

    // Indices can be initialized right now
    for (int i = 0; i < 36; i += 6)
    {
        mesh.indices[i] = 4*k;
        mesh.indices[i + 1] = 4*k + 1;
        mesh.indices[i + 2] = 4*k + 2;
        mesh.indices[i + 3] = 4*k;
        mesh.indices[i + 4] = 4*k + 2;
        mesh.indices[i + 5] = 4*k + 3;

        k++;
    }

    mesh.vertexCount = 24;
    mesh.triangleCount = 12;

    // VERY IMPORTANT: Free the dynamically allocated memory
    // delete[] all_vertices;

    // Upload vertex data to GPU (static mesh)
    UploadMesh(&mesh, false);

    return mesh;
}


int main(void)
{

    std::cout << "e" << std::endl;
    const double speed_of_light = 1.0;
    Vector3 observer_abs_velocity = {0.57, 0., 0}; // now get the relative velocity
    Vector3 observer_pos = {0,1,-20};


    float observer_rel_velocity[3];
    observer_rel_velocity[0] = observer_abs_velocity.x / speed_of_light; // x component
    observer_rel_velocity[1] = observer_abs_velocity.y / speed_of_light; // y component
    observer_rel_velocity[2] = observer_abs_velocity.z / speed_of_light; // z component
    // now check that he ain't going faster than light
    double magnitude_of_velocity = 0;
    for (float i : observer_rel_velocity) {magnitude_of_velocity += i*i;}
    magnitude_of_velocity = sqrt(magnitude_of_velocity);
    if(magnitude_of_velocity >= 1.0) {std::cout << "Going too fast!!" << std::endl;return 1;}
    double frame_time = GetTime();
    double last_frame_time = GetTime();
    double time_diff; int frame_number = -1; // it gets incremented at the start

    #define FPS 30
    #define MAX_DURATION 60

    // float **events =(float **)malloc(FPS * MAX_DURATION * 50* sizeof(float *));

    // Use std::vector for safe and automatic memory management
    std::vector<std::vector<float>> events(FPS * MAX_DURATION); // Create a vector of vectors




    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
    const int screenHeight = 950;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");


    // Define the camera to look into our 3d world (position, target, up vector)
    Camera camera = { 0 };
    camera.position = (Vector3){ 0.0f, 2.0f, -35.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    int cameraMode = CAMERA_FIRST_PERSON;

    // Generates some random columns
    float heights[MAX_COLUMNS] = { 0 };
    Vector3 positions[MAX_COLUMNS] = { 0 };
    Color colors[MAX_COLUMNS] = { 0 };

    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){  (unsigned char)GetRandomValue(20, 255), (unsigned char)GetRandomValue(10, 55), 30, 255 };
    }

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(FPS);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    Vector3 block_pos = Vector3{1,1,1};

    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {{
        // Update
        //----------------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE)){cameraMode = CAMERA_FREE;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        if (IsKeyPressed(KEY_TWO)){cameraMode = CAMERA_FIRST_PERSON;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset rol
        if (IsKeyPressed(KEY_THREE)){cameraMode = CAMERA_THIRD_PERSON;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        if (IsKeyPressed(KEY_FOUR)){cameraMode = CAMERA_ORBITAL;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        // Switch camera projection
        if (IsKeyPressed(KEY_P))
        {if (camera.projection == CAMERA_PERSPECTIVE){
            // Create isometric view
            cameraMode = CAMERA_THIRD_PERSON;
            // Note: The target distance is related to the render distance in the orthographic projection
            camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_ORTHOGRAPHIC;
            camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
            CameraYaw(&camera, -135 * DEG2RAD, true);
            CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
        }
        else if (camera.projection == CAMERA_ORTHOGRAPHIC)
        {
            // Reset to default view
            cameraMode = CAMERA_THIRD_PERSON;
            camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_PERSPECTIVE;
            camera.fovy = 60.0f;
        }
        }


        UpdateCamera(&camera, cameraMode);                  // Update camera
    }
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            if (frame_number < 3 && frame_time > 0.5) {
                //std::cout << frame_time << std::endl;
                return 1;
            }

            last_frame_time = frame_time;
            frame_time = GetTime();
            time_diff = frame_time-last_frame_time;
            frame_number++;


            ClearBackground(RAYWHITE);

            BeginMode3D(camera);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground
                //DrawCube((Vector3){ -16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, BLUE);     // Draw a blue wall
                //DrawCube((Vector3){ 16.0f, 2.5f, 0.0f }, 1.0f, 5.0f, 32.0f, LIME);      // Draw a green wall
                //DrawCube((Vector3){ 0.0f, 2.5f, 16.0f }, 32.0f, 5.0f, 1.0f, GOLD);      // Draw a yellow wall

                DrawCube( observer_pos, 0.5,3,0.5, GOLD    );
                observer_pos = addVector3(observer_pos, scalarMultiplyVector3(observer_abs_velocity, time_diff));
                //camera.position = observer_pos;


                //float* lorentzed_corners = vectorToFloatPointer(lookup_row(block_points_average_times, block_points_array, frame_time));
                //float* lorentzed_center = vectorToFloatPointer(lookup_row(block_points_average_times, block_center_point , frame_time));
                //float* lorentzed_vect_points = pts_to_vertices(lorentzed_corners, 4);
                //Model lorentzed_model = LoadModelFromMesh(GenMeshShape(lorentzed_vect_points));
                //DrawModel(lorentzed_model, {lorentzed_center[0], lorentzed_center[1], lorentzed_center[2]}, 1, RED);
                //delete[] lorentzed_corners; lorentzed_corners = nullptr;

                Vector3 block_abs_velocity = {0,0.,0};
                block_pos = addVector3(block_pos, scalarMultiplyVector3(block_abs_velocity, time_diff));
                std::vector<float> block_pos_sv = {block_pos.x, block_pos.y, block_pos.z};

                std::vector<float> corner_points = cube_vertices(3,4,5, 4);
                float* vect_points = pts_to_vertices(corner_points.data(), 4);

                corner_points = addVectorsCyclically(corner_points, block_pos_sv);
                std::vector<float> prepended_corner_p = corner_points;
                prepended_corner_p.insert(prepended_corner_p.begin(), block_pos_sv.begin(), block_pos_sv.end());

                Model model = LoadModelFromMesh(GenMeshShape(vect_points));
                DrawModel(model, block_pos, 1, RED);
                delete[] vect_points; //stop memory leaks

                int num_of_groups = 9;
                float *events_array = insert_t(prepended_corner_p.data(), num_of_groups, GetTime());

                int events_per_frame = num_of_groups * 4;
                events[frame_number].resize(events_per_frame); // Resize the vector for this frame
                std::copy(events_array, events_array + events_per_frame, events[frame_number].begin()); // Copy the data
                //events[frame_number] = static_cast<float *>(malloc(events_per_frame * sizeof(float)));
                //if (events[frame_number] == NULL){ std::cout << "Out of Memory" << std::endl;  return 1;}

                //memcpy(events[frame_number], events_array, sizeof(float) * events_per_frame);


                /*
                if (new_pts != NULL) {  // Check if allocation was successful
                    for (int i = 0; i < num_of_groups; i++) {
                        std::cout << new_pts[i] << std::endl;
                    }
                    printf("\n");

                    free(new_pts); // Free the dynamically allocated memory
                } */

                //std::cout << GetTime()  << std::endl;



                // Draw player cube
                if (cameraMode == CAMERA_THIRD_PERSON)
                {
                    DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                    DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
                }

            EndMode3D();

            // Draw info boxes
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);

            DrawText("Camera controls:", 15, 15, 10, BLACK);
            DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
            DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
            DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
            DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
            DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

            DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(600, 5, 195, 100, BLUE);

            DrawText("Camera status:", 610, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
            DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
                                                    (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }




    saveVector(events, "events_data.bin");


    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------



    auto rawloadedData = loadVector("events_data.bin");     std::vector<std::vector<float>> loadedData;     for (const auto& raw_corner_list : rawloadedData) {         loadedData.push_back( transformation( raw_corner_list.data(), observer_rel_velocity, 36)  );     }

    auto processed_events = processEvents(loadedData); if (processed_events.empty()) return 2;
    auto block_points_array = process_to_points(processed_events);
    auto block_center_point = get_lorentz_center_pos(processed_events);
    auto block_points_average_times = average_start_times(processed_events);

        for (float avg : block_points_average_times) {
      //std::cout << avg << " " << std::endl;
    }



    for (const auto& row : block_points_array) {
        for (double val : row) {
            //std::cout << val << " ";
        }
        //std::cout << std::endl;  // Newline after each row
    }



    InitWindow(screenWidth, screenHeight, "raylib [core] example - 3d camera first person");


    // Define the camera to look into our 3d world (position, target, up vector)
    camera.position = (Vector3){ 0.0f, 2.0f, -35.0f };    // Camera position
    camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };      // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };          // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type


    for (int i = 0; i < MAX_COLUMNS; i++)
    {
        heights[i] = (float)GetRandomValue(1, 12);
        positions[i] = (Vector3){ (float)GetRandomValue(-15, 15), heights[i]/2.0f, (float)GetRandomValue(-15, 15) };
        colors[i] = (Color){  (unsigned char)GetRandomValue(20, 255), (unsigned char)GetRandomValue(10, 55), 30, 255 };
    }

    DisableCursor();                    // Limit cursor to relative movement inside the window

    SetTargetFPS(FPS);                   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------


    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {{
        // Update
        //----------------------------------------------------------------------------------
        // Switch camera mode
        if (IsKeyPressed(KEY_ONE)){cameraMode = CAMERA_FREE;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        if (IsKeyPressed(KEY_TWO)){cameraMode = CAMERA_FIRST_PERSON;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset rol
        if (IsKeyPressed(KEY_THREE)){cameraMode = CAMERA_THIRD_PERSON;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        if (IsKeyPressed(KEY_FOUR)){cameraMode = CAMERA_ORBITAL;camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };} // Reset roll
        // Switch camera projection
        if (IsKeyPressed(KEY_P))
        {if (camera.projection == CAMERA_PERSPECTIVE){
            // Create isometric view
            cameraMode = CAMERA_THIRD_PERSON;
            // Note: The target distance is related to the render distance in the orthographic projection
            camera.position = (Vector3){ 0.0f, 2.0f, -100.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_ORTHOGRAPHIC;
            camera.fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
            CameraYaw(&camera, -135 * DEG2RAD, true);
            CameraPitch(&camera, -45 * DEG2RAD, true, true, false);
        }
        else if (camera.projection == CAMERA_ORTHOGRAPHIC)
        {
            // Reset to default view
            cameraMode = CAMERA_THIRD_PERSON;
            camera.position = (Vector3){ 0.0f, 2.0f, 10.0f };
            camera.target = (Vector3){ 0.0f, 2.0f, 0.0f };
            camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
            camera.projection = CAMERA_PERSPECTIVE;
            camera.fovy = 60.0f;
        }
        }


        UpdateCamera(&camera, cameraMode);                  // Update camera
    }
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            if (frame_number < 3 && frame_time > 0.5) {
                //std::cout << frame_time << std::endl;
                return 1;
            }

            last_frame_time = frame_time;
            frame_time = GetTime();
            time_diff = frame_time-last_frame_time;
            frame_number++;


            ClearBackground(BLACK);

            BeginMode3D(camera);

                DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 32.0f, 32.0f }, LIGHTGRAY); // Draw ground


                DrawCube( observer_pos, 0.5,3,0.5, GOLD    );
                observer_pos = addVector3(observer_pos, scalarMultiplyVector3(observer_abs_velocity, time_diff));
                //camera.position = observer_pos;


                float* lorentzed_corners = vectorToFloatPointer(lookup_row(block_points_average_times, block_points_array, frame_time));
                float* lorentzed_center = vectorToFloatPointer(lookup_row(block_points_average_times, block_center_point , frame_time));
                float* lorentzed_vect_points = pts_to_vertices(lorentzed_corners, 4);
                Model lorentzed_model = LoadModelFromMesh(GenMeshShape(lorentzed_vect_points));
                DrawModel(lorentzed_model, {lorentzed_center[0], lorentzed_center[1], lorentzed_center[2]}, 1, RED);
                delete[] lorentzed_corners; lorentzed_corners = nullptr;



                // Draw player cube
                if (cameraMode == CAMERA_THIRD_PERSON)
                {
                    DrawCube(camera.target, 0.5f, 0.5f, 0.5f, PURPLE);
                    DrawCubeWires(camera.target, 0.5f, 0.5f, 0.5f, DARKPURPLE);
                }

            EndMode3D();

            // Draw info boxes
            DrawRectangle(5, 5, 330, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(5, 5, 330, 100, BLUE);

            DrawText("Camera controls:", 15, 15, 10, BLACK);
            DrawText("- Move keys: W, A, S, D, Space, Left-Ctrl", 15, 30, 10, BLACK);
            DrawText("- Look around: arrow keys or mouse", 15, 45, 10, BLACK);
            DrawText("- Camera mode keys: 1, 2, 3, 4", 15, 60, 10, BLACK);
            DrawText("- Zoom keys: num-plus, num-minus or mouse scroll", 15, 75, 10, BLACK);
            DrawText("- Camera projection key: P", 15, 90, 10, BLACK);

            DrawRectangle(600, 5, 195, 100, Fade(SKYBLUE, 0.5f));
            DrawRectangleLines(600, 5, 195, 100, BLUE);

            DrawText("Camera status:", 610, 15, 10, BLACK);
            DrawText(TextFormat("- Mode: %s", (cameraMode == CAMERA_FREE) ? "FREE" :
                                              (cameraMode == CAMERA_FIRST_PERSON) ? "FIRST_PERSON" :
                                              (cameraMode == CAMERA_THIRD_PERSON) ? "THIRD_PERSON" :
                                              (cameraMode == CAMERA_ORBITAL) ? "ORBITAL" : "CUSTOM"), 610, 30, 10, BLACK);
            DrawText(TextFormat("- Projection: %s", (camera.projection == CAMERA_PERSPECTIVE) ? "PERSPECTIVE" :
                                                    (camera.projection == CAMERA_ORTHOGRAPHIC) ? "ORTHOGRAPHIC" : "CUSTOM"), 610, 45, 10, BLACK);
            DrawText(TextFormat("- Position: (%06.3f, %06.3f, %06.3f)", camera.position.x, camera.position.y, camera.position.z), 610, 60, 10, BLACK);
            DrawText(TextFormat("- Target: (%06.3f, %06.3f, %06.3f)", camera.target.x, camera.target.y, camera.target.z), 610, 75, 10, BLACK);
            DrawText(TextFormat("- Up: (%06.3f, %06.3f, %06.3f)", camera.up.x, camera.up.y, camera.up.z), 610, 90, 10, BLACK);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }




    /*
        // Display loaded data
    for (const auto& innerVec : loadedData) {
        for (float val : innerVec) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
    */


    // Access the stored frames (now safe)
    /*
    for (int i = 0; i < 10; i++) {
        //std::cout << i << std::endl;
        for (int j = 0; j < 32; j++) { // Check your dimensions; 96 might be wrong
            if(i < events.size() && j < events[i].size()){  //Check bounds before accessing
                std::cout << events[i][j] << std::endl;
            } else {
                std::cout << i << std::endl;
                std::cout << "Out of bounds" << std::endl;
            }
        }
    }
    */






    return 0;
}