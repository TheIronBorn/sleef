//          Copyright Naoki Shibata 2010 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#if CONFIG == 130 || CONFIG == 140 || CONFIG == 131 || CONFIG == 141

#if !defined(__VX__)
#error This helper is for IBM s390x.
#endif

#if (CONFIG == 140 || CONFIG == 141) && __ARCH__ < 12
#error Please specify -march=z14.
#endif

#else
#error CONFIG macro invalid or not defined
#endif

#define ENABLE_DP
#define LOG2VECTLENDP 1
#define VECTLENDP (1 << LOG2VECTLENDP)

#define ENABLE_SP
#define LOG2VECTLENSP (LOG2VECTLENDP+1)
#define VECTLENSP (1 << LOG2VECTLENSP)

#if CONFIG == 130 || CONFIG == 140
#define ENABLE_FMA_DP
#define ENABLE_FMA_SP
#endif

#define ACCURATE_SQRT
#define FULL_FP_ROUNDING

#include <vecintrin.h>

#include <stdint.h>
#include <math.h>
#include "misc.h"

#define VECTOR __attribute__((vector_size(16)))

typedef VECTOR unsigned long long vmask;
typedef VECTOR unsigned long long vopmask;

typedef VECTOR double vdouble;
typedef VECTOR int vint;

typedef VECTOR float vfloat;
typedef VECTOR int vint2;

//

static INLINE int vavailability_i(int n) {
#if CONFIG == 140
  if (n == 1 || n == 2) {
    return vec_max((vdouble) {n, n}, (vdouble) {n, n})[0];
  }
#endif
  return 0;
}

#if CONFIG == 140 || CONFIG == 141
#define ISANAME "z14"
#define DFTPRIORITY 14
#else
#define ISANAME "z13"
#define DFTPRIORITY 13
#endif

static INLINE void vprefetch_v_p(const void *ptr) { }

static vint2 vloadu_vi2_p(int32_t *p) { return (vint2) { p[0], p[1], p[2], p[3] }; }
static void vstoreu_v_p_vi2(int32_t *p, vint2 v) { p[0] = v[0]; p[1] = v[1]; p[2] = v[2]; p[3] = v[3]; }
static vint vloadu_vi_p(int32_t *p) { return (vint) { p[0], p[1] }; }
static void vstoreu_v_p_vi(int32_t *p, vint v) { p[0] = v[0]; p[1] = v[1]; }

static INLINE vdouble vload_vd_p(const double *p) { return (vdouble) { p[0], p[1] }; }
static INLINE void vstore_v_p_vd(double *p, vdouble v) { p[0] = v[0]; p[1] = v[1]; }
static INLINE vdouble vloadu_vd_p(const double *p) { return (vdouble) { p[0], p[1] }; }
static INLINE void vstoreu_v_p_vd(double *p, vdouble v) { p[0] = v[0]; p[1] = v[1]; }

static INLINE vfloat vload_vf_p(const float *p) { return (vfloat) { p[0], p[1], p[2], p[3] }; }
static INLINE void vstore_v_p_vf(float *p, vfloat v) { p[0] = v[0]; p[1] = v[1]; p[2] = v[2]; p[3] = v[3]; }
static INLINE void vscatter2_v_p_i_i_vf(float *p, int offset, int step, vfloat v) {
  *(p+(offset + step * 0)*2 + 0) = v[0];
  *(p+(offset + step * 0)*2 + 1) = v[1];
  *(p+(offset + step * 1)*2 + 0) = v[2];
  *(p+(offset + step * 1)*2 + 1) = v[3];
}

static INLINE vfloat vloadu_vf_p(const float *p) { return (vfloat) { p[0], p[1], p[2], p[3] }; }
static INLINE void vstoreu_v_p_vf(float *p, vfloat v) { p[0] = v[0]; p[1] = v[1]; p[2] = v[2]; p[3] = v[3]; }

static INLINE void vscatter2_v_p_i_i_vd(double *p, int offset, int step, vdouble v) { vstore_v_p_vd((double *)(&p[2*offset]), v); }

static INLINE vdouble vgather_vd_p_vi(const double *p, vint vi) {
  int a[VECTLENDP];
  return ((vdouble) { p[vi[0]], p[vi[1]] });
}

static INLINE vfloat vgather_vf_p_vi2(const float *p, vint2 vi2) {
  int a[VECTLENSP];
  return ((vfloat) { p[vi2[0]], p[vi2[1]], p[vi2[2]], p[vi2[3]] });
}

static INLINE vint vcast_vi_i(int i) { return (vint) { i, i }; }
static INLINE vint2 vcast_vi2_i(int i) { return (vint2) { i, i, i, i }; }
static INLINE vfloat vcast_vf_f(float f) { return (vfloat) { f, f, f, f }; }
static INLINE vdouble vcast_vd_d(double d) { return (vdouble) { d, d }; }

