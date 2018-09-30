#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <vector>

using namespace std;

struct grid_element {
    double value;
    double mean;
    double sigma;
};

int grid_size = 3;
int target_x = grid_size-1;
int target_y = grid_size-1;

std::default_random_engine generator;
std::vector<std::vector<double>> mean_estimate(grid_size);

double number_generator(double mean, double sigma) {
	std::normal_distribution<double> distribution(mean, sigma);
    return distribution(generator);   
}

int distance(int i, int j, int k, int l) {
    return abs(i - k) + abs(j - l);
}

bool atBoundary(int i, int j, int grid_size) {
    if (i == 0 || i == grid_size - 1 || j == 0 || j == grid_size - 1) {
        return true;
    }
    return false;
}

bool atCorner(int i, int j, int grid_size) {
    if ((i == 0 && j == 0) || (i == 0 && j == grid_size - 1) || (i == grid_size - 1 && j == 0) || (i == grid_size - 1 && j == grid_size - 1)) {
        return true;
    }
    else {
        return false;
    }
}

double max(int i, int j, int grid_size, std::vector<std::vector<double>> mat, std::vector<std::vector<double>> mean_estimate) {

    double gamma = 0.2;
    double beta = 0.7;
    double sum = 0;
    double reward = 0;

    bool boundary_element = atBoundary(i, j, grid_size);

    if (boundary_element) {
        bool corner_element = atCorner(i, j, grid_size);

        if (corner_element) {
            if (i == 0 && j == 0) {

                sum = 0.25*(mean_estimate[0][1] - beta*2 + gamma*mat[0][1] + mean_estimate[1][0] - beta*2 + gamma*mat[1][0]);
                // cout << "sum: " << sum << endl;
                return sum;
            }
            else if (i == 0 && j == grid_size - 1) {
                sum = 0.25*(mean_estimate[0][grid_size-2] - beta*grid_size + gamma*mat[0][grid_size-2] + mean_estimate[1][grid_size-1] - beta*grid_size + gamma*mat[1][grid_size-1]);
                return sum;
            }
            else if (i == grid_size - 1 && j == 0) {
                sum = 0.25*(mean_estimate[grid_size-2][0] - beta*grid_size + gamma*mat[grid_size-2][0] + mean_estimate[grid_size-1][1] - beta*grid_size + gamma*mat[grid_size-1][1]);
                return sum;
            }
            else {
                sum = 0.25*(mean_estimate[grid_size-2][grid_size-1] - beta*(2*grid_size-2) + gamma*mat[grid_size-2][grid_size-2] + mean_estimate[grid_size-1][grid_size-2]
                         - beta*(2*grid_size-2) + gamma*mat[grid_size-1][grid_size-2]);
            }
        }
        else {
            if (i == 0) {
                sum = 0.25*(mean_estimate[i][j-1] - beta*(j+4) + gamma*mat[i][j-1]) +
                        0.25*(mean_estimate[i][j+1] - beta*(j+3) + gamma*mat[i][j+1]) + 
                            0.25*(mean_estimate[i+1][j] - beta*(j+3) + gamma*mat[i+1][j]);
                return sum;
            }
            else if (i == grid_size - 1) {
                sum = 0.25*(mean_estimate[i][j-1] - beta*(grid_size+j+4) + gamma*mat[i][j-1]) +
                        0.25*(mean_estimate[i][j+1] - beta*(grid_size+j+3) + gamma*mat[i][j+1]) +
                            0.25*(mean_estimate[i-1][j] - beta*(grid_size+j+3) + gamma*mat[i-1][j]);
                return sum;
            }
            else if (j == 0) {
                sum = 0.25*(mean_estimate[i-1][j] - beta*(i+4) + gamma*mat[i-1][j]) +
                        0.25*(mean_estimate[i+1][j] - beta*(i+3) + gamma*mat[i+1][j]) +
                            0.25*(mean_estimate[i][j+1] - beta*(i+3) + gamma*mat[i][j+1]);
                return sum;
            }
            else if (j == grid_size - 1) {
                sum = 0.25*(mean_estimate[i-1][j] - beta*(grid_size+i+4) + gamma*mat[i-1][j]) +
                        0.25*(mean_estimate[i+1][j] - beta*(grid_size+i+3) + gamma*mat[i+1][j]) +
                            0.25*(mean_estimate[i][j+1] - beta*(grid_size+i+3) + gamma*mat[i][j+1]);
                return sum;
            }
        }
    }
    else {
        sum = 0.25*(mean_estimate[i-1][j] - beta*(i+j+4.33) + gamma*mat[i-1][j]) +
                0.25*(mean_estimate[i+1][j] - beta*(i+j+3.66) + gamma*mat[i+1][j]) +
                    0.25*(mean_estimate[i][j-1] - beta*(i+j+4.33) + gamma*mat[i][j-1]) +
                        0.25*(mean_estimate[i][j+1] - beta*(i+j+3.66) + gamma*mat[i][j+1]);
        return sum;
    }
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

    grid[0][0].mean = 2; grid[0][0].sigma = 0.3;
    grid[0][1].mean = 10; grid[0][1].sigma = 0.3;
    grid[0][2].mean = 3; grid[0][2].sigma = 0.3;
    grid[1][0].mean = 6; grid[1][0].sigma = 0.3;
    grid[1][1].mean = 5; grid[1][1].sigma = 0.3;
    grid[1][2].mean = 1; grid[1][2].sigma = 0.3;
    grid[2][0].mean = 9; grid[2][0].sigma = 0.3; 
    grid[2][1].mean = 4; grid[2][1].sigma = 0.3;
    grid[2][2].mean = 8; grid[2][2].sigma = 0.3;
    
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
                // if (i == target_x && j == target_y) {
                //     mean_estimate[i][j] = 0;
                // }
                // else {
                    mean_estimate[i][j] = mean_estimate[i][j] + (grid[i][j].value - mean_estimate[i][j])/count;
                // }
            }
        }

        std::cout << "MEAN ESTIMATE: " << std::endl;
        print(mean_estimate);

        int iterations = 100;
        // std::cin  >> iterations;   
        for (int m = 0; m < iterations; m++) {
            for (int i = 0; i < grid_size; i++) {
                for (int j = 0; j < grid_size; j++) {
                    // if (i == target_x && j == target_y) {
                    //     mat_next[i][j] = 0;
                    // }
                    // else {
                        mat_next[i][j] = max(i, j, grid_size, mat_prev, mean_estimate);
                    // }
                }
            }
            for (int i = 0; i < grid_size; i++) {
                for (int j = 0; j < grid_size; j++) {
                    mat_prev[i][j] = mat_next[i][j];
                }
            }
        }

        std::cout << "MAT PREV: " << std::endl;
        print(mat_prev);

    }
    return 0;
}