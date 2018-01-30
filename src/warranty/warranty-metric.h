/*  =========================================================================
    warranty/warranty-metric - class description

    Copyright (c) the Authors
    =========================================================================
*/

#ifndef WARRANTY/WARRANTY-METRIC_H_INCLUDED
#define WARRANTY/WARRANTY-METRIC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new warranty/warranty-metric
ETN_AUTH_REST_PRIVATE warranty/warranty-metric_t *
    warranty/warranty-metric_new (void);

//  Destroy the warranty/warranty-metric
ETN_AUTH_REST_PRIVATE void
    warranty/warranty-metric_destroy (warranty/warranty-metric_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
