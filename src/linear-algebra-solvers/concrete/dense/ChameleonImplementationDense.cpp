
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file ChameleonAllocateDescriptors.cpp
 * @brief Sets up the Chameleon descriptors needed for the dense matrix computations in ExaGeoStat.
 * @version 1.0.0
 * @author Sameh Abdulah
 * @date 2023-03-20
**/

#include <lapacke.h>
// Include Chameleon libraries
extern "C" {
#include <chameleon/struct.h>
#include <chameleon.h>
#include <control/descriptor.h>
#include <control/context.h>
}

#include <linear-algebra-solvers/concrete/dense/ChameleonImplementationDense.hpp>

using namespace std;

using namespace exageostat::linearAlgebra::dense;
using namespace exageostat::common;
using namespace exageostat::dataunits;
using namespace exageostat::kernels;
using namespace exageostat::helpers;

// Define a method to set up the Chameleon descriptors
template<typename T>
void ChameleonImplementationDense<T>::InitiateDescriptors() {

    // Check for Initialise the Chameleon context.
    if (!this->apContext) {
        throw std::runtime_error(
                "ExaGeoStat hardware is not initialized, please use 'ExaGeoStat<double/float>::ExaGeoStatInitializeHardware(configurations)'.");
    }

    // Declare variables for Chameleon descriptors
    vector<void *> &pDescriptorC = this->mpConfigurations->GetDescriptorC();
    vector<void *> &pDescriptorZ = this->mpConfigurations->GetDescriptorZ();
    auto pDescriptorZcpy = &this->mpConfigurations->GetDescriptorZcpy();
    vector<void *> &pDescriptorProduct = this->mpConfigurations->GetDescriptorProduct();
    auto pDescriptorDeterminant = &this->mpConfigurations->GetDescriptorDeterminant();

    // Get the problem size and other configuration parameters
    int vector_size;
    int N = this->mpConfigurations->GetProblemSize();
    int dts = this->mpConfigurations->GetDenseTileSize();
    int p_grid = this->mpConfigurations->GetPGrid();
    int q_grid = this->mpConfigurations->GetQGrid();
    bool is_OOC = this->mpConfigurations->GetIsOOC();

    // For distributed system and should be removed
    T *Zcpy = (T *) malloc(N * sizeof(T));
    T dot_product_value;

    // Create a Chameleon sequence
    RUNTIME_sequence_t *pSequence;
    RUNTIME_request_t request[2] = {CHAMELEON_SUCCESS, CHAMELEON_SUCCESS};
    CHAMELEON_Sequence_Create(&pSequence);

    // Set the floating point precision based on the template type
    FloatPoint float_point;
    if (sizeof(T) == SIZE_OF_FLOAT) {
        float_point = EXAGEOSTAT_REAL_FLOAT;
        vector_size = 1;
    } else {
        float_point = EXAGEOSTAT_REAL_DOUBLE;
        vector_size = 3;
    }

    // Create the Chameleon descriptors based on the configuration parameters
    // Depending on the passed Precession, the descriptor will resize for value 1 or 3.
    pDescriptorC.resize(vector_size + 1, nullptr);
    pDescriptorZ.resize(vector_size, nullptr);
    pDescriptorProduct.resize(vector_size, nullptr);

    auto **CHAM_descriptorC = &pDescriptorC[0];
    ExageostatAllocateMatrixTile(CHAM_descriptorC, is_OOC, nullptr, (cham_flttype_t) float_point, dts, dts,
                                 dts * dts, N, N, 0, 0, N, N, p_grid, q_grid);

    if (vector_size > 1) {

        auto **CHAM_descsubC11 = (CHAM_desc_t **) &pDescriptorC[1];
        auto **CHAM_descsubC12 = (CHAM_desc_t **) &pDescriptorC[2];
        auto **CHAM_descsubC22 = (CHAM_desc_t **) &pDescriptorC[3];

        *CHAM_descsubC11 = chameleon_desc_submatrix(*(CHAM_desc_t **) CHAM_descriptorC, 0, 0,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->m / 2,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->n / 2);
        *CHAM_descsubC12 = chameleon_desc_submatrix(*(CHAM_desc_t **) CHAM_descriptorC,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->m / 2, 0,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->m / 2,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->n / 2);
        *CHAM_descsubC22 = chameleon_desc_submatrix(*(CHAM_desc_t **) CHAM_descriptorC,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->m / 2,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->n / 2,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->m / 2,
                                                    (*(CHAM_desc_t **) CHAM_descriptorC)->n / 2);
    }
    ExageostatAllocateMatrixTile((&pDescriptorZ[0]), is_OOC, nullptr,
                                 (cham_flttype_t) float_point, dts, dts, dts * dts, N, 1, 0, 0, N, 1, p_grid,
                                 q_grid);
    ExageostatAllocateMatrixTile(pDescriptorZcpy, is_OOC, Zcpy, (cham_flttype_t) float_point, dts, dts,
                                 dts * dts, N, 1, 0, 0, N, 1, p_grid, q_grid);
    ExageostatAllocateMatrixTile(pDescriptorDeterminant, is_OOC, &dot_product_value, (cham_flttype_t) float_point,
                                 dts, dts, dts * dts, 1, 1, 0, 0, 1, 1, p_grid, q_grid);

    for (int idx = 1; idx < pDescriptorZ.size(); idx++) {
        auto **CHAM_descriptorZ_ = &pDescriptorZ[idx];
        ExageostatAllocateMatrixTile(CHAM_descriptorZ_, is_OOC, nullptr, (cham_flttype_t) float_point, dts, dts,
                                     dts * dts, N / 2, 1, 0, 0, N / 2, 1, p_grid, q_grid);
    }

    for (auto &idx: pDescriptorProduct) {
        auto **CHAM_descriptorProduct = &idx;
        ExageostatAllocateMatrixTile(CHAM_descriptorProduct, is_OOC, &dot_product_value,
                                     (cham_flttype_t) float_point, dts, dts, dts * dts, 1, 1, 0, 0, 1, 1, p_grid,
                                     q_grid);
    }

    this->mpConfigurations->SetSequence(pSequence);
    this->mpConfigurations->SetRequest(request);

    //stop gsl error handler
    gsl_set_error_handler_off();
    free(Zcpy);
}

