
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// Copyright (C) 2023 by Brightskies inc,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file UnivariateMaternDdsigmaSquare.hpp
 * @brief Defines the UnivariateMaternDdsigmaSquare class, a Univariate Matern Ddsigma Square kernel.
 * @version 1.0.0
 * @author Sameh Abdulah
 * @date 2023-04-14
**/

#ifndef EXAGEOSTATCPP_UNIVARIATEMATERNDDSIGMASQUARE_HPP
#define EXAGEOSTATCPP_UNIVARIATEMATERNDDSIGMASQUARE_HPP
#include <kernels/Kernel.hpp>

namespace exageostat {
    namespace kernels {

        /**
         * @class UnivariateMaternDdsigmaSquare
         * @brief A class representing a Univariate Matern Ddsigma Square kernel.
         *
         * This class represents a Univariate Matern Ddsigma Square kernel, which is a subclass of the Kernel class. It provides
         * a method for generating a covariance matrix using a set of input locations and kernel parameters.
         */
        class UnivariateMaternDdsigmaSquare : public Kernel {

        public:

            /**
             * @brief Constructs a new UnivariateMaternDdsigmaSquare object.
             *
             * Initializes a new UnivariateMaternDdsigmaSquare object with default values.
             */
            UnivariateMaternDdsigmaSquare();

            /**
             * @brief Generates a covariance matrix using a set of locations and kernel parameters.
             * @param[in] apMatrixA The output covariance matrix.
             * @param[in] aRowsNumber The number of rows in the output matrix.
             * @param[in] aColumnsNumber The number of columns in the output matrix.
             * @param[in] aRowOffset The row offset for the input locations.
             * @param[in] aColumnOffset The column offset for the input locations.
             * @param[in] apLocation1 The set of input locations 1.
             * @param[in] apLocation2 The set of input locations 2.
             * @param[in] apLocation3 The set of input locations 3.
             * @param[in] aLocalTheta An array of kernel parameters.
             * @param [in] aDistanceMetric Distance metric to be used (1 = Euclidean, 2 = Manhattan, 3 = Minkowski).
             */
            void GenerateCovarianceMatrix(double *apMatrixA, int aRowsNumber, int aColumnsNumber, int aRowOffset,
                                          int aColumnOffset, dataunits::Locations *apLocation1,
                                          dataunits::Locations *apLocation2, dataunits::Locations *apLocation3,
                                          std::vector<double> aLocalTheta, int aDistanceMetric) override ;
            /**
             * @brief Creates a new UnivariateMaternDdsigmaSquare object.
             * @return A pointer to the new UnivariateMaternDdsigmaSquare object.
             *
             * This method creates a new UnivariateMaternDdsigmaSquare object and returns a pointer to it.
             */
            static Kernel *Create();

        private:
            static bool plugin_name;
        };
    }//namespace Kernels
}//namespace exageostat

#endif //EXAGEOSTATCPP_UNIVARIATEMATERNDDSIGMASQUARE_HPP
