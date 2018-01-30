/*  =========================================================================
    db/topology2 - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef DB/TOPOLOGY2_H_INCLUDED
#define DB/TOPOLOGY2_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new db/topology2
ETN_AUTH_REST_EXPORT db/topology2_t *
    db/topology2_new (void);

//  Destroy the db/topology2
ETN_AUTH_REST_EXPORT void
    db/topology2_destroy (db/topology2_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
