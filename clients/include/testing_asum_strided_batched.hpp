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
hipblasStatus_t testing_asum_strided_batched(const Arguments& argus)
{
    using Tr                         = real_t<T>;
    bool FORTRAN                     = argus.fortran;
    auto hipblasAsumStridedBatchedFn = FORTRAN ? hipblasAsumStridedBatched<T, Tr, true>
                                               : hipblasAsumStridedBatched<T, Tr, false>;

    int    N            = argus.N;
    int    incx         = argus.incx;
    double stride_scale = argus.stride_scale;
    int    batch_count  = argus.batch_count;

    hipblasStride stridex = size_t(N) * incx * stride_scale;
    size_t        sizeX   = stridex * batch_count;

    double             gpu_time_used, hipblas_error_host, hipblas_error_device;
    hipblasLocalHandle handle(argus);

    // check to prevent undefined memory allocation error
    if(N <= 0 || incx <= 0 || batch_count <= 0)
    {
        device_vector<Tr> d_hipblas_result_0(std::max(1, batch_count));
        host_vector<Tr>   h_hipblas_result_0(std::max(1, batch_count));
        hipblas_init_nan(h_hipblas_result_0.data(), std::max(1, batch_count));
        CHECK_HIP_ERROR(hipMemcpy(d_hipblas_result_0,
                                  h_hipblas_result_0,
                                  sizeof(Tr) * std::max(1, batch_count),
                                  hipMemcpyHostToDevice));

        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
        CHECK_HIPBLAS_ERROR(hipblasAsumStridedBatchedFn(
            handle, N, nullptr, incx, stridex, batch_count, d_hipblas_result_0));

        if(batch_count > 0)
        {
            host_vector<Tr> cpu_0(batch_count);
            host_vector<Tr> gpu_0(batch_count);
            CHECK_HIP_ERROR(hipMemcpy(
                gpu_0, d_hipblas_result_0, sizeof(Tr) * batch_count, hipMemcpyDeviceToHost));
            unit_check_general<Tr>(1, batch_count, 1, cpu_0, gpu_0);
        }
        return HIPBLAS_STATUS_SUCCESS;
    }

    // Naming: dX is in GPU (device) memory. hK is in CPU (host) memory, plz follow this practice
    host_vector<T>  hx(sizeX);
    host_vector<Tr> cpu_result(batch_count);
    host_vector<Tr> hipblas_result_host(batch_count);
    host_vector<Tr> hipblas_result_device(batch_count);

    device_vector<T>  dx(sizeX);
    device_vector<Tr> d_hipblas_result(batch_count);

    // Initial Data on CPU
    srand(1);
    hipblas_init<T>(hx, 1, N, incx, stridex, batch_count);

    // copy data from CPU to device, does not work for incx != 1
    CHECK_HIP_ERROR(hipMemcpy(dx, hx.data(), sizeof(T) * sizeX, hipMemcpyHostToDevice));

    if(argus.unit_check || argus.norm_check)
    {
        /* =====================================================================
                    HIPBLAS
        =================================================================== */
        // hipblasAsum accept both dev/host pointer for the scalar
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
        CHECK_HIPBLAS_ERROR(hipblasAsumStridedBatchedFn(
            handle, N, dx, incx, stridex, batch_count, d_hipblas_result));

        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST));
        CHECK_HIPBLAS_ERROR(hipblasAsumStridedBatchedFn(
            handle, N, dx, incx, stridex, batch_count, hipblas_result_host));

        CHECK_HIP_ERROR(hipMemcpy(hipblas_result_device,
                                  d_hipblas_result,
                                  sizeof(Tr) * batch_count,
                                  hipMemcpyDeviceToHost));

        /* =====================================================================
                    CPU BLAS
        =================================================================== */
        for(int b = 0; b < batch_count; b++)
        {
            cblas_asum<T, Tr>(N, hx.data() + b * stridex, incx, &cpu_result[b]);
        }

        if(argus.unit_check)
        {
            unit_check_general<Tr>(1, batch_count, 1, cpu_result, hipblas_result_host);
            unit_check_general<Tr>(1, batch_count, 1, cpu_result, hipblas_result_device);
        }
        if(argus.norm_check)
        {
            hipblas_error_host
                = norm_check_general<Tr>('F', 1, batch_count, 1, cpu_result, hipblas_result_host);
            hipblas_error_device
                = norm_check_general<Tr>('F', 1, batch_count, 1, cpu_result, hipblas_result_device);
        }

    } // end of if unit/norm check

    if(argus.timing)
    {
        hipStream_t stream;
        CHECK_HIPBLAS_ERROR(hipblasGetStream(handle, &stream));
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));

        int runs = argus.cold_iters + argus.iters;
        for(int iter = 0; iter < runs; iter++)
        {
            if(iter == argus.cold_iters)
                gpu_time_used = get_time_us_sync(stream);

            CHECK_HIPBLAS_ERROR(hipblasAsumStridedBatchedFn(
                handle, N, dx, incx, stridex, batch_count, d_hipblas_result));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used;

        ArgumentModel<e_N, e_incx, e_stride_x, e_batch_count>{}.log_args<T>(std::cout,
                                                                            argus,
                                                                            gpu_time_used,
                                                                            asum_gflop_count<T>(N),
                                                                            asum_gbyte_count<T>(N),
                                                                            hipblas_error_host,
                                                                            hipblas_error_device);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
