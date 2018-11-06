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

std::default_random_engine generator; // random number generator
std::vector<std::vector<double>> mean_estimate; // estimate of the mean of all the grid elements
std::vector<std::vector<grid_element>> grid; // grid, containing sigma and mean for each grid element 
std::vector<std::vector<int>> action_transition_matrix; // probability of taking an action in an state: UP, DOWN, LEFT or RIGHT
std::vector<std::vector<double>> value_function;

double number_generator(double mean, double sigma) { // generates number according to the given gaussian distribution
	std::normal_distribution<double> distribution(mean, sigma);
    return distribution(generator);   
}

double maximum(double a, double b) {
    if (a > b) 
        return a;
    else 
        return b;
}

// for calculating the value function for each state, recursively, following bellman optimality equation
double max(int i, int j, std::vector<std::vector<int>> mat, int distance) {

    double gamma = 0.5; // dicount factor
    double beta = 0.7; // path length factor

    mat[i][j] = 1; // to prevent the agent from visiting the already visited location

    cout <<"value function, i: " << i << ", j: " << j << endl;
    
    if (target_i == i && target_j == j) { // if we reach terminal state
        value_function[i][j] = mean_estimate[i][j];
        return mean_estimate[i][j];
    }
    else {
        
        double v1 = 0;
        double v2 = 0;
        double v3 = 0;
        double v4 = 0;

        // calculating value faction for the two possible actions 
        if (j+1 < grid_size && mat[i][j+1] == 0) {
            v1 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j+1, mat, distance+1);
            
        }
        if (i+1 < grid_size && mat[i+1][j] == 0) {
            v2 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i+1, j, mat, distance+1);
        }
        if (i-1 >= 0 && mat[i-1][j] == 0) {
            v3 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i-1, j, mat, distance+1);
        }
        if (j-1 >= 0 && mat[i][j-1] == 0) {
            v4 = mean_estimate[i][j] - beta*(distance + 1) + gamma*max(i, j-1, mat, distance+1);
        }

        double temp = maximum(maximum(v1, v2), maximum(v3, v4));
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

void find_path(std::vector<std::vector<int>> mat, std::vector<int>& path) {
    int i = 0;
    int j = 0;

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

int main() {

    processInput("/home/ironman/grid-world-rl/inputs.txt");

    cout << "grid size: " << grid_size << endl;
    cout << "start_i: " << start_i << " start_j: " << start_j << endl;
    cout << "target_i: " << target_i << " target_j: " << target_j << endl;
    cout << "episode_count: " << episode_count << endl;

    action_transition_matrix.resize(grid_size, std::vector<int>(grid_size));

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

        value_function.resize(grid_size, std::vector<double>(grid_size));
        
        max(0, 0, action_transition_matrix, 0); // for calculating value_function

        print(value_function);

        cout << "Value function calculated" << endl;

        std::vector<int> path;

        std::ofstream f;
        f.open("/home/ironman/grid-world-rl/results.txt");

        cout << "Initialised the action matrix again" << endl;

        find_path(action_transition_matrix, path); // to find the optimal path by acting greedy on the value function

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