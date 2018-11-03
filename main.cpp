#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <vector>
#include <fstream>

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

int grid_size;
int episode_count;

int start_i;
int start_j;

int target_i;
int target_j;

std::default_random_engine generator;
std::vector<std::vector<double>> mean_estimate;
std::vector<std::vector<grid_element>> grid;
std::vector<std::vector<int>> action_transition_matrix;

double number_generator(double mean, double sigma) {
	std::normal_distribution<double> distribution(mean, sigma);
    return distribution(generator);   
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

void copy_action_matrices(std::vector<std::vector<int>>& mat1, std::vector<std::vector<int>>& mat2) {
    for (int i = 0; i < grid_size*grid_size; i++) {
        for (int j = 0; j < 4; j++) {
            mat1[i][j] = mat2[i][j];
        }
    }
}

double max(int i, int j, std::vector<std::vector<double>>& value_function, std::vector<std::vector<int>>& mat, std::vector<std::vector<double>> mean_estimate, int distance) {

    double gamma = 0.5;
    double beta = 0.7;
    
    std::vector<std::vector<int>> mat1(grid_size*grid_size, std::vector<int>(4));
    std::vector<std::vector<int>> mat2(grid_size*grid_size, std::vector<int>(4));
    std::vector<std::vector<int>> mat4(grid_size*grid_size, std::vector<int>(4));

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
            std::vector<std::vector<int>> mat3(grid_size*grid_size, std::vector<int>(4));
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
        std::vector<std::vector<int>> mat3(grid_size*grid_size, std::vector<int>(4));
        std::vector<std::vector<int>> mat4(grid_size*grid_size, std::vector<int>(4));

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

void initialize_action_matrix(std::vector<std::vector<int>>& action_transition_matrix) {

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

void find_path(std::vector<std::vector<double>> value_function, std::vector<int>& path) {
    int i = 0;
    int j = 0;

    while (i != target_i || j != target_j) {

        cout << "Inside find_path, i: " << i << ", j: " << j << endl;

        bool boundary_element = atBoundary(i, j, grid_size);
        if (boundary_element) {
            bool corner_element = atCorner(i, j, grid_size);
            if (corner_element) {
                if (i == 0 && j == 0) {
                    action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
                    action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
                }
                else if (i == 0 && j == grid_size-1) {
                    action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
                    action_transition_matrix[grid_size*i + j-1][RIGHT] = 0;
                }
                else if (i == grid_size-1 && j == 0) {
                    action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
                    action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
                }
                else if (i == grid_size-1 && j == grid_size-1) {
                    action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
                    action_transition_matrix[grid_size*i + j-1][RIGHT] = 0;
                }
            }
            else {
                if (i == 0) {
                    action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
                    action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
                    action_transition_matrix[grid_size*(i) + j-1][RIGHT] = 0;
                }
                else if (i == grid_size-1) {
                    action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
                    action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
                    action_transition_matrix[grid_size*(i) + j-1][RIGHT] = 0;
                }
                else if (j == 0) {
                    action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
                    action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
                    action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
                }
                else if (j == grid_size-1) {
                    action_transition_matrix[grid_size*i + j-1][RIGHT] = 0;
                    action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
                    action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
                }
            }
        }
        else {
            action_transition_matrix[grid_size*i + j-1][RIGHT] = 0;
            action_transition_matrix[grid_size*(i-1) + j][DOWN] = 0;
            action_transition_matrix[grid_size*(i+1) + j][UP] = 0;
            action_transition_matrix[grid_size*i + j+1][LEFT] = 0;
        }

        double max = 0;
        int action = -1;
        for (int k = 0; k < 4; k++) {
            if (action_transition_matrix[grid_size*i + j][k] == 1) {
                if (k == LEFT) {
                    if (value_function[i][j-1] > max) {
                        max = value_function[i][j-1];
                        action = k;
                    }
                }
                else if (k == RIGHT) {
                    if (value_function[i][j+1] > max) {
                        max = value_function[i][j+1];
                        action = k;
                    }
                }
                else if (k == UP) {
                    if (value_function[i-1][j] > max) {
                        max = value_function[i-1][j];
                        action = k;
                    }
                }
                else {
                    if (value_function[i+1][j] > max) {
                        max = value_function[i+1][j];
                        action = k;
                    }
                }
            }
        }
        if (action == -1) {
            cout << "Invalid Path" << endl;
            if (path[path.size()-1] == LEFT) {
                j++;
                action_transition_matrix[grid_size*i + j][LEFT] = 0;
            }
            else if (path[path.size()-1] == RIGHT) {
                j--;
                action_transition_matrix[grid_size*i + j][RIGHT] = 0;
            }
            else if (path[path.size()-1] == UP) {
                i++;
                action_transition_matrix[grid_size*i + j][UP] = 0;
            }
            else if (path[path.size()-1] == DOWN) {
                i--;
                action_transition_matrix[grid_size*i + j][DOWN] = 0;
            }
            path.pop_back();
            cout << "shifted to i: " << i << " j: " << j << endl;
            continue;
        }
        path.push_back(action);
        if (action == LEFT) {
            j--;
        }
        else if (action == RIGHT) {
            j++;
        }
        else if (action == UP) {
            i--;
        }
        else {
            i++;
        }
    }
}

void stringToInt (string s, std::vector<double>& arr) {
    for (int i = 0; i < s.length(); i++) {
        string n = "";
        while (s[i] != ' ' && i != s.length()) {
            n.push_back(s[i]);
            i++;
        }
        double t = std::stof(n);
        arr.push_back(t);
    }
}

void processInput (string str) {

    string line;
    ifstream myfile (str);
    if (myfile.is_open())
    {
        getline (myfile, line);
        std::vector<double> numbers;
        stringToInt(line, numbers);
        grid_size = numbers[0];

        mean_estimate.resize(grid_size);
        grid.resize(grid_size);

        numbers.clear();
        getline (myfile, line);
        stringToInt(line, numbers);
        start_i = numbers[0];
        start_j = numbers[1];

        numbers.clear();
        getline (myfile, line);
        stringToInt(line, numbers);
        target_i = numbers[0];
        target_j = numbers[1];

        numbers.clear();
        getline (myfile, line);
        stringToInt(line, numbers);
        episode_count = numbers[0];

        for (int i = 0; i < grid_size; i++) {
            numbers.clear();
            getline (myfile, line);
            stringToInt(line, numbers);
            grid[i].resize(grid_size);     
            for (int j = 0; j < grid_size; j++) {
                grid[i][j].mean = numbers[j];
            }
        }

        for (int i = 0; i < grid_size; i++) {
            numbers.clear();
            getline (myfile, line);
            stringToInt(line, numbers);
            for (int j = 0; j < grid_size; j++) {
                grid[i][j].sigma = numbers[j];
            }
        }

        myfile.close();
    }
    else cout << "Unable to open file"; 

    cout << "Inputs processed successfully" << endl;
}

int main() {

    processInput("/home/ironman/grid-world-rl/inputs.txt");

    cout << "grid size: " << grid_size << endl;
    cout << "start_i: " << start_i << " start_j: " << start_j << endl;
    cout << "target_i: " << target_i << " target_j: " << target_j << endl;
    cout << "episode_count: " << episode_count << endl;

    action_transition_matrix.resize(grid_size*grid_size, std::vector<int>(4));

    initialize_action_matrix(action_transition_matrix);

    cout << "Intialised the action matrix successfully" << endl;

    int count = 0;

    std::vector<std::vector<double>> mat_prev(grid_size, std::vector<double>(grid_size));
    std::vector<std::vector<double>> mat_next(grid_size, std::vector<double>(grid_size));

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

        std::vector<std::vector<double>> value_function(grid_size, std::vector<double>(grid_size));
        
        max(0, 0, value_function, action_transition_matrix, mean_estimate, 0);

        print(value_function);

        cout << "Value function calculated" << endl;

        std::vector<int> path;

        std::ofstream f;
        f.open("/home/ironman/grid-world-rl/results.txt");

        initialize_action_matrix(action_transition_matrix);

        cout << "Initialised the action matrix again" << endl;

        find_path(value_function, path);

        cout << "Path Found" << endl;

        if (f.is_open()) {
            f << grid_size << endl;
            for (int i = 0; i < grid_size; i++) {
                for (int j = 0; j < grid_size; j++) {
                    f << mean_estimate[i][j] << " "; 
                }
                f << endl;
            }
            for (int i = 0; i < path.size(); i++) {
                f << path[i] << " ";
            }
            f << endl;
        }
        else {
            cout << "unable to open the file" << endl;
        }
    }
    return 0;
}