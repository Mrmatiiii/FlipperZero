/*
 * M*LIB - dynamic priority queue module
 *
 * Copyright (c) 2017-2021, Patrick Pelissier
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * + Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * + Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef MSTARLIB_PRIOQUEUE_H
#define MSTARLIB_PRIOQUEUE_H

#include "m-core.h"
#include "m-array.h"            /* Priority queue are built upon array */

/* Priority queue based on binary heap implementation */

/* Define a prioqueue of a given type and its associated functions.
   USAGE: PRIOQUEUE_DEF(name, type [, oplist_of_the_type]) */
#define PRIOQUEUE_DEF(name, ...)                                              \
  PRIOQUEUE_DEF_AS(name, M_C(name,_t), M_C(name,_it_t), __VA_ARGS__)


/* Define a prioqueue of a given type and its associated functions.
  as the name name_t with an iterator named it_t
   USAGE: PRIOQUEUE_DEF_AS(name, name_t, it_t, type [, oplist_of_the_type]) */
#define PRIOQUEUE_DEF_AS(name, name_t, it_t, ...)                             \
  M_BEGIN_PROTECTED_CODE                                                      \
  M_PR1OQUEUE_DEF_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                              \
                    ((name, __VA_ARGS__, M_GLOBAL_OPLIST_OR_DEF(__VA_ARGS__)(), name_t, it_t ), \
                     (name, __VA_ARGS__,                                        name_t, it_t ))) \
  M_END_PROTECTED_CODE


/* Define the oplist of a prioqueue of type.
   USAGE: PRIOQUEUE_OPLIST(name[, oplist of the type]) */
#define PRIOQUEUE_OPLIST(...)                                                 \
  M_PR1OQUEUE_OPLIST_P1(M_IF_NARGS_EQ1(__VA_ARGS__)                           \
                       ((__VA_ARGS__, M_DEFAULT_OPLIST),                      \
                        (__VA_ARGS__ )))



/********************************** INTERNAL ************************************/

/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_PR1OQUEUE_OPLIST_P1(arg) M_PR1OQUEUE_OPLIST_P2 arg

/* Validation of the given oplist */
#define M_PR1OQUEUE_OPLIST_P2(name, oplist)                                   \
  M_IF_OPLIST(oplist)(M_PR1OQUEUE_OPLIST_P3, M_PR1OQUEUE_OPLIST_FAILURE)(name, oplist)

/* Prepare a clean compilation failure */
#define M_PR1OQUEUE_OPLIST_FAILURE(name, oplist)                              \
  ((M_LIB_ERROR(ARGUMENT_OF_PRIOQUEUE_OPLIST_IS_NOT_AN_OPLIST, name, oplist)))

/* Define oplist of a priority queue */
#define M_PR1OQUEUE_OPLIST_P3(name, oplist)                                   \
  (INIT(M_C(name, _init))                                                     \
   ,INIT_SET(M_C(name, _init_set))                                            \
   ,INIT_WITH(API_1(M_INIT_VAI))                                              \
   ,SET(M_C(name, _set))                                                      \
   ,CLEAR(M_C(name, _clear))                                                  \
   ,INIT_MOVE(M_C(name, _init_move))                                          \
   ,MOVE(M_C(name, _move))                                                    \
   ,SWAP(M_C(name, _swap))                                                    \
   ,NAME(name)                                                                \
   ,TYPE(M_C(name,_ct))                                                       \
   ,SUBTYPE(M_C(name, _subtype_ct))                                           \
   ,RESET(M_C(name,_reset))                                                   \
   ,PUSH(M_C(name,_push))                                                     \
   ,POP(M_C(name,_pop))                                                       \
   ,OPLIST(oplist)                                                            \
   ,EMPTY_P(M_C(name, _empty_p)),                                             \
   ,GET_SIZE(M_C(name, _size))                                                \
   ,IT_TYPE(M_C(name, _it_ct)),                                               \
   IT_FIRST(M_C(name,_it)),                                                   \
   IT_END(M_C(name,_it_end)),                                                 \
   IT_SET(M_C(name,_it_set)),                                                 \
   IT_END_P(M_C(name,_end_p)),                                                \
   IT_EQUAL_P(M_C(name,_it_equal_p)),                                         \
   IT_LAST_P(M_C(name,_last_p)),                                              \
   IT_NEXT(M_C(name,_next)),                                                  \
   IT_CREF(M_C(name,_cref)),                                                  \
   )