static INLINE vdouble vcast_vd_vi(vint vi) { return (vdouble) { vi[0], vi[1] }; }
static INLINE vfloat vcast_vf_vi2(vint2 vi) { return (vfloat) { vi[0], vi[1], vi[2], vi[3] }; }
static INLINE vdouble vtruncate_vd_vd(vdouble vd) { return __builtin_s390_vfidb(vd, 4, 5); }
static INLINE vdouble vrint_vd_vd(vdouble vd) { return __builtin_s390_vfidb(vd, 4, 4); }

static INLINE vint vrint_vi_vd(vdouble vd) {
  vd = vrint_vd_vd(vd);
  return (vint) { vd[0], vd[1] };
}
static INLINE vint vtruncate_vi_vd(vdouble vd) { return (vint) { vd[0], vd[1] }; }
static INLINE vint2 vtruncate_vi2_vf(vfloat vf) { return (vint) { vf[0], vf[1], vf[2], vf[3] }; }

static INLINE vmask vreinterpret_vm_vd(vdouble vd) { return (vmask)vd; }
static INLINE vdouble vreinterpret_vd_vm(vmask vm) { return (vdouble)vm; }
static INLINE vint2 vreinterpret_vi2_vd(vdouble vd) { return (vint2)vd; }
static INLINE vdouble vreinterpret_vd_vi2(vint2 vi) { return (vdouble)vi; }

static INLINE vmask vreinterpret_vm_vf(vfloat vf) { return (vmask)vf; }
static INLINE vfloat vreinterpret_vf_vm(vmask vm) { return (vfloat)vm; }
static INLINE vfloat vreinterpret_vf_vi2(vint2 vi) { return (vfloat)vi; }
static INLINE vint2 vreinterpret_vi2_vf(vfloat vf) { return (vint2)vf; }

static INLINE vdouble vadd_vd_vd_vd(vdouble x, vdouble y) { return x + y; }
static INLINE vdouble vsub_vd_vd_vd(vdouble x, vdouble y) { return x - y; }
static INLINE vdouble vmul_vd_vd_vd(vdouble x, vdouble y) { return x * y; }
static INLINE vdouble vdiv_vd_vd_vd(vdouble x, vdouble y) { return x / y; }
static INLINE vdouble vrec_vd_vd(vdouble x) { return 1 / x; }
static INLINE vdouble vneg_vd_vd(vdouble d) { return -d; }

static INLINE vfloat vadd_vf_vf_vf(vfloat x, vfloat y) { return x + y; }
static INLINE vfloat vsub_vf_vf_vf(vfloat x, vfloat y) { return x - y; }
static INLINE vfloat vmul_vf_vf_vf(vfloat x, vfloat y) { return x * y; }
static INLINE vfloat vdiv_vf_vf_vf(vfloat x, vfloat y) { return x / y; }
static INLINE vfloat vrec_vf_vf(vfloat x) { return 1 / x; }
static INLINE vfloat vneg_vf_vf(vfloat d) { return -d; }

static INLINE vmask vand_vm_vm_vm(vmask x, vmask y) { return x & y; }
static INLINE vmask vandnot_vm_vm_vm(vmask x, vmask y) { return y & ~x; }
static INLINE vmask vor_vm_vm_vm(vmask x, vmask y) { return x | y; }
static INLINE vmask vxor_vm_vm_vm(vmask x, vmask y) { return x ^ y; }

static INLINE vopmask vand_vo_vo_vo(vopmask x, vopmask y) { return x & y; }
static INLINE vopmask vandnot_vo_vo_vo(vopmask x, vopmask y) { return y & ~x; }
static INLINE vopmask vor_vo_vo_vo(vopmask x, vopmask y) { return x | y; }
static INLINE vopmask vxor_vo_vo_vo(vopmask x, vopmask y) { return x ^ y; }

static INLINE vmask vand_vm_vo64_vm(vopmask x, vmask y) { return x & y; }
static INLINE vmask vandnot_vm_vo64_vm(vopmask x, vmask y) { return y & ~x; }
static INLINE vmask vor_vm_vo64_vm(vopmask x, vmask y) { return x | y; }
static INLINE vmask vxor_vm_vo64_vm(vopmask x, vmask y) { return x ^ y; }

static INLINE vmask vand_vm_vo32_vm(vopmask x, vmask y) { return x & y; }
static INLINE vmask vandnot_vm_vo32_vm(vopmask x, vmask y) { return y & ~x; }
static INLINE vmask vor_vm_vo32_vm(vopmask x, vmask y) { return x | y; }
static INLINE vmask vxor_vm_vo32_vm(vopmask x, vmask y) { return x ^ y; }

