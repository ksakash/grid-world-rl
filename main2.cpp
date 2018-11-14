#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <cmath>
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
int iterations;

int start_i;
int start_j;

int target_i;
int target_j;

std::default_random_engine generator; // random number generator
std::vector<std::vector<double>> mean_estimate; // estimate of the mean of all the grid elements
std::vector<std::vector<grid_element>> grid; // grid, containing sigma and mean for each grid element 
std::vector<std::vector<int>> action_transition_matrix; // probability of taking an action in an state: UP, DOWN, LEFT or RIGHT
std::vector<std::vector<double>> value_function;
std::vector<std::vector<int>> path_length;

double number_generator(double mean, double sigma) { // generates number according to the given gaussian distribution
	std::normal_distribution<double> distribution(mean, sigma);
    return distribution(generator);   
}

void iterate(vector<vector<double>>& mat_prev, vector<vector<double>>& mat_next) {
    
    double gamma = 0.63; // dicount factor
    double beta = 0.10; // path length factor

    double v1 = 0;
    double v2 = 0;
    double v3 = 0;
    double v4 = 0;

    for (int i = 0; i < grid_size; i++) {
                
        for (int j = 0; j < grid_size; j++) {
            // calculating value faction for the two possible actions
            int action = -1;
            double max = 0;

            if (j+1 < grid_size && action_transition_matrix[i*grid_size + j+1][RIGHT] == 1) {
                v1 = mat_prev[i][j+1];
                if (v1 > max) { 
                    max = v1;
                    action = RIGHT;
                }
            }
            if (i+1 < grid_size && action_transition_matrix[(i+1)*grid_size + j][DOWN] == 1) {
                v2 = mat_prev[i+1][j];
                if (v2 > max) { 
                    max = v2;
                    action = DOWN;
                }
            }
            if (i-1 >= 0 && action_transition_matrix[(i-1)*grid_size + j][UP] == 1) {
                v3 = mat_prev[i-1][j];
                if (v3 > max) { 
                    max = v3;
                    action = UP;
                }
            }
            if (j-1 >= 0 && action_transition_matrix[i*grid_size + j-1][LEFT] == 1) {
                v4 = mat_prev[i][j-1];
                if (v4 > max) { 
                    max = v4;
                    action = LEFT;
                }
            }
            mat_next[i][j] = mean_estimate[i][j] - beta*path_length[i][j] + gamma*max;
            
            if (action == LEFT) {
                if (path_length[i][j-1] <= path_length[i][j] + 1)
                    path_length[i][j-1] = path_length[i][j] + 1;
            }
            if (action == RIGHT) {
                
                if (path_length[i][j+1] <= path_length[i][j] + 1)
                    path_length[i][j+1] = path_length[i][j] + 1;
            }
            if (action == UP) {
                if (path_length[i-1][j] <= path_length[i][j] + 1)
                    path_length[i-1][j] = path_length[i][j] + 1;
            }
            if (action == DOWN) {
                if (path_length[i+1][j] <= path_length[i][j] + 1)
                    path_length[i+1][j] = path_length[i][j] + 1;
            }
        }
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

void print(std::vector<std::vector<int>> a) {
    std::cout << "----------------------------" << std::endl;
    for (int i = 0; i < a.size(); i++) {
        for (int j = 0; j < a[i].size(); j++) {
            std::cout << "| " << a[i][j] << " ";
        }
        std::cout << "|" << std::endl;
        std::cout << "----------------------------" << std::endl;
    }
}

void find_path(std::vector<std::vector<int>> mat, std::vector<int>& path) {
    int i = start_i;
    int j = start_j;

    while (i != target_i || j != target_j) {

        mat[i][j] = 1;

        cout << "Inside find_path, i: " << i << ", j: " << j << endl;

        double max = 0;
        int action = -1;

        if (j-1 >= 0 && mat[i][j-1] == 0) {
            if (value_function[i][j-1] > max) {
                max = value_function[i][j-1];
                action = LEFT;
            }
        }
        if (j+1 < grid_size && mat[i][j+1] == 0) {
            if (value_function[i][j+1] > max) {
                max = value_function[i][j+1];
                action = RIGHT;
            }
        }
        if (i-1 >= 0 && mat[i-1][j] == 0) {
            if (value_function[i-1][j] > max) {
                max = value_function[i-1][j];
                action = UP;
            }
        }
        if (i+1 < grid_size && mat[i+1][j] == 0) {
            if (value_function[i+1][j] > max) {
                max = value_function[i+1][j];
                action = DOWN;
            }
        }

        if (action == -1) {
            cout << "Invalid Path" << endl;
            if (path[path.size()-1] == LEFT) {
                j++;
            }
            else if (path[path.size()-1] == RIGHT) {
                j--;
            }
            else if (path[path.size()-1] == UP) {
                i++;
            }
            else if (path[path.size()-1] == DOWN) {
                i--;    
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

void processInput (string str) { // for taking processing from a file

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

// hard coded for the current intial and final point
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
                        action_transition_matrix[grid_size*i + j][LEFT] = 0;
                    }
                    else if (i == grid_size-1 && j == 0) {
                        action_transition_matrix[grid_size*i + j][UP] = 0;
                        action_transition_matrix[grid_size*i + j][DOWN] = 0;
                        action_transition_matrix[grid_size*i + j][RIGHT] = 0;
                        action_transition_matrix[grid_size*i + j][LEFT] = 0;
                    }
                    else if (i == grid_size-1 && j == grid_size-1) {
                        action_transition_matrix[grid_size*i + j][UP] = 0;
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

int main() {

    processInput("/home/ironman/grid-world-rl/inputs.txt");

    iterations = 30;

    cout << "iterations: " << iterations << endl;
    cout << "grid size: " << grid_size << endl;
    cout << "start_i: " << start_i << " start_j: " << start_j << endl;
    cout << "target_i: " << target_i << " target_j: " << target_j << endl;
    cout << "episode_count: " << episode_count << endl;

    action_transition_matrix.resize(grid_size*grid_size, std::vector<int>(4));
    path_length.resize(grid_size, std::vector<int>(grid_size));
    initialize_action_matrix(action_transition_matrix);

    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            path_length[i][j] = abs(i-start_i) + abs(j-start_j);
        }
    }

    cout << "Intialised the action matrix successfully" << endl;

    int count = 0;

    std::vector<std::vector<double>> mat_prev(grid_size, std::vector<double>(grid_size));
    std::vector<std::vector<double>> mat_next(grid_size, std::vector<double>(grid_size));

    mean_estimate.resize(grid_size);
    value_function.resize(grid_size, std::vector<double>(grid_size));
    
    for (int k = 0; k < episode_count; k++) {
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

        value_function.resize(grid_size, std::vector<double>(grid_size));
        
        for (int j = 0; j < iterations; j++) {
            iterate(mat_prev, mat_next);
            mat_prev = mat_next;
        }

        cout << "Value function calculated" << endl;

        value_function = mat_prev;

        print(value_function);

        std::vector<int> path;

        std::ofstream f;
        f.open("/home/ironman/grid-world-rl/results0.txt");

        cout << "Initialised the action matrix again" << endl;

        std::vector<std::vector<int>> mat(grid_size, std::vector<int>(grid_size));
        find_path(mat, path); // to find the optimal path by acting greedy on the value function

        cout << "Path Found" << endl;

        cout << "Path size: " << path.size() << endl;

        print(path_length);

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