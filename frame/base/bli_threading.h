/*

   BLIS    
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name of The University of Texas nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#ifndef BLIS_THREADING_H
#define BLIS_THREADING_H


typedef omp_lock_t lock_t;

struct thread_comm_s
{
    void*   sent_object;
    dim_t   n_threads;

    bool_t  barrier_sense;
    lock_t  barrier_lock;
    dim_t   barrier_threads_arrived;
};
typedef struct thread_comm_s thread_comm_t;

void    bli_setup_communicator( thread_comm_t* communicator, dim_t n_threads );
thread_comm_t*    bli_create_communicator( dim_t n_threads );

void*   bli_broadcast_structure( thread_comm_t* communicator, dim_t inside_id, void* to_send );

void    bli_barrier( thread_comm_t* communicator, dim_t thread_id );
void    bli_set_lock( lock_t* lock );
void    bli_unset_lock( lock_t* lock );
void    bli_init_lock( lock_t* lock );
void    bli_destroy_lock( lock_t* lock );

struct thrinfo_s
{
    thread_comm_t*      ocomm;       //The thread communicator for the other threads sharing the same work at this level
    dim_t               ocomm_id;    //Our thread id within that thread comm
    thread_comm_t*      icomm;       //The thread communicator for the other threads sharing the same work at this level
    dim_t               icomm_id;    //Our thread id within that thread comm

    dim_t               n_way;       //Number of distinct  used to parallelize the loop
    dim_t               work_id;     //What we're working on
};
typedef struct thrinfo_s thrinfo_t;

#define thread_ocomm( thread )          thread->ocomm
#define thread_icomm( thread )          (thread->icomm)

#define thread_id( thread )             thread->ocomm_id
#define thread_num_threads( thread )    thread->ocomm->n_threads

#define thread_work_id( thread )        thread->work_id
#define thread_n_way( thread )          thread->n_way
#define thread_am_ochief( thread )      (thread->ocomm_id == 0)
#define thread_am_ichief( thread )      (thread->icomm_id == 0)

#define thread_obroadcast( thread, ptr )       bli_broadcast_structure( thread->ocomm, thread->ocomm_id, ptr )
#define thread_ibroadcast( thread, ptr )       bli_broadcast_structure( thread->icomm, thread->icomm_id, ptr )
#define thread_obarrier( thread )              bli_barrier( thread->ocomm, thread->ocomm_id )
#define thread_ibarrier( thread )              bli_barrier( thread->icomm, thread->icomm_id )

void bli_get_range( void* thread, dim_t size, dim_t block_factor, dim_t* start, dim_t* end );
thrinfo_t* bli_create_thread_info( thread_comm_t* ocomm, dim_t ocomm_id, thread_comm_t* icomm, dim_t icomm_id,
                             dim_t n_way, dim_t work_id );
void bli_setup_thread_info( thrinfo_t* thread, thread_comm_t* ocomm, dim_t ocomm_id, thread_comm_t* icomm, dim_t icomm_id,
                             dim_t n_way, dim_t work_id );
//void bli_setup_single_threaded_info( thrinfo_t* thr, thread_comm_t* comm );
//thrinfo_t* bli_create_thread_info( dim_t* n_threads_each_level, dim_t n_levels );

#include "bli_packm_threading.h"
#include "bli_gemm_threading.h"
#include "bli_herk_threading.h"

typedef void (*level3_int_t) ( obj_t* alpha, obj_t* a, obj_t* b, obj_t* beta, obj_t* c, void* cntl, void* thread );
void bli_level3_thread_decorator( dim_t num_threads, 
                                  level3_int_t* func, 
                                  obj_t* alpha, 
                                  obj_t* a,  
                                  obj_t* b,  
                                  obj_t* beta, 
                                  obj_t* c,  
                                  void* cntl, 
                                  void** thread );

#endif
