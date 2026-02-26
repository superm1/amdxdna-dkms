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
 * GPU scheduler compatibility
 * Kernel 6.14 has older drm_sched_init/drm_sched_job_init APIs
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 15, 0)

#ifndef DRM_GPU_SCHED_STAT_RESET
#define DRM_GPU_SCHED_STAT_RESET DRM_GPU_SCHED_STAT_NONE
#endif

/* In 6.14, drm_sched_init_args doesn't exist, use individual parameters */
struct drm_sched_init_args {
	const struct drm_sched_backend_ops *ops;
	u32 num_rqs;
	u32 credit_limit;
	long timeout;
	const char *name;
	struct device *dev;
};

/* Wrapper for drm_sched_init to translate from args struct to individual params */
static inline int
amdxdna_drm_sched_init(struct drm_gpu_scheduler *sched,
		       const struct drm_sched_init_args *args)
{
	return drm_sched_init(sched, args->ops, NULL, args->num_rqs,
			      args->credit_limit, 0, args->timeout, NULL,
			      NULL, args->name, args->dev);
}
#define drm_sched_init(sched, args) amdxdna_drm_sched_init(sched, args)

/* Wrapper for drm_sched_job_init - 6.14 takes 4 params, newer takes 5 */
static inline int
amdxdna_drm_sched_job_init(struct drm_sched_job *job,
			   struct drm_sched_entity *entity,
			   u32 credits, void *owner, u64 client_id)
{
	/* 6.14 version doesn't have client_id parameter */
	return drm_sched_job_init(job, entity, credits, owner);
}
#define drm_sched_job_init(job, entity, credits, owner, client_id) \
	amdxdna_drm_sched_job_init(job, entity, credits, owner, client_id)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(6, 15, 0) */

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

/*
 * DRM GEM vmap/vunmap helpers
 * In kernel 6.14, these are named drm_gem_shmem_vmap/vunmap for shmem objects
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 15, 0)
#include <drm/drm_gem_shmem_helper.h>

static inline int drm_gem_vmap(struct drm_gem_object *obj,
			       struct iosys_map *map)
{
	int ret;

	/* Try shmem vmap first - most common case */
	if (obj->funcs && obj->funcs->vmap) {
		ret = obj->funcs->vmap(obj, map);
	} else {
		/* Fallback for shmem objects */
		struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);
		ret = drm_gem_shmem_vmap(shmem, map);
	}

	return ret;
}

static inline void drm_gem_vunmap(struct drm_gem_object *obj,
				  struct iosys_map *map)
{
	if (obj->funcs && obj->funcs->vunmap) {
		obj->funcs->vunmap(obj, map);
	} else {
		/* Fallback for shmem objects */
		struct drm_gem_shmem_object *shmem = to_drm_gem_shmem_obj(obj);
		drm_gem_shmem_vunmap(shmem, map);
	}
}
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(6, 15, 0) */

#endif /* _AMDXDNA_COMPAT_H_ */
