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
hipblasStatus_t testing_rotmg_strided_batched(const Arguments& arg)
{
    bool FORTRAN = arg.fortran;
    auto hipblasRotmgStridedBatchedFn
        = FORTRAN ? hipblasRotmgStridedBatched<T, true> : hipblasRotmgStridedBatched<T, false>;

    int           batch_count  = arg.batch_count;
    double        stride_scale = arg.stride_scale;
    hipblasStride stride_d1    = stride_scale;
    hipblasStride stride_d2    = stride_scale;
    hipblasStride stride_x1    = stride_scale;
    hipblasStride stride_y1    = stride_scale;
    hipblasStride stride_param = 5 * stride_scale;

    const T rel_error = std::numeric_limits<T>::epsilon() * 1000;

    // check to prevent undefined memory allocation error
    if(batch_count == 0)
    {
        return HIPBLAS_STATUS_SUCCESS;
    }
    else if(batch_count < 0)
    {
        return HIPBLAS_STATUS_INVALID_VALUE;
    }

    double gpu_time_used, hipblas_error_host, hipblas_error_device;

    hipblasLocalHandle handle(arg);

    size_t size_d1    = batch_count * stride_d1;
    size_t size_d2    = batch_count * stride_d2;
    size_t size_x1    = batch_count * stride_x1;
    size_t size_y1    = batch_count * stride_y1;
    size_t size_param = batch_count * stride_param;

    // Initial Data on CPU
    // host data for hipBLAS host test
    host_vector<T> hd1(size_d1);
    host_vector<T> hd2(size_d2);
    host_vector<T> hx1(size_x1);
    host_vector<T> hy1(size_y1);
    host_vector<T> hparams(size_param);

    srand(1);
    hipblas_init<T>(hparams, 1, 5, 1, stride_param, batch_count);
    hipblas_init<T>(hd1, 1, 1, 1, stride_d1, batch_count);
    hipblas_init<T>(hd2, 1, 1, 1, stride_d2, batch_count);
    hipblas_init<T>(hx1, 1, 1, 1, stride_x1, batch_count);
    hipblas_init<T>(hy1, 1, 1, 1, stride_y1, batch_count);

    // host data for CBLAS test
    host_vector<T> cparams = hparams;
    host_vector<T> cd1     = hd1;
    host_vector<T> cd2     = hd2;
    host_vector<T> cx1     = hx1;
    host_vector<T> cy1     = hy1;

    // host data for hipBLAS device test
    host_vector<T> hd1_d(size_d1);
    host_vector<T> hd2_d(size_d2);
    host_vector<T> hx1_d(size_x1);
    host_vector<T> hy1_d(size_y1);
    host_vector<T> hparams_d(size_param);

    // device data for hipBLAS device test
    device_vector<T> dd1(size_d1);
    device_vector<T> dd2(size_d2);
    device_vector<T> dx1(size_x1);
    device_vector<T> dy1(size_y1);
    device_vector<T> dparams(size_param);

    CHECK_HIP_ERROR(hipMemcpy(dd1, hd1, sizeof(T) * size_d1, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dd2, hd2, sizeof(T) * size_d2, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dx1, hx1, sizeof(T) * size_x1, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dy1, hy1, sizeof(T) * size_y1, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dparams, hparams, sizeof(T) * size_param, hipMemcpyHostToDevice));

    if(arg.unit_check || arg.norm_check)
    {
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST));
        CHECK_HIPBLAS_ERROR(hipblasRotmgStridedBatchedFn(handle,
                                                         hd1,
                                                         stride_d1,
                                                         hd2,
                                                         stride_d2,
                                                         hx1,
                                                         stride_x1,
                                                         hy1,
                                                         stride_y1,
                                                         hparams,
                                                         stride_param,
                                                         batch_count));

        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
        CHECK_HIPBLAS_ERROR(hipblasRotmgStridedBatchedFn(handle,
                                                         dd1,
                                                         stride_d1,
                                                         dd2,
                                                         stride_d2,
                                                         dx1,
                                                         stride_x1,
                                                         dy1,
                                                         stride_y1,
                                                         dparams,
                                                         stride_param,
                                                         batch_count));

        CHECK_HIP_ERROR(hipMemcpy(hd1_d, dd1, sizeof(T) * size_d1, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(hd2_d, dd2, sizeof(T) * size_d2, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(hx1_d, dx1, sizeof(T) * size_x1, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(hy1_d, dy1, sizeof(T) * size_y1, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(
            hipMemcpy(hparams_d, dparams, sizeof(T) * size_param, hipMemcpyDeviceToHost));

        for(int b = 0; b < batch_count; b++)
        {
            cblas_rotmg<T>(cd1 + b * stride_d1,
                           cd2 + b * stride_d2,
                           cx1 + b * stride_x1,
                           cy1 + b * stride_y1,
                           cparams + b * stride_param);
        }

        if(arg.unit_check)
        {
            near_check_general<T>(1, 1, batch_count, 1, stride_d1, cd1, hd1, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_d2, cd2, hd2, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_x1, cx1, hx1, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_y1, cy1, hy1, rel_error);
            near_check_general<T>(1, 5, batch_count, 1, stride_param, cparams, hparams, rel_error);

            near_check_general<T>(1, 1, batch_count, 1, stride_d1, cd1, hd1_d, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_d2, cd2, hd2_d, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_x1, cx1, hx1_d, rel_error);
            near_check_general<T>(1, 1, batch_count, 1, stride_y1, cy1, hy1_d, rel_error);
            near_check_general<T>(
                1, 5, batch_count, 1, stride_param, cparams, hparams_d, rel_error);
        }

        if(arg.norm_check)
        {
            hipblas_error_host
                = norm_check_general<T>('F', 1, 1, 1, stride_d1, cd1, hd1, batch_count);
            hipblas_error_host
                += norm_check_general<T>('F', 1, 1, 1, stride_d2, cd2, hd2, batch_count);
            hipblas_error_host
                += norm_check_general<T>('F', 1, 1, 1, stride_x1, cx1, hx1, batch_count);
            hipblas_error_host
                += norm_check_general<T>('F', 1, 1, 1, stride_y1, cy1, hy1, batch_count);
            hipblas_error_host
                += norm_check_general<T>('F', 1, 5, 1, stride_param, cparams, hparams, batch_count);

            hipblas_error_device
                = norm_check_general<T>('F', 1, 1, 1, stride_d1, cd1, hd1_d, batch_count);
            hipblas_error_device
                += norm_check_general<T>('F', 1, 1, 1, stride_d2, cd2, hd2_d, batch_count);
            hipblas_error_device
                += norm_check_general<T>('F', 1, 1, 1, stride_x1, cx1, hx1_d, batch_count);
            hipblas_error_device
                += norm_check_general<T>('F', 1, 1, 1, stride_y1, cy1, hy1_d, batch_count);
            hipblas_error_device += norm_check_general<T>(
                'F', 1, 5, 1, stride_param, cparams, hparams_d, batch_count);
        }
    }

    if(arg.timing)
    {
        hipStream_t stream;
        CHECK_HIPBLAS_ERROR(hipblasGetStream(handle, &stream));
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));

        int runs = arg.cold_iters + arg.iters;
        for(int iter = 0; iter < runs; iter++)
        {
            if(iter == arg.cold_iters)
                gpu_time_used = get_time_us_sync(stream);

            CHECK_HIPBLAS_ERROR(hipblasRotmgStridedBatchedFn(handle,
                                                             dd1,
                                                             stride_d1,
                                                             dd2,
                                                             stride_d2,
                                                             dx1,
                                                             stride_x1,
                                                             dy1,
                                                             stride_y1,
                                                             dparams,
                                                             stride_param,
                                                             batch_count));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used;

        ArgumentModel<e_batch_count>{}.log_args<T>(std::cout,
                                                   arg,
                                                   gpu_time_used,
                                                   ArgumentLogging::NA_value,
                                                   ArgumentLogging::NA_value,
                                                   hipblas_error_host,
                                                   hipblas_error_device);
    }
    return HIPBLAS_STATUS_SUCCESS;
}
