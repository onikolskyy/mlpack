/**
 * @file sparse_autoencoder_test.cpp
 * @author Siddharth Agrawal
 *
 * Test the SparseAutoencoder class.
 */

#include <mlpack/core.hpp>
#include <mlpack/core/util/split_data.hpp>

#include <boost/test/unit_test.hpp>
#include "old_boost_test_definitions.hpp"

using namespace mlpack;
using namespace arma;

BOOST_AUTO_TEST_SUITE(SplitDataTest);

void compareData(arma::mat const &inputData, arma::mat const &compareData,
                 arma::Row<size_t> const &inputLabel)
{
    for(size_t i = 0; i != compareData.n_cols; ++i){
        arma::mat const &lhsCol = inputData.col(inputLabel(i));
        arma::mat const &rhsCol = compareData.col(i);
        for(size_t j = 0; j != lhsCol.n_rows; ++j){
            BOOST_REQUIRE_CLOSE(lhsCol(j), rhsCol(j), 1e-5);
        }
    }
}

void compareData(arma::cube const &inputData, arma::cube const &compareData,
                 arma::Row<size_t> const &inputLabel)
{
    for(size_t i = 0; i != compareData.n_slices; ++i){
        arma::mat const &lhsMat = inputData.slice(inputLabel(i));
        arma::mat const &rhsMat = compareData.slice(i);
        for(size_t j = 0; j != lhsMat.size(); ++j){
            BOOST_REQUIRE_CLOSE(lhsMat(j), rhsMat(j), 1e-5);
        }
    }
}

void compareData(arma::cube const &inputData, arma::cube const &compareData,
                 arma::Row<size_t> const &inputLabel, size_t slice)
{
    for(size_t i = 0; i != inputLabel.size(); ++i){
        size_t const inputIndex = inputLabel(i)*slice;
        arma::cube const &lhs = inputData.slices(inputIndex,
                                                 inputIndex+slice-1);
        arma::cube const &rhs = compareData.slices(i*slice, i+slice-1);
        for(size_t j = 0; j != lhs.size(); ++j){
            BOOST_REQUIRE_CLOSE(lhs(j), rhs(j), 1e-5);
        }
    }
}

BOOST_AUTO_TEST_CASE(SplitDataSplitResultMat)
{    
    arma::mat input(2,10);
    input.randu();
    using Labels = arma::Row<size_t>;
    Labels const labels =
            arma::linspace<Labels>(0, input.n_cols-1,
                                   input.n_cols);

    util::TrainTestSplit tts(0.2, 1);
    auto const value = tts.Split(input, labels);
    BOOST_REQUIRE(std::get<0>(value).n_cols == 8);
    BOOST_REQUIRE(std::get<1>(value).n_cols == 2);
    BOOST_REQUIRE(std::get<2>(value).n_cols == 8);
    BOOST_REQUIRE(std::get<3>(value).n_cols == 2);

    compareData(input, std::get<0>(value), std::get<2>(value));
    compareData(input, std::get<1>(value), std::get<3>(value));
}

BOOST_AUTO_TEST_CASE(SplitDataSplitResultCube)
{
    arma::cube input(2,2,10);
    input.randu();
    using Labels = arma::Row<size_t>;
    Labels const labels =
            arma::linspace<Labels>(0, input.n_slices-1,
                                   input.n_slices);

    util::TrainTestSplit tts(0.2, 1);
    auto const value = tts.Split(input, labels);
    BOOST_REQUIRE(std::get<0>(value).n_slices == 8);
    BOOST_REQUIRE(std::get<1>(value).n_slices == 2);
    BOOST_REQUIRE(std::get<2>(value).n_cols == 8);
    BOOST_REQUIRE(std::get<3>(value).n_cols == 2);

    compareData(input, std::get<0>(value), std::get<2>(value));
    compareData(input, std::get<1>(value), std::get<3>(value));
}

BOOST_AUTO_TEST_CASE(SplitDataSplitResultCubeMultiSlice)
{
    size_t const slice = 3;
    arma::cube input(2,2,slice*2);
    input.randu();
    using Labels = arma::Row<size_t>;
    Labels const labels =
            arma::linspace<Labels>(0, input.n_slices/slice-1,
                                   input.n_slices/slice);

    util::TrainTestSplit tts(0.5, slice);
    auto const value = tts.Split(input, labels);
    BOOST_REQUIRE(std::get<0>(value).n_slices == 3);
    BOOST_REQUIRE(std::get<1>(value).n_slices == 3);
    BOOST_REQUIRE(std::get<2>(value).n_cols == 1);
    BOOST_REQUIRE(std::get<3>(value).n_cols == 1);

    compareData(input, std::get<0>(value),
                std::get<2>(value), slice);
    compareData(input, std::get<1>(value),
                std::get<3>(value), slice);
}

BOOST_AUTO_TEST_SUITE_END();
