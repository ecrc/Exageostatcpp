
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file ChameleonImplementationDense.hpp
 * @brief This file contains the declaration of ChameleonImplementationDense class.
 * @details ChameleonImplementationDense is a concrete implementation of ChameleonImplementation class for dense matrices.
 * @version 1.0.0
 * @author Mahmoud ElKarargy
 * @author Sameh Abdulah
 * @date 2023-03-20
**/

#ifndef EXAGEOSTATCPP_CHAMELEONIMPLEMENTATIONDENSE_HPP
#define EXAGEOSTATCPP_CHAMELEONIMPLEMENTATIONDENSE_HPP

#include <linear-algebra-solvers/concrete/chameleon/ChameleonImplementation.hpp>

namespace exageostat::linearAlgebra::dense {

    /**
     * @brief ChameleonImplementationDense is a concrete implementation for dense matrices using Chameleon..
     * @tparam T Data Type: float or double
     *
     */
    template<typename T>
    class ChameleonImplementationDense : public ChameleonImplementation<T> {
    public:

        /**
         * @brief Default constructor.
         */
        explicit ChameleonImplementationDense() = default;

        /**
         * @brief Virtual destructor to allow calls to the correct concrete destructor.
         */
        ~ChameleonImplementationDense() override = default;

        /**
         * @brief Computes the Cholesky factorization of a symmetric positive definite or Symmetric positive definite matrix.
         * @copydoc LinearAlgebraMethods::ExaGeoStatPotrfTile()
         */
        void
        ExaGeoStatPotrfTile(const common::UpperLower &aUpperLower, void *apA, int aBand, void *apCD, void *apCrk,
                            const int &aMaxRank, const int &aAcc) override;

    };

    /**
    * @brief Instantiates the chameleon dense class for float and double types.
    * @tparam T Data Type: float or double
    *
    */
    EXAGEOSTAT_INSTANTIATE_CLASS(ChameleonImplementationDense)

}//namespace exageostat

#endif //EXAGEOSTATCPP_CHAMELEONIMPLEMENTATIONDENSE_HPP