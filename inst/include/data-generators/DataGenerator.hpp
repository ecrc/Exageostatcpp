
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file DataGenerator.hpp
 * @brief Contains definition for abstract Data Generator Class.
 * @version 1.0.1
 * @author Mahmoud ElKarargy
 * @date 2023-02-14
**/

#ifndef EXAGEOSTAT_CPP_DATAGENERATOR_HPP
#define EXAGEOSTAT_CPP_DATAGENERATOR_HPP

#include <memory>

#include <linear-algebra-solvers/LinearAlgebraFactory.hpp>
#include <linear-algebra-solvers/LinearAlgebraMethods.hpp>

namespace exageostat::generators {

    /**
     * @class DataGenerator
     * @brief Abstract base class for generating synthetic or real data.
     * @tparam T Data Type: float or double
     *
     */
    template<typename T>
    class DataGenerator {

    public:

        /**
         * @brief Either generates synthetic data or reads data files.
         * @details This method generates the X, Y, and Z variables used to define the locations of the data points.
         * @param[in] aConfigurations Reference to the data configurations.
         * @param[in] aHardware Reference to the used hardware.
         * @param[in] aKernel Reference to the used Kernel.
         * @return unique Pointer to a populated data.
         *
         */
        virtual std::unique_ptr<dataunits::ExaGeoStatData<T>>
        CreateData(exageostat::configurations::Configurations &aConfigurations,
                   const exageostat::hardware::ExaGeoStatHardware &aHardware,
                   exageostat::kernels::Kernel<T> &aKernel) = 0;

        /**
         * @brief Factory method for creating a data generator object.
         * @details This method creates a data generator object based on the specified configurations.
         * @param[in] aConfigurations Reference to the data configurations.
         * @return A unique pointer to the created data generator object.
         *
         */
        static std::unique_ptr<DataGenerator>
        CreateGenerator(exageostat::configurations::Configurations &aConfigurations);

        /**
         * @brief Destructor for the data generator object.
         * @details This method frees the memory used by the data generator object.
         *
         */
        virtual ~DataGenerator();

    protected:
        /// Used bool identifying type of generation.
        static bool mIsSynthetic;
        /// Used bool identifying type of generation.
        static bool mIsCSV;
    };

    /**
     * @brief Instantiates the Data Generator class for float and double types.
     * @tparam T Data Type: float or double
     *
     */
    EXAGEOSTAT_INSTANTIATE_CLASS(DataGenerator)
}//namespace exageostat

#endif //EXAGEOSTAT_CPP_DATAGENERATOR_HPP