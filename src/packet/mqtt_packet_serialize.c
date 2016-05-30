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
#include "mqtt_packet_serialize.h"

/** \brief Compute the length of the CONNECT packet */
static uint32_t mqtt_packet_serialize_compute_connect_length(const mqtt_const_string_t* const client_id,
                                                             const mqtt_const_credentials_t* const credentials, const mqtt_const_will_t* const will);

/** \brief Serialize a response packet with a packet id only */
static bool mqtt_packet_serialize_packet_id_only(output_stream_t* const stream, const mqtt_control_packet_type_t type, const uint16_t packet_id);

/** \brief Serialize a 0 length packet */
static bool mqtt_packet_serialize_zero_length(output_stream_t* const stream, const mqtt_control_packet_type_t type);

/** \brief Serialize a variable length field */
static bool mqtt_packet_serialize_lenght(output_stream_t* const stream, const uint32_t length);

/** \brief Serialize a string */
static bool mqtt_packet_serialize_string(output_stream_t* const stream, const mqtt_const_string_t* const mqtt_string);



/** \brief Serialize a CONNECT packet */
bool mqtt_packet_serialize_connect(output_stream_t* const stream, const mqtt_const_string_t* const client_id, 
                                   const mqtt_const_credentials_t* const credentials, const mqtt_const_will_t* const will, 
                                   const bool clean_session, const uint16_t keepalive)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (client_id != NULL) &&
        (client_id->str != NULL) &&
        (client_id->size != 0u) &&
        (!((will != NULL) && ((will->topic.str == NULL) || (will->message.str == NULL) || (will->qos > MQTT_MAX_QOS_LEVEL)))) &&
        (!((credentials != NULL) && ((credentials->username.str == NULL) || (credentials->username.size == 0u)))))
    {
        const uint8_t packet_type = ((uint8_t)(MQTT_PKT_CONNECT) << 4u);
        uint8_t protocol_name[MQTT_PROTOCOL_NAME_SIZE + 1u] = MQTT_PROTOCOL_NAME;
        const uint32_t remaining_length = mqtt_packet_serialize_compute_connect_length(client_id, credentials, will);
        const uint16_t keepalive_be = MQTT_BIG_ENDIAN_UINT16(keepalive);
        uint8_t connect_flags = 0;

        /* Packet type */
        ret = stream->writer(stream, &packet_type, sizeof(packet_type));

        /* Remaining length */
        if (ret)
        {
            ret = mqtt_packet_serialize_lenght(stream, remaining_length);
        }

        /* Protocol name */
        if (ret)
        {
            protocol_name[MQTT_PROTOCOL_NAME_SIZE] = MQTT_PROTOCOL_LEVEL;
            ret = stream->writer(stream, protocol_name, sizeof(protocol_name));
        }

        /* Connect flags */
        if (credentials != NULL)
        {
            if (credentials->username.str != NULL)
            {
                connect_flags |= MQTT_CONNECT_FLAG_USERNAME;
                if (credentials->password.str != NULL)
                {
                    connect_flags |= MQTT_CONNECT_FLAG_PASSWORD;
                }
            }
            if (will != NULL)
            {
                connect_flags |= (MQTT_CONNECT_FLAG_WILL_FLAG | (will->qos << MQTT_CONNECT_FLAG_WILL_QOS_POSITION));
                if (will->retain)
                {
                    connect_flags |= MQTT_CONNECT_FLAG_WILL_RETAIN;
                }
            }
        }
        if (clean_session)
        {
            connect_flags |= MQTT_CONNECT_FLAG_CLEAN_SESSION;
        }
        if (ret)
        {
            ret = stream->writer(stream, &connect_flags, sizeof(connect_flags));
        }

        /* Keepalive */
        if (ret)
        {
            ret = stream->writer(stream, &keepalive_be, sizeof(keepalive_be));
        }

        /* Client Id */
        if (ret)
        {
            ret = mqtt_packet_serialize_string(stream, client_id);
        }

        /* Will */
        if (ret && (will != NULL))
        {
            /* Will topic */
            ret = mqtt_packet_serialize_string(stream, &will->topic);

            /* Will message */
            if (ret)
            {
                ret = mqtt_packet_serialize_string(stream, &will->message);
            }
        }

        /* Credentials */
        if (ret && (credentials != NULL))
        {
            /* Username */
            ret = mqtt_packet_serialize_string(stream, &credentials->username);

            /* Password */
            if (ret && (credentials->password.str != NULL))
            {
                ret = mqtt_packet_serialize_string(stream, &credentials->password);
            }
        }
    }
    else
    {
        /* Error */
        if (stream != NULL)
        {
            mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
        }
    }

    return ret;
}

