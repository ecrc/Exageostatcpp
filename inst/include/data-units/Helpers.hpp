
// Copyright (c) 2017-2023 King Abdullah University of Science and Technology,
// Copyright (C) 2023 by Brightskies inc,
// All rights reserved.
// ExaGeoStat is a software package, provided by King Abdullah University of Science and Technology (KAUST).

/**
 * @file Helpers.hpp
 *
 * @version 1.0.0
 * @author Sameh Abdulah
 * @date 2023-03-05
**/

#ifndef EXAGEOSTAT_CPP_HELPERS_HPP
#define EXAGEOSTAT_CPP_HELPERS_HPP

#include <set>

namespace exageostat {
    namespace dataunits {

        std::set<std::string> availableKernels = {"univariate_matern_stationary",
                                                  "univariate_matern_non_stationary",
                                                  "bivariate_matern_flexible",
                                                  "bivariate_matern_parsimonious",
                                                  "bivariate_matern_parsimonious_profile",
                                                  "univariate_matern_nuggets_stationary",
                                                  "univariate_spacetime_matern_stationary",
                                                  "univariate_matern_dsigma_square",
                                                  "univariate_matern_dnu",
                                                  "univariate_matern_dbeta",
                                                  "univariate_matern_ddsigma_square",
                                                  "univariate_matern_ddsigma_square_beta",
                                                  "univariate_matern_ddsigma_square_nu",
                                                  "univariate_matern_ddbeta_beta",
                                                  "univariate_matern_ddbeta_nu",
                                                  "univariate_matern_ddnu_nu",
                                                  "bivariate_spacetime_matern_stationary",
                                                  "univariate_matern_non_gaussian",
                                                  "univariate_exp_non_gaussian",
                                                  "bivariate_spacetime_matern_stationary",
                                                  "trivariate_matern_parsimonious",
                                                  "trivariate_matern_parsimonious_profile",
                                                  "univariate_matern_non_stat"
        };

    }//namespace configurations
}//namespace exageostat
#endif //EXAGEOSTAT_CPP_HELPERS_HPP
