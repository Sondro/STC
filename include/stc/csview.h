/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CSVIEW_H_INCLUDED
#define CSVIEW_H_INCLUDED

#include "ccommon.h"
#include "forward.h"
#include "utf8.h"

#define             csview_null  c_sv("")
#define             csview_npos  (SIZE_MAX >> 1)

#define             csview_init() csview_null
#define             csview_drop   c_default_drop
#define             csview_clone  c_default_clone
#define             csview_from_n c_sv

STC_INLINE csview   csview_from(const char* str)
                        { return c_make(csview){str, strlen(str)}; }
STC_INLINE void     csview_clear(csview* self) { *self = csview_null; }

STC_INLINE size_t   csview_size(csview sv) { return sv.size; }
STC_INLINE bool     csview_empty(csview sv) { return sv.size == 0; }

STC_INLINE bool csview_equals(csview sv, csview sv2)
    { return sv.size == sv2.size && !memcmp(sv.str, sv2.str, sv.size); }

STC_API size_t csview_find(csview sv, csview search);

STC_INLINE bool csview_contains(csview sv, csview search)
    { return csview_find(sv, search) != csview_npos; }

STC_INLINE bool csview_starts_with(csview sv, csview sub) {
    if (sub.size > sv.size) return false;
    return !memcmp(sv.str, sub.str, sub.size);
}

STC_INLINE bool csview_ends_with(csview sv, csview sub) {
    if (sub.size > sv.size) return false;
    return !memcmp(sv.str + sv.size - sub.size, sub.str, sub.size);
}

STC_INLINE csview csview_substr(csview sv, size_t pos, size_t n) {
    if (pos + n > sv.size) n = sv.size - pos;
    sv.str += pos, sv.size = n;
    return sv;
}

STC_INLINE csview csview_slice(csview sv, size_t p1, size_t p2) {
    if (p2 > sv.size) p2 = sv.size;
    sv.str += p1, sv.size = p2 > p1 ? p2 - p1 : 0;
    return sv;
}

/* utf8 iterator */
STC_INLINE csview_iter csview_begin(const csview* self) { 
    if (!self->size) return c_make(csview_iter){NULL};
    return c_make(csview_iter){.u8 = {{self->str, utf8_chr_size(self->str)},
                                      self->str + self->size}};
}
STC_INLINE csview_iter csview_end(const csview* self) {
    return c_make(csview_iter){.u8 = {{NULL}, self->str + self->size}};
}
STC_INLINE void csview_next(csview_iter* it) {
    it->ref += it->u8.chr.size;
    it->u8.chr.size = utf8_chr_size(it->ref);
    if (it->ref == it->u8.end) it->ref = NULL;
}
STC_INLINE csview_iter csview_advance(csview_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && it.ref != it.u8.end) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.u8.chr.size = utf8_chr_size(it.ref);
    if (it.ref == it.u8.end) it.ref = NULL;
    return it;
}


/* utf8 */
STC_INLINE size_t csview_u8_size(csview sv)
    { return utf8_size_n(sv.str, sv.size); }

STC_INLINE csview csview_u8_substr(csview sv, size_t bytepos, size_t u8len) {
    sv.str += bytepos;
    sv.size = utf8_pos(sv.str, u8len);
    return sv;
}

STC_INLINE bool csview_valid_utf8(csview sv) // depends on src/utf8code.c
    { return utf8_valid_n(sv.str, sv.size); }

STC_API csview csview_substr_ex(csview sv, intptr_t pos, size_t n);
STC_API csview csview_slice_ex(csview sv, intptr_t p1, intptr_t p2);
STC_API csview csview_token(csview sv, csview sep, size_t* start);

#define c_fortoken_sv(it, input, sep) \
    for (struct { csview _inp, _sep, token, *ref; size_t pos; } \
          it = {._inp=input, ._sep=sep, .token=it._inp, .ref=&it.token, .pos=0} \
        ; it.pos <= it._inp.size && (it.token = csview_token(it._inp, it._sep, &it.pos)).str ; )

#define c_fortoken(it, input, sep) \
    c_fortoken_sv(it, csview_from(input), csview_from(sep))

/* csview interaction with cstr: */
#ifdef CSTR_H_INCLUDED

STC_INLINE csview cstr_substr(const cstr* self, size_t pos, size_t n)
    { return csview_substr(cstr_sv(self), pos, n); }

STC_INLINE csview cstr_slice(const cstr* self, size_t p1, size_t p2)
    { return csview_slice(cstr_sv(self), p1, p2); }

STC_INLINE csview cstr_substr_ex(const cstr* self, intptr_t pos, size_t n)
    { return csview_substr_ex(cstr_sv(self), pos, n); }

STC_INLINE csview cstr_slice_ex(const cstr* self, intptr_t p1, intptr_t p2)
    { return csview_slice_ex(cstr_sv(self), p1, p2); }

STC_INLINE csview cstr_u8_substr(const cstr* self , size_t bytepos, size_t u8len)
    { return csview_u8_substr(cstr_sv(self), bytepos, u8len); }

#endif
/* ---- Container helper functions ---- */

STC_INLINE int csview_cmp(const csview* x, const csview* y) {
    size_t n = x->size < y->size ? x->size : y->size;
    int c = memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE int csview_icmp(const csview* x, const csview* y)
    { return utf8_icmp_sv(*x, *y); }

STC_INLINE bool csview_eq(const csview* x, const csview* y)
    { return x->size == y->size && !memcmp(x->str, y->str, x->size); }

STC_API uint64_t csview_hash(const csview *self);

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_extern)

STC_DEF size_t csview_find(csview sv, csview search) {
    char* res = c_strnstrn(sv.str, search.str, sv.size, search.size);
    return res ? res - sv.str : csview_npos;
}

STC_DEF uint64_t csview_hash(const csview *self)
    { return c_fasthash(self->str, self->size); }

STC_DEF csview csview_substr_ex(csview sv, intptr_t pos, size_t n) {
    if (pos < 0) { 
        pos += sv.size;
        if (pos < 0) pos = 0;
    }
    if (pos > (intptr_t)sv.size) pos = sv.size; 
    if (pos + n > sv.size) n = sv.size - pos;
    sv.str += pos, sv.size = n;
    return sv;
}

STC_DEF csview csview_slice_ex(csview sv, intptr_t p1, intptr_t p2) {
    if (p1 < 0) { 
        p1 += sv.size;
        if (p1 < 0) p1 = 0;
    }
    if (p2 < 0) p2 += sv.size;
    if (p2 > (intptr_t)sv.size) p2 = sv.size;
    sv.str += p1, sv.size = p2 > p1 ? p2 - p1 : 0;
    return sv;
}

STC_DEF csview csview_token(csview sv, csview sep, size_t* start) {
    csview slice = {sv.str + *start, sv.size - *start};
    const char* res = c_strnstrn(slice.str, sep.str, slice.size, sep.size);
    csview tok = {slice.str, res ? res - slice.str : slice.size};
    *start += tok.size + sep.size;
    return tok;
}

#endif
#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_extern
