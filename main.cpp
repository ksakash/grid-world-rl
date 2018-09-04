#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <vector>

struct grid_element {
    double value;
    double mean;
    double sigma;
};

int target_x = 0;
int target_y = 0;
int grid_size = 0;

std::default_random_engine generator;

double number_generator(double mean, double sigma) {
	std::normal_distribution<double> distribution(mean, sigma);
    return distribution(generator);   
}

int distance(int i, int j, int k, int l) {
    return abs(i - k) + abs(j - l);
}

double max(int i, int j, int grid_size, std::vector<std::vector<double>> mat, double estimate) {

    double a[4];
    double gamma = 0.0;
    if (i + 1 >= grid_size) {
        a[0] = 0;
    }
    else {
        a[0] = gamma*mat[i + 1][j]*distance(i + 1, j, target_x, target_y);
    }
    if (i - 1 < 0) {
        a[2] = 0;
    }
    else {
        a[2] = gamma*mat[i - 1][j]*distance(i - 1, j, target_x, target_y);
    }
    if (j + 1 >= grid_size) {
        a[1] = 0;
    }
    else {
        a[1] = gamma*mat[i][j + 1]*distance(i, j + 1, target_x, target_y);
    }
    if (j - 1 < 0) {
        a[3] = 0;
    } 
    else {
        a[3] = gamma*mat[i][j - 1]*distance(i, j - 1, target_x, target_y);
    }

    double max = 0;

    for (int m = 0; m < 4; m++) {
        a[m] = (estimate + a[m])/distance(i, j, target_x, target_y);
    }

    for (int m = 0; m < 4; m++) {
        if (a[m] > max)
            max = a[m];
    }
    return max;
}

void print(std::vector<std::vector<double>> a) {
    std::cout << "----------------------------" << std::endl;
    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < a[i].size(); j++) {
            std::cout << "| " << a[i][j] << " ";
        }
        std::cout << "|" << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
}
int main() {

    // std::cin >> grid_size;
    grid_size = 3;
    int episode_count = 0;
    // std::cin >> episode_count;
    episode_count = 10;
    std::vector<std::vector<grid_element>> grid(grid_size);

    for (int i = 0; i < grid_size; i++) {
        grid[i].resize(grid_size);
    }

    // for (int i = 0; i < grid_size; i++) {
    //     for (int j = 0; j < grid_size; j++) {
    //         std::cin >> grid[i][j].mean >> grid[i][j].sigma;
    //     }
    // }

    grid[0][0].mean = 0; grid[0][0].sigma = 1;
    grid[0][1].mean = 4; grid[0][1].sigma = 1;
    grid[0][2].mean = 8; grid[0][2].sigma = 1;
    // grid[0][3].mean = 52; grid[0][3].sigma = 1;
    grid[1][0].mean = 60; grid[1][0].sigma = 1;
    grid[1][1].mean = 20; grid[1][1].sigma = 1;
    grid[1][2].mean = 24; grid[1][2].sigma = 1;
    // grid[1][3].mean = 28; grid[1][3].sigma = 1;
    grid[2][0].mean = 44; grid[2][0].sigma = 1; 
    grid[2][1].mean = 36; grid[2][1].sigma = 1;
    grid[2][2].mean = 40; grid[2][2].sigma = 1;
    // grid[2][3].mean = 32; grid[2][3].sigma = 1;
    // grid[3][0].mean = 48; grid[2][2].sigma = 1;
    // grid[3][1].mean = 12; grid[2][2].sigma = 1;
    // grid[3][2].mean = 56; grid[2][2].sigma = 1;
    // grid[3][3].mean = 16; grid[2][2].sigma = 1;

    
    std::vector<std::vector<double>> mean_estimate(grid_size);
    int count = 0;

    std::vector<std::vector<double>> mat_prev(grid_size);
    std::vector<std::vector<double>> mat_next(grid_size);

    for (int i = 0; i < grid_size; i++) {
        mat_next[i].resize(grid_size);
        mat_prev[i].resize(grid_size);
    }

    for (int i = 0; i < episode_count; i++) {
        
        count++;
        // std::cin >> target_x >> target_y;
        target_x = 0; target_y = 0;

        for (int i = 0; i < grid_size; i++) {
            for (int j = 0; j < grid_size; j++) {
                grid[i][j].value = number_generator(grid[i][j].mean, grid[i][j].sigma);
            }
        }

        for (int i = 0; i < grid_size; i++) {
            mean_estimate[i].resize(grid_size);
            for (int j = 0; j < grid_size; j++) {
                mean_estimate[i][j] = mean_estimate[i][j] + (grid[i][j].value - mean_estimate[i][j])/count;
            }
        }

        std::cout << "mean estimate" << std::endl;
        print(mean_estimate);

        int iterations = 100;
        // std::cin  >> iterations;   
        for (int m = 0; m < iterations; m++) {
            for (int i = 0; i < grid_size; i++) {
                for (int j = 0; j < grid_size; j++) {
                    mat_next[i][j] = max(i, j, grid_size, mat_prev, mean_estimate[i][j]);
                }
            }
            for (int i = 0; i < grid_size; i++) {
                for (int j = 0; j < grid_size; j++) {
                    mat_prev[i][j] = mat_next[i][j];
                }
            }
        }

        std::cout << "mat prev" << std::endl;
        print(mat_prev);

    }
    return 0;
}