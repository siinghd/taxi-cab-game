# Taxi cab game

Complete : [Link to PDF](./project.pdf)

brief:

Project Description:

A simulation of taxis moving within a city grid, responding to service requests.
City Map (Section 1.1):

The city streets are represented by a grid with parameters SO_WIDTH and SO_HEIGHT.
There are inaccessible cells (SO_HOLES) randomly placed within the grid with specific constraints.
Taxi Service Requests (Section 1.2):

Service requests are originated by SO_SOURCES processes, each linked to a specific grid cell.
Requests are characterized by a start cell and a randomly determined destination cell.
Taxi Movement (Section 1.3):

Taxis are initially placed at random positions and can only move horizontally or vertically.
A 'stalemate' situation is handled with specific rules, such as a taxi terminating after a timeout.
Simulation Start and Termination (Section 1.4):

The simulation starts after all source and taxi processes are initialized.
The duration of the simulation is determined by a set time limit.
Output (Section 1.5):

A "master" process collects statistics and prints the occupancy status of cells every second.
At the end of the simulation, various statistics are printed out, such as the number of trips completed, aborted, and the most traveled taxi.
Configuration (Section 2):

Runtime parameters include the number of taxis, source points, inaccessible cells, and the capacity of each cell.
The project must function correctly with various feasible parameters.
Implementation Requirements (Section 3):

The project should be modular, compiled with make, maximize process concurrency, and properly deallocate IPC resources.
