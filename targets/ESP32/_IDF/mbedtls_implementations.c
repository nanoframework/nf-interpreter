// //
// // Copyright (c) .NET Foundation and Contributors
// // See LICENSE file in the project root for full license information.
// //

// #include <mbedtls/error.h>
// #include <mbedtls/bn_mul.h>
// #include <mbedtls/platform_util.h>
// #include <mbedtls/bignum.h>

// // the following code is copied from IDF implementation

// #define MPI_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_MPI_BAD_INPUT_DATA)

// #define ciL (sizeof(mbedtls_mpi_uint)) /* chars in limb  */
// #define biL (ciL << 3)                 /* bits  in limb  */
// #define biH (ciL << 2)                 /* half limb size */

// static void mpi_mul_hlp(size_t i, const mbedtls_mpi_uint *s, mbedtls_mpi_uint *d, mbedtls_mpi_uint b)
// {
//     mbedtls_mpi_uint c = 0, t = 0;

// #if defined(MULADDC_HUIT)
//     for (; i >= 8; i -= 8)
//     {
//         MULADDC_INIT
//         MULADDC_HUIT
//         MULADDC_STOP
//     }

//     for (; i > 0; i--)
//     {
//         MULADDC_INIT
//         MULADDC_CORE
//         MULADDC_STOP
//     }
// #else  /* MULADDC_HUIT */
//     for (; i >= 16; i -= 16)
//     {
//         MULADDC_INIT
//         MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE

//             MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE
//                 MULADDC_STOP
//     }

//     for (; i >= 8; i -= 8)
//     {
//         MULADDC_INIT
//         MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE

//             MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_CORE MULADDC_STOP
//     }

//     for (; i > 0; i--)
//     {
//         MULADDC_INIT
//         MULADDC_CORE
//         MULADDC_STOP
//     }
// #endif /* MULADDC_HUIT */

//     t++;

//     while (c != 0)
//     {
//         *d += c;
//         c = (*d < c);
//         d++;
//     }
// }

// /*
//  * Baseline multiplication: X = A * b
//  */
// int mbedtls_mpi_mul_int(mbedtls_mpi *X, const mbedtls_mpi *A, mbedtls_mpi_uint b)
// {
//     MPI_VALIDATE_RET(X != NULL);
//     MPI_VALIDATE_RET(A != NULL);

//     /* mpi_mul_hlp can't deal with a leading 0. */
//     size_t n = A->n;
//     while (n > 0 && A->p[n - 1] == 0)
//         --n;

//     /* The general method below doesn't work if n==0 or b==0. By chance
//      * calculating the result is trivial in those cases. */
//     if (b == 0 || n == 0)
//     {
//         return (mbedtls_mpi_lset(X, 0));
//     }

//     /* Calculate A*b as A + A*(b-1) to take advantage of mpi_mul_hlp */
//     int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
//     /* In general, A * b requires 1 limb more than b. If
//      * A->p[n - 1] * b / b == A->p[n - 1], then A * b fits in the same
//      * number of limbs as A and the call to grow() is not required since
//      * copy() will take care of the growth if needed. However, experimentally,
//      * making the call to grow() unconditional causes slightly fewer
//      * calls to calloc() in ECP code, presumably because it reuses the
//      * same mpi for a while and this way the mpi is more likely to directly
//      * grow to its final size. */
//     MBEDTLS_MPI_CHK(mbedtls_mpi_grow(X, n + 1));
//     MBEDTLS_MPI_CHK(mbedtls_mpi_copy(X, A));
//     mpi_mul_hlp(n, A->p, X->p, b - 1);

// cleanup:
//     return (ret);
// }
