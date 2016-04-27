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

#include "mqtt_error.h"
#include "buffer_stream.h"


/** \brief Input stream reset function */
static bool buffer_stream_reset_input(input_stream_t* const stream, const size_t new_size);

/** \brief Input stream reader function */
static bool buffer_stream_reader(input_stream_t* const stream, void* data, const size_t size);

/** \brief Output stream reset function */
static bool buffer_stream_reset_output(output_stream_t* const stream);

/** \brief Output stream writer function */
static bool buffer_stream_writer(output_stream_t* const stream, const void* data, const size_t size);




/** \brief Initialize an input stream from a buffer */
bool buffer_stream_input_from_buffer(input_stream_t* const stream, uint8_t buffer[], const size_t size)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (buffer != NULL))
    {
        /* Init input stream */
        stream->reset = buffer_stream_reset_input;
        stream->reader = buffer_stream_reader;
        stream->size = size;
        stream->read = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;
        stream->param = buffer;

        ret = true;
    }
    else
    {
        /* Error */
        if (stream != NULL)
        {
            stream->last_error = MQTT_ERR_INVALID_PARAM;
        }
    }

    return ret;
}

/** \brief Initialize an output stream from a buffer */
bool buffer_stream_output_from_buffer(output_stream_t* const stream, uint8_t buffer[], const size_t size)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (buffer != NULL))
    {
        /* Init output stream */
        stream->reset = buffer_stream_reset_output;
        stream->writer = buffer_stream_writer;
        stream->size = size;
        stream->written = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;
        stream->param = buffer;

        ret = true;
    }
    else
    {
        /* Error */
        if (stream != NULL)
        {
            stream->last_error = MQTT_ERR_INVALID_PARAM;
        }
    }

    return ret;
}


/** \brief Input stream reset function */
static bool buffer_stream_reset_input(input_stream_t* const stream, const size_t new_size)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (stream->param != NULL))
    {

        /* Reset stream */
        stream->param = (void*)((intptr_t)stream->param - (intptr_t)stream->read);
        stream->size = new_size;
        stream->read = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;

        ret = true;
    }
    else
    {
        /* Error */
        if (stream != NULL)
        {
            stream->last_error = MQTT_ERR_INVALID_PARAM;
        }
    }

    return ret;
}

/** \brief Input stream reader function */
static bool buffer_stream_reader(input_stream_t* const stream, void* data, const size_t size)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Check stream initialization */
    if (stream->param != NULL)
    {
        /* Check size left into stream */
        if (stream->read + size <= stream->size)
        {
            /* Copy data into the buffer */
            memcpy(data, stream->param, size);
            stream->read += size;
            stream->param = (void*)((intptr_t)stream->param + (intptr_t)size);
            ret = true;
        }
        else
        {
            stream->last_error = MQTT_ERR_INPUT_STREAM_EMPTY;
        }
    }
    else
    {
        /* Error */
        stream->last_error = MQTT_ERR_INVALID_PARAM;
    }

    return ret;
}

/** \brief Output stream reset function */
static bool buffer_stream_reset_output(output_stream_t* const stream)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (stream->param != NULL))
    {

        /* Reset stream */
        stream->param = (void*)((intptr_t)stream->param - (intptr_t)stream->written);
        stream->written = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;

        ret = true;
    }
    else
    {
        /* Error */
        if (stream != NULL)
        {
            stream->last_error = MQTT_ERR_INVALID_PARAM;
        }
    }

    return ret;
}

/** \brief Output stream writer function */
static bool buffer_stream_writer(output_stream_t* const stream, const void* data, const size_t size)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function 
       is meant to be called from the inside of the library where the
       parameters are already checked.
    */

    /* Check stream initialization */
    if (stream->param != NULL)
    {
        /* Check size left into buffer */
        if (stream->written + size <= stream->size)
        {
            /* Copy data into the stream buffer */
            memcpy(stream->param, data, size);
            stream->written += size;
            stream->param = (void*)((intptr_t)stream->param + (intptr_t)size);
            ret = true;
        }
        else
        {
            stream->last_error = MQTT_ERR_OUTPUT_STREAM_FULL;
        }
    }
    else
    {
        /* Error */
        stream->last_error = MQTT_ERR_INVALID_PARAM;
    }

    return ret;
}
