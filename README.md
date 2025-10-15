## RCPSP Solver (2011) — Talbot ILP Approach

This repository showcases an early implementation (circa 2011) of a solver for the Resource-Constrained Project Scheduling Problem (RCPSP), based on [Talbot’s integer programming formulation](https://pubsonline.informs.org/doi/10.1287/mnsc.28.10.1197).

The solver was developed to provide a baseline for comparing the integer programming approach against the method proposed in  
[Alves, R., Alvelos, F., & Sousa, S. D. (2013). *Resource Constrained Project Scheduling with General Precedence Relations Optimized with SAT*.](https://link.springer.com/chapter/10.1007/978-3-642-40669-0_18)

Originally developed using IBM CPLEX, this version is no longer maintained due to expired licensing, but it remains a good illustration of my early work in optimization modeling, integer programming, and solver design.

I’m making the code public to preserve the technical value of this project, to contribute to the community as a didactic resource, and to provide a basis for modern reimplementations using open-source solvers.

Although the code may not compile or run out of the box, it reflects the modeling practices, structure, and design mindset I applied at the time.

This project was originally developed with:

* C++ (GCC 4.x)
* IBM ILOG CPLEX 12.x
* Make

Since CPLEX is proprietary software, this code is now provided for reference only, though you can adapt it to work with open-source MILP solvers by adjusting the solver API calls in the model setup.

## Usage
To build:
```
make
```
Run the solver with:
```
./rcpsp-ilp-talbot <project_instance_file>
```

Example:
```
./rcpsp-ilp-talbot example.pdsl
```

The solver takes as input a project instance defined in the PDSL format
(Project Description Specification Language), a simple scripting language
specifically designed for this project to describe task durations, precedence
constraints, and resource requirements.
For further details about this language, refer to this [PDSL Tutorial](https://github.com/rmfalves/rcpsp-gpr-sat/blob/main/PDSL_TUTORIAL.md) 
and the examples provided in this [examples directory](https://github.com/rmfalves/rcpsp-gpr-sat/tree/main/examples).


## License

This code is released under **The Unlicense**, placing it in the public domain.

You are free to use, modify, distribute, or re-implement this code for any purpose — academic, commercial, or educational — with no restrictions.
See the [`LICENSE`](./LICENSE) file for details.


## Acknowledgements

The original implementation was inspired by the [work of Talbot (1982) on ILP formulations for project scheduling](https://pubsonline.informs.org/doi/10.1287/mnsc.28.10.1197) and developed as part of an early research effort in exact optimization methods.
