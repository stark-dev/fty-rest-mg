/*  =========================================================================
    persist/dbhelpers - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef PERSIST/DBHELPERS_H_INCLUDED
#define PERSIST/DBHELPERS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new persist/dbhelpers
ETN_AUTH_REST_PRIVATE persist/dbhelpers_t *
    persist/dbhelpers_new (void);

//  Destroy the persist/dbhelpers
ETN_AUTH_REST_PRIVATE void
    persist/dbhelpers_destroy (persist/dbhelpers_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
