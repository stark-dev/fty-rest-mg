/*  =========================================================================
    db/inout/importcsv - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef DB/INOUT/IMPORTCSV_H_INCLUDED
#define DB/INOUT/IMPORTCSV_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new db/inout/importcsv
ETN_AUTH_REST_PRIVATE db/inout/importcsv_t *
    db/inout/importcsv_new (void);

//  Destroy the db/inout/importcsv
ETN_AUTH_REST_PRIVATE void
    db/inout/importcsv_destroy (db/inout/importcsv_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