static INLINE vdouble vsel_vd_vo_vd_vd(vopmask o, vdouble x, vdouble y) { return vec_sel(y, x, o); }
#if CONFIG == 140 || CONFIG == 141
static INLINE vfloat vsel_vf_vo_vf_vf(vopmask o, vfloat x, vfloat y) { return vec_sel(y, x, (VECTOR unsigned int)o); }
static INLINE vint2 vsel_vi2_vo_vi2_vi2(vopmask o, vint2 x, vint2 y) { return vec_sel(y, x, (VECTOR unsigned int)o); }
#else
static INLINE vfloat vsel_vf_vo_vf_vf(vopmask o, vfloat x, vfloat y) { return (vfloat)(((vopmask)x & o) | ((vopmask)y & ~o)); }
static INLINE vint2 vsel_vi2_vo_vi2_vi2(vopmask o, vint2 x, vint2 y) { return (vint2)(((vopmask)x & o) | ((vopmask)y & ~o)); }
#endif

static INLINE int vtestallones_i_vo64(vopmask g) { return g[0] == 0xffffffffffffffffLL && g[1] == 0xffffffffffffffffLL; }
static INLINE int vtestallones_i_vo32(vopmask g) { return g[0] == 0xffffffffffffffffLL && g[1] == 0xffffffffffffffffLL; }

static INLINE vopmask vcast_vo32_vo64(vopmask g) { return (vopmask)(vint) { g[0] != 0 ? -1 : 0, g[1] != 0 ? -1 : 0, 0, 0 }; }
static INLINE vopmask vcast_vo64_vo32(vopmask g) { return (vopmask) { ((vint)g)[0] != 0 ? 0xffffffffffffffffLL : 0, ((vint)g)[1] != 0 ? 0xffffffffffffffffLL : 0 }; }

static INLINE vmask vcast_vm_i_i(int h, int l) { return (vmask)(vint){ h, l, h, l }; }
static INLINE vint2 vcastu_vi2_vi(vint vi) { return (vint2){ vi[0], 0, vi[1], 0 }; }
static INLINE vint vcastu_vi_vi2(vint2 vi2) { return (vint){ vi2[0], vi2[2] }; }

static INLINE vint vreinterpretFirstHalf_vi_vi2(vint2 vi2) { return (vint){ vi2[0], vi2[1] }; }
static INLINE vint2 vreinterpretFirstHalf_vi2_vi(vint vi) { return (vint2){ vi[0], vi[1], 0, 0 }; }

static INLINE vdouble vrev21_vd_vd(vdouble vd) { return (vdouble) { vd[1], vd[0] }; }
static INLINE vdouble vreva2_vd_vd(vdouble vd) { return vd; }
static INLINE vfloat vrev21_vf_vf(vfloat vd) { return (vfloat) { vd[1], vd[0], vd[3], vd[2] }; }
static INLINE vfloat vreva2_vf_vf(vfloat vd) { return (vfloat) { vd[2], vd[3], vd[0], vd[1] }; }
static INLINE vint2 vrev21_vi2_vi2(vint2 i) { return vreinterpret_vi2_vf(vrev21_vf_vf(vreinterpret_vf_vi2(i))); }

static INLINE vopmask veq64_vo_vm_vm(vmask x, vmask y) {
  return (vopmask) { x[0] == y[0] ? 0xffffffffffffffffLL : 0, x[1] == y[1] ? 0xffffffffffffffffLL : 0 };
}

static INLINE vmask vadd64_vm_vm_vm(vmask x, vmask y) {
  return (vmask)((VECTOR long long)x +  (VECTOR long long)y);
}

//

#define PNMASK ((vdouble) { +0.0, -0.0 })
#define NPMASK ((vdouble) { -0.0, +0.0 })
#define PNMASKf ((vfloat) { +0.0f, -0.0f, +0.0f, -0.0f })
#define NPMASKf ((vfloat) { -0.0f, +0.0f, -0.0f, +0.0f })

static INLINE vdouble vposneg_vd_vd(vdouble d) { return vreinterpret_vd_vm(vxor_vm_vm_vm(vreinterpret_vm_vd(d), vreinterpret_vm_vd(PNMASK))); }
static INLINE vdouble vnegpos_vd_vd(vdouble d) { return vreinterpret_vd_vm(vxor_vm_vm_vm(vreinterpret_vm_vd(d), vreinterpret_vm_vd(NPMASK))); }
static INLINE vfloat vposneg_vf_vf(vfloat d) { return vreinterpret_vf_vm(vxor_vm_vm_vm(vreinterpret_vm_vf(d), vreinterpret_vm_vf(PNMASKf))); }
static INLINE vfloat vnegpos_vf_vf(vfloat d) { return vreinterpret_vf_vm(vxor_vm_vm_vm(vreinterpret_vm_vf(d), vreinterpret_vm_vf(NPMASKf))); }

//

static INLINE vdouble vabs_vd_vd(vdouble d) { return vec_abs(d); }
static INLINE vdouble vsubadd_vd_vd_vd(vdouble x, vdouble y) { return vadd_vd_vd_vd(x, vnegpos_vd_vd(y)); }

