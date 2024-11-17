#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric> // for accumulate
#include <limits> // Required for numeric_limits

using namespace std;

vector<float> shift_array(const vector<float>& input) {
    if (input.empty()) {
        return {}; // Handle empty input
    }

    //Find the minimum element.  Handle potential NaN values.
    float min_val = numeric_limits<float>::infinity();
    for(float val : input){
        if(val < min_val){
            min_val = val;
        }
    }

    float offset = 0.1f - min_val;
    vector<float> shifted_array(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        shifted_array[i] = input[i] + offset;
    }

    return shifted_array;
}


vector<float> average_start_times(const vector<vector<vector<float>>>& result) {
    int num_timesteps = result[0].size();
    int num_groups = result.size();
    vector<float> averages(num_timesteps);

    for (int t = 0; t < num_timesteps; ++t) {
        float sum = 0;
        for (int g = 0; g < num_groups; ++g) {
            sum += result[g][t][0];
        }
        averages[t] = sum / num_groups;
    }

    averages = shift_array(averages);
    return averages;
}



// Function to process events and return the reorganized data
vector<vector<vector<float>>> processEvents(const vector<vector<float>>& events, int groups=8, int values_per_group=4)
{
    int events_size = events.size();
    if (events.size() != events_size) {
        cerr << "Error: Inconsistent events_size and events size." << endl;
        //Return an empty vector to indicate an error.  Could throw an exception for better error handling
        return {};
    }

    vector<vector<vector<float>>> new_vectors(groups);

    for (int i = 0; i < groups; ++i) {
        new_vectors[i].resize(events_size);
        for (int j = 0; j < events_size; ++j) {
            if (events[j].size() < (i + 1) * values_per_group) {
                cerr << "Error: Insufficient elements in events[" << j << "]." << endl;
                return {}; //Return an empty vector to indicate an error
            }
            new_vectors[i][j].resize(values_per_group);
            for (int k = 0; k < values_per_group; ++k) {
                new_vectors[i][j][k] = events[j][i * values_per_group + k];
            }
        }
    }

    for (int i = 0; i < groups; ++i) {
        sort(new_vectors[i].begin(), new_vectors[i].end(), [](const vector<float>& a, const vector<float>& b) {
            return a[0] < b[0];
        });
    }

    return new_vectors;

}

vector<vector<float>> process_to_points(const vector<vector<vector<float>>>& input) {
    int num_timesteps = input[0].size(); // Assumes all groups have the same number of time steps.
    int num_groups = input.size();
    int values_per_group = input[0][0].size();

    vector<vector<float>> points(num_timesteps);

    for (int t = 0; t < num_timesteps; ++t) {
        for (int g = 0; g < num_groups; ++g) {
            for (int i = 1; i < values_per_group; ++i) { // indices 1,2,3
                points[t].push_back(input[g][t][i]);
            }
        }
    }

    return points;
}


int main() {
    std::vector<std::vector<float>> events = {
        {-10.241648, -15.5, -1, 2.5, 0.241648, 1.5, -1, 2.5, 0.241648, 1.5, 1, 2.5, 0.241648, -1.5, 1, 2.5, 0.241648, -1.5, -1, -2.5, 0.241648, -1.5, 1, -2.5, 0.241648, 1.5, 1, -2.5, 0.241648, 1.5, -1, -2.5},
        {0.274753, -1.5, -1, 2.5, 0.274753, 1.5, -1, 2.5, 0, 21.5, 1, 2.5, 0.274753, -1.5, 1, 2.5, 0.274753, -1.5, -1, -2.5, 0.274753, -1.5, 1, -2.5, 0.274753, 1.5, 1, -2.5, 0.274753, 1.5, -1, -2.5},
        {2.308201, -1.5, -1, 2.5, 0.308201, 1.5, -1, 2.5, 0.308201, 1.5, 1, 2.5, 0.308201, -1.5, 1, 2.5, 0.308201, -1.5, -1, -2.5, 0.308201, -1.5, 1, -2.5, 0.308201, 1.5, 1, -2.5, 0.308201, 1.5, -1, -2.5}
    };


    int groups = 8;
    int values_per_group = 4;

    auto result = processEvents(events);

    //Check for errors before printing
    if(result.empty()){
        return 1; // Indicate an error
    }



    /*
    for (int i = 0; i < groups; ++i) {
        cout << "new_vector" << i + 1 << ":" << endl;
        for (const auto& inner_vec : result[i]) {
            for (float val : inner_vec) {
                cout << val << " ";
            }
            cout << endl;
        }
        cout << endl;
    }

     */


    auto averages = average_start_times(result);
    auto points = process_to_points(result);


    /*
    for (const auto& time_step : points) {
        for (float val : time_step) {
            cout << val << " ";
        }
        cout << endl;
    }

     */

        for (float avg : averages) {
      cout << avg << " ";
    }
    cout << endl;


    return 0;
}