/** \brief Serialize a CONNACK packet */
bool mqtt_packet_serialize_connack(output_stream_t* const stream, const bool session_present, const mqtt_connack_retcode_t retcode)
{
    bool ret = false;

    /* Check params */
    if (stream != NULL)
    {
        uint8_t payload[2u];
        const uint8_t packet_header[] = { 
                                            ((uint8_t)(MQTT_PKT_CONNACK) << 4u) ,
                                            MQTT_CONNACK_PACKET_SIZE
                                        };

        /* Packet header */
        ret = stream->writer(stream, &packet_header, sizeof(packet_header));

        /* Payload */
        if (session_present)
        {
            payload[0u] = 1u;
        }
        else
        {
            payload[0u] = 0u;
        }
        payload[1u] = (uint8_t)retcode;
        if (ret)
        {
            ret = stream->writer(stream, &payload, sizeof(payload));
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize a PUBLISH packet */
bool mqtt_packet_serialize_publish(output_stream_t* const stream, const mqtt_const_string_t* const topic, const void* data,
                                   const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate,
                                   const uint16_t packet_id)
{
    bool ret = false;

    /* Check params */
    if ( (stream != NULL) &&
         (topic != NULL) &&
         (topic->str != NULL) &&
         (!((data == NULL) && (length != 0u))) &&
         (qos <= MQTT_CFG_MAX_QOS_LEVEL) )
    {
        uint32_t remaining_length = 0u;
        uint8_t packet_type = ((uint8_t)(MQTT_PKT_PUBLISH) << 4u) | (uint8_t)(qos << MQTT_PUBLISH_FLAG_QOS_POSITION);

        /* Packet type */
        if (retain)
        {
            packet_type |= MQTT_PUBLISH_FLAG_RETAIN;
        }
        if (duplicate)
        {
            packet_type |= MQTT_PUBLISH_FLAG_DUP;
        }
        ret = stream->writer(stream, &packet_type, sizeof(packet_type));

        /* Remaining length */
        if (ret)
        {
            remaining_length = topic->size + 2u + length;
            if (qos > 0u)
            {
                remaining_length += sizeof(packet_id);
            }
            ret = mqtt_packet_serialize_lenght(stream, remaining_length);
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_serialize_string(stream, topic);
        }
        
        /* Packet id */
        if ( (ret) && (qos > 0u) )
        {
            const uint16_t packet_id_be = MQTT_BIG_ENDIAN_UINT16(packet_id);
            ret = stream->writer(stream, &packet_id_be, sizeof(packet_id_be));
        }

        /* Data */
        if ((ret) && (length > 0u))
        {
            ret = stream->writer(stream, data, length);
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize a PUBACK packet */
bool mqtt_packet_serialize_puback(output_stream_t* const stream, const uint16_t packet_id)
{
    const bool ret = mqtt_packet_serialize_packet_id_only(stream, MQTT_PKT_PUBACK, packet_id);
    return ret;
}

/** \brief Serialize a PUBREC packet */
bool mqtt_packet_serialize_pubrec(output_stream_t* const stream, const uint16_t packet_id)
{
    const bool ret = mqtt_packet_serialize_packet_id_only(stream, MQTT_PKT_PUBREC, packet_id);
    return ret;
}

/** \brief Serialize a PUBREL packet */
bool mqtt_packet_serialize_pubrel(output_stream_t* const stream, const uint16_t packet_id)
{
    const bool ret = mqtt_packet_serialize_packet_id_only(stream, MQTT_PKT_PUBREL, packet_id);
    return ret;
}

/** \brief Serialize a PUBCOMP packet */
bool mqtt_packet_serialize_pubcomp(output_stream_t* const stream, const uint16_t packet_id)
{
    const bool ret = mqtt_packet_serialize_packet_id_only(stream, MQTT_PKT_PUBCOMP, packet_id);
    return ret;
}

/** \brief Serialize a SUBSCRIBE packet */
bool mqtt_packet_serialize_subscribe(output_stream_t* const stream, const mqtt_const_string_t* const topic, const uint8_t qos,
                                     const uint16_t packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (topic != NULL) &&
        (topic->str != NULL) &&
        (qos <= MQTT_CFG_MAX_QOS_LEVEL))
    {
        uint32_t remaining_length = MQTT_SUBSCRIBE_PACKET_MIN_SIZE;
        uint8_t packet_type = ((uint8_t)(MQTT_PKT_SUBSCRIBE) << 4u) | MQTT_SUB_UNSUBSCRIBE_HEADER_VALUE;

        /* Packet type */
        ret = stream->writer(stream, &packet_type, sizeof(packet_type));

        /* Remaining length */
        if (ret)
        {
            remaining_length += topic->size + 2u;
            ret = mqtt_packet_serialize_lenght(stream, remaining_length);
        }

        /* Packet id */
        if (ret)
        {
            const uint16_t packet_id_be = MQTT_BIG_ENDIAN_UINT16(packet_id);
            ret = stream->writer(stream, &packet_id_be, sizeof(packet_id_be));
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_serialize_string(stream, topic);
        }

        /* QoS */
        if (ret)
        {
            ret = stream->writer(stream, &qos, sizeof(qos));
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize a SUBACK packet */
bool mqtt_packet_serialize_suback(output_stream_t* const stream, const uint8_t qos, const uint16_t packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        ((qos <= MQTT_CFG_MAX_QOS_LEVEL) || (qos == MQTT_FAILURE_QOS)) )
    {
        const uint8_t packet_header[] = {
            ((uint8_t)(MQTT_PKT_SUBACK) << 4u) ,
            MQTT_SUBACK_PACKET_SIZE
        };

        /* Packet header */
        ret = stream->writer(stream, &packet_header, sizeof(packet_header));

        /* Packet id */
        if (ret)
        {
            const uint16_t packet_id_be = MQTT_BIG_ENDIAN_UINT16(packet_id);
            ret = stream->writer(stream, &packet_id_be, sizeof(packet_id_be));
        }

        /* QoS */
        if (ret)
        {
            ret = stream->writer(stream, &qos, sizeof(qos));
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize an UNSUBSCRIBE packet */
bool mqtt_packet_serialize_unsubscribe(output_stream_t* const stream, const mqtt_const_string_t* const topic, const uint16_t packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (topic != NULL) &&
        (topic->str != NULL))
    {
        uint32_t remaining_length = MQTT_UNSUBSCRIBE_PACKET_MIN_SIZE;
        uint8_t packet_type = ((uint8_t)(MQTT_PKT_UNSUBSCRIBE) << 4u) | MQTT_SUB_UNSUBSCRIBE_HEADER_VALUE;

        /* Packet type */
        ret = stream->writer(stream, &packet_type, sizeof(packet_type));

        /* Remaining length */
        if (ret)
        {
            remaining_length += topic->size + 2u;
            ret = mqtt_packet_serialize_lenght(stream, remaining_length);
        }

        /* Packet id */
        if (ret)
        {
            const uint16_t packet_id_be = MQTT_BIG_ENDIAN_UINT16(packet_id);
            ret = stream->writer(stream, &packet_id_be, sizeof(packet_id_be));
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_serialize_string(stream, topic);
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize an UNSUBACK packet */
bool mqtt_packet_serialize_unsuback(output_stream_t* const stream, const uint16_t packet_id)
{
    const bool ret = mqtt_packet_serialize_packet_id_only(stream, MQTT_PKT_UNSUBACK, packet_id);
    return ret;
}

/** \brief Serialize a PINGREQ packet */
bool mqtt_packet_serialize_pingreq(output_stream_t* const stream)
{
    const bool ret = mqtt_packet_serialize_zero_length(stream, MQTT_PKT_PINGREQ);
    return ret;
}

/** \brief Serialize a PINGRESP packet */
bool mqtt_packet_serialize_pingresp(output_stream_t* const stream)
{
    const bool ret = mqtt_packet_serialize_zero_length(stream, MQTT_PKT_PINGRESP);
    return ret;
}

/** \brief Serialize a DISCONNECT packet */
bool mqtt_packet_serialize_disconnect(output_stream_t* const stream)
{
    const bool ret = mqtt_packet_serialize_zero_length(stream, MQTT_PKT_DISCONNECT);
    return ret;
}


/** \brief Compute the length of the CONNECT packet */
static uint32_t mqtt_packet_serialize_compute_connect_length(const mqtt_const_string_t* const client_id,
                                                             const mqtt_const_credentials_t* const credentials, const mqtt_const_will_t* const will)
{
    uint32_t length = MQTT_CONNECT_PACKET_MIN_SIZE;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    length += MQTT_MIN_ENCODED_STRING_SIZE + client_id->size;
    if (will != NULL)
    {
        length += MQTT_MIN_ENCODED_STRING_SIZE + will->topic.size;
        length += MQTT_MIN_ENCODED_STRING_SIZE + will->message.size;
    }
    if (credentials != NULL)
    {
        length += MQTT_MIN_ENCODED_STRING_SIZE + credentials->username.size;
        if (credentials->password.str != NULL)
        {
            length += MQTT_MIN_ENCODED_STRING_SIZE + credentials->password.size;
        }
    }

    return length;
}

/** \brief Serialize a response packet with a packet id only */
static bool mqtt_packet_serialize_packet_id_only(output_stream_t* const stream, const mqtt_control_packet_type_t type, const uint16_t packet_id)
{
    bool ret = false;

    /* Check params */
    if (stream != NULL)
    {
        uint8_t packet_header[2u] = {
            0u,
            MQTT_PACKET_ID_ONLY_PACKET_SIZE
        };

        /* Packet header */
        packet_header[0u] = ((uint8_t)(type) << 4u);
        ret = stream->writer(stream, &packet_header, sizeof(packet_header));

        /* Packet identifier */
        if (ret)
        {
            const uint16_t packet_id_be = MQTT_BIG_ENDIAN_UINT16(packet_id);
            ret = stream->writer(stream, &packet_id_be, sizeof(packet_id_be));
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize a 0 length packet */
static bool mqtt_packet_serialize_zero_length(output_stream_t* const stream, const mqtt_control_packet_type_t type)
{
    bool ret = false;

    /* Check params */
    if (stream != NULL)
    {
        uint8_t packet_header[2u] = {0u};
        packet_header[0u] = ((uint8_t)(type) << 4u);

        /* Packet header */
        ret = stream->writer(stream, packet_header, sizeof(packet_header));
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Serialize a variable length field */
static bool mqtt_packet_serialize_lenght(output_stream_t* const stream, const uint32_t length)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Encode the length */
    uint8_t index = 0u;
    uint32_t len = length;
    uint8_t current_byte;
    uint8_t encoded_length[4u];
    do
    {
        current_byte = (len & 0xFFu);
        if (current_byte > 0x7Fu)
        {
            encoded_length[index] = current_byte & 0x7F;
        }
        else
        {
            encoded_length[index] = current_byte;
        }
        index++;
        len = len >> 7u;
    }
    while(len > 0u);

    /* Write to output stream */
    ret = stream->writer(stream, encoded_length, index);

    return ret;
}

/** \brief Serialize a string */
static bool mqtt_packet_serialize_string(output_stream_t* const stream, const mqtt_const_string_t* const mqtt_string)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Length */
    const uint16_t len_be = MQTT_BIG_ENDIAN_UINT16(mqtt_string->size);
    ret = stream->writer(stream, &len_be, sizeof(len_be));
    
    /* String */
    if( ret )
    {
        ret = stream->writer(stream, mqtt_string->str, mqtt_string->size);
    }

    return ret;
}
