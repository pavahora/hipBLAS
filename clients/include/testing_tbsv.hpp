/* ************************************************************************
 * Copyright 2016-2021 Advanced Micro Devices, Inc.
 *
 * ************************************************************************ */

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <vector>

#include "testing_common.hpp"

using namespace std;

/* ============================================================================================ */

template <typename T>
hipblasStatus_t testing_tbsv(const Arguments& argus)
{
    bool FORTRAN       = argus.fortran;
    auto hipblasTbsvFn = FORTRAN ? hipblasTbsv<T, true> : hipblasTbsv<T, false>;

    int                M           = argus.M;
    int                K           = argus.K;
    int                incx        = argus.incx;
    int                lda         = argus.lda;
    char               char_uplo   = argus.uplo_option;
    char               char_diag   = argus.diag_option;
    char               char_transA = argus.transA_option;
    hipblasFillMode_t  uplo        = char2hipblas_fill(char_uplo);
    hipblasDiagType_t  diag        = char2hipblas_diagonal(char_diag);
    hipblasOperation_t transA      = char2hipblas_operation(char_transA);

    hipblasLocalHandle handle(argus);

    // argument sanity check, quick return if input parameters are invalid before allocating invalid
    // memory
    bool invalid_size = M < 0 || K < 0 || lda < K + 1 || !incx;
    if(invalid_size || !M)
    {
        hipblasStatus_t actual
            = hipblasTbsvFn(handle, uplo, transA, diag, M, K, nullptr, lda, nullptr, incx);
        EXPECT_HIPBLAS_STATUS(
            actual, (invalid_size ? HIPBLAS_STATUS_INVALID_VALUE : HIPBLAS_STATUS_SUCCESS));
        return actual;
    }

    int    abs_incx = incx < 0 ? -incx : incx;
    size_t size_A   = size_t(M) * M;
    size_t size_AB  = size_t(lda) * M;
    size_t size_x   = abs_incx * size_t(M);

    // Naming: dK is in GPU (device) memory. hK is in CPU (host) memory
    host_vector<T> hA(size_A);
    host_vector<T> hAB(size_AB);
    host_vector<T> AAT(size_A);
    host_vector<T> hb(size_x);
    host_vector<T> hx(size_x);
    host_vector<T> hx_or_b_1(size_x);

    device_vector<T> dAB(size_AB);
    device_vector<T> dx_or_b(size_x);

    double gpu_time_used, hipblas_error;

    // Initial Data on CPU
    srand(1);
    hipblas_init<T>(hA, M, M, M);

    banded_matrix_setup(uplo == HIPBLAS_FILL_MODE_UPPER, (T*)hA, M, M, K);

    prepare_triangular_solve((T*)hA, M, (T*)AAT, M, char_uplo);
    if(diag == HIPBLAS_DIAG_UNIT)
    {
        make_unit_diagonal(uplo, (T*)hA, M, M);
    }

    regular_to_banded(uplo == HIPBLAS_FILL_MODE_UPPER, (T*)hA, M, (T*)hAB, lda, M, K);
    CHECK_HIP_ERROR(hipMemcpy(dAB, hAB.data(), sizeof(T) * size_AB, hipMemcpyHostToDevice));

    hipblas_init<T>(hx, 1, M, abs_incx);
    hb = hx;

    cblas_tbmv<T>(uplo, transA, diag, M, K, hAB, lda, hb, incx);
    hx_or_b_1 = hb;

    // copy data from CPU to device
    CHECK_HIP_ERROR(
        hipMemcpy(dx_or_b, hx_or_b_1.data(), sizeof(T) * size_x, hipMemcpyHostToDevice));

    /* =====================================================================
           HIPBLAS
    =================================================================== */
    if(argus.unit_check || argus.norm_check)
    {
        CHECK_HIPBLAS_ERROR(
            hipblasTbsvFn(handle, uplo, transA, diag, M, K, dAB, lda, dx_or_b, incx));

        // copy output from device to CPU
        CHECK_HIP_ERROR(
            hipMemcpy(hx_or_b_1.data(), dx_or_b, sizeof(T) * size_x, hipMemcpyDeviceToHost));

        // Calculating error
        hipblas_error = std::abs(vector_norm_1<T>(M, abs_incx, hx.data(), hx_or_b_1.data()));

        if(argus.unit_check)
        {
            double tolerance = std::numeric_limits<real_t<T>>::epsilon() * 40 * M;
            unit_check_error(hipblas_error, tolerance);
        }
    }

    if(argus.timing)
    {
        hipStream_t stream;
        CHECK_HIPBLAS_ERROR(hipblasGetStream(handle, &stream));

        int runs = argus.cold_iters + argus.iters;
        for(int iter = 0; iter < runs; iter++)
        {
            if(iter == argus.cold_iters)
                gpu_time_used = get_time_us_sync(stream);

            CHECK_HIPBLAS_ERROR(
                hipblasTbsvFn(handle, uplo, transA, diag, M, K, dAB, lda, dx_or_b, incx));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used; // in microseconds

        ArgumentModel<e_uplo_option, e_transA_option, e_diag_option, e_M, e_K, e_lda, e_incx>{}
            .log_args<T>(std::cout,
                         argus,
                         gpu_time_used,
                         tbsv_gflop_count<T>(M, K),
                         tbsv_gbyte_count<T>(M, K),
                         hipblas_error);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
