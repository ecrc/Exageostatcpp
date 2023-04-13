
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// Copyright (C) 2023 by Brightskies inc,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file ChameleonAllocateDescriptors.cpp
 * @brief Sets up the Chameleon descriptors needed for the dense matrix computations in ExaGeoStat.
 * @version 1.0.0
 * @author Sameh Abdulah
 * @date 2023-03-20
**/

#include <linear-algebra-solvers/concrete/dense/ChameleonImplementationDense.hpp>
// Include Chameleon libraries
extern "C" {
#include <chameleon/struct.h>
#include <chameleon.h>
#include <control/descriptor.h>
#include <control/context.h>
}

// Use the following namespaces for convenience
using namespace exageostat::linearAlgebra::dense;
using namespace exageostat::common;
using namespace std;

// Define a method to set up the Chameleon descriptors
template<typename T>
void ChameleonImplementationDense<T>::InitiateDescriptors() {

    // Initialize the Chameleon context
    this->ExaGeoStatInitContext(this->mpConfigurations->GetCoresNumber(), this->mpConfigurations->GetGPUsNumber());

    // Declare variables for Chameleon descriptors
    vector<void *> &pDescriptorC = this->mpConfigurations->GetDescriptorC();
    vector<void *> &pDescriptorZ = this->mpConfigurations->GetDescriptorZ();
    auto pDescriptorZcpy = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorZcpy();
    vector<void *> &pDescriptorProduct = this->mpConfigurations->GetDescriptorProduct();
    auto pDescriptorDeterminant = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorDeterminant();

    // Get the problem size and other configuration parameters
    int vectorSize;
    int N = this->mpConfigurations->GetProblemSize() * this->mpConfigurations->GetP();
    int dts = this->mpConfigurations->GetDenseTileSize();
    int pGrid = this->mpConfigurations->GetPGrid();
    int qGrid = this->mpConfigurations->GetQGrid();
    bool isOOC = this->mpConfigurations->GetIsOOC();

    // For distributed system and should be removed
    T *Zcpy = (T *) malloc(N * sizeof(T));
    T dotProductValue;

    // Create a Chameleon sequence
    RUNTIME_sequence_t *pSequence;
    CHAMELEON_Sequence_Create(&pSequence);

    // Set the floating point precision based on the template type
    FloatPoint floatPoint;
    if (sizeof(T) == SIZE_OF_FLOAT) {
        floatPoint = EXAGEOSTAT_REAL_FLOAT;
        vectorSize = 1;
    } else {
        floatPoint = EXAGEOSTAT_REAL_DOUBLE;
        vectorSize = 3;
    }

    // Create the Chameleon descriptors based on the configuration parameters
    // Depending on the passed Precession, the descriptor will resize for value 1 or 3.
    pDescriptorC.resize(vectorSize + 1, nullptr);
    pDescriptorZ.resize(vectorSize, nullptr);
    pDescriptorProduct.resize(vectorSize, nullptr);

    auto **CHAM_descriptorC = (CHAM_desc_t **) &pDescriptorC[0];
    EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(CHAM_descriptorC, isOOC, nullptr, (cham_flttype_t) floatPoint, dts, dts,
                                          dts * dts, N, N, 0, 0, N, N, pGrid, qGrid)

    if (vectorSize > 1) {

        auto **CHAM_descsubC11 = (CHAM_desc_t **) &pDescriptorC[1];
        auto **CHAM_descsubC12 = (CHAM_desc_t **) &pDescriptorC[2];
        auto **CHAM_descsubC22 = (CHAM_desc_t **) &pDescriptorC[3];

        *CHAM_descsubC11 = chameleon_desc_submatrix(*CHAM_descriptorC, 0, 0, (*CHAM_descriptorC)->m / 2,
                                                    (*CHAM_descriptorC)->n / 2);
        *CHAM_descsubC12 = chameleon_desc_submatrix(*CHAM_descriptorC, (*CHAM_descriptorC)->m / 2, 0,
                                                    (*CHAM_descriptorC)->m / 2, (*CHAM_descriptorC)->n / 2);
        *CHAM_descsubC22 = chameleon_desc_submatrix(*CHAM_descriptorC, (*CHAM_descriptorC)->m / 2,
                                                    (*CHAM_descriptorC)->n / 2,
                                                    (*CHAM_descriptorC)->m / 2, (*CHAM_descriptorC)->n / 2);
    }
    EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(((CHAM_desc_t **) &pDescriptorZ[0]), isOOC, nullptr,
                                          (cham_flttype_t) floatPoint, dts, dts, dts * dts, N, 1, 0, 0, N, 1, pGrid,
                                          qGrid)
    EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(pDescriptorZcpy, isOOC, Zcpy, (cham_flttype_t) floatPoint, dts, dts,
                                          dts * dts, N, 1, 0, 0, N, 1, pGrid, qGrid)
    EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(pDescriptorDeterminant, isOOC, &dotProductValue, (cham_flttype_t) floatPoint,
                                          dts, dts, dts * dts, 1, 1, 0, 0, 1, 1, pGrid, qGrid)

    for (int idx = 1; idx < pDescriptorZ.size(); idx++) {
        auto **CHAM_descriptorZ_ = (CHAM_desc_t **) &pDescriptorZ[idx];
        EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(CHAM_descriptorZ_, isOOC, nullptr, (cham_flttype_t) floatPoint, dts, dts,
                                              dts * dts, N / 2, 1, 0, 0, N / 2, 1, pGrid, qGrid)
    }

    for (auto & idx : pDescriptorProduct) {
        auto **CHAM_descriptorProduct = (CHAM_desc_t **) &idx;
        EXAGEOSTAT_ALLOCATE_DENSE_MATRIX_TILE(CHAM_descriptorProduct, isOOC, &dotProductValue,
                                              (cham_flttype_t) floatPoint, dts, dts, dts * dts, 1, 1, 0, 0, 1, 1, pGrid,
                                              qGrid)
    }
    this->ExaGeoStatFinalizeContext();
    //stop gsl error handler
    gsl_set_error_handler_off();
}

