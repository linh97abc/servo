#pragma once
// MESSAGE COM_MANAGER PACKING

#define MAVLINK_MSG_ID_COM_MANAGER 4


typedef struct __mavlink_com_manager_t {
 uint32_t baudrate; /*<  */
 uint32_t send; /*<  */
 uint32_t recv; /*<  */
} mavlink_com_manager_t;

#define MAVLINK_MSG_ID_COM_MANAGER_LEN 12
#define MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN 12
#define MAVLINK_MSG_ID_4_LEN 12
#define MAVLINK_MSG_ID_4_MIN_LEN 12

#define MAVLINK_MSG_ID_COM_MANAGER_CRC 239
#define MAVLINK_MSG_ID_4_CRC 239



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_COM_MANAGER { \
    4, \
    "COM_MANAGER", \
    3, \
    {  { "baudrate", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_com_manager_t, baudrate) }, \
         { "send", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_com_manager_t, send) }, \
         { "recv", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_com_manager_t, recv) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_COM_MANAGER { \
    "COM_MANAGER", \
    3, \
    {  { "baudrate", NULL, MAVLINK_TYPE_UINT32_T, 0, 0, offsetof(mavlink_com_manager_t, baudrate) }, \
         { "send", NULL, MAVLINK_TYPE_UINT32_T, 0, 4, offsetof(mavlink_com_manager_t, send) }, \
         { "recv", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_com_manager_t, recv) }, \
         } \
}
#endif

/**
 * @brief Pack a com_manager message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param baudrate  
 * @param send  
 * @param recv  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_com_manager_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint32_t baudrate, uint32_t send, uint32_t recv)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COM_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, baudrate);
    _mav_put_uint32_t(buf, 4, send);
    _mav_put_uint32_t(buf, 8, recv);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COM_MANAGER_LEN);
#else
    mavlink_com_manager_t packet;
    packet.baudrate = baudrate;
    packet.send = send;
    packet.recv = recv;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COM_MANAGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_COM_MANAGER;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
}

/**
 * @brief Pack a com_manager message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param baudrate  
 * @param send  
 * @param recv  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_com_manager_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint32_t baudrate,uint32_t send,uint32_t recv)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COM_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, baudrate);
    _mav_put_uint32_t(buf, 4, send);
    _mav_put_uint32_t(buf, 8, recv);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_COM_MANAGER_LEN);
#else
    mavlink_com_manager_t packet;
    packet.baudrate = baudrate;
    packet.send = send;
    packet.recv = recv;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_COM_MANAGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_COM_MANAGER;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
}

/**
 * @brief Encode a com_manager struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param com_manager C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_com_manager_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_com_manager_t* com_manager)
{
    return mavlink_msg_com_manager_pack(system_id, component_id, msg, com_manager->baudrate, com_manager->send, com_manager->recv);
}

/**
 * @brief Encode a com_manager struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param com_manager C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_com_manager_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_com_manager_t* com_manager)
{
    return mavlink_msg_com_manager_pack_chan(system_id, component_id, chan, msg, com_manager->baudrate, com_manager->send, com_manager->recv);
}

/**
 * @brief Send a com_manager message
 * @param chan MAVLink channel to send the message
 *
 * @param baudrate  
 * @param send  
 * @param recv  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_com_manager_send(mavlink_channel_t chan, uint32_t baudrate, uint32_t send, uint32_t recv)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_COM_MANAGER_LEN];
    _mav_put_uint32_t(buf, 0, baudrate);
    _mav_put_uint32_t(buf, 4, send);
    _mav_put_uint32_t(buf, 8, recv);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COM_MANAGER, buf, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
#else
    mavlink_com_manager_t packet;
    packet.baudrate = baudrate;
    packet.send = send;
    packet.recv = recv;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COM_MANAGER, (const char *)&packet, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
#endif
}

/**
 * @brief Send a com_manager message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_com_manager_send_struct(mavlink_channel_t chan, const mavlink_com_manager_t* com_manager)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_com_manager_send(chan, com_manager->baudrate, com_manager->send, com_manager->recv);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COM_MANAGER, (const char *)com_manager, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
#endif
}

#if MAVLINK_MSG_ID_COM_MANAGER_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_com_manager_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint32_t baudrate, uint32_t send, uint32_t recv)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint32_t(buf, 0, baudrate);
    _mav_put_uint32_t(buf, 4, send);
    _mav_put_uint32_t(buf, 8, recv);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COM_MANAGER, buf, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
#else
    mavlink_com_manager_t *packet = (mavlink_com_manager_t *)msgbuf;
    packet->baudrate = baudrate;
    packet->send = send;
    packet->recv = recv;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_COM_MANAGER, (const char *)packet, MAVLINK_MSG_ID_COM_MANAGER_MIN_LEN, MAVLINK_MSG_ID_COM_MANAGER_LEN, MAVLINK_MSG_ID_COM_MANAGER_CRC);
#endif
}
#endif

#endif

// MESSAGE COM_MANAGER UNPACKING


/**
 * @brief Get field baudrate from com_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_com_manager_get_baudrate(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  0);
}

/**
 * @brief Get field send from com_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_com_manager_get_send(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  4);
}

/**
 * @brief Get field recv from com_manager message
 *
 * @return  
 */
static inline uint32_t mavlink_msg_com_manager_get_recv(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Decode a com_manager message into a struct
 *
 * @param msg The message to decode
 * @param com_manager C-struct to decode the message contents into
 */
static inline void mavlink_msg_com_manager_decode(const mavlink_message_t* msg, mavlink_com_manager_t* com_manager)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    com_manager->baudrate = mavlink_msg_com_manager_get_baudrate(msg);
    com_manager->send = mavlink_msg_com_manager_get_send(msg);
    com_manager->recv = mavlink_msg_com_manager_get_recv(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_COM_MANAGER_LEN? msg->len : MAVLINK_MSG_ID_COM_MANAGER_LEN;
        memset(com_manager, 0, MAVLINK_MSG_ID_COM_MANAGER_LEN);
    memcpy(com_manager, _MAV_PAYLOAD(msg), len);
#endif
}