#if CONFIG == 130 || CONFIG == 140
static INLINE vdouble vmla_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_madd(x, y, z); }
static INLINE vdouble vmlapn_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_msub(x, y, z); }
#if CONFIG == 140 || CONFIG == 141
static INLINE vdouble vmlanp_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_nmsub(x, y, z); }
#else
static INLINE vdouble vmlanp_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return -vmlanp_vd_vd_vd_vd(x, y, z); }
#endif
#else
static INLINE vdouble vmla_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vadd_vd_vd_vd(vmul_vd_vd_vd(x, y), z); }
static INLINE vdouble vmlapn_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vsub_vd_vd_vd(vmul_vd_vd_vd(x, y), z); }
#endif

static INLINE vdouble vmlsubadd_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vmla_vd_vd_vd_vd(x, y, vnegpos_vd_vd(z)); }
static INLINE vdouble vfma_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_madd(x, y, z); }
static INLINE vdouble vfmapp_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_madd(x, y, z); }
static INLINE vdouble vfmapn_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_msub(x, y, z); }
#if CONFIG == 140 || CONFIG == 141
static INLINE vdouble vfmanp_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_nmsub(x, y, z); }
static INLINE vdouble vfmann_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vec_nmadd(x, y, z); }
#else
static INLINE vdouble vfmanp_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return vfma_vd_vd_vd_vd(-x, y, z); }
static INLINE vdouble vfmann_vd_vd_vd_vd(vdouble x, vdouble y, vdouble z) { return -vfma_vd_vd_vd_vd(x, y, z); }
#endif

static INLINE vfloat vsubadd_vf_vf_vf(vfloat x, vfloat y) { return vadd_vf_vf_vf(x, vnegpos_vf_vf(y)); }

#if CONFIG == 130 || CONFIG == 140
#if CONFIG == 140 || CONFIG == 141
static INLINE vfloat vmla_vf_vf_vf_vf  (vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_madd (x, y, z); }
static INLINE vfloat vmlanp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_nmsub(x, y, z); }
static INLINE vfloat vmlapn_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_msub (x, y, z); }
static INLINE vfloat vfma_vf_vf_vf_vf  (vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_madd (x, y, z); }
static INLINE vfloat vfmapp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_madd (x, y, z); }
static INLINE vfloat vfmapn_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_msub (x, y, z); }
static INLINE vfloat vfmanp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_nmsub(x, y, z); }
static INLINE vfloat vfmann_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return __builtin_s390_vec_nmadd(x, y, z); }
#else
static vfloat vfma_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) {
  return (vfloat) {
    __builtin_fmaf(x[0], y[0], z[0]),
    __builtin_fmaf(x[1], y[1], z[1]),
    __builtin_fmaf(x[2], y[2], z[2]),
    __builtin_fmaf(x[3], y[3], z[3])
  };
}
static INLINE vfloat vmlanp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(-x, y, +z); }
static INLINE vfloat vmlapn_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(+x, y, -z); }
static INLINE vfloat vmla_vf_vf_vf_vf  (vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(+x, y, +z); }
static INLINE vfloat vfmapp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(+x, y, +z); }
static INLINE vfloat vfmapn_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(+x, y, -z); }
static INLINE vfloat vfmanp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(-x, y, +z); }
static INLINE vfloat vfmann_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vfma_vf_vf_vf_vf(-x, y, -z); }
#endif
#else
static INLINE vfloat vmla_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vadd_vf_vf_vf(vmul_vf_vf_vf(x, y), z); }
static INLINE vfloat vmlanp_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vsub_vf_vf_vf(z, vmul_vf_vf_vf(x, y)); }
static INLINE vfloat vmlapn_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vsub_vf_vf_vf(vmul_vf_vf_vf(x, y), z); }
#endif

static INLINE vfloat vmlsubadd_vf_vf_vf_vf(vfloat x, vfloat y, vfloat z) { return vmla_vf_vf_vf_vf(x, y, vnegpos_vf_vf(z)); }

//

static INLINE CONST vdouble vsel_vd_vo_d_d(vopmask o, double v1, double v0) {
  return vsel_vd_vo_vd_vd(o, vcast_vd_d(v1), vcast_vd_d(v0));
}

static INLINE vdouble vsel_vd_vo_vo_d_d_d(vopmask o0, vopmask o1, double d0, double d1, double d2) {
  return vsel_vd_vo_vd_vd(o0, vcast_vd_d(d0), vsel_vd_vo_d_d(o1, d1, d2));
}

static INLINE vdouble vsel_vd_vo_vo_vo_d_d_d_d(vopmask o0, vopmask o1, vopmask o2, double d0, double d1, double d2, double d3) {
  return vsel_vd_vo_vd_vd(o0, vcast_vd_d(d0), vsel_vd_vo_vd_vd(o1, vcast_vd_d(d1), vsel_vd_vo_d_d(o2, d2, d3)));
}

