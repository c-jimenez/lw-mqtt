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

#ifndef MQTT_PACKET_SERIALIZE_H
#define MQTT_PACKET_SERIALIZE_H

#include "mqtt.h"
#include "output_stream.h"


#ifdef __cplusplus
extern "C"
{
#endif


/** \brief Serialize a CONNECT packet */
bool mqtt_packet_serialize_connect(output_stream_t* const stream, const mqtt_const_string_t* const client_id,
                                   const mqtt_const_credentials_t* const credentials, const mqtt_const_will_t* const will,
                                   const bool clean_session, const uint16_t keepalive);

/** \brief Serialize a CONNACK packet */
bool mqtt_packet_serialize_connack(output_stream_t* const stream, const bool session_present, const mqtt_connack_retcode_t retcode);

/** \brief Serialize a PUBLISH packet */
bool mqtt_packet_serialize_publish(output_stream_t* const stream, const mqtt_const_string_t* const topic, const void* data, 
                                   const uint32_t length, const uint8_t qos, const bool retain, const bool duplicate,
                                   const uint16_t packet_id);

/** \brief Serialize a PUBACK packet */
bool mqtt_packet_serialize_puback(output_stream_t* const stream, const uint16_t packet_id);

/** \brief Serialize a PUBREC packet */
bool mqtt_packet_serialize_pubrec(output_stream_t* const stream, const uint16_t packet_id);

/** \brief Serialize a PUBREL packet */
bool mqtt_packet_serialize_pubrel(output_stream_t* const stream, const uint16_t packet_id);

/** \brief Serialize a PUBCOMP packet */
bool mqtt_packet_serialize_pubcomp(output_stream_t* const stream, const uint16_t packet_id);

/** \brief Serialize a SUBSCRIBE packet */
bool mqtt_packet_serialize_subscribe(output_stream_t* const stream, const mqtt_const_string_t* const topic, const uint8_t qos,
                                     const uint16_t packet_id);

/** \brief Serialize a SUBACK packet */
bool mqtt_packet_serialize_suback(output_stream_t* const stream, const uint8_t qos, const uint16_t packet_id);

/** \brief Serialize an UNSUBSCRIBE packet */
bool mqtt_packet_serialize_unsubscribe(output_stream_t* const stream, const mqtt_const_string_t* const topic, const uint16_t packet_id);

/** \brief Serialize an UNSUBACK packet */
bool mqtt_packet_serialize_unsuback(output_stream_t* const stream, const uint16_t packet_id);

/** \brief Serialize a PINGREQ packet */
bool mqtt_packet_serialize_pingreq(output_stream_t* const stream);

/** \brief Serialize a PINGRESP packet */
bool mqtt_packet_serialize_pingresp(output_stream_t* const stream);

/** \brief Serialize a DISCONNECT packet */
bool mqtt_packet_serialize_disconnect(output_stream_t* const stream);



#ifdef __cplusplus
}
#endif /* __cplusplus */




#endif /* MQTT_PACKET_SERIALIZE_H */
