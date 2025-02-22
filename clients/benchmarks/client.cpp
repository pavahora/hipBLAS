/* ************************************************************************
 * Copyright 2016-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#include "program_options.hpp"

#include "hipblas.hpp"

#include "argument_model.hpp"
#include "hipblas_datatype2string.hpp"
#include "hipblas_parse_data.hpp"
#include "type_dispatch.hpp"
#include "utility.h"

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>
// aux
#include "testing_set_get_matrix.hpp"
#include "testing_set_get_matrix_async.hpp"
#include "testing_set_get_vector.hpp"
#include "testing_set_get_vector_async.hpp"
// blas1
#include "testing_asum.hpp"
#include "testing_asum_batched.hpp"
#include "testing_asum_strided_batched.hpp"
#include "testing_axpy.hpp"
#include "testing_axpy_batched.hpp"
#include "testing_axpy_batched_ex.hpp"
#include "testing_axpy_ex.hpp"
#include "testing_axpy_strided_batched.hpp"
#include "testing_axpy_strided_batched_ex.hpp"
#include "testing_copy.hpp"
#include "testing_copy_batched.hpp"
#include "testing_copy_strided_batched.hpp"
#include "testing_dot.hpp"
#include "testing_dot_batched.hpp"
#include "testing_dot_batched_ex.hpp"
#include "testing_dot_ex.hpp"
#include "testing_dot_strided_batched.hpp"
#include "testing_dot_strided_batched_ex.hpp"
#include "testing_iamax_iamin.hpp"
#include "testing_iamax_iamin_batched.hpp"
#include "testing_iamax_iamin_strided_batched.hpp"
#include "testing_nrm2.hpp"
#include "testing_nrm2_batched.hpp"
#include "testing_nrm2_batched_ex.hpp"
#include "testing_nrm2_ex.hpp"
#include "testing_nrm2_strided_batched.hpp"
#include "testing_nrm2_strided_batched_ex.hpp"
#include "testing_rot.hpp"
#include "testing_rot_batched.hpp"
#include "testing_rot_batched_ex.hpp"
#include "testing_rot_ex.hpp"
#include "testing_rot_strided_batched.hpp"
#include "testing_rot_strided_batched_ex.hpp"
#include "testing_rotg.hpp"
#include "testing_rotg_batched.hpp"
#include "testing_rotg_strided_batched.hpp"
#include "testing_rotm.hpp"
#include "testing_rotm_batched.hpp"
#include "testing_rotm_strided_batched.hpp"
#include "testing_rotmg.hpp"
#include "testing_rotmg_batched.hpp"
#include "testing_rotmg_strided_batched.hpp"
#include "testing_scal.hpp"
#include "testing_scal_batched.hpp"
#include "testing_scal_batched_ex.hpp"
#include "testing_scal_ex.hpp"
#include "testing_scal_strided_batched.hpp"
#include "testing_scal_strided_batched_ex.hpp"
#include "testing_swap.hpp"
#include "testing_swap_batched.hpp"
#include "testing_swap_strided_batched.hpp"
// blas2
#include "testing_gbmv.hpp"
#include "testing_gbmv_batched.hpp"
#include "testing_gbmv_strided_batched.hpp"
#include "testing_gemv.hpp"
#include "testing_gemv_batched.hpp"
#include "testing_gemv_strided_batched.hpp"
#include "testing_ger.hpp"
#include "testing_ger_batched.hpp"
#include "testing_ger_strided_batched.hpp"
#include "testing_hbmv.hpp"
#include "testing_hbmv_batched.hpp"
#include "testing_hbmv_strided_batched.hpp"
#include "testing_hemv.hpp"
#include "testing_hemv_batched.hpp"
#include "testing_hemv_strided_batched.hpp"
#include "testing_her.hpp"
#include "testing_her2.hpp"
#include "testing_her2_batched.hpp"
#include "testing_her2_strided_batched.hpp"
#include "testing_her_batched.hpp"
#include "testing_her_strided_batched.hpp"
#include "testing_hpmv.hpp"
#include "testing_hpmv_batched.hpp"
#include "testing_hpmv_strided_batched.hpp"
#include "testing_hpr.hpp"
#include "testing_hpr2.hpp"
#include "testing_hpr2_batched.hpp"
#include "testing_hpr2_strided_batched.hpp"
#include "testing_hpr_batched.hpp"
#include "testing_hpr_strided_batched.hpp"
#include "testing_sbmv.hpp"
#include "testing_sbmv_batched.hpp"
#include "testing_sbmv_strided_batched.hpp"
#include "testing_spmv.hpp"
#include "testing_spmv_batched.hpp"
#include "testing_spmv_strided_batched.hpp"
#include "testing_spr.hpp"
#include "testing_spr2.hpp"
#include "testing_spr2_batched.hpp"
#include "testing_spr2_strided_batched.hpp"
#include "testing_spr_batched.hpp"
#include "testing_spr_strided_batched.hpp"
#include "testing_symv.hpp"
#include "testing_symv_batched.hpp"
#include "testing_symv_strided_batched.hpp"
#include "testing_syr.hpp"
#include "testing_syr2.hpp"
#include "testing_syr2_batched.hpp"
#include "testing_syr2_strided_batched.hpp"
#include "testing_syr_batched.hpp"
#include "testing_syr_strided_batched.hpp"
#include "testing_tbmv.hpp"
#include "testing_tbmv_batched.hpp"
#include "testing_tbmv_strided_batched.hpp"
#include "testing_tbsv.hpp"
#include "testing_tbsv_batched.hpp"
#include "testing_tbsv_strided_batched.hpp"
#include "testing_tpmv.hpp"
#include "testing_tpmv_batched.hpp"
#include "testing_tpmv_strided_batched.hpp"
#include "testing_tpsv.hpp"
#include "testing_tpsv_batched.hpp"
#include "testing_tpsv_strided_batched.hpp"
#include "testing_trmv.hpp"
#include "testing_trmv_batched.hpp"
#include "testing_trmv_strided_batched.hpp"
// blas3
#include "syrkx_reference.hpp"
#include "testing_dgmm.hpp"
#include "testing_dgmm_batched.hpp"
#include "testing_dgmm_strided_batched.hpp"
#include "testing_geam.hpp"
#include "testing_geam_batched.hpp"
#include "testing_geam_strided_batched.hpp"
#include "testing_gemm.hpp"
#include "testing_gemm_batched.hpp"
#include "testing_gemm_batched_ex.hpp"
#include "testing_gemm_ex.hpp"
#include "testing_gemm_strided_batched.hpp"
#include "testing_gemm_strided_batched_ex.hpp"
#include "testing_hemm.hpp"
#include "testing_hemm_batched.hpp"
#include "testing_hemm_strided_batched.hpp"
#include "testing_her2k.hpp"
#include "testing_her2k_batched.hpp"
#include "testing_her2k_strided_batched.hpp"
#include "testing_herk.hpp"
#include "testing_herk_batched.hpp"
#include "testing_herk_strided_batched.hpp"
#include "testing_herkx.hpp"
#include "testing_herkx_batched.hpp"
#include "testing_herkx_strided_batched.hpp"
#include "testing_symm.hpp"
#include "testing_symm_batched.hpp"
#include "testing_symm_strided_batched.hpp"
#include "testing_syr2k.hpp"
#include "testing_syr2k_batched.hpp"
#include "testing_syr2k_strided_batched.hpp"
#include "testing_syrk.hpp"
#include "testing_syrk_batched.hpp"
#include "testing_syrk_strided_batched.hpp"
#include "testing_syrkx.hpp"
#include "testing_syrkx_batched.hpp"
#include "testing_syrkx_strided_batched.hpp"
#include "testing_trmm.hpp"
#include "testing_trmm_batched.hpp"
#include "testing_trmm_strided_batched.hpp"
#include "testing_trsm.hpp"
#include "testing_trsm_batched.hpp"
#include "testing_trsm_batched_ex.hpp"
#include "testing_trsm_ex.hpp"
#include "testing_trsm_strided_batched.hpp"
#include "testing_trsm_strided_batched_ex.hpp"
#include "testing_trsv.hpp"
#include "testing_trsv_batched.hpp"
#include "testing_trsv_strided_batched.hpp"
#include "testing_trtri.hpp"
#include "testing_trtri_batched.hpp"
#include "testing_trtri_strided_batched.hpp"
// solver functions
#ifdef __HIP_PLATFORM_SOLVER__
#include "testing_geqrf.hpp"
#include "testing_geqrf_batched.hpp"
#include "testing_geqrf_strided_batched.hpp"
#include "testing_getrf.hpp"
#include "testing_getrf_batched.hpp"
#include "testing_getrf_npvt.hpp"
#include "testing_getrf_npvt_batched.hpp"
#include "testing_getrf_npvt_strided_batched.hpp"
#include "testing_getrf_strided_batched.hpp"
#include "testing_getri_batched.hpp"
#include "testing_getri_npvt_batched.hpp"
#include "testing_getrs.hpp"
#include "testing_getrs_batched.hpp"
#include "testing_getrs_strided_batched.hpp"
#endif

#include "utility.h"
#include <algorithm>
#undef I

using namespace roc; // For emulated program_options
using namespace std::literals; // For std::string literals of form "str"s

typedef int hipblas_int;

struct str_less
{
    bool operator()(const char* a, const char* b) const
    {
        return strcmp(a, b) < 0;
    }
};

// ----------------------------------------------------------------------------
// Normal tests which return true when converted to bool
// ----------------------------------------------------------------------------
struct hipblas_test_valid
{
    // Return true to indicate the type combination is valid, for filtering
    virtual explicit operator bool() final
    {
        return true;
    }

    // Require derived class to define functor which takes (const Arguments &)
    virtual void operator()(const Arguments&) = 0;

    virtual ~hipblas_test_valid() = default;
};

// ----------------------------------------------------------------------------
// Error case which returns false when converted to bool. A void specialization
// of the FILTER class template above, should be derived from this class, in
// order to indicate that the type combination is invalid.
// ----------------------------------------------------------------------------
struct hipblas_test_invalid
{
    // Return false to indicate the type combination is invalid, for filtering
    virtual explicit operator bool() final
    {
        return false;
    }

    // If this specialization is actually called, print fatal error message
    virtual void operator()(const Arguments&) final
    {
        static constexpr char msg[] = "Internal error: Test called with invalid types";

#ifdef GOOGLE_TEST
        FAIL() << msg;
#else
        std::cerr << msg << std::endl;
        abort();
#endif
    }

    virtual ~hipblas_test_invalid() = default;
};

// Map from const char* to function taking const Arguments& using comparison above
using func_map = std::map<const char*, hipblasStatus_t (*)(const Arguments&), str_less>;

// Run a function by using map to map arg.function to function
void run_function(const func_map& map, const Arguments& arg, const std::string& msg = "")
{
    auto match = map.find(arg.function);
    if(match == map.end())
        throw std::invalid_argument("Invalid combination --function "s + arg.function
                                    + " --a_type "s + hipblas_datatype2string(arg.a_type) + msg);
    match->second(arg);
}

// Template to dispatch testing_gemm_ex for performance tests
// When Ti == void or Ti == To == Tc == bfloat16, the test is marked invalid
template <typename Ti, typename To = Ti, typename Tc = To, typename = void>
struct perf_gemm_ex : hipblas_test_invalid
{
};

template <typename Ti, typename To, typename Tc>
struct perf_gemm_ex<Ti,
                    To,
                    Tc,
                    std::enable_if_t<!std::is_same<Ti, void>{}
                                     && !(std::is_same<Ti, To>{} && std::is_same<Ti, Tc>{}
                                          && std::is_same<Ti, hipblasBfloat16>{})>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"gemm_ex", testing_gemm_ex_template<Ti, Ti, To, Tc>},
            {"gemm_batched_ex", testing_gemm_batched_ex_template<Ti, Ti, To, Tc>},
        };
        run_function(map, arg);
    }
};

// Template to dispatch testing_gemm_strided_batched_ex for performance tests
// When Ti == void or Ti == To == Tc == bfloat16, the test is marked invalid
template <typename Ti, typename To = Ti, typename Tc = To, typename = void>
struct perf_gemm_strided_batched_ex : hipblas_test_invalid
{
};

template <typename Ti, typename To, typename Tc>
struct perf_gemm_strided_batched_ex<
    Ti,
    To,
    Tc,
    std::enable_if_t<!std::is_same<Ti, void>{}
                     && !(std::is_same<Ti, To>{} && std::is_same<Ti, Tc>{}
                          && std::is_same<Ti, hipblasBfloat16>{})>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"gemm_strided_batched_ex", testing_gemm_strided_batched_ex_template<Ti, Ti, To, Tc>},
        };
        run_function(map, arg);
    }
};

template <typename T, typename U = T, typename = void>
struct perf_blas : hipblas_test_invalid
{
};

template <typename T, typename U>
struct perf_blas<T, U, std::enable_if_t<std::is_same<T, float>{} || std::is_same<T, double>{}>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map fmap = {
            // L1
            {"asum", testing_asum<T>},
            {"asum_batched", testing_asum_batched<T>},
            {"asum_strided_batched", testing_asum_strided_batched<T>},
            {"axpy", testing_axpy<T>},
            {"axpy_batched", testing_axpy_batched<T>},
            {"axpy_strided_batched", testing_axpy_strided_batched<T>},
            {"copy", testing_copy<T>},
            {"copy_batched", testing_copy_batched<T>},
            {"copy_strided_batched", testing_copy_strided_batched<T>},
            {"dot", testing_dot<T>},
            {"dot_batched", testing_dot_batched<T>},
            {"dot_strided_batched", testing_dot_strided_batched<T>},
            {"iamax", testing_amax<T>},
            {"iamax_batched", testing_amax_batched<T>},
            {"iamax_strided_batched", testing_amax_strided_batched<T>},
            {"iamin", testing_amin<T>},
            {"iamin_batched", testing_amin_batched<T>},
            {"iamin_strided_batched", testing_amin_strided_batched<T>},
            {"nrm2", testing_nrm2<T>},
            {"nrm2_batched", testing_nrm2_batched<T>},
            {"nrm2_strided_batched", testing_nrm2_strided_batched<T>},
            {"rotg", testing_rotg<T>},
            {"rotg_batched", testing_rotg_batched<T>},
            {"rotg_strided_batched", testing_rotg_strided_batched<T>},
            {"rotm", testing_rotm<T>},
            {"rotm_batched", testing_rotm_batched<T>},
            {"rotm_strided_batched", testing_rotm_strided_batched<T>},
            {"rotmg", testing_rotmg<T>},
            {"rotmg_batched", testing_rotmg_batched<T>},
            {"rotmg_strided_batched", testing_rotmg_strided_batched<T>},
            {"swap", testing_swap<T>},
            {"swap_batched", testing_swap_batched<T>},
            {"swap_strided_batched", testing_swap_strided_batched<T>},
            {"scal", testing_scal<T>},
            {"scal_batched", testing_scal_batched<T>},
            {"scal_strided_batched", testing_scal_strided_batched<T>},

            // L2
            {"gbmv", testing_gbmv<T>},
            {"gbmv_batched", testing_gbmv_batched<T>},
            {"gbmv_strided_batched", testing_gbmv_strided_batched<T>},
            {"gemv", testing_gemv<T>},
            {"gemv_batched", testing_gemv_batched<T>},
            {"gemv_strided_batched", testing_gemv_strided_batched<T>},
            {"ger", testing_ger<T, false>},
            {"ger_batched", testing_ger_batched<T, false>},
            {"ger_strided_batched", testing_ger_strided_batched<T, false>},
            {"sbmv", testing_sbmv<T>},
            {"sbmv_batched", testing_sbmv_batched<T>},
            {"sbmv_strided_batched", testing_sbmv_strided_batched<T>},
            {"spmv", testing_spmv<T>},
            {"spmv_batched", testing_spmv_batched<T>},
            {"spmv_strided_batched", testing_spmv_strided_batched<T>},
            {"spr", testing_spr<T>},
            {"spr_batched", testing_spr_batched<T>},
            {"spr_strided_batched", testing_spr_strided_batched<T>},
            {"spr2", testing_spr2<T>},
            {"spr2_batched", testing_spr2_batched<T>},
            {"spr2_strided_batched", testing_spr2_strided_batched<T>},
            {"symv", testing_symv<T>},
            {"symv_batched", testing_symv_batched<T>},
            {"symv_strided_batched", testing_symv_strided_batched<T>},
            {"syr", testing_syr<T>},
            {"syr_batched", testing_syr_batched<T>},
            {"syr_strided_batched", testing_syr_strided_batched<T>},
            {"syr2", testing_syr2<T>},
            {"syr2_batched", testing_syr2_batched<T>},
            {"syr2_strided_batched", testing_syr2_strided_batched<T>},
            {"tbmv", testing_tbmv<T>},
            {"tbmv_batched", testing_tbmv_batched<T>},
            {"tbmv_strided_batched", testing_tbmv_strided_batched<T>},
            {"tbsv", testing_tbsv<T>},
            {"tbsv_batched", testing_tbsv_batched<T>},
            {"tbsv_strided_batched", testing_tbsv_strided_batched<T>},
            {"tpmv", testing_tpmv<T>},
            {"tpmv_batched", testing_tpmv_batched<T>},
            {"tpmv_strided_batched", testing_tpmv_strided_batched<T>},
            {"tpsv", testing_tpsv<T>},
            {"tpsv_batched", testing_tpsv_batched<T>},
            {"tpsv_strided_batched", testing_tpsv_strided_batched<T>},
            {"trmv", testing_trmv<T>},
            {"trmv_batched", testing_trmv_batched<T>},
            {"trmv_strided_batched", testing_trmv_strided_batched<T>},
            {"trsv", testing_trsv<T>},
            {"trsv_batched", testing_trsv_batched<T>},
            {"trsv_strided_batched", testing_trsv_strided_batched<T>},

            // L3
            {"geam", testing_geam<T>},
            {"geam_batched", testing_geam_batched<T>},
            {"geam_strided_batched", testing_geam_strided_batched<T>},
            {"dgmm", testing_dgmm<T>},
            {"dgmm_batched", testing_dgmm_batched<T>},
            {"dgmm_strided_batched", testing_dgmm_strided_batched<T>},
            {"trmm", testing_trmm<T>},
            {"trmm_batched", testing_trmm_batched<T>},
            {"trmm_strided_batched", testing_trmm_strided_batched<T>},
            {"gemm", testing_gemm<T>},
            {"gemm_batched", testing_gemm_batched<T>},
            {"gemm_strided_batched", testing_gemm_strided_batched<T>},
            {"symm", testing_symm<T>},
            {"symm_batched", testing_symm_batched<T>},
            {"symm_strided_batched", testing_symm_strided_batched<T>},
            {"syrk", testing_syrk<T>},
            {"syrk_batched", testing_syrk_batched<T>},
            {"syrk_strided_batched", testing_syrk_strided_batched<T>},
            {"syr2k", testing_syr2k<T>},
            {"syr2k_batched", testing_syr2k_batched<T>},
            {"syr2k_strided_batched", testing_syr2k_strided_batched<T>},
            {"trtri", testing_trtri<T>},
            {"trtri_batched", testing_trtri_batched<T>},
            {"trtri_strided_batched", testing_trtri_strided_batched<T>},
            {"syrkx", testing_syrkx<T>},
            {"syrkx_batched", testing_syrkx_batched<T>},
            {"syrkx_strided_batched", testing_syrkx_strided_batched<T>},
            {"trsm", testing_trsm<T>},
            {"trsm_ex", testing_trsm_ex<T>},
            {"trsm_batched", testing_trsm_batched<T>},
            {"trsm_batched_ex", testing_trsm_batched_ex<T>},
            {"trsm_strided_batched", testing_trsm_strided_batched<T>},
            {"trsm_strided_batched_ex", testing_trsm_strided_batched_ex<T>},

#ifdef __HIP_PLATFORM_SOLVER__
            {"geqrf", testing_geqrf<T>},
            {"geqrf_batched", testing_geqrf_batched<T>},
            {"geqrf_strided_batched", testing_geqrf_strided_batched<T>},
            {"getrf", testing_getrf<T>},
            {"getrf_batched", testing_getrf_batched<T>},
            {"getrf_strided_batched", testing_getrf_strided_batched<T>},
            {"getrf_npvt", testing_getrf_npvt<T>},
            {"getrf_npvt_batched", testing_getrf_npvt_batched<T>},
            {"getrf_npvt_strided_batched", testing_getrf_npvt_strided_batched<T>},
            {"getri_batched", testing_getri_batched<T>},
            {"getri_npvt_batched", testing_getri_npvt_batched<T>},
            {"getrs", testing_getrs<T>},
            {"getrs_batched", testing_getrs_batched<T>},
            {"getrs_strided_batched", testing_getrs_strided_batched<T>},
#endif

            // Aux
            {"set_get_vector", testing_set_get_vector<T>},
            {"set_get_vector_async", testing_set_get_vector_async<T>},
            {"set_get_matrix", testing_set_get_matrix<T>},
            {"set_get_matrix_async", testing_set_get_matrix_async<T>},
        };
        run_function(fmap, arg);
    }
};

template <typename T, typename U>
struct perf_blas<T, U, std::enable_if_t<std::is_same<T, hipblasBfloat16>{}>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"dot", testing_dot<T>},
            {"dot_batched", testing_dot_batched<T>},
            {"dot_strided_batched", testing_dot_strided_batched<T>},
        };
        run_function(map, arg);
    }
};

template <typename T, typename U>
struct perf_blas<T, U, std::enable_if_t<std::is_same<T, hipblasHalf>{}>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"axpy", testing_axpy<T>},
            {"axpy_batched", testing_axpy_batched<T>},
            {"axpy_strided_batched", testing_axpy_strided_batched<T>},
            {"dot", testing_dot<T>},
            {"dot_batched", testing_dot_batched<T>},
            {"dot_strided_batched", testing_dot_strided_batched<T>},
            {"gemm", testing_gemm<T>},
            {"gemm_batched", testing_gemm_batched<T>},
            {"gemm_strided_batched", testing_gemm_strided_batched<T>},

        };
        run_function(map, arg);
    }
};

template <typename T, typename U>
struct perf_blas<
    T,
    U,
    std::enable_if_t<std::is_same<T, hipblasDoubleComplex>{} || std::is_same<T, hipblasComplex>{}>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            // L1
            {"asum", testing_asum<T>},
            {"asum_batched", testing_asum_batched<T>},
            {"asum_strided_batched", testing_asum_strided_batched<T>},
            {"axpy", testing_axpy<T>},
            {"axpy_batched", testing_axpy_batched<T>},
            {"axpy_strided_batched", testing_axpy_strided_batched<T>},
            {"copy", testing_copy<T>},
            {"copy_batched", testing_copy_batched<T>},
            {"copy_strided_batched", testing_copy_strided_batched<T>},
            {"dot", testing_dot<T>},
            {"dot_batched", testing_dot_batched<T>},
            {"dot_strided_batched", testing_dot_strided_batched<T>},
            {"dotc", testing_dotc<T>},
            {"dotc_batched", testing_dotc_batched<T>},
            {"dotc_strided_batched", testing_dotc_strided_batched<T>},
            {"iamax", testing_amax<T>},
            {"iamax_batched", testing_amax_batched<T>},
            {"iamax_strided_batched", testing_amax_strided_batched<T>},
            {"iamin", testing_amin<T>},
            {"iamin_batched", testing_amin_batched<T>},
            {"iamin_strided_batched", testing_amin_strided_batched<T>},
            {"nrm2", testing_nrm2<T>},
            {"nrm2_batched", testing_nrm2_batched<T>},
            {"nrm2_strided_batched", testing_nrm2_strided_batched<T>},
            {"rotg", testing_rotg<T>},
            {"rotg_batched", testing_rotg_batched<T>},
            {"rotg_strided_batched", testing_rotg_strided_batched<T>},
            {"swap", testing_swap<T>},
            {"swap_batched", testing_swap_batched<T>},
            {"swap_strided_batched", testing_swap_strided_batched<T>},
            {"scal", testing_scal<T>},
            {"scal_batched", testing_scal_batched<T>},
            {"scal_strided_batched", testing_scal_strided_batched<T>},

            // L2
            {"gemv", testing_gemv<T>},
            {"gemv_batched", testing_gemv_batched<T>},
            {"gemv_strided_batched", testing_gemv_strided_batched<T>},
            {"gbmv", testing_gbmv<T>},
            {"gbmv_batched", testing_gbmv_batched<T>},
            {"gbmv_strided_batched", testing_gbmv_strided_batched<T>},
            {"geru", testing_ger<T, false>},
            {"geru_batched", testing_ger_batched<T, false>},
            {"geru_strided_batched", testing_ger_strided_batched<T, false>},
            {"gerc", testing_ger<T, true>},
            {"gerc_batched", testing_ger_batched<T, true>},
            {"gerc_strided_batched", testing_ger_strided_batched<T, true>},
            {"hbmv", testing_hbmv<T>},
            {"hbmv_batched", testing_hbmv_batched<T>},
            {"hbmv_strided_batched", testing_hbmv_strided_batched<T>},
            {"hemv", testing_hemv<T>},
            {"hemv_batched", testing_hemv_batched<T>},
            {"hemv_strided_batched", testing_hemv_strided_batched<T>},
            {"her", testing_her<T>},
            {"her_batched", testing_her_batched<T>},
            {"her_strided_batched", testing_her_strided_batched<T>},
            {"her2", testing_her2<T>},
            {"her2_batched", testing_her2_batched<T>},
            {"her2_strided_batched", testing_her2_strided_batched<T>},
            {"hpmv", testing_hpmv<T>},
            {"hpmv_batched", testing_hpmv_batched<T>},
            {"hpmv_strided_batched", testing_hpmv_strided_batched<T>},
            {"hpr", testing_hpr<T>},
            {"hpr_batched", testing_hpr_batched<T>},
            {"hpr_strided_batched", testing_hpr_strided_batched<T>},
            {"hpr2", testing_hpr2<T>},
            {"hpr2_batched", testing_hpr2_batched<T>},
            {"hpr2_strided_batched", testing_hpr2_strided_batched<T>},
            {"spr", testing_spr<T>},
            {"spr_batched", testing_spr_batched<T>},
            {"spr_strided_batched", testing_spr_strided_batched<T>},
            {"symv", testing_symv<T>},
            {"symv_batched", testing_symv_batched<T>},
            {"symv_strided_batched", testing_symv_strided_batched<T>},
            {"syr", testing_syr<T>},
            {"syr_batched", testing_syr_batched<T>},
            {"syr_strided_batched", testing_syr_strided_batched<T>},
            {"syr2", testing_syr2<T>},
            {"syr2_batched", testing_syr2_batched<T>},
            {"syr2_strided_batched", testing_syr2_strided_batched<T>},
            {"tbmv", testing_tbmv<T>},
            {"tbmv_batched", testing_tbmv_batched<T>},
            {"tbmv_strided_batched", testing_tbmv_strided_batched<T>},
            {"tbsv", testing_tbsv<T>},
            {"tbsv_batched", testing_tbsv_batched<T>},
            {"tbsv_strided_batched", testing_tbsv_strided_batched<T>},
            {"tpmv", testing_tpmv<T>},
            {"tpmv_batched", testing_tpmv_batched<T>},
            {"tpmv_strided_batched", testing_tpmv_strided_batched<T>},
            {"tpsv", testing_tpsv<T>},
            {"tpsv_batched", testing_tpsv_batched<T>},
            {"tpsv_strided_batched", testing_tpsv_strided_batched<T>},
            {"trmv", testing_trmv<T>},
            {"trmv_batched", testing_trmv_batched<T>},
            {"trmv_strided_batched", testing_trmv_strided_batched<T>},
            {"trsv", testing_trsv<T>},
            {"trsv_batched", testing_trsv_batched<T>},
            {"trsv_strided_batched", testing_trsv_strided_batched<T>},

            // L3
            {"dgmm", testing_dgmm<T>},
            {"dgmm_batched", testing_dgmm_batched<T>},
            {"dgmm_strided_batched", testing_dgmm_strided_batched<T>},
            {"geam", testing_geam<T>},
            {"geam_batched", testing_geam_batched<T>},
            {"geam_strided_batched", testing_geam_strided_batched<T>},
            {"gemm", testing_gemm<T>},
            {"gemm_batched", testing_gemm_batched<T>},
            {"gemm_strided_batched", testing_gemm_strided_batched<T>},
            {"hemm", testing_hemm<T>},
            {"hemm_batched", testing_hemm_batched<T>},
            {"hemm_strided_batched", testing_hemm_strided_batched<T>},
            {"herk", testing_herk<T>},
            {"herk_batched", testing_herk_batched<T>},
            {"herk_strided_batched", testing_herk_strided_batched<T>},
            {"her2k", testing_her2k<T>},
            {"her2k_batched", testing_her2k_batched<T>},
            {"her2k_strided_batched", testing_her2k_strided_batched<T>},
            {"herkx", testing_herkx<T>},
            {"herkx_batched", testing_herkx_batched<T>},
            {"herkx_strided_batched", testing_herkx_strided_batched<T>},
            {"symm", testing_symm<T>},
            {"symm_batched", testing_symm_batched<T>},
            {"symm_strided_batched", testing_symm_strided_batched<T>},
            {"syrk", testing_syrk<T>},
            {"syrk_batched", testing_syrk_batched<T>},
            {"syrk_strided_batched", testing_syrk_strided_batched<T>},
            {"syr2k", testing_syr2k<T>},
            {"syr2k_batched", testing_syr2k_batched<T>},
            {"syr2k_strided_batched", testing_syr2k_strided_batched<T>},
            {"trtri", testing_trtri<T>},
            {"trtri_batched", testing_trtri_batched<T>},
            {"trtri_strided_batched", testing_trtri_strided_batched<T>},
            {"syrkx", testing_syrkx<T>},
            {"syrkx_batched", testing_syrkx_batched<T>},
            {"syrkx_strided_batched", testing_syrkx_strided_batched<T>},
            {"trsm", testing_trsm<T>},
            {"trsm_ex", testing_trsm_ex<T>},
            {"trsm_batched", testing_trsm_batched<T>},
            {"trsm_batched_ex", testing_trsm_batched_ex<T>},
            {"trsm_strided_batched", testing_trsm_strided_batched<T>},
            {"trsm_strided_batched_ex", testing_trsm_strided_batched_ex<T>},

            {"trmm", testing_trmm<T>},
            {"trmm_batched", testing_trmm_batched<T>},
            {"trmm_strided_batched", testing_trmm_strided_batched<T>},

#ifdef __HIP_PLATFORM_SOLVER__
            {"geqrf", testing_geqrf<T>},
            {"geqrf_batched", testing_geqrf_batched<T>},
            {"geqrf_strided_batched", testing_geqrf_strided_batched<T>},
            {"getrf", testing_getrf<T>},
            {"getrf_batched", testing_getrf_batched<T>},
            {"getrf_strided_batched", testing_getrf_strided_batched<T>},
            {"getrf_npvt", testing_getrf_npvt<T>},
            {"getrf_npvt_batched", testing_getrf_npvt_batched<T>},
            {"getrf_npvt_strided_batched", testing_getrf_npvt_strided_batched<T>},
            {"getri_batched", testing_getri_batched<T>},
            {"getri_npvt_batched", testing_getri_npvt_batched<T>},
            {"getrs", testing_getrs<T>},
            {"getrs_batched", testing_getrs_batched<T>},
            {"getrs_strided_batched", testing_getrs_strided_batched<T>},
#endif
        };
        run_function(map, arg);
    }
};

template <typename Ta, typename Tx = Ta, typename Ty = Tx, typename Tex = Ty, typename = void>
struct perf_blas_axpy_ex : hipblas_test_invalid
{
};

template <typename Ta, typename Tx, typename Ty, typename Tex>
struct perf_blas_axpy_ex<
    Ta,
    Tx,
    Ty,
    Tex,
    std::enable_if_t<((std::is_same<Ta, float>{} && std::is_same<Ta, Tx>{} && std::is_same<Tx, Ty>{}
                       && std::is_same<Ty, Tex>{})
                      || (std::is_same<Ta, double>{} && std::is_same<Ta, Tx>{}
                          && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tex>{})
                      || (std::is_same<Ta, hipblasHalf>{} && std::is_same<Ta, Tx>{}
                          && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tex>{})
                      || (std::is_same<Ta, hipblasComplex>{} && std::is_same<Ta, Tx>{}
                          && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tex>{})
                      || (std::is_same<Ta, hipblasDoubleComplex>{} && std::is_same<Ta, Tx>{}
                          && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tex>{})
                      || (std::is_same<Ta, hipblasHalf>{} && std::is_same<Ta, Tx>{}
                          && std::is_same<Tx, Ty>{} && std::is_same<Tex, float>{}))>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"axpy_ex", testing_axpy_ex_template<Ta, Tx, Ty>},
            {"axpy_batched_ex", testing_axpy_batched_ex_template<Ta, Tx, Ty>},
            {"axpy_strided_batched_ex", testing_axpy_strided_batched_ex_template<Ta, Tx, Ty>},
        };
        run_function(map, arg);
    }
};

template <typename Tx, typename Ty = Tx, typename Tr = Ty, typename Tex = Tr, typename = void>
struct perf_blas_dot_ex : hipblas_test_invalid
{
};

template <typename Tx, typename Ty, typename Tr, typename Tex>
struct perf_blas_dot_ex<
    Tx,
    Ty,
    Tr,
    Tex,
    std::enable_if_t<(std::is_same<Tx, float>{} && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tr>{}
                      && std::is_same<Tr, Tex>{})
                     || (std::is_same<Tx, double>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tr, Tex>{})
                     || (std::is_same<Tx, hipblasHalf>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tr, Tex>{})
                     || (std::is_same<Tx, hipblasComplex>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tr, Tex>{})
                     || (std::is_same<Tx, hipblasDoubleComplex>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tr, Tex>{})
                     || (std::is_same<Tx, hipblasHalf>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tex, float>{})
                     || (std::is_same<Tx, hipblasBfloat16>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Ty, Tr>{} && std::is_same<Tex, float>{})>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"dot_ex", testing_dot_ex_template<Tx, Ty, Tr, Tex, false>},
            {"dot_batched_ex", testing_dot_batched_ex_template<Tx, Ty, Tr, Tex, false>},
            {"dot_strided_batched_ex",
             testing_dot_strided_batched_ex_template<Tx, Ty, Tr, Tex, false>},
            {"dotc_ex", testing_dot_ex_template<Tx, Ty, Tr, Tex, true>},
            {"dotc_batched_ex", testing_dot_batched_ex_template<Tx, Ty, Tr, Tex, true>},
            {"dotc_strided_batched_ex",
             testing_dot_strided_batched_ex_template<Tx, Ty, Tr, Tex, true>},
        };
        run_function(map, arg);
    }
};

template <typename Tx, typename Tr = Tx, typename Tex = Tr, typename = void>
struct perf_blas_nrm2_ex : hipblas_test_invalid
{
};

template <typename Tx, typename Tr, typename Tex>
struct perf_blas_nrm2_ex<
    Tx,
    Tr,
    Tex,
    std::enable_if_t<
        (std::is_same<Tx, float>{} && std::is_same<Tx, Tr>{} && std::is_same<Tr, Tex>{})
        || (std::is_same<Tx, double>{} && std::is_same<Tx, Tr>{} && std::is_same<Tr, Tex>{})
        || (std::is_same<Tx, hipblasComplex>{} && std::is_same<Tr, float>{}
            && std::is_same<Tr, Tex>{})
        || (std::is_same<Tx, hipblasDoubleComplex>{} && std::is_same<Tr, double>{}
            && std::is_same<Tr, Tex>{})
        || (std::is_same<Tx, hipblasHalf>{} && std::is_same<Tr, Tx>{}
            && std::is_same<Tex, float>{})>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"nrm2_ex", testing_nrm2_ex_template<Tx, Tr, Tex>},
            {"nrm2_batched_ex", testing_nrm2_batched_ex_template<Tx, Tr, Tex>},
            {"nrm2_strided_batched_ex", testing_nrm2_strided_batched_ex_template<Tx, Tr, Tex>},
        };
        run_function(map, arg);
    }
};

template <typename Tx, typename Ty = Tx, typename Tcs = Ty, typename Tex = Tcs, typename = void>
struct perf_blas_rot_ex : hipblas_test_invalid
{
};

template <typename Tx, typename Ty, typename Tcs, typename Tex>
struct perf_blas_rot_ex<
    Tx,
    Ty,
    Tcs,
    Tex,
    std::enable_if_t<(std::is_same<Tx, float>{} && std::is_same<Tx, Ty>{} && std::is_same<Ty, Tcs>{}
                      && std::is_same<Tcs, Tex>{})
                     || (std::is_same<Tx, double>{} && std::is_same<Ty, Tx>{}
                         && std::is_same<Ty, Tcs>{} && std::is_same<Tex, Tcs>{})
                     || (std::is_same<Tx, hipblasComplex>{} && std::is_same<Ty, Tx>{}
                         && std::is_same<Tcs, Ty>{} && std::is_same<Tcs, Tex>{})
                     || (std::is_same<Tx, hipblasDoubleComplex>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Tcs, Ty>{} && std::is_same<Tex, Tcs>{})
                     || (std::is_same<Tx, hipblasComplex>{} && std::is_same<Ty, Tx>{}
                         && std::is_same<Tcs, float>{} && std::is_same<Tex, hipblasComplex>{})
                     || (std::is_same<Tx, hipblasDoubleComplex>{} && std::is_same<Tx, Ty>{}
                         && std::is_same<Tcs, double>{}
                         && std::is_same<Tex, hipblasDoubleComplex>{})
                     || (std::is_same<Tx, hipblasHalf>{} && std::is_same<Ty, Tx>{}
                         && std::is_same<Tcs, Ty>{} && std::is_same<Tex, float>{})
                     || (std::is_same<Tx, hipblasBfloat16>{} && std::is_same<Ty, Tx>{}
                         && std::is_same<Tcs, Ty>{} && std::is_same<Tex, float>{})>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"rot_ex", testing_rot_ex_template<Tex, Tx, Tcs>},
            {"rot_batched_ex", testing_rot_batched_ex_template<Tex, Tx, Tcs>},
            {"rot_strided_batched_ex", testing_rot_strided_batched_ex_template<Tex, Tx, Tcs>},
        };
        run_function(map, arg);
    }
};

template <typename Ti, typename To = Ti, typename Tc = To, typename = void>
struct perf_blas_rot : hipblas_test_invalid
{
};

template <typename Ti, typename To, typename Tc>
struct perf_blas_rot<
    Ti,
    To,
    Tc,
    std::enable_if_t<(std::is_same<Ti, float>{} && std::is_same<Ti, To>{} && std::is_same<To, Tc>{})
                     || (std::is_same<Ti, double>{} && std::is_same<Ti, To>{}
                         && std::is_same<To, Tc>{})
                     || (std::is_same<Ti, hipblasComplex>{} && std::is_same<To, float>{}
                         && std::is_same<Tc, hipblasComplex>{})
                     || (std::is_same<Ti, hipblasComplex>{} && std::is_same<To, float>{}
                         && std::is_same<Tc, float>{})
                     || (std::is_same<Ti, hipblasDoubleComplex>{} && std::is_same<To, double>{}
                         && std::is_same<Tc, hipblasDoubleComplex>{})
                     || (std::is_same<Ti, hipblasDoubleComplex>{} && std::is_same<To, double>{}
                         && std::is_same<Tc, double>{})>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"rot", testing_rot<Ti, To, Tc>},
            {"rot_batched", testing_rot_batched<Ti, To, Tc>},
            {"rot_strided_batched", testing_rot_strided_batched<Ti, To, Tc>},
        };
        run_function(map, arg);
    }
};

template <typename Ta, typename Tb = Ta, typename = void>
struct perf_blas_scal : hipblas_test_invalid
{
};

template <typename Ta, typename Tb>
struct perf_blas_scal<
    Ta,
    Tb,
    std::enable_if_t<(std::is_same<Ta, double>{} && std::is_same<Tb, hipblasDoubleComplex>{})
                     || (std::is_same<Ta, float>{} && std::is_same<Tb, hipblasComplex>{})
                     || (std::is_same<Ta, Tb>{} && std::is_same<Ta, float>{})
                     || (std::is_same<Ta, Tb>{} && std::is_same<Ta, double>{})
                     || (std::is_same<Ta, Tb>{} && std::is_same<Ta, hipblasComplex>{})
                     || (std::is_same<Ta, Tb>{} && std::is_same<Ta, hipblasDoubleComplex>{})>>
    : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"scal", testing_scal<Ta, Tb>},
            {"scal_batched", testing_scal_batched<Ta, Tb>},
            {"scal_strided_batched", testing_scal_strided_batched<Ta, Tb>},
        };
        run_function(map, arg);
    }
};

template <typename Ta, typename Tx = Ta, typename Tex = Tx, typename = void>
struct perf_blas_scal_ex : hipblas_test_invalid
{
};

template <typename Ta, typename Tx, typename Tex>
struct perf_blas_scal_ex<
    Ta,
    Tx,
    Tex,
    std::enable_if_t<
        (std::is_same<Ta, float>{} && std::is_same<Ta, Tx>{} && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, double>{} && std::is_same<Ta, Tx>{} && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, hipblasHalf>{} && std::is_same<Ta, Tx>{} && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, hipblasComplex>{} && std::is_same<Ta, Tx>{} && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, hipblasDoubleComplex>{} && std::is_same<Ta, Tx>{}
            && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, hipblasHalf>{} && std::is_same<Ta, Tx>{} && std::is_same<Tex, float>{})
        || (std::is_same<Ta, float>{} && std::is_same<Tx, hipblasHalf>{} && std::is_same<Ta, Tex>{})
        || (std::is_same<Ta, float>{} && std::is_same<Tx, hipblasComplex>{}
            && std::is_same<Tx, Tex>{})
        || (std::is_same<Ta, double>{} && std::is_same<Tx, hipblasDoubleComplex>{}
            && std::is_same<Tx, Tex>{})>> : hipblas_test_valid
{
    void operator()(const Arguments& arg)
    {
        static const func_map map = {
            {"scal_ex", testing_scal_ex_template<Ta, Tx, Tex>},
            {"scal_batched_ex", testing_scal_batched_ex_template<Ta, Tx, Tex>},
            {"scal_strided_batched_ex", testing_scal_strided_batched_ex_template<Ta, Tx, Tex>},
        };
        run_function(map, arg);
    }
};

int run_bench_test(Arguments& arg)
{
    //hipblas_initialize(); // Initialize rocBLAS

    std::cout << std::setiosflags(std::ios::fixed)
              << std::setprecision(7); // Set precision to 7 digits

    // disable unit_check in client benchmark, it is only used in gtest unit test
    arg.unit_check = 0;

    // enable timing check,otherwise no performance data collected
    arg.timing = 1;

    // Skip past any testing_ prefix in function
    static constexpr char prefix[] = "testing_";
    const char*           function = arg.function;
    if(!strncmp(function, prefix, sizeof(prefix) - 1))
        function += sizeof(prefix) - 1;

    if(!strcmp(function, "gemm") || !strcmp(function, "gemm_batched"))
    {
        // adjust dimension for GEMM routines
        hipblas_int min_lda = arg.transA_option == 'N' ? arg.M : arg.K;
        hipblas_int min_ldb = arg.transB_option == 'N' ? arg.K : arg.N;
        hipblas_int min_ldc = arg.M;

        if(arg.lda < min_lda)
        {
            std::cout << "hipblas-bench INFO: lda < min_lda, set lda = " << min_lda << std::endl;
            arg.lda = min_lda;
        }
        if(arg.ldb < min_ldb)
        {
            std::cout << "hipblas-bench INFO: ldb < min_ldb, set ldb = " << min_ldb << std::endl;
            arg.ldb = min_ldb;
        }
        if(arg.ldc < min_ldc)
        {
            std::cout << "hipblas-bench INFO: ldc < min_ldc, set ldc = " << min_ldc << std::endl;
            arg.ldc = min_ldc;
        }
    }
    else if(!strcmp(function, "gemm_strided_batched"))
    {
        // adjust dimension for GEMM routines
        hipblas_int min_lda = arg.transA_option == 'N' ? arg.M : arg.K;
        hipblas_int min_ldb = arg.transB_option == 'N' ? arg.K : arg.N;
        hipblas_int min_ldc = arg.M;
        if(arg.lda < min_lda)
        {
            std::cout << "hipblas-bench INFO: lda < min_lda, set lda = " << min_lda << std::endl;
            arg.lda = min_lda;
        }
        if(arg.ldb < min_ldb)
        {
            std::cout << "hipblas-bench INFO: ldb < min_ldb, set ldb = " << min_ldb << std::endl;
            arg.ldb = min_ldb;
        }
        if(arg.ldc < min_ldc)
        {
            std::cout << "hipblas-bench INFO: ldc < min_ldc, set ldc = " << min_ldc << std::endl;
            arg.ldc = min_ldc;
        }

        //      hipblas_int min_stride_a =
        //          arg.transA_option == 'N' ? arg.K * arg.lda : arg.M * arg.lda;
        //      hipblas_int min_stride_b =
        //          arg.transB_option == 'N' ? arg.N * arg.ldb : arg.K * arg.ldb;
        //      hipblas_int min_stride_a =
        //          arg.transA_option == 'N' ? arg.K * arg.lda : arg.M * arg.lda;
        //      hipblas_int min_stride_b =
        //          arg.transB_option == 'N' ? arg.N * arg.ldb : arg.K * arg.ldb;
        hipblas_int min_stride_c = arg.ldc * arg.N;
        //      if (arg.stride_a < min_stride_a)
        //      {
        //          std::cout << "hipblas-bench INFO: stride_a < min_stride_a, set stride_a = " <<
        //          min_stride_a << std::endl;
        //          arg.stride_a = min_stride_a;
        //      }
        //      if (arg.stride_b < min_stride_b)
        //      {
        //          std::cout << "hipblas-bench INFO: stride_b < min_stride_b, set stride_b = " <<
        //          min_stride_b << std::endl;
        //          arg.stride_b = min_stride_b;
        //      }
        if(arg.stride_c < min_stride_c)
        {
            std::cout << "hipblas-bench INFO: stride_c < min_stride_c, set stride_c = "
                      << min_stride_c << std::endl;
            arg.stride_c = min_stride_c;
        }
    }

    if(!strcmp(function, "gemm_ex") || !strcmp(function, "gemm_batched_ex"))
    {
        // adjust dimension for GEMM routines
        hipblas_int min_lda = arg.transA_option == 'N' ? arg.M : arg.K;
        hipblas_int min_ldb = arg.transB_option == 'N' ? arg.K : arg.N;
        hipblas_int min_ldc = arg.M;
        hipblas_int min_ldd = arg.M;

        if(arg.lda < min_lda)
        {
            std::cout << "hipblas-bench INFO: lda < min_lda, set lda = " << min_lda << std::endl;
            arg.lda = min_lda;
        }
        if(arg.ldb < min_ldb)
        {
            std::cout << "hipblas-bench INFO: ldb < min_ldb, set ldb = " << min_ldb << std::endl;
            arg.ldb = min_ldb;
        }
        if(arg.ldc < min_ldc)
        {
            std::cout << "hipblas-bench INFO: ldc < min_ldc, set ldc = " << min_ldc << std::endl;
            arg.ldc = min_ldc;
        }
        if(arg.ldd < min_ldd)
        {
            std::cout << "hipblas-bench INFO: ldd < min_ldd, set ldd = " << min_ldc << std::endl;
            arg.ldd = min_ldd;
        }
        hipblas_gemm_dispatch<perf_gemm_ex>(arg);
    }
    else if(!strcmp(function, "gemm_strided_batched_ex"))
    {
        // adjust dimension for GEMM routines
        hipblas_int min_lda = arg.transA_option == 'N' ? arg.M : arg.K;
        hipblas_int min_ldb = arg.transB_option == 'N' ? arg.K : arg.N;
        hipblas_int min_ldc = arg.M;
        hipblas_int min_ldd = arg.M;
        if(arg.lda < min_lda)
        {
            std::cout << "hipblas-bench INFO: lda < min_lda, set lda = " << min_lda << std::endl;
            arg.lda = min_lda;
        }
        if(arg.ldb < min_ldb)
        {
            std::cout << "hipblas-bench INFO: ldb < min_ldb, set ldb = " << min_ldb << std::endl;
            arg.ldb = min_ldb;
        }
        if(arg.ldc < min_ldc)
        {
            std::cout << "hipblas-bench INFO: ldc < min_ldc, set ldc = " << min_ldc << std::endl;
            arg.ldc = min_ldc;
        }
        if(arg.ldd < min_ldd)
        {
            std::cout << "hipblas-bench INFO: ldd < min_ldd, set ldd = " << min_ldc << std::endl;
            arg.ldd = min_ldd;
        }
        hipblas_int min_stride_c = arg.ldc * arg.N;
        if(arg.stride_c < min_stride_c)
        {
            std::cout << "hipblas-bench INFO: stride_c < min_stride_c, set stride_c = "
                      << min_stride_c << std::endl;
            arg.stride_c = min_stride_c;
        }

        hipblas_gemm_dispatch<perf_gemm_strided_batched_ex>(arg);
    }
    else
    {
        if(!strcmp(function, "scal_ex") || !strcmp(function, "scal_batched_ex")
           || !strcmp(function, "scal_strided_batched_ex"))
            hipblas_blas1_ex_dispatch<perf_blas_scal_ex>(arg);
        /*
        if(!strcmp(function, "scal") || !strcmp(function, "scal_batched")
           || !strcmp(function, "scal_strided_batched"))
            hipblas_blas1_dispatch<perf_blas_scal>(arg);
        */
        else if(!strcmp(function, "rot") || !strcmp(function, "rot_batched")
                || !strcmp(function, "rot_strided_batched"))
            hipblas_rot_dispatch<perf_blas_rot>(arg);
        else if(!strcmp(function, "axpy_ex") || !strcmp(function, "axpy_batched_ex")
                || !strcmp(function, "axpy_strided_batched_ex"))
            hipblas_blas1_ex_dispatch<perf_blas_axpy_ex>(arg);
        else if(!strcmp(function, "dot_ex") || !strcmp(function, "dot_batched_ex")
                || !strcmp(function, "dot_strided_batched_ex") || !strcmp(function, "dotc_ex")
                || !strcmp(function, "dotc_batched_ex")
                || !strcmp(function, "dotc_strided_batched_ex"))
            hipblas_blas1_ex_dispatch<perf_blas_dot_ex>(arg);
        else if(!strcmp(function, "nrm2_ex") || !strcmp(function, "nrm2_batched_ex")
                || !strcmp(function, "nrm2_strided_batched_ex"))
            hipblas_blas1_ex_dispatch<perf_blas_nrm2_ex>(arg);
        else if(!strcmp(function, "rot_ex") || !strcmp(function, "rot_batched_ex")
                || !strcmp(function, "rot_strided_batched_ex"))
            hipblas_blas1_ex_dispatch<perf_blas_rot_ex>(arg);
        else
            hipblas_simple_dispatch<perf_blas>(arg);
    }
    return 0;
}

