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
hipblasStatus_t testing_trtri_strided_batched(const Arguments& argus)
{
    bool FORTRAN = argus.fortran;
    auto hipblasTrtriStridedBatchedFn
        = FORTRAN ? hipblasTrtriStridedBatched<T, true> : hipblasTrtriStridedBatched<T, false>;

    const double rel_error = get_epsilon<T>() * 1000;

    int N           = argus.N;
    int lda         = argus.lda;
    int ldinvA      = lda;
    int batch_count = argus.batch_count;

    hipblasStride strideA = size_t(lda) * N;
    size_t        A_size  = strideA * batch_count;

    // check here to prevent undefined memory allocation error
    if(N < 0 || lda < 0 || lda < N || batch_count < 0)
    {
        return HIPBLAS_STATUS_INVALID_VALUE;
    }
    // Naming: dK is in GPU (device) memory. hK is in CPU (host) memory
    host_vector<T> hA(A_size);
    host_vector<T> hB(A_size);

    device_vector<T> dA(A_size);
    device_vector<T> dinvA(A_size);

    double             gpu_time_used, hipblas_error;
    hipblasLocalHandle handle(argus);

    char char_uplo = argus.uplo_option;
    char char_diag = argus.diag_option;

    hipblasFillMode_t uplo = char2hipblas_fill(char_uplo);
    hipblasDiagType_t diag = char2hipblas_diagonal(char_diag);

    srand(1);
    hipblas_init_symmetric<T>(hA, N, lda, strideA, batch_count);
    for(int b = 0; b < batch_count; b++)
    {
        T* hAb = hA.data() + b * strideA;

        // proprocess the matrix to avoid ill-conditioned matrix
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < N; j++)
            {
                hAb[i + j * lda] *= 0.01;

                if(j % 2)
                    hAb[i + j * lda] *= -1;
                if(uplo == HIPBLAS_FILL_MODE_LOWER && j > i)
                    hAb[i + j * lda] = 0.0f;
                else if(uplo == HIPBLAS_FILL_MODE_UPPER && j < i)
                    hAb[i + j * lda] = 0.0f;
                if(i == j)
                {
                    if(diag == HIPBLAS_DIAG_UNIT)
                        hAb[i + j * lda] = 1.0;
                    else
                        hAb[i + j * lda] *= 100.0;
                }
            }
        }
    }

    hB = hA;

    // copy data from CPU to device
    CHECK_HIP_ERROR(hipMemcpy(dA, hA, sizeof(T) * A_size, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dinvA, hA, sizeof(T) * A_size, hipMemcpyHostToDevice));

    if(argus.unit_check || argus.norm_check)
    {
        /* =====================================================================
            HIPBLAS
        =================================================================== */
        CHECK_HIPBLAS_ERROR(hipblasTrtriStridedBatchedFn(
            handle, uplo, diag, N, dA, lda, strideA, dinvA, ldinvA, strideA, batch_count));

        // copy output from device to CPU
        CHECK_HIP_ERROR(hipMemcpy(hA, dinvA, sizeof(T) * A_size, hipMemcpyDeviceToHost));

        /* =====================================================================
           CPU BLAS
        =================================================================== */
        for(int b = 0; b < batch_count; b++)
        {
            cblas_trtri<T>(char_uplo, char_diag, N, hB.data() + b * strideA, lda);
        }

        // enable unit check, notice unit check is not invasive, but norm check is,
        // unit check and norm check can not be interchanged their order
        if(argus.unit_check)
        {
            near_check_general<T>(N, N, batch_count, lda, strideA, hB, hA, rel_error);
        }
        if(argus.norm_check)
        {
            hipblas_error = norm_check_general<T>('F', N, N, lda, strideA, hB, hA, batch_count);
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

            CHECK_HIPBLAS_ERROR(hipblasTrtriStridedBatchedFn(
                handle, uplo, diag, N, dA, lda, strideA, dinvA, ldinvA, strideA, batch_count));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used;

        ArgumentModel<e_uplo_option, e_diag_option, e_N, e_lda, e_stride_a, e_batch_count>{}
            .log_args<T>(std::cout,
                         argus,
                         gpu_time_used,
                         trtri_gflop_count<T>(N),
                         trtri_gbyte_count<T>(N),
                         hipblas_error);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
