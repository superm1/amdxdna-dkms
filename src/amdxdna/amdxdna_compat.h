/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Compatibility shims for older kernel versions
 * Copyright (C) 2025, Advanced Micro Devices, Inc.
 */

#ifndef _AMDXDNA_COMPAT_H_
#define _AMDXDNA_COMPAT_H_

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <drm/drm_gem.h>
#include <drm/gpu_scheduler.h>

/*
 * Bit manipulation macros added in newer kernels
 */
#ifndef BIT_U64
#define BIT_U64(nr) BIT_ULL(nr)
#endif

/*
 * Type-safe allocation helpers added in kernel 6.15+
 * These provide compile-time type checking for allocations
 */

#ifndef kzalloc_obj
/**
 * kzalloc_obj - Allocate memory for an object and zero it
 * @obj: Dereferenced pointer to get the size from (e.g., *ptr)
 *
 * Allocates sizeof(obj) bytes and zeros the memory.
 * Returns a pointer to the allocated memory.
 */
#define kzalloc_obj(obj) \
	((typeof(&(obj)))kzalloc(sizeof(obj), GFP_KERNEL))
#endif

#ifndef kzalloc_flex
/**
 * kzalloc_flex - Allocate memory for object with flexible array member
 * @obj: Dereferenced pointer to object
 * @member: Name of the flexible array member
 * @count: Number of elements in the flexible array
 *
 * Allocates sizeof(obj) + sizeof(obj.member[0]) * count bytes.
 */
#define kzalloc_flex(obj, member, count) \
	((typeof(&(obj)))kzalloc(struct_size(&(obj), member, (count)), GFP_KERNEL))
#endif

#ifndef kvzalloc_objs
/**
 * kvzalloc_objs - Allocate virtually contiguous memory for objects and zero
 * @obj: Dereferenced pointer to object
 * @n: Number of objects
 *
 * Allocates sizeof(obj) * n bytes using kvzalloc.
 */
#define kvzalloc_objs(obj, n) \
	((typeof(&(obj)))kvzalloc(size_mul(sizeof(obj), (n)), GFP_KERNEL))
#endif

#ifndef kvmalloc_objs
/**
 * kvmalloc_objs - Allocate virtually contiguous memory for objects
 * @obj: Dereferenced pointer to object
 * @n: Number of objects
 *
 * Allocates sizeof(obj) * n bytes using kvmalloc.
 */
#define kvmalloc_objs(obj, n) \
	((typeof(&(obj)))kvmalloc(size_mul(sizeof(obj), (n)), GFP_KERNEL))
#endif

#endif /* _AMDXDNA_COMPAT_H_ */
