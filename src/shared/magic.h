/*  =========================================================================
    shared/magic - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef SHARED/MAGIC_H_INCLUDED
#define SHARED/MAGIC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new shared/magic
FTY_REST_PRIVATE shared/magic_t *
    shared/magic_new (void);

//  Destroy the shared/magic
FTY_REST_PRIVATE void
    shared/magic_destroy (shared/magic_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