template<typename T>
void ChameleonImplementationDense<T>::ExaGeoStatInitContext(const int &apCoresNumber, const int &apGPUs) {

    if (!this->apContext) {
        CHAMELEON_user_tag_size(31, 26);
        CHAMELEON_Init(apCoresNumber, apGPUs)
        this->apContext = chameleon_context_self();
    }
}

template<typename T>
void ChameleonImplementationDense<T>::ExaGeoStatFinalizeContext() {

    if (!this->apContext) {
        cout
                << "No initialised context of Chameleon, Please use 'ExaGeoStat<double/or/float>::ExaGeoStatInitializeHardware(configurations);'"
                << endl;
    } else {
        CHAMELEON_Finalize()
        this->apContext = nullptr;
    }
}

template<typename T>
void ChameleonImplementationDense<T>::CovarianceMatrixCodelet(void *apDescriptor, int &aTriangularPart,
                                                              dataunits::Locations *apLocation1,
                                                              dataunits::Locations *apLocation2,
                                                              dataunits::Locations *apLocation3,
                                                              double *aLocalTheta, int aDistanceMetric,
                                                              exageostat::kernels::Kernel *apKernel) {

    // Check for Initialise the Chameleon context.
    if (!this->apContext) {
        throw std::runtime_error(
                "ExaGeoStat hardware is not initialized, please use 'ExaGeoStat<double/float>::ExaGeoStatInitializeHardware(configurations)'.");
    }

    RUNTIME_option_t options;
    RUNTIME_options_init(&options, (CHAM_context_t *) this->apContext,
                         (RUNTIME_sequence_t *) this->mpConfigurations->GetSequence(),
                         (RUNTIME_request_t *) this->mpConfigurations->GetRequest());

    int tempmm, tempnn;
    auto *CHAM_apDescriptor = (CHAM_desc_t *) apDescriptor;
    CHAM_desc_t A = *CHAM_apDescriptor;

    struct starpu_codelet *cl = &this->cl_dcmg;
    int m, n, m0 = 0, n0 = 0;

    for (n = 0; n < A.nt; n++) {
        tempnn = n == A.nt - 1 ? A.n - n * A.nb : A.nb;
        if (aTriangularPart == ChamUpperLower) {
            m = 0;
        } else {
            m = A.m == A.n ? n : 0;
        }
        for (; m < A.mt; m++) {

            tempmm = m == A.mt - 1 ? A.m - m * A.mb : A.mb;
            m0 = m * A.mb;
            n0 = n * A.nb;

            // Register the data with StarPU
            starpu_insert_task(cl,
                               STARPU_VALUE, &tempmm, sizeof(int),
                               STARPU_VALUE, &tempnn, sizeof(int),
                               STARPU_VALUE, &m0, sizeof(int),
                               STARPU_VALUE, &n0, sizeof(int),
                               STARPU_W, (starpu_data_handle_t) RUNTIME_data_getaddr(CHAM_apDescriptor, m, n),
                               STARPU_VALUE, &apLocation1, sizeof(dataunits::Locations *),
                               STARPU_VALUE, &apLocation2, sizeof(dataunits::Locations *),
                               STARPU_VALUE, &apLocation3, sizeof(dataunits::Locations *),
                               STARPU_VALUE, &aLocalTheta, sizeof(double *),
                               STARPU_VALUE, &aDistanceMetric, sizeof(int),
                               STARPU_VALUE, &apKernel, sizeof(exageostat::kernels::Kernel *),
                               0);

            auto handle = (starpu_data_handle_t) RUNTIME_data_getaddr(CHAM_apDescriptor, m, n);
            this->apMatrix = (double *) starpu_variable_get_local_ptr(handle);
        }
    }
    RUNTIME_options_ws_free(&options);
    RUNTIME_options_finalize(&options, (CHAM_context_t *) this->apContext);

    CHAMELEON_Sequence_Wait((RUNTIME_sequence_t *) this->mpConfigurations->GetSequence());

}