template<typename T>
void ChameleonImplementationDense<T>::ExaGeoStatInitContext(const int &apCoresNumber, const int &apGPUs) {

    CHAM_context_t *chameleonContext;
    chameleonContext = chameleon_context_self();
    if (chameleonContext != nullptr) {
        printf("Another instance of Chameleon is already running...!");
    } else {
        CHAMELEON_user_tag_size(31, 26);
        CHAMELEON_Init(apCoresNumber, apGPUs)
    }
}

template<typename T>
void ChameleonImplementationDense<T>::ExaGeoStatFinalizeContext() {

    CHAM_context_t *chameleonContext;
    chameleonContext = chameleon_context_self();
    if (chameleonContext == nullptr) {
        printf("No active instance oh Chameleon...please use ExaGeoStatInitContext() function to initiate a new instance!\n");
    } else
        CHAMELEON_Finalize();
}


#include <kernels/concrete/UnivariateMaternStationary.hpp>
#include <kernels/Kernel.hpp>
#include <data-generators/DataGenerator.hpp>

#define EXAGEOSTAT_RTBLKADDR(desc, type, m, n) ( (starpu_data_handle_t)RUNTIME_data_getaddr( desc, m, n ) )

template<typename T>
void ChameleonImplementationDense<T>::testKernelfornow() {
//    auto kernel = new exageostat::kernels::UnivariateMaternStationary();
//
//    auto **CHAM_descriptorC = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorC()[0];
//
//    // Create a unique pointer to a DataGenerator object
//    unique_ptr<exageostat::generators::DataGenerator> d1;
//
//
////    this->mpConfigurations->SetIsSynthetic(true);
//    cout << "eh kol dh? " << endl;
//    // Create the DataGenerator object
//    d1 = d1->CreateGenerator(
//            dynamic_cast<configurations::data_configurations::SyntheticDataConfigurations *>(this->mpConfigurations));
//
////     Initialize the locations of the generated data
//    d1->InitializeLocations();
//    dataunits::Locations * l1 = d1->GetLocations();
//
//    unique_ptr<exageostat::generators::DataGenerator> d2;
//
////     Create the DataGenerator object
//    d2 = d2->CreateGenerator(
//            dynamic_cast<configurations::data_configurations::SyntheticDataConfigurations *>(this->mpConfigurations));
//
////     Initialize the locations of the generated data
//    d2->InitializeLocations();
//    dataunits::Locations * l2 = d2->GetLocations();
//
//    double * initial_theta = (double* ) malloc(3 * sizeof(double));
//
//    initial_theta[0] = 1.0;
//    initial_theta[1] = 0.1;
//    initial_theta[2] = 0.5;
//
////    this->ExaGeoStatInitContext(this->mpConfigurations->GetCoresNumber(), this->mpConfigurations->GetGPUsNumber());
//    CHAM_context_t *chamctxt;
//    RUNTIME_option_t options;
//    chamctxt = chameleon_context_self();
//
//    // Create a Chameleon sequence
//    RUNTIME_sequence_t *sequence;
////    CHAMELEON_Sequence_Create(&sequence);
//
//    RUNTIME_request_t *request;
////    CHAMELEON_Request_Create(&request);
//
//
//
////    RUNTIME_options_init(&options, chamctxt, sequence, request);
//
//
//    double * A = (double* ) STARPU_MATRIX_GET_PTR(EXAGEOSTAT_RTBLKADDR((*CHAM_descriptorC), ChamRealDouble, 0, 0));
//
//    cout << "(*CHAM_descriptorC): " << (*CHAM_descriptorC) << endl;
//    cout << "EXAGEOSTAT_RTBLKADDR((*CHAM_descriptorC), ChamRealDouble, 0, 0): " << EXAGEOSTAT_RTBLKADDR((*CHAM_descriptorC), ChamRealDouble, 0, 0) << endl;
//    cout << "yaba argok: " << STARPU_MATRIX_GET_PTR(EXAGEOSTAT_RTBLKADDR((*CHAM_descriptorC), ChamRealDouble, 0, 0)) << endl;
////    cout << "yaba ha: " << (double* ) STARPU_MATRIX_GET_PTR((*CHAM_descriptorC)->get_blktile( (*CHAM_descriptorC), 0, 0 )) << endl;
////    cout << "A: " << A << endl;
////    cout << "A[0]: " << A[0] << endl;
////    kernel->GenerateCovarianceMatrix(A, 8, 8, 0, 0, l1, l2, nullptr, initial_theta, 0);
}
