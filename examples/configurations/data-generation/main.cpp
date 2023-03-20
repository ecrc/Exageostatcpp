/*
 * Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
 * Copyright (C) 2023 by Brightskies inc,
 * All rights reserved.
 * ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).
 */

/**
* @file synthatic_data.cpp
* @version 1.0.0
* @author Sameh Abdulah
* @date 2023-01-31
**/

#include <iostream>
#include <configurations/data-generation/concrete/SyntheticDataConfigurations.hpp>

using namespace std;
using namespace exageostat::configurations::data_configurations;
using namespace exageostat::dataunits;

int main(int argc, char **argv) {

    // Object has automatic storage duration (usually is on the stack)
    auto syntheticDataConfigurations = new SyntheticDataConfigurations(argc, argv);

    int N = syntheticDataConfigurations->GetProblemSize();
    if (N != 0) {
        cout << "You set N by: " << N << endl;
    }

    string kernel = syntheticDataConfigurations->GetKernel();
    if (!kernel.empty()) {
        cout << "You set Kernel by: " << kernel << endl;
    }

    Dimension dimension = syntheticDataConfigurations->GetDimension();
    if (dimension == Dimension2D) {
        cout << "You set Dimension by: 2D" << endl;
    }
    else if (dimension == Dimension3D) {
        cout << "You set Dimension by: 3D" << endl;
    }
    else if (dimension == DimensionST) {
        cout << "You set Dimension by: ST" << endl;
    }

    int PGrid = syntheticDataConfigurations->GetPGrid();
    if (PGrid != 0) {
        cout << "You set P by: " << PGrid << endl;
    }

    int timeSlot = syntheticDataConfigurations->GetTimeSlot();
    if(timeSlot != 0){
        cout << "You set time slot by: " << timeSlot << endl;
    }

    Computation computation = syntheticDataConfigurations->GetComputation();
    if (computation == EXACT_DENSE) {
        cout << "You set Computation to: EXACT" << endl;
    }
    else if (computation == DIAGONAL_APPROX) {
        cout << "You set Computation to: DIAGONAL APPROX" << endl;
    }
    else if (computation == TILE_LOW_RANK) {
        cout << "You set Computation to: TILE LOW RANK" << endl;
    }

    Precision precision = syntheticDataConfigurations->GetPrecision();
    if (precision == SINGLE) {
        cout << "You set precision to: SINGLE" << endl;
    }
    else if (precision == DOUBLE) {
        cout << "You set precision to: DOUBLE" << endl;
    }
    else if (precision == MIXED) {
        cout << "You set precision to: MIXED PRECISION" << endl;
    }


    delete syntheticDataConfigurations;
    return 0;
}