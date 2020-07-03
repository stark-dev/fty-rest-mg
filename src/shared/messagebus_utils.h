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

#ifndef SHARED_MESSAGEBUS_UTILS_H_INCLUDED
#define SHARED_MESSAGEBUS_UTILS_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new shared_messagebus_utils
FTY_REST_PRIVATE shared_messagebus_utils_t *
    shared_messagebus_utils_new (void);

//  Destroy the shared_messagebus_utils
FTY_REST_PRIVATE void
    shared_messagebus_utils_destroy (shared_messagebus_utils_t **self_p);


//  @end

#ifdef __cplusplus
}
#endif

#endif
