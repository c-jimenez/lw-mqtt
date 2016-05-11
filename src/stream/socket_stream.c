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
#include "socket_stream.h"


/** \brief Input stream reset function */
static bool socket_stream_reset_input(input_stream_t* const stream, const size_t new_size);

/** \brief Input stream reader function */
static bool socket_stream_reader(input_stream_t* const stream, void* data, const size_t size);

/** \brief Output stream reset function */
static bool socket_stream_reset_output(output_stream_t* const stream);

/** \brief Output stream writer function */
static bool socket_stream_writer(output_stream_t* const stream, const void* data, const size_t size);



/** \brief Initialize an input stream from a socket */
bool socket_stream_input_from_socket(input_stream_t* const stream, mqtt_socket_t* const mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (mqtt_socket != NULL))
    {
        /* Init input stream */
        stream->reset = socket_stream_reset_input;
        stream->reader = socket_stream_reader;
        stream->size = UINT32_MAX;
        stream->read = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;
        stream->param = mqtt_socket;

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

/** \brief Initialize an output stream from a socket */
bool socket_stream_output_from_socket(output_stream_t* const stream, mqtt_socket_t* const mqtt_socket)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (mqtt_socket != NULL))
    {
        /* Init output stream */
        stream->reset = socket_stream_reset_output;
        stream->writer = socket_stream_writer;
        stream->size = UINT32_MAX;
        stream->written = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;
        stream->param = mqtt_socket;

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
static bool socket_stream_reset_input(input_stream_t* const stream, const size_t new_size)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (stream->param != NULL))
    {

        /* Reset stream */
        stream->read = 0u;
        stream->last_error = MQTT_ERR_SUCCESS;
        (void)new_size;

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
static bool socket_stream_reader(input_stream_t* const stream, void* data, const size_t size)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Read data from the socket */
    size_t received = 0u;
    size_t left = size;
    void* data_ptr = data;
    mqtt_socket_t* const mqtt_socket = (mqtt_socket_t*)stream->param;
    do
    {
        ret = mqtt_socket_receive(mqtt_socket, data_ptr, left, &received);
        if (ret)
        {
            left -= received;
            stream->read += received;
            data_ptr = (void*)((intptr_t)data_ptr + (intptr_t)received);
        }
        else
        {
            stream->last_error = mqtt_socket->last_error;
        }
    }
    while (ret && (left != 0u));

    return ret;
}

/** \brief Output stream reset function */
static bool socket_stream_reset_output(output_stream_t* const stream)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (stream->param != NULL))
    {

        /* Reset stream */
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
static bool socket_stream_writer(output_stream_t* const stream, const void* data, const size_t size)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Send data using the socket */
    size_t sent = 0u;
    size_t left = size;
    const void* data_ptr = data;
    mqtt_socket_t* const mqtt_socket = (mqtt_socket_t*)stream->param;
    do
    {
        ret = mqtt_socket_send(mqtt_socket, data_ptr, left, &sent);
        if (ret)
        {
            left -= sent;
            stream->written += sent;
            data_ptr = (void*)((intptr_t)data_ptr + (intptr_t)sent);
        }
        else
        {
            stream->last_error = mqtt_socket->last_error;
        }
    } while (ret && (left != 0u));

    return ret;
}
