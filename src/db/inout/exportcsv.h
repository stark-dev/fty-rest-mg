/*  =========================================================================
    db/inout/exportcsv - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef DB/INOUT/EXPORTCSV_H_INCLUDED
#define DB/INOUT/EXPORTCSV_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new db/inout/exportcsv
ETN_AUTH_REST_PRIVATE db/inout/exportcsv_t *
    db/inout/exportcsv_new (void);

//  Destroy the db/inout/exportcsv
ETN_AUTH_REST_PRIVATE void
    db/inout/exportcsv_destroy (db/inout/exportcsv_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