//

static INLINE vopmask vnot_vo_vo(vopmask o) { return ~o; }

static INLINE vopmask veq_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vec_cmpeq(x, y); }
static INLINE vopmask vneq_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vnot_vo_vo(vec_cmpeq(x, y)); }
static INLINE vopmask vlt_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vec_cmplt(x, y); }
static INLINE vopmask vle_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vec_cmple(x, y); }
static INLINE vopmask vgt_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vec_cmpgt(x, y); }
static INLINE vopmask vge_vo_vd_vd(vdouble x, vdouble y) { return (vopmask)vec_cmpge(x, y); }

static INLINE vint vadd_vi_vi_vi(vint x, vint y) { return x + y; }
static INLINE vint vsub_vi_vi_vi(vint x, vint y) { return x - y; }
static INLINE vint vneg_vi_vi(vint e) { return -e; }

static INLINE vint vand_vi_vi_vi(vint x, vint y) { return x & y; }
static INLINE vint vandnot_vi_vi_vi(vint x, vint y) { return y & ~x; }
static INLINE vint vor_vi_vi_vi(vint x, vint y) { return x | y; }
static INLINE vint vxor_vi_vi_vi(vint x, vint y) { return x ^ y; }

static INLINE vint vand_vi_vo_vi(vopmask x, vint y) { return vreinterpretFirstHalf_vi_vi2((vint2)x) & y; }
static INLINE vint vandnot_vi_vo_vi(vopmask x, vint y) { return vec_andc(y, vreinterpretFirstHalf_vi_vi2((vint2)x)); }

static INLINE vint vsll_vi_vi_i(vint x, int c) { return (vint)(((VECTOR unsigned int)x) << (VECTOR unsigned int){c, c, c, c}); }
static INLINE vint vsrl_vi_vi_i(vint x, int c) { return (vint)(((VECTOR unsigned int)x) >> (VECTOR unsigned int){c, c, c, c}); }
static INLINE vint vsra_vi_vi_i(vint x, int c) { return x >> (VECTOR int){c, c, c, c}; }

static INLINE vint veq_vi_vi_vi(vint x, vint y) { return vec_cmpeq(x, y); }
static INLINE vint vgt_vi_vi_vi(vint x, vint y) { return vec_cmpgt(x, y); }

static INLINE vopmask veq_vo_vi_vi(vint x, vint y) { return (vopmask)vreinterpretFirstHalf_vi2_vi(vec_cmpeq(x, y)); }
static INLINE vopmask vgt_vo_vi_vi(vint x, vint y) { return (vopmask)vreinterpretFirstHalf_vi2_vi(vec_cmpgt(x, y));}

static INLINE vint vsel_vi_vo_vi_vi(vopmask m, vint x, vint y) {
  return vor_vi_vi_vi(vand_vi_vi_vi(vreinterpretFirstHalf_vi_vi2((vint2)m), x),
		      vandnot_vi_vi_vi(vreinterpretFirstHalf_vi_vi2((vint2)m), y));
}

static INLINE vopmask visinf_vo_vd(vdouble d) { return (vopmask)(vec_cmpeq(vabs_vd_vd(d), vcast_vd_d(SLEEF_INFINITY))); }
static INLINE vopmask vispinf_vo_vd(vdouble d) { return (vopmask)(vec_cmpeq(d, vcast_vd_d(SLEEF_INFINITY))); }
static INLINE vopmask visminf_vo_vd(vdouble d) { return (vopmask)(vec_cmpeq(d, vcast_vd_d(-SLEEF_INFINITY))); }
static INLINE vopmask visnan_vo_vd(vdouble d) { return (vopmask)(vnot_vo_vo(vec_cmpeq(d, d))); }

static INLINE double vcast_d_vd(vdouble v) { return v[0]; }
static INLINE float vcast_f_vf(vfloat v) { return v[0]; }

static INLINE void vstream_v_p_vd(double *p, vdouble v) { vstore_v_p_vd(p, v); }
static INLINE void vsscatter2_v_p_i_i_vd(double *p, int offset, int step, vdouble v) { vscatter2_v_p_i_i_vd(p, offset, step, v); }

//

static INLINE CONST vfloat vsel_vf_vo_f_f(vopmask o, float v1, float v0) {
  return vsel_vf_vo_vf_vf(o, vcast_vf_f(v1), vcast_vf_f(v0));
}

static INLINE vfloat vsel_vf_vo_vo_f_f_f(vopmask o0, vopmask o1, float d0, float d1, float d2) {
  return vsel_vf_vo_vf_vf(o0, vcast_vf_f(d0), vsel_vf_vo_f_f(o1, d1, d2));
}

