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
#include "mqtt_packet_deserialize.h"

/** \brief Deserialize the packet type */
static bool mqtt_packet_deserialize_packet_type(input_stream_t* const stream, mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags);

/** \brief Deserialize a variable length field */
static bool mqtt_packet_deserialize_lenght(input_stream_t* const stream, uint32_t* const length);

/** \brief Deserialize a string */
static bool mqtt_packet_deserialize_string(input_stream_t* const stream, mqtt_string_t* const mqtt_string);

/** \brief Deserialize a response packet with a packet id only */
static bool mqtt_packet_deserialize_packet_id_only(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a 0 length packet */
static bool mqtt_packet_deserialize_zero_length(input_stream_t* const stream, const uint32_t packet_length);




/** \brief Initialize a whole packet deserialization */
bool mqtt_packet_deserialize_init_whole_packet(mqtt_deserialize_whole_data_t* const whole_data)
{
    bool ret = false;

    /* Check params */
    if (whole_data != NULL)
    {
        whole_data->state = MQTT_DWS_PACKET_TYPE;
        whole_data->bytes_left = 0u;
    }

    return ret;
}

/** \brief Deserialize a whole packet */
bool mqtt_packet_deserialize_whole_packet(mqtt_deserialize_whole_data_t* const whole_data, input_stream_t* const instream, output_stream_t* const outstream,
                                          mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags)
{
    bool ret = false;

    /* Check params */
    if ((instream != NULL) &&
        (outstream != NULL) &&
        (packet_type != NULL) &&
        (packet_flags != NULL))
    {
        bool again;
        do
        {
            again = false;
            switch (whole_data->state)
            {
                case MQTT_DWS_PACKET_TYPE:
                {
                    const bool callret = mqtt_packet_deserialize_packet_type(instream, packet_type, packet_flags);
                    if (callret)
                    {
                        whole_data->state = MQTT_DWS_PACKET_TYPE;
                        again = true;
                    }
                    break;
                }

                case MQTT_DWS_PACKET_LENGTH:
                {
                    uint8_t len_byte;
                    /* Read one byte */
                    const bool callret = instream->reader(instream, &len_byte, sizeof(len_byte));
                    if (callret)
                    {
                        /* Update len */
                        whole_data->bytes_left = (whole_data->bytes_left << 7u) + (len_byte & 0x7F);

                        /* Check end of length field */
                        if ((len_byte & 0x80u) == 0u)
                        {
                            whole_data->state = MQTT_DWS_PACKET_PAYLOAD;
                            again = true;
                        }
                    }
                    break;
                }
                break;

                case MQTT_DWS_PACKET_PAYLOAD:
                {
                    uint8_t payload_byte;

                    /* Read one byte at a time*/
                    bool callret = instream->reader(instream, &payload_byte, sizeof(payload_byte));
                    if (callret)
                    {
                        /* Write byte to output stream */
                        callret = outstream->writer(outstream, &payload_byte, sizeof(payload_byte));
                        if (callret)
                        {
                            /* Check end of frame */
                            whole_data->bytes_left--;
                            if (whole_data->bytes_left == 0u)
                            {
                                whole_data->state = MQTT_DWS_PACKET_END;
                                ret = true;
                            }
                            else
                            {
                                again = true;
                            }
                        }
                    }
                }
                break;
            
                default:
                {
                    break;
                }
            }
        }
        while (!ret && again);

        /* In progress indication */
        if (!ret)
        {
            const int32_t err = mqtt_errno_get();
            if (err == MQTT_ERR_INPUT_STREAM_EMPTY)
            {
                mqtt_errno_set(MQTT_ERR_IN_PROGRESS);
            }
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize the packet header */
bool mqtt_packet_deserialize_packet_header(input_stream_t* const stream, mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags,
                                           uint32_t* const length)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (packet_type != NULL) &&
        (packet_flags != NULL) &&
        (length != NULL))
    {
        /* Packet type and flags */
        ret = mqtt_packet_deserialize_packet_type(stream, packet_type, packet_flags);

        /* Remaining length */
        if (ret)
        {
            ret = mqtt_packet_deserialize_lenght(stream, length);
        } 
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize a CONNECT packet */
bool mqtt_packet_deserialize_connect(input_stream_t* const stream, mqtt_string_t* const client_id, mqtt_string_t* const protocol_name,
                                     uint8_t* const protocol_level, mqtt_credentials_t* const credentials, mqtt_will_t* const will,
                                     bool* const clean_session, uint16_t* const keepalive)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (client_id != NULL) &&
        (client_id->str != NULL) &&
        (client_id->size != 0u) &&
        (protocol_name != NULL) &&
        (protocol_name->str != NULL) &&
        (protocol_name->size != 0u) &&
        (protocol_level != NULL) &&
        (credentials != NULL) &&
        (credentials->username.str != NULL) &&
        (credentials->username.size != 0u) &&
        (credentials->password.str != NULL) &&
        (credentials->password.size != 0u) &&
        (will != NULL) &&
        (will->topic.str != NULL) &&
        (will->topic.size != 0u) &&
        (will->message.str != NULL) &&
        (will->message.size != 0u) &&
        (clean_session != NULL) &&
        (keepalive != NULL))
    {
        bool will_flag = false;
        bool username_flag = false;
        bool password_flag = false;

        /* Protocol name */
        ret = mqtt_packet_deserialize_string(stream, protocol_name);

        /* Protocol level */
        if (ret)
        {
            ret = stream->reader(stream, protocol_level, sizeof(*protocol_level));
        }

        /* Flags */
        if (ret)
        {
            uint8_t flags;
            ret = stream->reader(stream, &flags, sizeof(flags));
            if (ret)
            {
                (*clean_session) = ((flags & MQTT_CONNECT_FLAG_CLEAN_SESSION) != 0u);
                will_flag = ((flags & MQTT_CONNECT_FLAG_WILL_FLAG) != 0u);
                will->qos = (((flags & MQTT_CONNECT_FLAG_WILL_QOS) >> MQTT_CONNECT_FLAG_WILL_QOS_POSITION) & 0x03u);
                will->retain = ((flags & MQTT_CONNECT_FLAG_WILL_RETAIN) != 0u);
                password_flag = ((flags & MQTT_CONNECT_FLAG_PASSWORD) != 0u);
                username_flag = ((flags & MQTT_CONNECT_FLAG_USERNAME) != 0u);
            }
        }

        /* Keep alive */
        if (ret)
        {
            uint16_t keepalive_be;

            ret = stream->reader(stream, &keepalive_be, sizeof(keepalive_be));
            if (ret)
            {
                (*keepalive) = MQTT_BIG_ENDIAN_UINT16(keepalive_be);
            }
        }

        /* Client id */
        if (ret)
        {
            ret = mqtt_packet_deserialize_string(stream, client_id);
        }

        /* Will */
        if (ret && will_flag)
        {
            /* Topic */
            ret = mqtt_packet_deserialize_string(stream, &will->topic);
            if (ret)
            {
                /* Message */
                ret = mqtt_packet_deserialize_string(stream, &will->message);
            }
        }

        /* Credentials */
        if (ret && username_flag)
        {
            /* Username */
            ret = mqtt_packet_deserialize_string(stream, &credentials->username);
            if (ret && password_flag)
            {
                ret = mqtt_packet_deserialize_string(stream, &credentials->password);
            }
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize a CONNACK packet */
bool mqtt_packet_deserialize_connack(input_stream_t* const stream, bool* const session_present, mqtt_connack_retcode_t* const retcode)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (session_present != NULL) &&
        (retcode != NULL))
    {
        uint8_t payload[2u];

        /* Payload */
        ret = stream->reader(stream, payload, sizeof(payload));

        /* Extract data */
        if (ret)
        {
            switch (payload[0])
            {
                case 0:
                    (*session_present) = false;
                    break;
                case 1:
                    (*session_present) = true;
                    break;
                default:
                    ret = false;
                    mqtt_errno_set(MQTT_ERR_INVALID_PACKET_PAYLOAD);
                    break;
            }
            (*retcode) = (mqtt_connack_retcode_t)payload[1];
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }


    return ret;
}

/** \brief Deserialize a PUBLISH packet */
bool mqtt_packet_deserialize_publish(input_stream_t* const stream, const uint8_t packet_flags, const uint32_t packet_length, mqtt_string_t* const topic,
                                     void* data, uint32_t* const length, uint8_t* const qos, bool* const retain, bool* const duplicate, 
                                     uint16_t* const packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (topic != NULL) &&
        (topic->str != NULL) &&
        (topic->size != 0u) &&
        (data != NULL) &&
        (length != NULL) &&
        (length != 0u) &&
        (qos != NULL) &&
        (retain != NULL) &&
        (duplicate != NULL) &&
        (packet_id != NULL))
    {
        uint32_t remaining_length = packet_length;

        /* Duplicate flag */
        (*duplicate) = ((packet_flags & MQTT_PUBLISH_FLAG_DUP) != 0u);

        /* Retain flag */
        (*retain) = ((packet_flags & MQTT_PUBLISH_FLAG_RETAIN) != 0u);

        /* QoS */
        (*qos) = (packet_flags & MQTT_PUBLISH_FLAG_QOS) >> MQTT_PUBLISH_FLAG_QOS_POSITION;
        if ((*qos) > MQTT_CFG_MAX_QOS_LEVEL)
        {
            mqtt_errno_set(MQTT_ERR_INVALID_PACKET_QOS);
            ret = false;
        }
        else
        {
            ret = true;
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_deserialize_string(stream, topic);
            remaining_length -= topic->size + MQTT_MIN_ENCODED_STRING_SIZE;
        }

        /* Packet id */
        if (ret && ((*qos) > 0u) && (remaining_length > sizeof(packet_id)))
        {
            uint16_t received_id_be;

            ret = stream->reader(stream, &received_id_be, sizeof(received_id_be));
            if (ret)
            {
                (*packet_id) = MQTT_BIG_ENDIAN_UINT16(received_id_be);
                remaining_length -= sizeof(packet_id);
            }
        }

        /* Payload */
        if (ret)
        {
            if (remaining_length <= (stream->size - stream->read))
            {
                ret = stream->reader(stream, data, remaining_length);
                (*length) = remaining_length;
            }
            else
            {
                ret = false;
                mqtt_errno_set(MQTT_ERR_BUFFER_TOO_SMALL);
            }
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize a PUBACK packet */
bool mqtt_packet_deserialize_puback(input_stream_t* const stream, uint16_t* const packet_id)
{
    const bool ret = mqtt_packet_deserialize_packet_id_only(stream, packet_id);
    return ret;
}

/** \brief Deserialize a PUBREC packet */
bool mqtt_packet_deserialize_pubrec(input_stream_t* const stream, uint16_t* const packet_id)
{
    const bool ret = mqtt_packet_deserialize_packet_id_only(stream, packet_id);
    return ret;
}

/** \brief Deserialize a PUBREL packet */
bool mqtt_packet_deserialize_pubrel(input_stream_t* const stream, uint16_t* const packet_id)
{
    const bool ret = mqtt_packet_deserialize_packet_id_only(stream, packet_id);
    return ret;
}

/** \brief Deserialize a PUBCOMP packet */
bool mqtt_packet_deserialize_pubcomp(input_stream_t* const stream, uint16_t* const packet_id)
{
    const bool ret = mqtt_packet_deserialize_packet_id_only(stream, packet_id);
    return ret;
}

/** \brief Deserialize a SUBSCRIBE packet */
bool mqtt_packet_deserialize_subscribe(input_stream_t* const stream, mqtt_string_t* const topic, uint8_t* const qos,
                                       uint16_t* const packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (topic != NULL) &&
        (topic->str != NULL) &&
        (topic->size != 0u) &&
        (qos != NULL) &&
        (packet_id != NULL))
    {
        /* Packet id */
        uint16_t received_id_be;

        ret = stream->reader(stream, &received_id_be, sizeof(received_id_be));
        if (ret)
        {
            (*packet_id) = MQTT_BIG_ENDIAN_UINT16(received_id_be);
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_deserialize_string(stream, topic);
        }

        /* QoS */
        if (ret)
        {
            uint8_t received_qos;

            ret = stream->reader(stream, &received_qos, sizeof(received_qos));
            if (ret)
            {
                if ((received_qos < MQTT_CFG_MAX_QOS_LEVEL) ||
                    (received_qos == MQTT_FAILURE_QOS))
                {
                    (*qos) = received_qos;
                }
                else
                {
                    ret = false;
                    mqtt_errno_set(MQTT_ERR_INVALID_PACKET_QOS);
                }
            }
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize a SUBACK packet */
bool mqtt_packet_deserialize_suback(input_stream_t* const stream, uint8_t* const qos, uint16_t* const packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (qos != NULL) &&
        (packet_id != NULL))
    {
        /* Packet id */
        uint16_t received_id_be;

        ret = stream->reader(stream, &received_id_be, sizeof(received_id_be));
        if (ret)
        {
            (*packet_id) = MQTT_BIG_ENDIAN_UINT16(received_id_be);
        }

        /* QoS */
        if (ret)
        {
            uint8_t received_qos;

            ret = stream->reader(stream, &received_qos, sizeof(received_qos));
            if (ret)
            {
                if ((received_qos < MQTT_CFG_MAX_QOS_LEVEL) ||
                    (received_qos == MQTT_FAILURE_QOS))
                {
                    (*qos) = received_qos;
                }
                else
                {
                    ret = false;
                    mqtt_errno_set(MQTT_ERR_INVALID_PACKET_QOS);
                }
            }
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize an UNSUBSCRIBE packet */
bool mqtt_packet_deserialize_unsubscribe(input_stream_t* const stream, mqtt_string_t* const topic, uint16_t* const packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (topic != NULL) &&
        (topic->str != NULL) &&
        (topic->size != 0u) &&
        (packet_id != NULL))
    {
        /* Packet id */
        uint16_t received_id_be;

        ret = stream->reader(stream, &received_id_be, sizeof(received_id_be));
        if (ret)
        {
            (*packet_id) = MQTT_BIG_ENDIAN_UINT16(received_id_be);
        }

        /* Topic */
        if (ret)
        {
            ret = mqtt_packet_deserialize_string(stream, topic);
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize an UNSUBACK packet */
bool mqtt_packet_deserialize_unsuback(input_stream_t* const stream, uint16_t* const packet_id)
{
    const bool ret = mqtt_packet_deserialize_packet_id_only(stream, packet_id);
    return ret;
}

/** \brief Deserialize a PINGREQ packet */
bool mqtt_packet_deserialize_pingreq(input_stream_t* const stream, const uint32_t packet_length)
{
    const bool ret = mqtt_packet_deserialize_zero_length(stream, packet_length);
    return ret;
}

/** \brief Deserialize a PINGRESP packet */
bool mqtt_packet_deserialize_pingresp(input_stream_t* const stream, const uint32_t packet_length)
{
    const bool ret = mqtt_packet_deserialize_zero_length(stream, packet_length);
    return ret;
}

/** \brief Deserialize a DISCONNECT packet */
bool mqtt_packet_deserialize_disconnect(input_stream_t* const stream, const uint32_t packet_length)
{
    const bool ret = mqtt_packet_deserialize_zero_length(stream, packet_length);
    return ret;
}




/** \brief Deserialize the packet type */
static bool mqtt_packet_deserialize_packet_type(input_stream_t* const stream, mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    uint8_t packet_byte;

    /* Packet type */
    ret = stream->reader(stream, &packet_byte, sizeof(packet_byte));

    /* Extract packet type and flags */
    if (ret)
    {
        const uint8_t type = packet_byte >> 4u;
        (*packet_flags) = (packet_byte & 0x0Fu);

        switch (type)
        {
            case MQTT_PKT_CONNECT:
                /* Fall throught */
            case MQTT_PKT_CONNACK:
                /* Fall throught */
            case MQTT_PKT_PUBLISH:
                /* Fall throught */
            case MQTT_PKT_PUBACK:
                /* Fall throught */
            case MQTT_PKT_PUBREC:
                /* Fall throught */
            case MQTT_PKT_PUBREL:
                /* Fall throught */
            case MQTT_PKT_PUBCOMP:
                /* Fall throught */
            case MQTT_PKT_SUBACK:
                /* Fall throught */
            case MQTT_PKT_UNSUBACK:
                /* Fall throught */
            case MQTT_PKT_PINGREQ:
                /* Fall throught */
            case MQTT_PKT_PINGRESP:
                /* Fall throught */
            case MQTT_PKT_DISCONNECT:
                (*packet_type) = type;
                break;
            case MQTT_PKT_SUBSCRIBE:
                /* Fall throught */
            case MQTT_PKT_UNSUBSCRIBE:
                if ((packet_byte & 0x0Fu) == MQTT_SUB_UNSUBSCRIBE_HEADER_VALUE)
                {
                    (*packet_type) = type;
                }
                else
                {
                    ret = false;
                    mqtt_errno_set(MQTT_ERR_INVALID_PACKET_TYPE);
                }
                break;
            default:
                ret = false;
                mqtt_errno_set(MQTT_ERR_INVALID_PACKET_TYPE);
                break;
        }
    }

    return ret;
}

/** \brief Deserialize a variable length field */
static bool mqtt_packet_deserialize_lenght(input_stream_t* const stream, uint32_t* const length)
{
    bool ret = false;
    uint8_t len_byte;

    (*length) = 0u;
    do
    {
        /* Read one byte */
        ret = stream->reader(stream, &len_byte, sizeof(len_byte));
        if (ret)
        {
            /* Update len */
            (*length) = ((*length) << 7u) + (len_byte & 0x7F);
        }
    }
    while( ret && ((len_byte & 0x80u) != 0u));

    return ret;
}

/** \brief Deserialize a string */
static bool mqtt_packet_deserialize_string(input_stream_t* const stream, mqtt_string_t* const mqtt_string)
{
    bool ret = false;

    /* No null-pointer test for the parameters because this function
    is meant to be called from the inside of the library where the
    parameters are already checked.
    */

    /* Length */
    uint16_t len = 0u;
    uint16_t len_be = 0u;
    ret = stream->reader(stream, &len_be, sizeof(len_be));
    if (ret)
    {
        len = MQTT_BIG_ENDIAN_UINT16(len_be);
        if (len <= mqtt_string->size)
        {
            mqtt_string->size = len;
        }
        else
        {
            ret = false;
            mqtt_errno_set(MQTT_ERR_MQTT_STRING_TOO_SMALL);
        }
    }

    /* String */
    if (ret)
    {
        ret = stream->reader(stream, mqtt_string->str, len);
    }

    return ret;
}

/** \brief Deserialize a response packet with a packet id only */
static bool mqtt_packet_deserialize_packet_id_only(input_stream_t* const stream, uint16_t* const packet_id)
{
    bool ret = false;

    /* Check params */
    if ((stream != NULL) &&
        (packet_id != NULL))
    {
        /* Packet id */
        uint16_t received_id_be;

        ret = stream->reader(stream, &received_id_be, sizeof(received_id_be));
        if (ret)
        {
            (*packet_id) = MQTT_BIG_ENDIAN_UINT16(received_id_be);
        }
    }
    else
    {
        /* Error */
        mqtt_errno_set(MQTT_ERR_INVALID_PARAM);
    }

    return ret;
}

/** \brief Deserialize a 0 length packet */
static bool mqtt_packet_deserialize_zero_length(input_stream_t* const stream, const uint32_t packet_length)
{
    bool ret = false;

    /* Check params */
    if (stream != NULL)
    {
        /* Check packet size */
        if (packet_length != 0u)
        {
            mqtt_errno_set(MQTT_ERR_INVALID_PACKET_SIZE);
        }
        else
        {
            ret = true;
        }
    }

    return ret;
}
