# grid-world-rl

This repository contains the implementation of bellman optimality equation on a grid-world of various sizes by taking path length as a factor in the reward function, to calculate the value function. `inputs.txt` takes all the inputs like `grid_size`, `start_point`, `end_point` and the stochastic `grid` matrix. The result is put in `results.txt`, which contains the value function, and the path as a sequence of integers. `main.cpp` contains the implementation.

## Build

For building the code follow the given instruction:

```
// change the location of 'inputs.txt' in 'main.cpp' wrt your system

// in the project source directory

mkdir build
cd build
cmake ..
make
```

This will create an executable `main` in the `build` dir. To run the program just do `./main`.
To change input one can edit the file `inputs.txt`.

## Algorithm used

Let s<sub>1</sub>, s<sub>2</sub>, s<sub>3</sub> and s<sub>4</sub> be the states which can be accessed from the current state s, then the algorithm will update the value of s in the following way:

's = r(s) - beta*(length of the path till s) + max (s<sub>1</sub>, s<sub>2</sub>, s<sub>3</sub>, s<sub>4</sub>)'

For which the value of all s<sub>i</sub> have to calculated, so the algorithm goes on recursively and in this way the value function is calculated for every element in the grid.

`max(...)` function in `main.cpp` is called with `intial point` as an argument which results in updation of all the element in the grid.
