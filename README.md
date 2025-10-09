## RCPSP Solver (2011) — Talbot ILP Approach

This repository showcases an early implementation (circa 2011) of a solver for the Resource-Constrained Project Scheduling Problem (RCPSP), based on Talbot’s integer programming formulation.

Originally developed using IBM CPLEX, this version is no longer maintained due to expired licensing, but it remains a good illustration of my early work in optimization modeling, integer programming, and solver design.

I’m making the code public to preserve the technical value of this project, to contribute to the community as a didactic resource, and to provide a basis for modern reimplementations using open-source solvers.

Although the code may not compile or run out of the box, it reflects the modeling practices, structure, and design mindset I applied at the time.

This project was originally developed with:

* C++ (GCC 4.x)
* IBM ILOG CPLEX 12.x
* Make

Since CPLEX is proprietary software, this code is now provided **for reference only**, though you can adapt it to work with open-source MILP solvers by adjusting the solver API calls in the model setup.


### License

This code is released under **The Unlicense**, placing it in the **public domain**.

You are free to **use, modify, distribute, or re-implement** this code for any purpose — academic, commercial, or educational — with **no restrictions**.
See the [`LICENSE`](./LICENSE) file for details.


### Acknowledgements

The original implementation was inspired by the work of **Talbot (1982)** on ILP formulations for project scheduling and developed as part of an early research effort in exact optimization methods.
