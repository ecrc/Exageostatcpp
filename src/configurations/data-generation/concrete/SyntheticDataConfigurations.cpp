
/*
 * Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
 * Copyright (C) 2023 by Brightskies inc,
 * All rights reserved.
 * ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).
 */

/**
 * @file SyntheticDataConfigurations.cpp
 * Implementation for Synthetic data Configurations.
 * @version 1.0.0
 * @author Sameh Abdulah
 * @date 2023-02-01
**/

#include <configurations/data-generation/concrete/SyntheticDataConfigurations.hpp>
#include <iostream>

using namespace exageostat::configurations::data_configurations;
using namespace exageostat::common;
using namespace std;


SyntheticDataConfigurations::SyntheticDataConfigurations(int argc, char **argv) {
    this->InitializeArguments(argc, argv);
}

SyntheticDataConfigurations::SyntheticDataConfigurations(string JSON_path) {

}

void SyntheticDataConfigurations::InitializeArguments(int argc, char **argv) {

    string example_name = argv[0];
    // Removes the './'
    example_name.erase(0, 2);
    cout << "Running " << example_name << endl;

    string argument;
    string argumentName;
    string argumentValue;
    int equalSignIdx;
    int numericalValue;

    // Skipping first argument as it's the example name.
    for (int i = 1; i < argc; ++i) {

        argument = argv[i];
        equalSignIdx = argument.find('=');
        argumentName = argument.substr(0, equalSignIdx);

        // Check if argument has an equal sign.
        if (equalSignIdx != string::npos) {
            argumentValue = argument.substr(equalSignIdx + 1);

            if (argumentName == "--N" || argumentName == "--n") {
                numericalValue = CheckNumericalValue(argumentValue);
                SetProblemSize(numericalValue);
            } else if (argumentName == "--Kernel" || argumentName == "--kernel") {
                CheckKernelValue(argumentValue);
                SetKernel(argumentValue);
            } else if (argumentName == "--Dimension" || argumentName == "--dimension"
                       || argumentName == "--dim" || argumentName == "--Dim") {
                SetDimension(CheckDimensionValue(argumentValue));
            } else if (argumentName == "--P" || argumentName == "--p") {
                numericalValue = CheckNumericalValue(argumentValue);
                SetPGrid(numericalValue);
            } else if (argumentName == "--TimeSlot" || argumentName == "--timeslot") {
                numericalValue = CheckNumericalValue(argumentValue);
                SetTimeSlot(numericalValue);
            } else if (argumentName == "--Computation" || argumentName == "--computation") {
                SetComputation(CheckComputationValue(argumentValue));
            } else if (argumentName == "--precision" || argumentName == "--Precision") {
                SetPrecision(CheckPrecisionValue(argumentValue));
            }
            else {
                cout << "!! " << argumentName << " !!" << endl;
                throw invalid_argument(
                        "This argument is undefined, Please use --help to print all available arguments");
            }
        }
            // If none then, just set the argument to True.
        else {
            if (argumentName == "--help") {
                PrintUsage();
            }
            if (argumentName == "--syntheticData") {
                SetIsSynthetic(true);
            } else {
                throw invalid_argument(
                        "This argument is undefined, Please use --help to print all available arguments");
            }
        }
    }
}

void SyntheticDataConfigurations::PrintUsage() {
    cout << "\n\t*** Available Arguments For Synthetic Data Configurations***" << endl;
    cout << "\t\t --N=value : Problem size." << endl;
    cout << "\t\t --Kernel=value : Used Kernel." << endl;
    cout << "\t\t --Dimension=value : Used Dimension." << endl;
    cout << "\t\t --P=value : Used P-Grid." << endl;
    cout << "\t\t --TimeSlot=value : Time slot value for ST." << endl;
    cout << "\t\t --Computation=value : Used computation" << endl;
    cout << "\t\t --Precision=value : Used precision" << endl;
    cout << "\t\t --syntheticData : Used to enable generating synthetic data." << endl;
    cout << "\n\n";

    exit(0);

}

Dimension SyntheticDataConfigurations::GetDimension() {
    return this->mDimension;
}

void SyntheticDataConfigurations::SetDimension(Dimension aDimension) {
    this->mDimension = aDimension;
}

Dimension SyntheticDataConfigurations::CheckDimensionValue(string aDimension) {

    if (aDimension != "2D" and aDimension != "2d"
        and aDimension != "3D" and aDimension != "3d"
        and aDimension != "st" and aDimension != "ST") {
        throw range_error("Invalid value for Dimension. Please use 2D, 3D or ST.");
    }
    if (aDimension == "2D" or aDimension == "2d") {
        return Dimension2D;
    } else if (aDimension == "3D" or aDimension == "3d") {
        return Dimension3D;
    }
    return DimensionST;
}

//if (nZmiss >= N)
//{
//printf("please use nZmiss < N!!\n");
//exit(0);
//}