static INLINE vfloat vsel_vf_vo_vo_vo_f_f_f_f(vopmask o0, vopmask o1, vopmask o2, float d0, float d1, float d2, float d3) {
  return vsel_vf_vo_vf_vf(o0, vcast_vf_f(d0), vsel_vf_vo_vf_vf(o1, vcast_vf_f(d1), vsel_vf_vo_f_f(o2, d2, d3)));
}

static INLINE vint2 vcast_vi2_vm(vmask vm) { return (vint2)vm; }
static INLINE vmask vcast_vm_vi2(vint2 vi) { return (vmask)vi; }

static INLINE vint2 vadd_vi2_vi2_vi2(vint2 x, vint2 y) { return x + y; }
static INLINE vint2 vsub_vi2_vi2_vi2(vint2 x, vint2 y) { return x - y; }
static INLINE vint2 vneg_vi2_vi2(vint2 e) { return -e; }

static INLINE vint2 vand_vi2_vi2_vi2(vint2 x, vint2 y) { return x & y; }
static INLINE vint2 vandnot_vi2_vi2_vi2(vint2 x, vint2 y) { return  y & ~x; }
static INLINE vint2 vor_vi2_vi2_vi2(vint2 x, vint2 y) { return x | y; }
static INLINE vint2 vxor_vi2_vi2_vi2(vint2 x, vint2 y) { return x ^ y; }

static INLINE vint2 vand_vi2_vo_vi2(vopmask x, vint2 y) { return (vint2)x & y; }
static INLINE vint2 vandnot_vi2_vo_vi2(vopmask x, vint2 y) { return y & ~(vint2)x; }

static INLINE vint2 vsll_vi2_vi2_i(vint2 x, int c) { return (vint2)(((VECTOR unsigned int)x) << (VECTOR unsigned int){c, c, c, c}); }
static INLINE vint2 vsrl_vi2_vi2_i(vint2 x, int c) { return (vint2)(((VECTOR unsigned int)x) >> (VECTOR unsigned int){c, c, c, c}); }
static INLINE vint2 vsra_vi2_vi2_i(vint2 x, int c) { return x >> (VECTOR int){c, c, c, c}; }

static INLINE vopmask veq_vo_vi2_vi2(vint2 x, vint2 y) { return (vopmask)vec_cmpeq(x, y); }
static INLINE vopmask vgt_vo_vi2_vi2(vint2 x, vint2 y) { return (vopmask)vec_cmpgt(x, y); }
static INLINE vint2 veq_vi2_vi2_vi2(vint2 x, vint2 y) { return vec_cmpeq(x, y); }
static INLINE vint2 vgt_vi2_vi2_vi2(vint2 x, vint2 y) { return vec_cmpgt(x, y); }

static INLINE void vsscatter2_v_p_i_i_vf(float *p, int offset, int step, vfloat v) { vscatter2_v_p_i_i_vf(p, offset, step, v); }
static INLINE void vstream_v_p_vf(float *p, vfloat v) { vstore_v_p_vf(p, v); }

//

#if CONFIG == 140 || CONFIG == 141
static INLINE vdouble vmax_vd_vd_vd(vdouble x, vdouble y) { return vec_max(x, y); }
static INLINE vdouble vmin_vd_vd_vd(vdouble x, vdouble y) { return vec_min(x, y); }
static INLINE vdouble vsqrt_vd_vd(vdouble d) { return __builtin_s390_vec_sqrt(d); }

static INLINE vopmask veq_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vec_cmpeq(x, y); }
static INLINE vopmask vneq_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vnot_vo_vo(vec_cmpeq(x, y)); }
static INLINE vopmask vlt_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vec_cmplt(x, y); }
static INLINE vopmask vle_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vec_cmple(x, y); }
static INLINE vopmask vgt_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vec_cmpgt(x, y); }
static INLINE vopmask vge_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)vec_cmpge(x, y); }

static INLINE vfloat vabs_vf_vf(vfloat f) { return vec_abs(f); }
static INLINE vfloat vrint_vf_vf(vfloat vf) { return __builtin_s390_vfisb(vf, 4, 4); }
static INLINE vfloat vtruncate_vf_vf(vfloat vf) { return __builtin_s390_vfisb(vf, 4, 5); }
static INLINE vfloat vmax_vf_vf_vf(vfloat x, vfloat y) { return vec_max(x, y); }
static INLINE vfloat vmin_vf_vf_vf(vfloat x, vfloat y) { return vec_min(x, y); }

