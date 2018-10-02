#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <vector>

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

using namespace std;

struct grid_element {
    double value;
    double mean;
    double sigma;
};

int grid_size = 3;
int target_x = grid_size-1;
int target_y = grid_size-1;

int action_transition_matrix[9][4];

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

double maximum(double a, double b) {
    if (a > b) 
        return a;
    else 
        return b;
}

void copy_action_matrices(int mat1[][4], int mat2[][4]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 4; j++) {
            mat1[i][j] = mat2[i][j];
        }
    }
}

double max(int i, int j, double value_function[][3], int mat[][4], std::vector<std::vector<double>> mean_estimate, int distance) {

    double gamma = 0.5;
    double beta = 0.5;
    
    int mat1[9][4];
    int mat2[9][4];
    int mat4[9][4];

    copy_action_matrices(mat1, mat);
    copy_action_matrices(mat2, mat);

    bool boundary_element = atBoundary(i, j, grid_size);

    if (boundary_element) {
        bool corner_element = atCorner(i, j, grid_size);

        if (corner_element) {
            if (i == 0 && j == 0) {
                mat1[3*i + j+1][LEFT] = 0;
                mat2[3*i + j+1][LEFT] = 0;
                mat2[3*(i+1) + j][UP] = 0;
                mat1[3*(i+1) + j][UP] = 0;

                double v1 = 0;
                double v2 = 0;
                if (mat[3*i+j][RIGHT] == 1) {
                    v1 = mean_estimate[0][1] - beta*(distance + 1) + gamma*max(i, j+1, value_function,  mat1, mean_estimate, distance+1);
                    
                }
                if (mat[3*i+j][DOWN] == 1) {
                    v2 = mean_estimate[1][0] - beta*(distance + 1) + gamma*max(i+1, j, value_function,  mat2, mean_estimate, distance+1);
                }

                double temp = maximum(v1, v2);
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else if (i == 0 && j == grid_size - 1) {
                mat1[3*i + j-1][RIGHT] = 0;
                mat1[3*(i+1) + j][UP] = 0;
                mat2[3*i + j-1][RIGHT] = 0;
                mat2[3*(i+1) + j][UP] = 0;

                double v1 = 0;
                double v2 = 0;

                if (mat[3*i + j][LEFT] == 1) {
                    v1 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][DOWN] == 1) {
                    v2 = mean_estimate[i+1][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat2, mean_estimate, distance+1);
                }

                double temp = maximum(v1, v2);
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else if (i == grid_size - 1 && j == 0) {
                mat1[3*i + j+1][LEFT] = 0;
                mat1[3*(i-1) + j][DOWN] = 0;
                mat2[3*i + j+1][LEFT] = 0;
                mat2[3*(i-1) + j][DOWN] = 0;

                double v1 = 0;
                double v2 = 0;

                if (mat[3*i + j][RIGHT] == 1) {
                    v1 = mean_estimate[i][j+1] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][UP] == 1) {
                    v2 = mean_estimate[i-1][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                }

                double temp = maximum(v1, v2);
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else {
                value_function[i][j] = mean_estimate[i][j];
                return mean_estimate[i][j];
            }
        }
        else {
            if (j == 0) {

                int mat3[9][4];
                copy_action_matrices(mat3, mat);

                mat1[3*i + j+1][LEFT] = 0;
                mat1[3*(i-1) + j][DOWN] = 0;
                mat1[3*(i+1) + j][UP] = 0;
                mat2[3*i + j+1][LEFT] = 0;
                mat2[3*(i-1) + j][DOWN] = 0;
                mat2[3*(i+1) + j][UP] = 0;
                mat3[3*i + j+1][LEFT] = 0;
                mat3[3*(i-1) + j][DOWN] = 0;
                mat3[3*(i+1) + j][UP] = 0;

                double v1 = 0;
                double v2 = 0;
                double v3 = 0;

                if (mat[3*i + j][RIGHT] == 1) {
                    v1 = mean_estimate[i][j+1] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][UP] == 1) {
                    v2 = mean_estimate[i-1][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                }
                if (mat[3*i + j][DOWN] == 1) {
                    v3 = mean_estimate[i+1][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                }

                double temp = maximum(v1, maximum(v2, v3));
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else if (i == grid_size - 1) {
                int mat3[9][4];
                copy_action_matrices(mat3, mat);

                mat1[3*i + j+1][LEFT] = 0;
                mat1[3*(i-1) + j][DOWN] = 0;
                mat1[3*(i) + j-1][RIGHT] = 0;
                mat2[3*i + j+1][LEFT] = 0;
                mat2[3*(i-1) + j][DOWN] = 0;
                mat2[3*(i) + j-1][RIGHT] = 0;
                mat3[3*i + j+1][LEFT] = 0;
                mat3[3*(i-1) + j][DOWN] = 0;
                mat3[3*(i) + j-1][RIGHT] = 0;

                double v1 = 0;
                double v2 = 0;
                double v3 = 0;

                if (mat[3*i + j][RIGHT] == 1) {
                    v1 = mean_estimate[i][j+1] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][UP] == 1) {
                    v2 = mean_estimate[i-1][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                }
                if (mat[3*i + j][LEFT] == 1) {
                    v3 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat3, mean_estimate, distance+1);
                }

                double temp = maximum(v1, maximum(v2, v3));
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else if (i == 0) {
                int mat3[9][4];
                copy_action_matrices(mat3, mat);

                mat1[3*i + j+1][LEFT] = 0;
                mat1[3*(i+1) + j][UP] = 0;
                mat1[3*(i) + j-1][RIGHT] = 0;
                mat2[3*i + j+1][LEFT] = 0;
                mat2[3*(i+1) + j][UP] = 0;
                mat2[3*(i) + j-1][RIGHT] = 0;
                mat3[3*i + j+1][LEFT] = 0;
                mat3[3*(i+1) + j][UP] = 0;
                mat3[3*(i) + j-1][RIGHT] = 0;

                double v1 = 0;
                double v2 = 0;
                double v3 = 0;

                if (mat[3*i + j][RIGHT] == 1) {
                    v1 = mean_estimate[i][j+1] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][LEFT] == 1) {
                    v2 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat2, mean_estimate, distance+1);
                }
                if (mat[3*i + j][DOWN] == 1) {
                    v3 = mean_estimate[i+1][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                }

                double temp = maximum(v1, maximum(v2, v3));
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            }
            else if (j == grid_size - 1) {
                int mat3[9][4];
                copy_action_matrices(mat3, mat);

                mat1[3*i + j-1][RIGHT] = 0;
                mat1[3*(i-1) + j][DOWN] = 0;
                mat1[3*(i+1) + j][UP] = 0;
                mat2[3*i + j-1][RIGHT] = 0;
                mat2[3*(i-1) + j][DOWN] = 0;
                mat2[3*(i+1) + j][UP] = 0;
                mat3[3*i + j-1][RIGHT] = 0;
                mat3[3*(i-1) + j][DOWN] = 0;
                mat3[3*(i+1) + j][UP] = 0;

                double v1 = 0;
                double v2 = 0;
                double v3 = 0;

                if (mat[3*i + j][LEFT] == 1) {
                    v1 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
                }
                if (mat[3*i + j][UP] == 1) {
                    v2 = mean_estimate[i-1][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                }
                if (mat[3*i + j][DOWN] == 1) {
                    v3 = mean_estimate[i+1][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                }

                double temp = maximum(v1, maximum(v2, v3));
                if (temp > value_function[i][j]) {
                    value_function[i][j] = temp;
                }
                return temp;
            
            }
        }
    }
    else {
        int mat3[9][4];
        int mat4[9][4];
        copy_action_matrices(mat3, mat);
        copy_action_matrices(mat4, mat);

        mat1[3*i + j-1][RIGHT] = 0;
        mat1[3*(i-1) + j][DOWN] = 0;
        mat1[3*(i+1) + j][UP] = 0;
        mat1[3*i + j+1][LEFT] = 0;

        mat2[3*i + j-1][RIGHT] = 0;
        mat2[3*(i-1) + j][DOWN] = 0;
        mat2[3*(i+1) + j][UP] = 0;
        mat2[3*i + j+1][LEFT] = 0;
        
        mat3[3*i + j-1][RIGHT] = 0;
        mat3[3*(i-1) + j][DOWN] = 0;
        mat3[3*(i+1) + j][UP] = 0;
        mat3[3*i + j+1][LEFT] = 0;
        
        mat4[3*i + j-1][RIGHT] = 0;
        mat4[3*(i-1) + j][DOWN] = 0;
        mat4[3*(i+1) + j][UP] = 0;
        mat4[3*i + j+1][LEFT] = 0;

        double v1 = 0;
        double v2 = 0;
        double v3 = 0;
        double v4 = 0;

        if (mat[3*i + j][LEFT] == 1) {
            v1 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
        }
        if (mat[3*i + j][UP] == 1) {
            v2 = mean_estimate[i-1][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
        }
        if (mat[3*i + j][DOWN] == 1) {
            v3 = mean_estimate[i+1][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
        }
        if (mat[3*i + j][RIGHT] == 1) {
            v4 = mean_estimate[i][j-1] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat4, mean_estimate, distance+1); 
        }

        double temp = maximum(maximum(v1, v4), maximum(v2, v3));
        if (temp > value_function[i][j]) {
            value_function[i][j] = temp;
        }
        return temp;
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

void initialize_action_matrix(int action_transition_matrix[][4]) {

    action_transition_matrix[0][UP] = 0;
    action_transition_matrix[0][DOWN] = 1;
    action_transition_matrix[0][RIGHT] = 1;
    action_transition_matrix[0][LEFT] = 0;

    action_transition_matrix[1][UP] = 0;
    action_transition_matrix[1][DOWN] = 1;
    action_transition_matrix[1][RIGHT] = 1;
    action_transition_matrix[1][LEFT] = 0;
    
    action_transition_matrix[2][UP] = 0;
    action_transition_matrix[2][DOWN] = 1;
    action_transition_matrix[2][RIGHT] = 0;
    action_transition_matrix[2][LEFT] = 1;
    
    action_transition_matrix[3][UP] = 1;
    action_transition_matrix[3][DOWN] = 1;
    action_transition_matrix[3][RIGHT] = 1;
    action_transition_matrix[3][LEFT] = 0;

    action_transition_matrix[4][UP] = 1;
    action_transition_matrix[4][DOWN] = 1;
    action_transition_matrix[4][RIGHT] = 1;
    action_transition_matrix[4][LEFT] = 1;
    
    action_transition_matrix[5][UP] = 1;
    action_transition_matrix[5][DOWN] = 1;
    action_transition_matrix[5][RIGHT] = 0;
    action_transition_matrix[5][LEFT] = 1;
    
    action_transition_matrix[6][UP] = 1;
    action_transition_matrix[6][DOWN] = 0;
    action_transition_matrix[6][RIGHT] = 1;
    action_transition_matrix[6][LEFT] = 0;
    
    action_transition_matrix[7][UP] = 1;
    action_transition_matrix[7][DOWN] = 0;
    action_transition_matrix[7][RIGHT] = 1;
    action_transition_matrix[7][LEFT] = 0;
    
    action_transition_matrix[8][UP] = 0;
    action_transition_matrix[8][DOWN] = 0;
    action_transition_matrix[8][RIGHT] = 0;
    action_transition_matrix[8][LEFT] = 0;

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

    initialize_action_matrix(action_transition_matrix);

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

        double value_function[3][3];

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                value_function[i][j] = 0;
            }
        }

        // int iterations = 100;
        // std::cin  >> iterations;
        // for (int m = 0; m < iterations; m++) {
        //     for (int i = 0; i < grid_size; i++) {
        //         for (int j = 0; j < grid_size; j++) {
        //             // if (i == target_x && j == target_y) {
        //             //     mat_next[i][j] = 0;
        //             // }
        //             // else {
        //                 mat_next[i][j] = max(i, j, grid_size, mat_prev, mean_estimate);
        //             // }
        //         }
        //     }
        //     for (int i = 0; i < grid_size; i++) {
        //         for (int j = 0; j < grid_size; j++) {
        //             mat_prev[i][j] = mat_next[i][j];
        //         }
        //     }
        // }

        max(0, 0, value_function, action_transition_matrix, mean_estimate, 0);

        std::cout << "VALUE FUNCTION: " << std::endl;

    std::cout << "----------------------------" << std::endl;
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            std::cout << "| " << value_function[i][j] << " ";
        }
        std::cout << "|" << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
    }
    return 0;
}