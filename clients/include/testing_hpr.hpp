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
hipblasStatus_t testing_hpr(const Arguments& argus)
{
    using U           = real_t<T>;
    bool FORTRAN      = argus.fortran;
    auto hipblasHprFn = FORTRAN ? hipblasHpr<T, U, true> : hipblasHpr<T, U, false>;

    int N    = argus.N;
    int incx = argus.incx;

    int               abs_incx = incx >= 0 ? incx : -incx;
    size_t            x_size   = size_t(N) * abs_incx;
    size_t            A_size   = size_t(N) * (N + 1) / 2;
    hipblasFillMode_t uplo     = char2hipblas_fill(argus.uplo_option);

    hipblasLocalHandle handle(argus);

    // argument sanity check, quick return if input parameters are invalid before allocating invalid
    // memory
    bool invalid_size = N < 0 || !incx;
    if(invalid_size || !N)
    {
        hipblasStatus_t actual = hipblasHprFn(handle, uplo, N, nullptr, nullptr, incx, nullptr);
        EXPECT_HIPBLAS_STATUS(
            actual, (invalid_size ? HIPBLAS_STATUS_INVALID_VALUE : HIPBLAS_STATUS_SUCCESS));
        return actual;
    }

    // Naming: dK is in GPU (device) memory. hK is in CPU (host) memory
    host_vector<T> hA(A_size);
    host_vector<T> hA_cpu(A_size);
    host_vector<T> hA_host(A_size);
    host_vector<T> hA_device(A_size);
    host_vector<T> hx(x_size);

    device_vector<T> dA(A_size);
    device_vector<T> dx(x_size);
    device_vector<U> d_alpha(1);

    double gpu_time_used, hipblas_error_host, hipblas_error_device;

    U h_alpha = argus.get_alpha<U>();

    // Initial Data on CPU
    srand(1);
    hipblas_init<T>(hA, 1, A_size, 1);
    hipblas_init<T>(hx, 1, N, abs_incx);

    // copy matrix is easy in STL; hA_cpu = hA: save a copy in hA_cpu which will be output of CPU BLAS
    hA_cpu = hA;

    // copy data from CPU to device
    CHECK_HIP_ERROR(hipMemcpy(dA, hA.data(), sizeof(T) * A_size, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(dx, hx.data(), sizeof(T) * x_size, hipMemcpyHostToDevice));
    CHECK_HIP_ERROR(hipMemcpy(d_alpha, &h_alpha, sizeof(U), hipMemcpyHostToDevice));

    if(argus.unit_check || argus.norm_check)
    {
        /* =====================================================================
            HIPBLAS
        =================================================================== */
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_HOST));
        CHECK_HIPBLAS_ERROR(hipblasHprFn(handle, uplo, N, (U*)&h_alpha, dx, incx, dA));

        CHECK_HIP_ERROR(hipMemcpy(hA_host.data(), dA, sizeof(T) * A_size, hipMemcpyDeviceToHost));
        CHECK_HIP_ERROR(hipMemcpy(dA, hA.data(), sizeof(T) * A_size, hipMemcpyHostToDevice));

        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));
        CHECK_HIPBLAS_ERROR(hipblasHprFn(handle, uplo, N, d_alpha, dx, incx, dA));

        CHECK_HIP_ERROR(hipMemcpy(hA_device.data(), dA, sizeof(T) * A_size, hipMemcpyDeviceToHost));

        /* =====================================================================
           CPU BLAS
        =================================================================== */
        cblas_hpr<T>(uplo, N, h_alpha, hx.data(), incx, hA_cpu.data());

        // enable unit check, notice unit check is not invasive, but norm check is,
        // unit check and norm check can not be interchanged their order
        if(argus.unit_check)
        {
            unit_check_general<T>(1, A_size, 1, hA_cpu.data(), hA_host.data());
            unit_check_general<T>(1, A_size, 1, hA_cpu.data(), hA_device.data());
        }
        if(argus.norm_check)
        {
            hipblas_error_host   = norm_check_general<T>('F', 1, A_size, 1, hA_cpu, hA_host);
            hipblas_error_device = norm_check_general<T>('F', 1, A_size, 1, hA_cpu, hA_device);
        }
    }

    if(argus.timing)
    {
        CHECK_HIP_ERROR(hipMemcpy(dA, hA.data(), sizeof(T) * A_size, hipMemcpyHostToDevice));
        hipStream_t stream;
        CHECK_HIPBLAS_ERROR(hipblasGetStream(handle, &stream));
        CHECK_HIPBLAS_ERROR(hipblasSetPointerMode(handle, HIPBLAS_POINTER_MODE_DEVICE));

        int runs = argus.cold_iters + argus.iters;
        for(int iter = 0; iter < runs; iter++)
        {
            if(iter == argus.cold_iters)
                gpu_time_used = get_time_us_sync(stream);

            CHECK_HIPBLAS_ERROR(hipblasHprFn(handle, uplo, N, d_alpha, dx, incx, dA));
        }
        gpu_time_used = get_time_us_sync(stream) - gpu_time_used;

        ArgumentModel<e_N, e_alpha, e_incx>{}.log_args<U>(std::cout,
                                                          argus,
                                                          gpu_time_used,
                                                          hpr_gflop_count<T>(N),
                                                          hpr_gbyte_count<T>(N),
                                                          hipblas_error_host,
                                                          hipblas_error_device);
    }

    return HIPBLAS_STATUS_SUCCESS;
}