template<typename T>
void ChameleonImplementationDense<T>::GenerateObservationsVector(void *apDescriptor, Locations *apLocation1,
                                                                 Locations *apLocation2, Locations *apLocation3,
                                                                 vector<double> aLocalTheta, int aDistanceMetric,
                                                                 Kernel *apKernel) {

    // Check for Initialise the Chameleon context.
    if (!this->apContext) {
        throw std::runtime_error(
                "ExaGeoStat hardware is not initialized, please use 'ExaGeoStat<double/float>::ExaGeoStatInitializeHardware(configurations)'.");
    }

    const int N = this->mpConfigurations->GetProblemSize();
    int seed = this->mpConfigurations->GetSeed();
    int iseed[4] = {seed, seed, seed, 1};

    //nomral random generation of e -- ei~N(0, 1) to generate Z
    auto *Nrand = (double *) malloc(N * sizeof(double));
    LAPACKE_dlarnv(3, iseed, N, Nrand);

    //Generate the co-variance matrix C
    auto *theta = (double *) malloc(aLocalTheta.size() * sizeof(double));
    for (int i = 0; i < aLocalTheta.size(); i++) {
        theta[i] = aLocalTheta[i];
    }

    VERBOSE("Initializing Covariance Matrix (Synthetic Dataset Generation Phase).....")
    int upper_lower = EXAGEOSTAT_LOWER;
    this->CovarianceMatrixCodelet(apDescriptor, upper_lower, apLocation1, apLocation2, apLocation3, theta,
                                  aDistanceMetric, apKernel);

    free(theta);
    VERBOSE("Done.\n")

    //Copy Nrand to Z
    VERBOSE("Generate Normal Random Distribution Vector Z (Synthetic Dataset Generation Phase) .....")
    auto **CHAM_descriptorZ = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorZ()[0];
    CopyDescriptorZ(*CHAM_descriptorZ, Nrand);
    VERBOSE("Done.\n")

    //Cholesky factorization for the Co-variance matrix C
    VERBOSE("Cholesky factorization of Sigma (Synthetic Dataset Generation Phase) .....")
    int potential_failure = CHAMELEON_dpotrf_Tile(ChamLower, (CHAM_desc_t *) apDescriptor);
    FAILURE_LOGGER(potential_failure, "Factorization cannot be performed..\nThe matrix is not positive definite")
    VERBOSE("Done.\n")

    //Triangular matrix-matrix multiplication
    VERBOSE("Triangular matrix-matrix multiplication Z=L.e (Synthetic Dataset Generation Phase) .....")
    CHAMELEON_dtrmm_Tile(ChamLeft, ChamLower, ChamNoTrans, ChamNonUnit, 1, (CHAM_desc_t *) apDescriptor,
                         *CHAM_descriptorZ);
    VERBOSE("Done.\n")

    const int P = this->mpConfigurations->GetP();
    if (this->mpConfigurations->GetLogger()) {
        T *pMatrix;
        VERBOSE("Writing generated data to the disk (Synthetic Dataset Generation Phase) .....")
#ifdef CHAMELEON_USE_MPI
        pMatrix = (T*) malloc(N * sizeof(T));
        CHAMELEON_Tile_to_Lapack( *CHAM_descriptorZ, pMatrix, N);
        if ( CHAMELEON_My_Mpi_Rank() == 0 ){
            DiskWriter<T>::WriteVectorsToDisk(pMatrix, &N, &P, this->mpConfigurations->GetLoggerPath(), apLocation1);
        }
        free(pMatrix);
#else
        pMatrix = (T *) (*CHAM_descriptorZ)->mat;
        DiskWriter<T>::WriteVectorsToDisk(pMatrix, &N, &P, this->mpConfigurations->GetLoggerPath(), apLocation1);
        free(pMatrix);
#endif
        VERBOSE(" Done.\n")
    }

    CHAMELEON_dlaset_Tile(ChamUpperLower, 0, 0, (CHAM_desc_t *) apDescriptor);
    free(Nrand);
    VERBOSE("Done Z Vector Generation Phase. (Chameleon Synchronous)")
}

