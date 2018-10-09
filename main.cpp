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

int grid_size = 4;

int target_i;
int target_j;

int action_transition_matrix[16][4];

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
    for (int i = 0; i < grid_size*grid_size; i++) {
        for (int j = 0; j < 4; j++) {
            mat1[i][j] = mat2[i][j];
        }
    }
}

double max(int i, int j, std::vector<std::vector<double>>& value_function, int mat[][4], std::vector<std::vector<double>> mean_estimate, int distance) {

    double gamma = 0.5;
    double beta = 0.7;
    
    int mat1[grid_size*grid_size][4];
    int mat2[grid_size*grid_size][4];
    int mat4[grid_size*grid_size][4];

    copy_action_matrices(mat1, mat);
    copy_action_matrices(mat2, mat);

    bool boundary_element = atBoundary(i, j, grid_size);

    if (boundary_element) {
        bool corner_element = atCorner(i, j, grid_size);

        if (corner_element) {
            if (i == 0 && j == 0) {
                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*(i+1) + j][UP] = 0;
                    mat1[grid_size*(i+1) + j][UP] = 0;

                    double v1 = 0;
                    double v2 = 0;
                    if (mat[grid_size*i+j][RIGHT] == 1) {
                        v1 = mean_estimate[0][0] - beta*(distance + 1) + gamma*max(i, j+1, value_function,  mat1, mean_estimate, distance+1);
                        
                    }
                    if (mat[grid_size*i+j][DOWN] == 1) {
                        v2 = mean_estimate[0][0] - beta*(distance + 1) + gamma*max(i+1, j, value_function,  mat2, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, v2);
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
            else if (i == 0 && j == grid_size - 1) {

                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j-1][RIGHT] = 0;
                    mat1[grid_size*(i+1) + j][UP] = 0;
                    mat2[grid_size*i + j-1][RIGHT] = 0;
                    mat2[grid_size*(i+1) + j][UP] = 0;

                    double v1 = 0;
                    double v2 = 0;

                    if (mat[grid_size*i + j][LEFT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][DOWN] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat2, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, v2);
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
            else if (i == grid_size - 1 && j == 0) {
                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j+1][LEFT] = 0;
                    mat1[grid_size*(i-1) + j][DOWN] = 0;
                    mat2[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*(i-1) + j][DOWN] = 0;

                    double v1 = 0;
                    double v2 = 0;

                    if (mat[grid_size*i + j][RIGHT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][UP] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, v2);
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
            else if (i == grid_size-1 && j == grid_size-1) {
                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j-1][RIGHT] = 0;
                    mat1[grid_size*(i-1) + j][DOWN] = 0;
                    mat2[grid_size*i + j-1][RIGHT] = 0;
                    mat2[grid_size*(i-1) + j][DOWN] = 0;

                    double v1 = 0;
                    double v2 = 0;

                    if (mat[grid_size*i + j][LEFT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][UP] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, v2);
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
        }
        else {
            int mat3[grid_size*grid_size][4];
            copy_action_matrices(mat3, mat);

            if (j == 0) {

                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j+1][LEFT] = 0;
                    mat1[grid_size*(i-1) + j][DOWN] = 0;
                    mat1[grid_size*(i+1) + j][UP] = 0;
                    mat2[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*(i-1) + j][DOWN] = 0;
                    mat2[grid_size*(i+1) + j][UP] = 0;
                    mat3[grid_size*i + j+1][LEFT] = 0;
                    mat3[grid_size*(i-1) + j][DOWN] = 0;
                    mat3[grid_size*(i+1) + j][UP] = 0;

                    double v1 = 0;
                    double v2 = 0;
                    double v3 = 0;

                    if (mat[grid_size*i + j][RIGHT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][UP] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][DOWN] == 1) {
                        v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, maximum(v2, v3));
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
            else if (i == grid_size - 1) {
                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j+1][LEFT] = 0;
                    mat1[grid_size*(i-1) + j][DOWN] = 0;
                    mat1[grid_size*(i) + j-1][RIGHT] = 0;
                    mat2[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*(i-1) + j][DOWN] = 0;
                    mat2[grid_size*(i) + j-1][RIGHT] = 0;
                    mat3[grid_size*i + j+1][LEFT] = 0;
                    mat3[grid_size*(i-1) + j][DOWN] = 0;
                    mat3[grid_size*(i) + j-1][RIGHT] = 0;

                    double v1 = 0;
                    double v2 = 0;
                    double v3 = 0;

                    if (mat[grid_size*i + j][RIGHT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][UP] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][LEFT] == 1) {
                        v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat3, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, maximum(v2, v3));
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
            else if (i == 0) {
                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j+1][LEFT] = 0;
                    mat1[grid_size*(i+1) + j][UP] = 0;
                    mat1[grid_size*(i) + j-1][RIGHT] = 0;

                    mat2[grid_size*i + j+1][LEFT] = 0;
                    mat2[grid_size*(i+1) + j][UP] = 0;
                    mat2[grid_size*(i) + j-1][RIGHT] = 0;

                    mat3[grid_size*i + j+1][LEFT] = 0;
                    mat3[grid_size*(i+1) + j][UP] = 0;
                    mat3[grid_size*(i) + j-1][RIGHT] = 0;

                    double v1 = 0;
                    double v2 = 0;
                    double v3 = 0;

                    if (mat[grid_size*i + j][RIGHT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][LEFT] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat2, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][DOWN] == 1) {
                        v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, maximum(v2, v3));
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }                
            }
            else if (j == grid_size - 1) {

                if (target_i == i && target_j == j) {
                    value_function[i][j] = mean_estimate[i][j];
                    return mean_estimate[i][j];
                }
                else {
                    mat1[grid_size*i + j-1][RIGHT] = 0;
                    mat1[grid_size*(i-1) + j][DOWN] = 0;
                    mat1[grid_size*(i+1) + j][UP] = 0;

                    mat2[grid_size*i + j-1][RIGHT] = 0;
                    mat2[grid_size*(i-1) + j][DOWN] = 0;
                    mat2[grid_size*(i+1) + j][UP] = 0;

                    mat3[grid_size*i + j-1][RIGHT] = 0;
                    mat3[grid_size*(i-1) + j][DOWN] = 0;
                    mat3[grid_size*(i+1) + j][UP] = 0;

                    double v1 = 0;
                    double v2 = 0;
                    double v3 = 0;

                    if (mat[grid_size*i + j][LEFT] == 1) {
                        v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][UP] == 1) {
                        v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
                    }
                    if (mat[grid_size*i + j][DOWN] == 1) {
                        v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
                    }

                    double temp = maximum(v1, maximum(v2, v3));
                    if (temp > value_function[i][j]) {
                        value_function[i][j] = temp;
                    }
                    return temp;
                }
            }
        }
    }
    else {
        int mat3[grid_size*grid_size][4];
        int mat4[grid_size*grid_size][4];

        copy_action_matrices(mat3, mat);
        copy_action_matrices(mat4, mat);

        mat1[grid_size*i + j-1][RIGHT] = 0;
        mat1[grid_size*(i-1) + j][DOWN] = 0;
        mat1[grid_size*(i+1) + j][UP] = 0;
        mat1[grid_size*i + j+1][LEFT] = 0;

        mat2[grid_size*i + j-1][RIGHT] = 0;
        mat2[grid_size*(i-1) + j][DOWN] = 0;
        mat2[grid_size*(i+1) + j][UP] = 0;
        mat2[grid_size*i + j+1][LEFT] = 0;
        
        mat3[grid_size*i + j-1][RIGHT] = 0;
        mat3[grid_size*(i-1) + j][DOWN] = 0;
        mat3[grid_size*(i+1) + j][UP] = 0;
        mat3[grid_size*i + j+1][LEFT] = 0;
        
        mat4[grid_size*i + j-1][RIGHT] = 0;
        mat4[grid_size*(i-1) + j][DOWN] = 0;
        mat4[grid_size*(i+1) + j][UP] = 0;
        mat4[grid_size*i + j+1][LEFT] = 0;

        double v1 = 0;
        double v2 = 0;
        double v3 = 0;
        double v4 = 0;

        if (mat[grid_size*i + j][LEFT] == 1) {
            v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, value_function, mat1, mean_estimate, distance+1);
        }
        if (mat[grid_size*i + j][UP] == 1) {
            v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, value_function, mat2, mean_estimate, distance+1);
        }
        if (mat[grid_size*i + j][DOWN] == 1) {
            v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, value_function, mat3, mean_estimate, distance+1);
        }
        if (mat[grid_size*i + j][RIGHT] == 1) {
            v4 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, value_function, mat4, mean_estimate, distance+1); 
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

    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            bool boundary_element = atBoundary(i, j, grid_size);
            if (boundary_element) {
                bool corner_element = atCorner(i, j, grid_size);
                if (corner_element) {
                    if (i == 0 && j == 0) {
                        action_transition_matrix[grid_size*i + j][UP] = 0;
                        action_transition_matrix[grid_size*i + j][DOWN] = 1;
                        action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                        action_transition_matrix[grid_size*i + j][LEFT] = 0;
                    }
                    else if (i == 0 && j == grid_size-1) {
                        action_transition_matrix[grid_size*i + j][UP] = 0;
                        action_transition_matrix[grid_size*i + j][DOWN] = 1;
                        action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        action_transition_matrix[grid_size*i + j][LEFT] = 1;
                    }
                    else if (i == grid_size-1 && j == 0) {
                        action_transition_matrix[grid_size*i + j][UP] = 1;
                        action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                        action_transition_matrix[grid_size*i + j][LEFT] = 0;
                    }
                    else if (i == grid_size-1 && j == grid_size-1) {
                        action_transition_matrix[grid_size*i + j][UP] = 1;
                        action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        action_transition_matrix[grid_size*i + j][LEFT] = 1;
                    }
                }
                else {
                    if (i == 0) {
                        if (target_j == grid_size-1) {
                            action_transition_matrix[grid_size*i + j][LEFT] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][LEFT] = 1;
                        }
                        if (target_j == 0) {
                            action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                        }
                        action_transition_matrix[grid_size*i + j][UP] = 0;
                        action_transition_matrix[grid_size*i + j][DOWN] = 1;                        
                    }
                    else if (i == grid_size-1) {
                        action_transition_matrix[grid_size*i + j][UP] = 1;
                        action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        if (target_j == grid_size-1) {
                            action_transition_matrix[grid_size*i + j][LEFT] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][LEFT] = 1;
                        }
                        if (target_j == 0) {
                            action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                        }
                    }
                    else if (j == 0) {
                        if (target_i == grid_size-1) {
                            action_transition_matrix[grid_size*i + j][UP] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][UP] = 1;
                        }
                        if (target_i == 0) {
                            action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][DOWN] = 1;
                        }
                        action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                        action_transition_matrix[grid_size*i + j][LEFT] = 0;
                    }
                    else if (j == grid_size-1) {
                        if (target_i == grid_size-1) {
                            action_transition_matrix[grid_size*i + j][UP] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][UP] = 1;
                        }
                        if (target_i == 0) {
                            action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        }
                        else {
                            action_transition_matrix[grid_size*i + j][DOWN] = 1;
                        }
                        action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        action_transition_matrix[grid_size*i + j][LEFT] = 1;
                    }
                }
            }
            else {
                action_transition_matrix[grid_size*i + j][UP] = 1;
                action_transition_matrix[grid_size*i + j][DOWN] = 1;
                action_transition_matrix[grid_size*i + j][RIGHT] = 1;
                action_transition_matrix[grid_size*i + j][LEFT] = 1;
            }
        }
    }
}

