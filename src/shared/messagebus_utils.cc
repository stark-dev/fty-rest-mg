/*  =========================================================================
    shared_messagebus_utils - class description

    Copyright (C) 2014 - 2020 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    shared_messagebus_utils -
@discuss
@end
*/

#include "fty_rest_classes.h"

//  Structure of our class

struct _shared_messagebus_utils_t {
    int filler;     //  Declare class properties here
};


//  --------------------------------------------------------------------------
//  Create a new shared_messagebus_utils

shared_messagebus_utils_t *
shared_messagebus_utils_new (void)
{
    shared_messagebus_utils_t *self = (shared_messagebus_utils_t *) zmalloc (sizeof (shared_messagebus_utils_t));
    assert (self);
    //  Initialize class properties here
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the shared_messagebus_utils

void
shared_messagebus_utils_destroy (shared_messagebus_utils_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        shared_messagebus_utils_t *self = *self_p;
        //  Free class properties here
        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