int hipblas_bench_datafile()
{
    int ret = 0;
    // for(Arguments arg : RocBLAS_TestData())
    //     ret |= run_bench_test(arg);
    //test_cleanup::cleanup();
    return ret;
}

// Replace --batch with --batch_count for backward compatibility
void fix_batch(int argc, char* argv[])
{
    static char b_c[] = "--batch_count";
    for(int i = 1; i < argc; ++i)
        if(!strcmp(argv[i], "--batch"))
        {
            static int once = (std::cerr << argv[0]
                                         << " warning: --batch is deprecated, and --batch_count "
                                            "should be used instead."
                                         << std::endl,
                               0);
            argv[i]         = b_c;
        }
}

int main(int argc, char* argv[])
try
{
    fix_batch(argc, argv);
    Arguments   arg;
    std::string function;
    std::string precision;
    std::string a_type;
    std::string b_type;
    std::string c_type;
    std::string d_type;
    std::string compute_type;
    std::string initialization;
    hipblas_int device_id;

    // TODO: currently hipblas_parse_data not implemented
    bool datafile            = hipblas_parse_data(argc, argv);
    bool atomics_not_allowed = false;

    options_description desc("hipblas-bench command line options");

    // clang-format off
    desc.add_options()

        ("sizem,m",
         value<hipblas_int>(&arg.M)->default_value(128),
         "Specific matrix size: sizem is only applicable to BLAS-2 & BLAS-3: the number of "
         "rows or columns in matrix.")

        ("sizen,n",
         value<hipblas_int>(&arg.N)->default_value(128),
         "Specific matrix/vector size: BLAS-1: the length of the vector. BLAS-2 & "
         "BLAS-3: the number of rows or columns in matrix")

        ("sizek,k",
         value<hipblas_int>(&arg.K)->default_value(128),
         "Specific matrix size: BLAS-2: the number of sub or super-diagonals of A. BLAS-3: "
         "the number of columns in A and rows in B.")

        ("kl",
         value<hipblas_int>(&arg.KL)->default_value(128),
         "Specific matrix size: kl is only applicable to BLAS-2: The number of sub-diagonals "
         "of the banded matrix A.")

        ("ku",
         value<hipblas_int>(&arg.KU)->default_value(128),
         "Specific matrix size: ku is only applicable to BLAS-2: The number of super-diagonals "
         "of the banded matrix A.")

        ("lda",
         value<hipblas_int>(&arg.lda)->default_value(128),
         "Leading dimension of matrix A, is only applicable to BLAS-2 & BLAS-3.")

        ("ldb",
         value<hipblas_int>(&arg.ldb)->default_value(128),
         "Leading dimension of matrix B, is only applicable to BLAS-2 & BLAS-3.")

        ("ldc",
         value<hipblas_int>(&arg.ldc)->default_value(128),
         "Leading dimension of matrix C, is only applicable to BLAS-2 & BLAS-3.")

        ("ldd",
         value<hipblas_int>(&arg.ldd)->default_value(128),
         "Leading dimension of matrix D, is only applicable to BLAS-EX ")

        ("stride_a",
         value<hipblasStride>(&arg.stride_a)->default_value(128*128),
         "Specific stride of strided_batched matrix A, is only applicable to strided batched"
         "BLAS-2 and BLAS-3: second dimension * leading dimension.")

        ("stride_b",
         value<hipblasStride>(&arg.stride_b)->default_value(128*128),
         "Specific stride of strided_batched matrix B, is only applicable to strided batched"
         "BLAS-2 and BLAS-3: second dimension * leading dimension.")

        ("stride_c",
         value<hipblasStride>(&arg.stride_c)->default_value(128*128),
         "Specific stride of strided_batched matrix C, is only applicable to strided batched"
         "BLAS-2 and BLAS-3: second dimension * leading dimension.")

        ("stride_d",
         value<hipblasStride>(&arg.stride_d)->default_value(128*128),
         "Specific stride of strided_batched matrix D, is only applicable to strided batched"
         "BLAS_EX: second dimension * leading dimension.")

        ("stride_x",
         value<hipblasStride>(&arg.stride_x)->default_value(128),
         "Specific stride of strided_batched vector x, is only applicable to strided batched"
         "BLAS_2: second dimension.")

        ("stride_y",
         value<hipblasStride>(&arg.stride_y)->default_value(128),
         "Specific stride of strided_batched vector y, is only applicable to strided batched"
         "BLAS_2: leading dimension.")

        ("incx",
         value<hipblas_int>(&arg.incx)->default_value(1),
         "increment between values in x vector")

        ("incy",
         value<hipblas_int>(&arg.incy)->default_value(1),
         "increment between values in y vector")

        ("alpha",
          value<double>(&arg.alpha)->default_value(1.0), "specifies the scalar alpha")

        ("alphai",
         value<double>(&arg.alphai)->default_value(0.0), "specifies the imaginary part of the scalar alpha")

        ("beta",
         value<double>(&arg.beta)->default_value(0.0), "specifies the scalar beta")

        ("betai",
         value<double>(&arg.betai)->default_value(0.0), "specifies the imaginary part of the scalar beta")

        ("function,f",
         value<std::string>(&function),
         "BLAS function to test.")

        ("precision,r",
         value<std::string>(&precision)->default_value("f32_r"), "Precision. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("a_type",
         value<std::string>(&a_type), "Precision of matrix A. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("b_type",
         value<std::string>(&b_type), "Precision of matrix B. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("c_type",
         value<std::string>(&c_type), "Precision of matrix C. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("d_type",
         value<std::string>(&d_type), "Precision of matrix D. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("compute_type",
         value<std::string>(&compute_type), "Precision of computation. "
         "Options: h,s,d,c,z,f16_r,f32_r,f64_r,bf16_r,f32_c,f64_c,i8_r,i32_r")

        ("initialization",
         value<std::string>(&initialization)->default_value("rand_int"),
         "Intialize with random integers, trig functions sin and cos, or hpl-like input. "
         "Options: rand_int, trig_float, hpl")

        ("transposeA",
         value<char>(&arg.transA_option)->default_value('N'),
         "N = no transpose, T = transpose, C = conjugate transpose")

        ("transposeB",
         value<char>(&arg.transB_option)->default_value('N'),
         "N = no transpose, T = transpose, C = conjugate transpose")

        ("side",
         value<char>(&arg.side_option)->default_value('L'),
         "L = left, R = right. Only applicable to certain routines")

        ("uplo",
         value<char>(&arg.uplo_option)->default_value('U'),
         "U = upper, L = lower. Only applicable to certain routines") // xsymv xsyrk xsyr2k xtrsm xtrsm_ex
                                                                     // xtrmm xtrsv
        ("diag",
         value<char>(&arg.diag_option)->default_value('N'),
         "U = unit diagonal, N = non unit diagonal. Only applicable to certain routines") // xtrsm xtrsm_ex xtrsv xtrmm

        ("batch_count",
         value<hipblas_int>(&arg.batch_count)->default_value(1),
         "Number of matrices. Only applicable to batched and strided_batched routines")

        ("verify,v",
         value<hipblas_int>(&arg.norm_check)->default_value(0),
         "Validate GPU results with CPU? 0 = No, 1 = Yes (default: No)")

        ("iters,i",
         value<hipblas_int>(&arg.iters)->default_value(10),
         "Iterations to run inside timing loop")

        ("cold_iters,j",
         value<hipblas_int>(&arg.cold_iters)->default_value(2),
         "Cold Iterations to run before entering the timing loop")

        ("algo",
         value<uint32_t>(&arg.algo)->default_value(0),
         "extended precision gemm algorithm")

        ("solution_index",
         value<int32_t>(&arg.solution_index)->default_value(0),
         "extended precision gemm solution index")

        ("flags",
         value<uint32_t>(&arg.flags)->default_value(0),
         "gemm_ex flags")

        ("atomics_not_allowed",
         bool_switch(&atomics_not_allowed)->default_value(false),
         "Atomic operations with non-determinism in results are not allowed")

        ("device",
         value<hipblas_int>(&device_id)->default_value(0),
         "Set default device to be used for subsequent program runs")

        // ("c_noalias_d",
        //  bool_switch(&arg.c_noalias_d)->default_value(false),
        //  "C and D are stored in separate memory")

        ("fortran",
         bool_switch(&arg.fortran)->default_value(false),
         "Run using Fortran interface")

        ("help,h", "produces this help message");

        //("version", "Prints the version number");

    // clang-format on

    arg.atomics_mode = atomics_not_allowed ? HIPBLAS_ATOMICS_NOT_ALLOWED : HIPBLAS_ATOMICS_ALLOWED;

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);
    notify(vm);

    if((argc <= 1 && !datafile) || vm.count("help"))
    {
        std::cout << desc << std::endl;
        return 0;
    }

    // if(vm.find("version") != vm.end())
    // {
    //     char blas_version[100];
    //     hipblas_get_version_string(blas_version, sizeof(blas_version));
    //     std::cout << "hipBLAS version: " << blas_version << std::endl;
    //     return 0;
    // }

    // Device Query
    hipblas_int device_count = query_device_property();

    std::cout << std::endl;
    if(device_count <= device_id)
        throw std::invalid_argument("Invalid Device ID");
    set_device(device_id);

    if(datafile)
        return hipblas_bench_datafile();

    std::transform(precision.begin(), precision.end(), precision.begin(), ::tolower);
    auto prec = string2hipblas_datatype(precision);
    if(prec == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --precision " + precision);

    arg.a_type = a_type == "" ? prec : string2hipblas_datatype(a_type);
    if(arg.a_type == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --a_type " + a_type);

    arg.b_type = b_type == "" ? prec : string2hipblas_datatype(b_type);
    if(arg.b_type == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --b_type " + b_type);

    arg.c_type = c_type == "" ? prec : string2hipblas_datatype(c_type);
    if(arg.c_type == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --c_type " + c_type);

    arg.d_type = d_type == "" ? prec : string2hipblas_datatype(d_type);
    if(arg.d_type == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --d_type " + d_type);

    arg.compute_type = compute_type == "" ? prec : string2hipblas_datatype(compute_type);
    if(arg.compute_type == static_cast<hipblasDatatype_t>(-1))
        throw std::invalid_argument("Invalid value for --compute_type " + compute_type);

    arg.initialization = string2hipblas_initialization(initialization);
    if(arg.initialization == static_cast<hipblas_initialization>(-1))
        throw std::invalid_argument("Invalid value for --initialization " + initialization);

    if(arg.M < 0)
        throw std::invalid_argument("Invalid value for -m " + std::to_string(arg.M));
    if(arg.N < 0)
        throw std::invalid_argument("Invalid value for -n " + std::to_string(arg.N));
    if(arg.K < 0)
        throw std::invalid_argument("Invalid value for -k " + std::to_string(arg.K));

    int copied = snprintf(arg.function, sizeof(arg.function), "%s", function.c_str());
    if(copied <= 0 || copied >= sizeof(arg.function))
        throw std::invalid_argument("Invalid value for --function");

    return run_bench_test(arg);
}
catch(const std::invalid_argument& exp)
{
    std::cerr << exp.what() << std::endl;
    return -1;
}
