#ifndef CARRAY__H__
#define CARRAY__H__

#include "cdefs.h"

#define carray_xdim(a) ((a).xdim)
#define carray_ydim(a) ((a)._yxdim / (a).xdim)
#define carray_zdim(a) ((a).zdim)
#define carray_ref(a) (++(a)._refCount, a)
#define carray_unref(a) ({if (--(a)._refCount == 0) free((a).data);0;})

/* // demo:    
#include "ccl/carray.h"
declare_CArray(f, float);

int main()
{
    CArray3f a3 = carray3f_make(30, 20, 10);
    carray3f_at2(a3, 3, 2).data[1] = 10.2f;
    CArray2f a2 = carray3f_at(a3, 3);
    printf("%f\n", carray2f_at(a2, 2).data[1]);
    carray_unref(a2);
    carray_unref(a3);
}
*/

#define declare_CArray(tag, T) \
    c_struct (CArray1##tag) { \
        T* data; \
        int _refCount; \
        size_t xdim; \
    }; \
    c_struct (CArray2##tag) { \
        T* data; \
        int _refCount; \
        size_t xdim, _yxdim; \
    }; \
    c_struct (CArray3##tag) { \
        T* data; \
        int _refCount; \
        size_t xdim, _yxdim, zdim; \
    }; \
    \
    static inline CArray1##tag carray1##tag##_make(size_t xdim) { \
        CArray1##tag a = {c_new_2(T, xdim), 1, xdim}; return a; \
    } \
    static inline CArray2##tag carray2##tag##_make(size_t ydim, size_t xdim) { \
        CArray2##tag a = {c_new_2(T, ydim*xdim), 1, xdim, ydim*xdim}; return a; \
    } \
    static inline CArray3##tag carray3##tag##_make(size_t zdim, size_t ydim, size_t xdim) { \
        CArray3##tag a = {c_new_2(T, zdim*ydim*xdim), 1, xdim, ydim*xdim, zdim}; return a; \
    } \
    \
	static inline CArray1##tag carray2##tag##_at(CArray2##tag a, size_t y) { \
        CArray1##tag out = {a.data + y*a.xdim, 1000, a.xdim}; return out; \
    } \
	static inline CArray2##tag carray3##tag##_at(CArray3##tag a, size_t z) { \
        CArray2##tag out = {a.data + z*a._yxdim, 1000, a.xdim, a._yxdim}; return out; \
    } \
	static inline CArray1##tag carray3##tag##_at2(CArray3##tag a, size_t z, size_t y) { \
        CArray1##tag out = {a.data + z*a._yxdim + y*a.xdim, 1000, a.xdim}; return out; \
    } \
    typedef T carray_##tag##_t

#endif