
# C compiler for PHP




## Overview

This project is a C implementation of a compiler for the stripped-down PHP language. It is a school project for Formal Languages and Compilers. The goal of this project is to understand how compilers work and to learn how to create them.

Project assignment `./doc/Task 2022.pdf`


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.


### Prerequisites

* A C compiler, such as GCC or Clang

### Compilation

* Clone the repository: `git clone https://github.com/Tricked111/C-compiler-to-PHP.git`
* Navigate to the project directory: `cd C-compiler-to-PHP`
* Compile the source code: `make`

### Usage
* Write the progrm in `./test_files/test.php`
* Run `make test`

## Built With
* C programming language

## Results

Percentage rating of compiler modules: 
* Lexical analysis (error detection): 89% (147/165)
* Syntactic analysis (error detection): 100% (208/208)
* Semantic analysis (error detection): 88% (320/363)
* Interpretation of translated code (basic): 86% (247/285)
* Interpretation of translated code (expressions, built-in functions): 81% (150/185)
* Interpretation of translated code (complex): 88% (324/370)

Total without extension: 89 % (1396/1576)

## Note
Please ensure that you are using the latest version of C as some of the features in the code may not be compatible with older version of C.