static INLINE vfloat vsqrt_vf_vf(vfloat d) { return __builtin_s390_vec_sqrt(d); }
#else
static INLINE vdouble vmax_vd_vd_vd(vdouble x, vdouble y) {
  VECTOR long long m = (VECTOR long long)((VECTOR unsigned long long)(x > y) << 63) >> 63;
  return (vdouble)((m & (VECTOR long long)x) | (~m & (VECTOR long long)y));
}
static INLINE vdouble vmin_vd_vd_vd(vdouble x, vdouble y) {
  VECTOR long long m = (VECTOR long long)((VECTOR unsigned long long)(x < y) << 63) >> 63;
  return (vdouble)((m & (VECTOR long long)x) | (~m & (VECTOR long long)y));
}
static INLINE vdouble vsqrt_vd_vd(vdouble d) {
  vdouble q, w, x, y, z;
  
  vopmask o = vlt_vo_vd_vd(d, vcast_vd_d(8.636168555094445E-78));
  d = vsel_vd_vo_vd_vd(o, vmul_vd_vd_vd(d, vcast_vd_d(1.157920892373162E77)), d);
  q = vsel_vd_vo_vd_vd(o, vcast_vd_d(2.9387358770557188E-39), vcast_vd_d(1));

  y = vreinterpret_vd_vi2(vsub_vi2_vi2_vi2(vcast_vi2_vm(vcast_vm_i_i(0x5fe6ec85, 0xe7de30da)), vsrl_vi2_vi2_i(vreinterpret_vi2_vd(d), 1)));

  x = vmul_vd_vd_vd(d, y);         w = vmul_vd_vd_vd(vcast_vd_d(0.5), y);
  y = vfmanp_vd_vd_vd_vd(x, w, vcast_vd_d(0.5));
  x = vfma_vd_vd_vd_vd(x, y, x);   w = vfma_vd_vd_vd_vd(w, y, w);
  y = vfmanp_vd_vd_vd_vd(x, w, vcast_vd_d(0.5));
  x = vfma_vd_vd_vd_vd(x, y, x);   w = vfma_vd_vd_vd_vd(w, y, w);
  y = vfmanp_vd_vd_vd_vd(x, w, vcast_vd_d(0.5));
  x = vfma_vd_vd_vd_vd(x, y, x);   w = vfma_vd_vd_vd_vd(w, y, w);

  y = vfmanp_vd_vd_vd_vd(x, w, vcast_vd_d(1.5));  w = vadd_vd_vd_vd(w, w);
  w = vmul_vd_vd_vd(w, y);
  x = vmul_vd_vd_vd(w, d);
  y = vfmapn_vd_vd_vd_vd(w, d, x); z = vfmanp_vd_vd_vd_vd(w, x, vcast_vd_d(1));

  z = vfmanp_vd_vd_vd_vd(w, y, z); w = vmul_vd_vd_vd(vcast_vd_d(0.5), x);
  w = vfma_vd_vd_vd_vd(w, z, y);
  w = vadd_vd_vd_vd(w, x);

  w = vmul_vd_vd_vd(w, q);

  w = vsel_vd_vo_vd_vd(vor_vo_vo_vo(veq_vo_vd_vd(d, vcast_vd_d(0)),
				    veq_vo_vd_vd(d, vcast_vd_d(SLEEF_INFINITY))), d, w);

  w = vsel_vd_vo_vd_vd(vlt_vo_vd_vd(d, vcast_vd_d(0)), vcast_vd_d(SLEEF_NAN), w);

  return w;
}

static INLINE vopmask veq_vo_vf_vf (vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x == y) << 31) >> 31); }
static INLINE vopmask vneq_vo_vf_vf(vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x != y) << 31) >> 31); }
static INLINE vopmask vlt_vo_vf_vf (vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x <  y) << 31) >> 31); }
static INLINE vopmask vle_vo_vf_vf (vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x <= y) << 31) >> 31); }
static INLINE vopmask vgt_vo_vf_vf (vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x >  y) << 31) >> 31); }
static INLINE vopmask vge_vo_vf_vf (vfloat x, vfloat y) { return (vopmask)((VECTOR int)((VECTOR unsigned int)(x >= y) << 31) >> 31); }

static INLINE vfloat vabs_vf_vf(vfloat f) { return (vfloat)(vcast_vm_i_i(0x7fffffff, 0x7fffffff) & (VECTOR unsigned long long)f); }

static INLINE vfloat vrint_vf_vf(vfloat d) {
  vfloat c = vreinterpret_vf_vm(vxor_vm_vm_vm(vreinterpret_vm_vf(vcast_vf_f(1 << 23)),
					      vand_vm_vm_vm(vreinterpret_vm_vf(d), vreinterpret_vm_vf(vcast_vf_f(-0.0f)))));
  vfloat e = vsub_vf_vf_vf(vadd_vf_vf_vf(d, c), c);
  e = vreinterpret_vf_vm(vor_vm_vm_vm(vreinterpret_vm_vf(e), vand_vm_vm_vm(vreinterpret_vm_vf(d), vreinterpret_vm_vf(vcast_vf_f(-0.0f)))));
  return vsel_vf_vo_vf_vf(vgt_vo_vf_vf(vabs_vf_vf(d), vcast_vf_f(1 << 23)), d, e);
}