template<typename T>
void
ChameleonImplementationDense<T>::CopyDescriptorZ(void *apapDescriptor, double *apDoubleVector) {

    // Check for Initialise the Chameleon context.
    if (!this->apContext) {
        throw std::runtime_error(
                "ExaGeoStat hardware is not initialized, please use 'ExaGeoStat<double/float>::ExaGeoStatInitializeHardware(configurations)'.");
    }

    RUNTIME_option_t options;
    RUNTIME_options_init(&options, (CHAM_context_t *) this->apContext,
                         (RUNTIME_sequence_t *) this->mpConfigurations->GetSequence(),
                         (RUNTIME_request_t *) this->mpConfigurations->GetRequest());

    int m, m0;
    int tempmm;
    auto A = (CHAM_desc_t *) apapDescriptor;
    struct starpu_codelet *cl = &this->cl_dzcpy;

    for (m = 0; m < A->mt; m++) {
        tempmm = m == A->mt - 1 ? A->m - m * A->mb : A->mb;
        m0 = m * A->mb;

        starpu_insert_task(cl,
                           STARPU_VALUE, &tempmm, sizeof(int),
                           STARPU_VALUE, &m0, sizeof(int),
                           STARPU_VALUE, &apDoubleVector, sizeof(double),
                           STARPU_W, RUNTIME_data_getaddr(A, m, 0),
#if defined(CHAMELEON_CODELETS_HAVE_NAME)
                STARPU_NAME, "dzcpy",
#endif
                           0);
    }
    RUNTIME_options_ws_free(&options);

}

template<typename T>
void ChameleonImplementationDense<T>::DestoryDescriptors() {

    vector<void *> &pDescriptorC = this->mpConfigurations->GetDescriptorC();
    vector<void *> &pDescriptorZ = this->mpConfigurations->GetDescriptorZ();
    auto pChameleonDescriptorZcpy = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorZcpy();
    vector<void *> &pDescriptorProduct = this->mpConfigurations->GetDescriptorProduct();
    auto pChameleonDescriptorDeterminant = (CHAM_desc_t **) &this->mpConfigurations->GetDescriptorDeterminant();

    if (!pDescriptorC.empty() && pDescriptorC[0]) {
        CHAMELEON_Desc_Destroy((CHAM_desc_t **) &pDescriptorC[0]);
    }
    if (!pDescriptorZ.empty() && pDescriptorZ[0]) {
        CHAMELEON_Desc_Destroy((CHAM_desc_t **) &pDescriptorZ[0]);
    }
    if (!pDescriptorProduct.empty() && pDescriptorProduct[0]) {
        CHAMELEON_Desc_Destroy((CHAM_desc_t **) &pDescriptorProduct[0]);
    }
    if (*pChameleonDescriptorZcpy) {
        CHAMELEON_Desc_Destroy(pChameleonDescriptorZcpy);
    }
    if (*pChameleonDescriptorDeterminant) {
        CHAMELEON_Desc_Destroy(pChameleonDescriptorDeterminant);
    }

    if ((RUNTIME_sequence_t *) this->mpConfigurations->GetSequence()) {
        CHAMELEON_Sequence_Destroy((RUNTIME_sequence_t *) this->mpConfigurations->GetSequence());
    }
}

template<typename T>
void ChameleonImplementationDense<T>::ExageostatAllocateMatrixTile(void **apDescriptor, bool ais_OOC, T *apMemSpace,
                                                                   int aType2, int aMB,
                                                                   int aNB, int aMBxNB, int aLda, int aN, int aSMB,
                                                                   int aSNB, int aM, int aN2, int aP, int aQ) {
    if (ais_OOC && apMemSpace == nullptr && aMB != 1 && aNB != 1) {
        CHAMELEON_Desc_Create_OOC((CHAM_desc_t **) apDescriptor, (cham_flttype_t) aType2, aMB, aNB, aMBxNB, aLda, aN,
                                  aSMB, aSNB, aM, aN2, aP, aQ);
    } else {
        CHAMELEON_Desc_Create((CHAM_desc_t **) apDescriptor, apMemSpace, (cham_flttype_t) aType2, aMB, aNB, aMBxNB,
                              aLda, aN, aSMB, aSNB, aM, aN2, aP, aQ);
    }
}

namespace exageostat::linearAlgebra::dense {
    template<typename T> void *ChameleonImplementationDense<T>::apContext = nullptr;
}