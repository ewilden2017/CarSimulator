# Car Simulator

CarSim is a basic program that I wrote as an exercise in basic rendering, and implementing the [NEAT](https://www.cs.ucf.edu/~kstanley/neat.html) library.

## Building
Download the code, and the submodules, with git:

    git clone --recursive https://github.com/ewilden2017/CarSimulator
    //or, to pull submodles after downloading:
    git clone https://github.com/ewilden2017/CarSimulator
    cd CarSimulator
    git submodule update --init --recursive

Then, build the project with cmake:

    mkdir build
    cd build
    cmake ..
    make

Use `-DBUILD_THIRD_PARTY=OFF` to search the system for the needed libraries. Otherwise, it will build them from source.

## Usage
Run the code as `./CarSim [Speed Multiplier]`. The speed multiplier is multiplied by deltaTime to speed up the simulation.

There are a few basic keyboard inputs:
* ESC - closes the window
* W,A,S,D - moves the camera
* Q,E - cycles car selection
* Z,X - zoom in and out