static INLINE vfloat vtruncate_vf_vf(vfloat x) {
  vfloat fr = vsub_vf_vf_vf(x, vcast_vf_vi2(vtruncate_vi2_vf(x)));
  return vsel_vf_vo_vf_vf(vor_vo_vo_vo(veq_vo_vf_vf(vabs_vf_vf(x), vcast_vf_f(SLEEF_INFINITYf)),
				       vge_vo_vf_vf(vabs_vf_vf(x), vcast_vf_f(1LL << 23      ))),
			  x,
			  vreinterpret_vf_vm(vxor_vm_vm_vm(vandnot_vm_vm_vm(vreinterpret_vm_vf(vcast_vf_f(-0.0f)), vreinterpret_vm_vf(vsub_vf_vf_vf(x, fr))), 
							   vand_vm_vm_vm   (vreinterpret_vm_vf(vcast_vf_f(-0.0f)), vreinterpret_vm_vf(x)))));
}

static INLINE vfloat vmax_vf_vf_vf(vfloat x, vfloat y) {
  VECTOR int m = (VECTOR int)((VECTOR unsigned int)(x > y) << 31) >> 31;
  return (vfloat)((m & (VECTOR int)x) | (~m & (VECTOR int)y));
}

static INLINE vfloat vmin_vf_vf_vf(vfloat x, vfloat y) {
  VECTOR int m = (VECTOR int)((VECTOR unsigned int)(x < y) << 31) >> 31;
  return (vfloat)((m & (VECTOR int)x) | (~m & (VECTOR int)y));
}

static INLINE vfloat vsqrt_vf_vf(vfloat d) {
  vfloat q, w, x, y, z;

  d = vsel_vf_vo_vf_vf(vlt_vo_vf_vf(d, vcast_vf_f(0)), vcast_vf_f(SLEEF_NANf), d);

  vopmask o = vlt_vo_vf_vf(d, vcast_vf_f(5.2939559203393770e-23f));
  d = vsel_vf_vo_vf_vf(o, vmul_vf_vf_vf(d, vcast_vf_f(1.8889465931478580e+22f)), d);
  q = vsel_vf_vo_vf_vf(o, vcast_vf_f(7.2759576141834260e-12f), vcast_vf_f(1.0f));

  y = vreinterpret_vf_vi2(vsub_vi2_vi2_vi2(vcast_vi2_i(0x5f3759df), vsrl_vi2_vi2_i(vreinterpret_vi2_vf(d), 1)));

  x = vmul_vf_vf_vf(d, y);         w = vmul_vf_vf_vf(vcast_vf_f(0.5), y);
  y = vfmanp_vf_vf_vf_vf(x, w, vcast_vf_f(0.5));
  x = vfma_vf_vf_vf_vf(x, y, x);   w = vfma_vf_vf_vf_vf(w, y, w);
  y = vfmanp_vf_vf_vf_vf(x, w, vcast_vf_f(0.5));
  x = vfma_vf_vf_vf_vf(x, y, x);   w = vfma_vf_vf_vf_vf(w, y, w);

  y = vfmanp_vf_vf_vf_vf(x, w, vcast_vf_f(1.5));  w = vadd_vf_vf_vf(w, w);
  w = vmul_vf_vf_vf(w, y);
  x = vmul_vf_vf_vf(w, d);
  y = vfmapn_vf_vf_vf_vf(w, d, x); z = vfmanp_vf_vf_vf_vf(w, x, vcast_vf_f(1));

  z = vfmanp_vf_vf_vf_vf(w, y, z); w = vmul_vf_vf_vf(vcast_vf_f(0.5), x);
  w = vfma_vf_vf_vf_vf(w, z, y);
  w = vadd_vf_vf_vf(w, x);

  w = vmul_vf_vf_vf(w, q);

  w = vsel_vf_vo_vf_vf(vor_vo_vo_vo(veq_vo_vf_vf(d, vcast_vf_f(0)),
				    veq_vo_vf_vf(d, vcast_vf_f(SLEEF_INFINITYf))), d, w);

  w = vsel_vf_vo_vf_vf(vlt_vo_vf_vf(d, vcast_vf_f(0)), vcast_vf_f(SLEEF_NANf), w);

  return w;
}
#endif

static INLINE vopmask visinf_vo_vf (vfloat d) { return veq_vo_vf_vf(vabs_vf_vf(d), vcast_vf_f(SLEEF_INFINITYf)); }
static INLINE vopmask vispinf_vo_vf(vfloat d) { return veq_vo_vf_vf(d, vcast_vf_f(SLEEF_INFINITYf)); }
static INLINE vopmask visminf_vo_vf(vfloat d) { return veq_vo_vf_vf(d, vcast_vf_f(-SLEEF_INFINITYf)); }
static INLINE vopmask visnan_vo_vf (vfloat d) { return vneq_vo_vf_vf(d, d); }

static INLINE vint2 vrint_vi2_vf(vfloat vf) {
  vf = vrint_vf_vf(vf);
  return (vint) { vf[0], vf[1], vf[2], vf[3] };
}