/* Deferred evaluation for the definition,
   so that all arguments are evaluated before further expansion */
#define M_PR1OQUEUE_DEF_P1(arg)    M_PR1OQUEUE_DEF_P2 arg

/* Validate the oplist before going further */
#define M_PR1OQUEUE_DEF_P2(name, type, oplist, prioqueue_t, it_t)             \
  M_IF_OPLIST(oplist)(M_PR1OQUEUE_DEF_P3, M_PR1OQUEUE_DEF_FAILURE)(name, type, oplist, prioqueue_t, it_t)

/* Stop processing with a compilation failure */
#define M_PR1OQUEUE_DEF_FAILURE(name, type, oplist, prioqueue_t, it_t)        \
  M_STATIC_FAILURE(M_LIB_NOT_AN_OPLIST, "(PRIOQUEUE_DEF): the given argument is not a valid oplist: " #oplist)

/* Define the priority queue:
   - name: prefix to use,
   - type: type of the contained objects,
   - oplist: oplist of the contained objects,
   - prioqueue_t: type of the container,
   - it_t: iterator of the container
*/
#define M_PR1OQUEUE_DEF_P3(name, type, oplist, prioqueue_t, it_t)             \
                                                                              \
  /* Definition of the internal array used to construct the priority queue */ \
  ARRAY_DEF(M_C(name, _array), type, oplist)                                  \
                                                                              \
  /* Define the priority queue over the defined array */                      \
  typedef struct M_C(name, _s) {                                              \
    M_C(name, _array_t) array;                                                \
  } prioqueue_t[1];                                                           \
  /* Define the pointer references to the priority queue */                   \
  typedef struct M_C(name, _s) *M_C(name, _ptr);                              \
  typedef const struct M_C(name, _s) *M_C(name, _srcptr);                     \
                                                                              \
  /* The iterator is the same one as the one of the internal array */         \
  typedef M_C(name, _array_it_t) it_t;                                        \
                                                                              \
  /* Definition of the internal types used by the oplist */                   \
  typedef prioqueue_t M_C(name, _ct);                                         \
  typedef type        M_C(name, _subtype_ct);                                 \
  typedef it_t        M_C(name, _it_ct);                                      \
                                                                              \
  M_CHECK_COMPATIBLE_OPLIST(name, 1, type, oplist)                            \
                                                                              \
  static inline void                                                          \
  M_C(name, _init)(prioqueue_t p)                                             \
  {                                                                           \
    M_C(name, _array_init)(p->array);                                         \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init_set)(prioqueue_t p, prioqueue_t const o)                    \
  {                                                                           \
    M_C(name, _array_init_set)(p->array, o->array);                           \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _set)(prioqueue_t p, prioqueue_t const o)                         \
  {                                                                           \
    M_C(name, _array_set)(p->array, o->array);                                \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _clear)(prioqueue_t p)                                            \
  {                                                                           \
    M_C(name, _array_clear)(p->array);                                        \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _init_move)(prioqueue_t p, prioqueue_t o)                         \
  {                                                                           \
    M_C(name, _array_init_move)(p->array, o->array);                          \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _move)(prioqueue_t p, prioqueue_t o)                              \
  {                                                                           \
    M_C(name, _array_move)(p->array, o->array);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _swap)(prioqueue_t p, prioqueue_t o)                              \
  {                                                                           \
    M_C(name, _array_swap)(p->array, o->array);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _reset)(prioqueue_t p)                                            \
  {                                                                           \
    M_C(name, _array_reset)(p->array);                                        \
  }                                                                           \
                                                                              \
  static inline void M_ATTR_DEPRECATED                                        \
  M_C(name, _clean)(prioqueue_t p)                                            \
  {                                                                           \
    M_C(name,_reset)(p);                                                      \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _i_parent)(size_t i)                                              \
  {                                                                           \
    M_ASSERT (i > 0);                                                         \
    return (i - 1) / 2;                                                       \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _i_lchild)(size_t i)                                              \
  {                                                                           \
    M_ASSERT(i <= ((SIZE_MAX)-2)/2);                                          \
    return 2*i + 1;                                                           \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _i_rchild)(size_t i)                                              \
  {                                                                           \
    M_ASSERT(i <= ((SIZE_MAX)-2)/2);                                          \
    return 2*i + 2;                                                           \
  }                                                                           \
                                                                              \
  static inline int                                                           \
  M_C(name, _i_cmp)(const prioqueue_t p, size_t i, size_t j)                  \
  {                                                                           \
    return M_CALL_CMP(oplist, *M_C(name, _array_cget)(p->array, i),           \
                      *M_C(name, _array_cget)(p->array, j));                  \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _empty_p)(prioqueue_t const p)                                    \
  {                                                                           \
    return M_C(name, _array_empty_p)(p->array);                               \
  }                                                                           \
                                                                              \
  static inline size_t                                                        \
  M_C(name, _size)(prioqueue_t const p)                                       \
  {                                                                           \
    return M_C(name, _array_size)(p->array);                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _push)(prioqueue_t p, type const x)                               \
  {                                                                           \
    /* Push back the new element at the end of the array */                   \
    M_C(name, _array_push_back)(p->array, x);                                 \
                                                                              \
    /* Reorder the array by swapping with its parent                          \
     * until it reaches the right position */                                 \
    size_t i = M_C(name, _array_size)(p->array)-1;                            \
    while (i > 0) {                                                           \
      size_t j = M_C(name, _i_parent)(i);                                     \
      if (M_C(name, _i_cmp)(p, j, i) <= 0)                                    \
        break;                                                                \
      M_C(name, _array_swap_at) (p->array, i, j);                             \
      i = j;                                                                  \
    }                                                                         \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_C(name, _front)(prioqueue_t const p)                                      \
  {                                                                           \
    return M_C(name, _array_cget)(p->array, 0);                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _pop)(type *x, prioqueue_t p)                                     \
  {                                                                           \
    /* Swap the front element with the last element */                        \
    size_t size = M_C(name, _array_size)(p->array)-1;                         \
    M_C(name, _array_swap_at) (p->array, 0, size);                            \
    /* Swap the new last element  */                                          \
    M_C(name, _array_pop_back)(x, p->array);                                  \
                                                                              \
    /* Reorder the heap */                                                    \
    size_t i = 0;                                                             \
    while (true) {                                                            \
      size_t child = M_C(name, _i_lchild)(i);                                 \
      if (child >= size)                                                      \
        break;                                                                \
      size_t otherChild = M_C(name, _i_rchild)(i);                            \
      if (otherChild < size                                                   \
          && M_C(name, _i_cmp)(p, otherChild, child) < 0 ) {                  \
        child = otherChild;                                                   \
      }                                                                       \
      if (M_C(name, _i_cmp)(p, i, child) <= 0)                                \
        break;                                                                \
      M_C(name, _array_swap_at) (p->array, i, child);                         \
      i = child;                                                              \
    }                                                                         \
  }                                                                           \
                                                                              \
  /* Define iterators over the array iterator */                              \
  static inline void                                                          \
  M_C(name, _it)(it_t it, prioqueue_t const v)                                \
  {                                                                           \
    M_C(name, _array_it)(it, v->array);                                       \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_last)(it_t it, prioqueue_t const v)                           \
  {                                                                           \
    M_C(name, _array_it_last)(it, v->array);                                  \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_end)(it_t it, prioqueue_t const v)                            \
  {                                                                           \
    M_C(name, _array_it_end)(it, v->array);                                   \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _it_set)(it_t it, const it_t org)                                 \
  {                                                                           \
    M_C(name, _array_it_set)(it, org);                                        \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _end_p)(const it_t it)                                            \
  {                                                                           \
    return M_C(name, _array_end_p)(it);                                       \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _last_p)(const it_t it)                                           \
  {                                                                           \
    return M_C(name, _array_last_p)(it);                                      \
  }                                                                           \
                                                                              \
  static inline bool                                                          \
  M_C(name, _it_equal_p)(const it_t it1,                                      \
                         const it_t it2)                                      \
  {                                                                           \
    return M_C(name, _array_it_equal_p)(it1, it2);                            \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _next)(it_t it)                                                   \
  {                                                                           \
    M_C(name, _array_next)(it);                                               \
  }                                                                           \
                                                                              \
  static inline void                                                          \
  M_C(name, _previous)(it_t it)                                               \
  {                                                                           \
    M_C(name, _array_previous)(it);                                           \
  }                                                                           \
                                                                              \
  static inline type const *                                                  \
  M_C(name, _cref)(const it_t it)                                             \
  {                                                                           \
    return M_C(name, _array_cref)(it);                                        \
  }                                                                           \
                                                                              \
  M_IF_METHOD(EQUAL, oplist)                                                  \
  (                                                                           \
    /* EQUAL & CMP may be uncorrelated */                                     \
    static inline bool                                                        \
    M_C(name, _equal_p)(prioqueue_t const p, prioqueue_t const q)             \
    {                                                                         \
      return M_C(name, _array_equal_p)(p->array, q->array);                   \
    }                                                                         \
                                                                              \
   static inline bool                                                         \
   M_C(name, _erase)(prioqueue_t p, type x)                                   \
   {                                                                          \
     /* First pass: search for an item EQUAL to x */                          \
     /* NOTE: An HASHMAP may be a good idea to optimize this pass. */         \
     size_t size = M_C(name, _array_size)(p->array);                          \
     size_t i = 0;                                                            \
     for(i = 0; i < size; i++) {                                              \
       if (M_CALL_EQUAL(oplist, *M_C(name, _array_cget)(p->array, i), x))     \
         break;                                                               \
     }                                                                        \
     /* If x is not found, then stop */                                       \
     if (i >= size)                                                           \
       return false;                                                          \
     /* Swap the found item and the last element */                           \
     size--;                                                                  \
     M_C(name, _array_swap_at) (p->array, i, size);                           \
     M_C(name, _array_pop_back)(NULL, p->array);                              \
     /* Move back the last swapped element to its right position in the heap */ \
     while (true) {                                                           \
       size_t child = M_C(name, _i_lchild)(i);                                \
       if (child >= size) break;                                              \
       size_t otherChild = M_C(name, _i_rchild)(i);                           \
       if (otherChild < size                                                  \
           && M_C(name, _i_cmp)(p, otherChild, child) < 0 ) {                 \
         child = otherChild;                                                  \
       }                                                                      \
       if (M_C(name, _i_cmp)(p, i, child) <= 0) break;                        \
       M_C(name, _array_swap_at) (p->array, i, child);                        \
       i = child;                                                             \
     }                                                                        \
     return true;                                                             \
   }                                                                          \
                                                                              \
   static inline void                                                         \
   M_C(name, _update)(prioqueue_t p, type const xold, type const xnew)        \
   {                                                                          \
     /* NOTE: xold can be the same pointer than xnew */                       \
     /* First pass: search for an item EQUAL to x */                          \
     size_t size = M_C(name, _array_size)(p->array);                          \
     size_t i = 0;                                                            \
     for(i = 0; i < size; i++) {                                              \
       if (M_CALL_EQUAL(oplist, *M_C(name, _array_cget)(p->array, i), xold))  \
         break;                                                               \
     }                                                                        \
     /* We shall have found the item */                                       \
     M_ASSERT (i < size);                                                     \
     /* Test if the position of the old data is further or nearer than the new */ \
     int cmp = M_CALL_CMP(oplist, *M_C(name, _array_cget)(p->array, i), xnew); \
     /* Set the found item to the new element */                              \
     M_C(name, _array_set_at) (p->array, i, xnew);                            \
     if (cmp < 0) {                                                           \
      /* Move back the updated element to its new position, further in the heap */ \
      while (true) {                                                          \
        size_t child = M_C(name, _i_lchild)(i);                               \
        if (child >= size) break;                                             \
        size_t otherChild = M_C(name, _i_rchild)(i);                          \
        if (otherChild < size                                                 \
            && M_C(name, _i_cmp)(p, otherChild, child) < 0 ) {                \
          child = otherChild;                                                 \
        }                                                                     \
        if (M_C(name, _i_cmp)(p, i, child) <= 0) break;                       \
        M_C(name, _array_swap_at) (p->array, i, child);                       \
        i = child;                                                            \
      }                                                                       \
     } else {                                                                 \
      /* Move back the updated element to its new position, nearest in the heap */ \
      while (i > 0) {                                                         \
        size_t parent = M_C(name, _i_parent)(i);                              \
        if (M_C(name, _i_cmp)(p, parent, i) <= 0) break;                      \
        M_C(name, _array_swap_at) (p->array, i, parent);                      \
        i = parent;                                                           \
      }                                                                       \
     }                                                                        \
   }                                                                          \
   , /* No EQUAL */ )                                                         \


// TODO: set all & remove all function

#endif
