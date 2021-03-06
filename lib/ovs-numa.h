/*
 * Copyright (c) 2014 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OVS_NUMA_H
#define OVS_NUMA_H 1

#include <limits.h>
#include <stdbool.h>

#include "compiler.h"

#define OVS_CORE_UNSPEC INT_MAX
#define OVS_SOCKET_UNSPEC INT_MAX

#ifdef __linux__

void ovs_numa_init(void);
bool ovs_numa_cpu_socket_id_is_valid(int sid);
bool ovs_numa_cpu_core_id_is_valid(int cid);
int ovs_numa_get_n_sockets(void);
int ovs_numa_get_n_cores(void);
int ovs_numa_get_n_cores_on_socket(int socket_id);
int ovs_numa_get_n_unpinned_cores_on_socket(int socket_id);
bool ovs_numa_try_pin_core_specific(int core_id);
int ovs_numa_get_unpinned_core_any(void);
int ovs_numa_get_unpinned_core_on_socket(int socket_id);
void ovs_numa_unpin_core(int core_id);

#else

static inline void
ovs_numa_init(void)
{
    /* Nothing */
}

static inline bool
ovs_numa_cpu_socket_id_is_valid(int sid OVS_UNUSED)
{
    return false;
}

static inline bool
ovs_numa_cpu_core_id_is_valid(int cid OVS_UNUSED)
{
    return false;
}

static inline int
ovs_numa_get_n_sockets(void)
{
    return OVS_SOCKET_UNSPEC;
}

static inline int
ovs_numa_get_n_cores(void)
{
    return OVS_CORE_UNSPEC;
}

static inline int
ovs_numa_get_n_cores_on_socket(int socket_id OVS_UNUSED)
{
    return OVS_CORE_UNSPEC;
}

static inline int
ovs_numa_get_n_unpinned_cores_on_socket(int socket_id OVS_UNUSED)
{
    return OVS_CORE_UNSPEC;
}

static inline bool
ovs_numa_try_pin_core_specific(int core_id OVS_UNUSED)
{
    return false;
}

static inline int
ovs_numa_get_unpinned_core_any(void)
{
    return OVS_CORE_UNSPEC;
}

static inline int
ovs_numa_get_unpinned_core_on_socket(int socket_id OVS_UNUSED)
{
    return OVS_CORE_UNSPEC;
}

static inline void
ovs_numa_unpin_core(int core_id OVS_UNUSED)
{
    /* Nothing */
}

#endif /* __linux__ */
#endif /* ovs-thead.h */
