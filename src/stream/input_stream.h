/*
Copyright(c) 2016 Cedric Jimenez

This file is part of lw-mqtt.

lw-mqtt is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lw-mqtt is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with lw-mqtt.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INPUT_STREAM_H
#define INPUT_STREAM_H

#include "stdheaders.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Pre-declaration of input_stream_t structure which represents an input stream */
typedef struct _input_stream_t input_stream_t;

/** \brief Input stream reset function */
typedef bool(*fp_input_stream_reset_t)(input_stream_t* const, const size_t new_size);

/** \brief Output stream reader function */
typedef bool(*fp_input_stream_reader_t)(input_stream_t* const, void*, const size_t);

/** \brief Represents an output stream */
struct _input_stream_t
{
    /** \brief Reset function */
    fp_input_stream_reset_t reset;

    /** \brief Writer function */
    fp_input_stream_reader_t reader;

    /** \brief Number of bytes read */
    size_t read;

    /** \brief Stream size in bytes (UINT32_MAX = infinite size) */
    size_t size;

    /** \brief Parameter => to be used for implementation dependant data */
    void* param;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* INPUT_STREAM_H */