void find_path(std::vector<std::vector<double>> value_function, std::vector<std::vector<int>>& path) {
    int i = 0;
    int j = 0;

    while (i != target_i || j != target_j) {
        double max = 0;
        int action = -1;
        int k;
        for (int k = 0; k < 4; k++) {
            if (action_transition_matrix[grid_size*i + j][k] == 1) {
                if (k == LEFT) {
                    if (value_function[i][j-1] > max) {
                        max = value_function[i][j-1];
                    }
                }
                else if (k == RIGHT) {
                    if (value_function[i][j+1] > max) {
                        max = value_function[i][j+1];
                    }
                }
                else if (k == UP) {
                    if (value_function[i-1][j] > max) {
                        max = value_function[i-1][j];
                    }
                }
                else {
                    if (value_function[i+1][j] > max) {
                        max = value_function[i+1][j];
                    }
                }
            }
        }
        path[i][j] = k;
        if (k == LEFT) {
            j--;
        }
        else if (k == RIGHT) {
            j++;
        }
        else if (k == UP) {
            i--;
        }
        else {
            i++;
        }
    }
}

int main() {

    // std::cin >> grid_size;
    int episode_count = 0;
    // std::cin >> episode_count;
    episode_count = 1;
    std::vector<std::vector<grid_element>> grid(grid_size);

    for (int i = 0; i < grid_size; i++) {
        grid[i].resize(grid_size);
    }

    target_i = 3;
    target_j = 3;

    initialize_action_matrix(action_transition_matrix);

    grid[0][0].mean = 7; grid[0][0].sigma = 0.3;
    grid[0][1].mean = 8; grid[0][1].sigma = 0.3;
    grid[0][2].mean = 16; grid[0][2].sigma = 0.3;
    grid[0][3].mean = 19; grid[0][3].sigma = 0.3;

    grid[1][0].mean = 6; grid[1][0].sigma = 0.3;
    grid[1][1].mean = 5; grid[1][1].sigma = 0.3;
    grid[1][2].mean = 6; grid[1][2].sigma = 0.3;
    grid[1][3].mean = 18; grid[1][3].sigma = 0.3;

    grid[2][0].mean = 7; grid[2][0].sigma = 0.3; 
    grid[2][1].mean = 10; grid[2][1].sigma = 0.3;
    grid[2][2].mean = 1; grid[2][2].sigma = 0.3;
    grid[2][3].mean = 3; grid[2][3].sigma = 0.3;

    grid[3][0].mean = 4; grid[3][0].sigma = 0.3;
    grid[3][1].mean = 2; grid[3][1].sigma = 0.3;
    grid[3][2].mean = 11; grid[3][2].sigma = 0.3;
    grid[3][3].mean = 17; grid[3][3].sigma = 0.3;

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

        std::cout << "MEAN ESTIMATE: " << std::endl;
        print(mean_estimate);

        std::vector<std::vector<double>> value_function(grid_size);


        for (int i = 0; i < grid_size; i++) {
            value_function[i].resize(grid_size);
        }
        
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

        std::vector<std::vector<int>> path(grid_size);

        for (int i = 0; i < grid_size; i++) {
            path[i].resize(grid_size);
            for (int j = 0; j < grid_size; j++) {
                path[i][j] = -1;
            }
        }

        find_path(value_function, path);
    }
    return 0;
}