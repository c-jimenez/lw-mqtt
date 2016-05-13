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

#ifndef OUTPUT_STREAM_H
#define OUTPUT_STREAM_H

#include "stdheaders.h"


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/** \brief Pre-declaration of output_stream_t structure which represents an output stream */
typedef struct _output_stream_t output_stream_t;

/** \brief Output stream reset function */
typedef bool(*fp_output_stream_reset_t)(output_stream_t* const);

/** \brief Output stream writer function */
typedef bool (*fp_output_stream_writer_t)(output_stream_t* const, const void*, const size_t);


/** \brief Represents an output stream */
struct _output_stream_t
{
    /** \brief Reset function */
    fp_output_stream_reset_t reset;

    /** \brief Writer function */
    fp_output_stream_writer_t writer;

    /** \brief Number of bytes written */
    size_t written;

    /** \brief Stream size in bytes (UINT32_MAX = infinite size) */
    size_t size;

    /** \brief Parameter => to be used for implementation dependant data */
    void* param;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif /* OUTPUT_STREAM_H */
