/* ************************************************************************
 * Copyright 2016-2021 Advanced Micro Devices, Inc.
 *
 * ************************************************************************ */

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "testing_common.hpp"

using namespace std;

/* ============================================================================================ */

template <typename T>
hipblasStatus_t testing_rotm_batched(const Arguments& arg)
{
    bool FORTRAN = arg.fortran;
    auto hipblasRotmBatchedFn
        = FORTRAN ? hipblasRotmBatched<T, true> : hipblasRotmBatched<T, false>;

    int N           = arg.N;
    int incx        = arg.incx;
    int incy        = arg.incy;
    int batch_count = arg.batch_count;

    const T rel_error = std::numeric_limits<T>::epsilon() * 1000;

    hipblasLocalHandle handle(arg);

    // check to prevent undefined memory allocation error
    if(N <= 0 || batch_count <= 0)
    {
        CHECK_HIPBLAS_ERROR(
            hipblasRotmBatchedFn(handle, N, nullptr, incx, nullptr, incy, nullptr, batch_count));

        return HIPBLAS_STATUS_SUCCESS;
    }

    int abs_incx = incx >= 0 ? incx : -incx;
    int abs_incy = incy >= 0 ? incy : -incy;

    double gpu_time_used, hipblas_error_device;

    device_batch_vector<T> dx(N, incx, batch_count);
    device_batch_vector<T> dy(N, incy, batch_count);
    device_batch_vector<T> dparam(5, 1, batch_count);

    host_batch_vector<T> hx(N, incx, batch_count);
    host_batch_vector<T> hy(N, incy, batch_count);
    host_batch_vector<T> hdata(4, 1, batch_count);
    host_batch_vector<T> hparam(5, 1, batch_count);

    hipblas_init(hx, true);
    hipblas_init(hy, false);
    hipblas_init(hdata, false);

    for(int b = 0; b < batch_count; b++)
    {
        cblas_rotmg<T>(&hdata[b][0], &hdata[b][1], &hdata[b][2], &hdata[b][3], hparam[b]);
    }

    constexpr int FLAG_COUNT        = 4;
    const T       FLAGS[FLAG_COUNT] = {-1, 0, 1, -2};

    for(int i = 0; i < FLAG_COUNT; i++)
    {
        if(arg.unit_check || arg.norm_check)
        {
            for(int b = 0; b < batch_count; b++)
                hparam[b][0] = FLAGS[i];

            // Test device
            CHECK_HIP_ERROR(dx.transfer_from(hx));
            CHECK_HIP_ERROR(dy.transfer_from(hy));
            CHECK_HIP_ERROR(dparam.transfer_from(hparam));
            CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
            CHECK_HIPBLAS_ERROR(hipblasRotmBatchedFn(handle,
                                                     N,
                                                     dx.ptr_on_device(),
                                                     incx,
                                                     dy.ptr_on_device(),
                                                     incy,
                                                     dparam.ptr_on_device(),
                                                     batch_count));

            host_batch_vector<T> rx(N, incx, batch_count);
            host_batch_vector<T> ry(N, incy, batch_count);
            CHECK_HIP_ERROR(rx.transfer_from(dx));
            CHECK_HIP_ERROR(ry.transfer_from(dy));

            host_batch_vector<T> cx(N, incx, batch_count);
            host_batch_vector<T> cy(N, incy, batch_count);
            cx.copy_from(hx);
            cy.copy_from(hy);

            for(int b = 0; b < batch_count; b++)
            {
                // CPU BLAS reference data
                cblas_rotm<T>(N, cx[b], incx, cy[b], incy, hparam[b]);
            }

            if(arg.unit_check)
            {
                for(int b = 0; b < batch_count; b++)
                {
                    near_check_general<T>(1, N, abs_incx, cx[b], rx[b], rel_error);
                    near_check_general<T>(1, N, abs_incy, cy[b], ry[b], rel_error);
                }
            }
            if(arg.norm_check)
            {
                hipblas_error_device
                    = norm_check_general<T>('F', 1, N, abs_incx, cx, rx, batch_count);
                hipblas_error_device
                    += norm_check_general<T>('F', 1, N, abs_incy, cy, ry, batch_count);
            }
        }
    }

    if(arg.timing)
    {
        for(int b = 0; b < batch_count; b++)
            hparam[b][0] = 0;
        hipStream_t stream;
        CHECK_HIPBLAS_ERROR(hipblasGetStream(handle, &stream));
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
        CHECK_HIP_ERROR(dx.transfer_from(hx));
        CHECK_HIP_ERROR(dy.transfer_from(hy));
        CHECK_HIP_ERROR(dparam.transfer_from(hparam));

        int runs = arg.cold_iters + arg.iters;
        for(int iter = 0; iter < runs; iter++)
        {
            if(iter == arg.cold_iters)
                gpu_time_used = get_time_us_sync(stream);

            CHECK_HIPBLAS_ERROR(hipblasRotmBatchedFn(handle,
                                                     N,
                                                     dx.ptr_on_device(),
                                                     incx,
                                                     dy.ptr_on_device(),
                                                     incy,
                                                     dparam.ptr_on_device(),
                                                     batch_count));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used;

        ArgumentModel<e_N, e_incx, e_incy, e_batch_count>{}.log_args<T>(std::cout,
                                                                        arg,
                                                                        gpu_time_used,
                                                                        rotm_gflop_count<T>(N, 0),
                                                                        rotm_gbyte_count<T>(N, 0),
                                                                        0,
                                                                        hipblas_error_device);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
