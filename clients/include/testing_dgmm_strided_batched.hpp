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
hipblasStatus_t testing_dgmm_strided_batched(const Arguments& argus)
{
    bool FORTRAN = argus.fortran;
    auto hipblasDgmmStridedBatchedFn
        = FORTRAN ? hipblasDgmmStridedBatched<T, true> : hipblasDgmmStridedBatched<T, false>;

    hipblasSideMode_t side = char2hipblas_side(argus.side_option);

    int    M            = argus.M;
    int    N            = argus.N;
    int    lda          = argus.lda;
    int    incx         = argus.incx;
    int    ldc          = argus.ldc;
    int    batch_count  = argus.batch_count;
    double stride_scale = argus.stride_scale;
    int    k            = (side == HIPBLAS_SIDE_RIGHT ? N : M);

    int           abs_incx = incx >= 0 ? incx : -incx;
    hipblasStride stride_A = size_t(lda) * N * stride_scale;
    hipblasStride stride_x = size_t(abs_incx) * k * stride_scale;
    hipblasStride stride_C = size_t(ldc) * N * stride_scale;
    if(!stride_x)
        stride_x = 1;

    size_t A_size = size_t(stride_A) * batch_count;
    size_t C_size = size_t(stride_C) * batch_count;
    size_t X_size = size_t(stride_x) * batch_count;

    hipblasLocalHandle handle(argus);

    // argument sanity check, quick return if input parameters are invalid before allocating invalid
    // memory
    bool invalid_size = M < 0 || N < 0 || ldc < M || lda < M || batch_count < 0;
    if(invalid_size || !N || !M || !batch_count)
    {
        hipblasStatus_t actual = hipblasDgmmStridedBatchedFn(handle,
                                                             side,
                                                             M,
                                                             N,
                                                             nullptr,
                                                             lda,
                                                             stride_A,
                                                             nullptr,
                                                             incx,
                                                             stride_x,
                                                             nullptr,
                                                             ldc,
                                                             stride_C,
                                                             batch_count);
        EXPECT_HIPBLAS_STATUS(
            actual, (invalid_size ? HIPBLAS_STATUS_INVALID_VALUE : HIPBLAS_STATUS_SUCCESS));
        return actual;
    }

    // Naming: dK is in GPU (device) memory. hK is in CPU (host) memory
    host_vector<T> hA(A_size);
    host_vector<T> hA_copy(A_size);
    host_vector<T> hx(X_size);
    host_vector<T> hx_copy(X_size);
    host_vector<T> hC(C_size);
    host_vector<T> hC_1(C_size);
    host_vector<T> hC_gold(C_size);

    device_vector<T> dA(A_size);
    device_vector<T> dx(X_size);
    device_vector<T> dC(C_size);

    double gpu_time_used, hipblas_error;

    // Initial Data on CPU
    srand(1);
    hipblas_init<T>(hA, M, N, lda, stride_A, batch_count);
    hipblas_init<T>(hx, 1, k, abs_incx, stride_x, batch_count);
    hipblas_init<T>(hC, M, N, ldc, stride_C, batch_count);
    hA_copy = hA;
    hx_copy = hx;
    hC_1    = hC;
    hC_gold = hC;

    // copy data from CPU to device
    CHECK_HIP_ERROR(hipMemcpy(dA, hA.data(), sizeof(T) * A_size, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dx, hx.data(), sizeof(T) * X_size, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dC, hC.data(), sizeof(T) * C_size, hipMemcpyHostToDevice));

    if(argus.unit_check || argus.norm_check)
    {
        /* =====================================================================
            HIPBLAS
        =================================================================== */
        CHECK_HIPBLAS_ERROR(hipblasDgmmStridedBatchedFn(handle,
                                                        side,
                                                        M,
                                                        N,
                                                        dA,
                                                        lda,
                                                        stride_A,
                                                        dx,
                                                        incx,
                                                        stride_x,
                                                        dC,
                                                        ldc,
                                                        stride_C,
                                                        batch_count));

        // copy output from device to CPU
        CHECK_HIP_ERROR(hipMemcpy(hC_1.data(), dC, sizeof(T) * C_size, hipMemcpyDeviceToHost));

        /* =====================================================================
           CPU BLAS
        =================================================================== */

        // reference calculation
        ptrdiff_t shift_x = incx < 0 ? -ptrdiff_t(incx) * (N - 1) : 0;
        for(int b = 0; b < batch_count; b++)
        {
            auto hC_goldb = hC_gold + b * stride_C;
            auto hA_copyb = hA_copy + b * stride_A;
            auto hx_copyb = hx_copy + b * stride_x;
            for(size_t i1 = 0; i1 < M; i1++)
            {
                for(size_t i2 = 0; i2 < N; i2++)
                {
                    if(HIPBLAS_SIDE_RIGHT == side)
                    {
                        hC_goldb[i1 + i2 * ldc]
                            = hA_copyb[i1 + i2 * lda] * hx_copyb[shift_x + i2 * incx];
                    }
                    else
                    {
                        hC_goldb[i1 + i2 * ldc]
                            = hA_copyb[i1 + i2 * lda] * hx_copyb[shift_x + i1 * incx];
                    }
                }
            }
        }

        // enable unit check, notice unit check is not invasive, but norm check is,
        // unit check and norm check can not be interchanged their order
        if(argus.unit_check)
        {
            unit_check_general<T>(M, N, batch_count, ldc, stride_C, hC_gold, hC_1);
        }

        if(argus.norm_check)
        {
            hipblas_error
                = norm_check_general<T>('F', M, N, ldc, stride_C, hC_gold, hC_1, batch_count);
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

            CHECK_HIPBLAS_ERROR(hipblasDgmmStridedBatchedFn(handle,
                                                            side,
                                                            M,
                                                            N,
                                                            dA,
                                                            lda,
                                                            stride_A,
                                                            dx,
                                                            incx,
                                                            stride_x,
                                                            dC,
                                                            ldc,
                                                            stride_C,
                                                            batch_count));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used; // in microseconds

        ArgumentModel<e_side_option,
                      e_M,
                      e_N,
                      e_lda,
                      e_stride_a,
                      e_incx,
                      e_stride_x,
                      e_ldc,
                      e_stride_c,
                      e_batch_count>{}
            .log_args<T>(std::cout,
                         argus,
                         gpu_time_used,
                         dgmm_gflop_count<T>(M, N),
                         dgmm_gbyte_count<T>(M, N, k),
                         hipblas_error);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
