# Advanced Operating Systems project: Accelerometer Game
Project repository for the Advanced Operating Systems course of M.Sc. Computer Science and Engineering at Politecnico di Milano.

You can find the code documentation at [this](https://marcoieni.github.io/accelerometer_game) link.

## Assignment
### Description
Design a game for 2 to 4 players, where each player should tap the accelerometer once and then again after 5 seconds.
The player who gets closer to 5 seconds wins.

The project is built for the [STM32F4-DISCOVERY board](https://www.st.com/en/evaluation-tools/stm32f4discovery.html) on top of [Miosix RTOS](http://miosix.org/).

### Goal
The project requires to implement tap detection for
the accelerometer and implement the game logic

## Installation
Miosix kernel is used as submodule as specified in [this](https://miosix.org/wiki/index.php?title=Miosix_and_git_workflow#Setting_up_an_out_of_git_tree_project) wiki page.

In order to build the project, first clone the repository.

### Submodule Initialization
Initialize the submodule:

```
git submodule init
```

If your git authentication method is https instead of ssh you need to open `.git/config` with your favourite text editor and inside `submodule "miosix-kernel"` change the url to `https://github.com/fedetft/miosix-kernel.git`

### Submodule Update

Update the submodule:

```
git submodule update --remote --rebase
```

## Group members
* Marco Ieni
* Simone Perriello
