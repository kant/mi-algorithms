#Machine Intelligence Core: Algorithms

Status of Travis Continuous Integration:

[![Build Status](https://travis.ibm.com/tkornut/mi-algorithms.svg?token=9XHfj7QaSbmFqHsyaQes&branch=master)](https://travis.ibm.com/tkornut/mi-algorithms)

## Description

A subproject of Machine Intelligence Core framework.

The project contains core types and several machine intelligence algorithms, such as HTM, RBM accompanied by some useful modules and functions.

## MIC dependencies

   * MIToolchain - the core of MIC framework.

## External dependencies

Additionally it depends on the following external libraries:
   * Boost - library of free (open source) peer-reviewed portable C++ source libraries.
   * Eigen - a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
   * OpenBlas (optional) - An optimized library implementing BLAS routines. If present - used for fastening operation on matrices.
   * Doxygen (optional) - Tool for generation of documentation.
   * GTest (optional) - Framework for unit testing.

### Installation of the dependencies/required tools

On Linux (Ubuntu 14.04): 

    sudo apt-get install git cmake doxygen libboost1.54-all-dev libeigen3-dev

On Mac (OS X 10.14): 

    brew install git cmake doxygen boost eigen

To install GTest on MAC OS X:

    brew install --HEAD https://gist.githubusercontent.com/Kronuz/96ac10fbd8472eb1e7566d740c4034f8/raw/gtest.rb



## Main modules

   * types - core types and classes (image, tensor, matrix, vector etc.)
   * data_io - data i/o classes and functions (e.g. data importers)
   * data_utils - data utils (e.g. functions for interpolations, generation of series, random generator etc.)
   * auto_encoders - auto (as well as some simple) encoders
   * classifiers - different classifiers (naive bayes, softmax etc.)
   * htm - Hierarchical Temporal Memory
   * rbm - Restricted Boltzmann Machine

## Applications

   * char_encoder_test - examplary 1-of-k Char Encoder test application.
   * data_collector_test - program for testing data collector.
   * tensor_test - program for testing tensor functionality.

## Unit tests

   *  types/matrixTestsRunner -- dense (Eigen-derived) matrix unit tests
   *  types/matrixArrayTestsRunner -- dense array of matrices matrix unit tests
 
## Installation

In order to download, configure, make and install new "clean" version of mi-algorithms please execute the following:

    cd ~/workspace
    git clone git@github.ibm.com:tkornut/mi-algorithms.git
    cd mi-algorithms
    mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=~/workspace/mic/
    make -j4 install

## Documentation

In order to generate a "living" documentation of the code please run Doxygen:

    cd ~/workspace/mi-algorithms
    doxygen mi-algorithms.doxyfile
    firefox html/index.html

The current documentation (generated straight from the code and automatically uploaded to GH pages by Travis) is available at:

https://pages.github.ibm.com/tkornut/mi-algorithms/

## Maintainer

Tomasz Kornuta (tkornut@us.ibm.com)

