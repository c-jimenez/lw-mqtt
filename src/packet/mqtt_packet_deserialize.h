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

#ifndef MQTT_PACKET_DESERIALIZE_H
#define MQTT_PACKET_DESERIALIZE_H

#include "mqtt.h"
#include "input_stream.h"
#include "output_stream.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** \brief Store the state of a whole packet deserialization */
typedef enum mqtt_deserialize_whole_state_t
{
    MQTT_DWS_PACKET_TYPE = 0u,
    MQTT_DWS_PACKET_LENGTH = 1u,
    MQTT_DWS_PACKET_PAYLOAD = 2u,
    MQTT_DWS_PACKET_END = 3u
} mqtt_deserialize_whole_state_t;

/** \brief Store the state of a whole packet deserialization */
typedef struct _mqtt_deserialize_whole_data_t
{
    /** \brief Current state */
    mqtt_deserialize_whole_state_t state;
    /** \brief Bytes lefts */
    uint32_t bytes_left;
} mqtt_deserialize_whole_data_t;




/** \brief Initialize a whole packet deserialization */
bool mqtt_packet_deserialize_init_whole_packet(mqtt_deserialize_whole_data_t* const whole_data);

/** \brief Deserialize a whole packet */
bool mqtt_packet_deserialize_whole_packet(mqtt_deserialize_whole_data_t* const whole_data, input_stream_t* const instream, output_stream_t* const outstream,
                                          mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags);

/** \brief Deserialize the packet header */
bool mqtt_packet_deserialize_packet_header(input_stream_t* const stream, mqtt_control_packet_type_t* const packet_type, uint8_t* const packet_flags,
                                           uint32_t* const length);

/** \brief Deserialize a CONNECT packet */
bool mqtt_packet_deserialize_connect(input_stream_t* const stream, mqtt_string_t* const client_id, mqtt_string_t* const protocol_name,
                                     uint8_t* const protocol_level, mqtt_credentials_t* const credentials, mqtt_will_t* const will,
                                     bool* const clean_session, uint16_t* const keepalive);

/** \brief Deserialize a CONNACK packet */
bool mqtt_packet_deserialize_connack(input_stream_t* const stream, bool* const session_present, mqtt_connack_retcode_t* const retcode);

/** \brief Deserialize a PUBLISH packet */
bool mqtt_packet_deserialize_publish(input_stream_t* const stream, const uint8_t packet_flags, const uint32_t packet_length, mqtt_string_t* const topic,
                                     void* data, uint32_t* const length, uint8_t* const qos, bool* const retain, bool* const duplicate,
                                     uint16_t* const packet_id);

/** \brief Deserialize a PUBACK packet */
bool mqtt_packet_deserialize_puback(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a PUBREC packet */
bool mqtt_packet_deserialize_pubrec(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a PUBREL packet */
bool mqtt_packet_deserialize_pubrel(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a PUBCOMP packet */
bool mqtt_packet_deserialize_pubcomp(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a SUBSCRIBE packet */
bool mqtt_packet_deserialize_subscribe(input_stream_t* const stream, mqtt_string_t* const topic, uint8_t* const qos,
                                       uint16_t* const packet_id);

/** \brief Deserialize a SUBACK packet */
bool mqtt_packet_deserialize_suback(input_stream_t* const stream, uint8_t* const qos, uint16_t* const packet_id);

/** \brief Deserialize an UNSUBSCRIBE packet */
bool mqtt_packet_deserialize_unsubscribe(input_stream_t* const stream, mqtt_string_t* const topic, uint16_t* const packet_id);

/** \brief Deserialize an UNSUBACK packet */
bool mqtt_packet_deserialize_unsuback(input_stream_t* const stream, uint16_t* const packet_id);

/** \brief Deserialize a PINGREQ packet */
bool mqtt_packet_deserialize_pingreq(input_stream_t* const stream, const uint32_t packet_length);

/** \brief Deserialize a PINGRESP packet */
bool mqtt_packet_deserialize_pingresp(input_stream_t* const stream, const uint32_t packet_length);

/** \brief Deserialize a DISCONNECT packet */
bool mqtt_packet_deserialize_disconnect(input_stream_t* const stream, const uint32_t packet_length);



#ifdef __cplusplus
}
#endif /* __cplusplus */




#endif /* MQTT_PACKET_DESERIALIZE_H */
