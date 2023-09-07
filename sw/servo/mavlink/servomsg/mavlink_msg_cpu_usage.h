#pragma once
// MESSAGE CPU_USAGE PACKING

#define MAVLINK_MSG_ID_CPU_USAGE 2


typedef struct __mavlink_cpu_usage_t {
 uint8_t seq; /*<  */
 uint8_t cpu_usage; /*<  */
} mavlink_cpu_usage_t;

#define MAVLINK_MSG_ID_CPU_USAGE_LEN 2
#define MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN 2
#define MAVLINK_MSG_ID_2_LEN 2
#define MAVLINK_MSG_ID_2_MIN_LEN 2

#define MAVLINK_MSG_ID_CPU_USAGE_CRC 160
#define MAVLINK_MSG_ID_2_CRC 160



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_CPU_USAGE { \
    2, \
    "CPU_USAGE", \
    2, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_cpu_usage_t, seq) }, \
         { "cpu_usage", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_cpu_usage_t, cpu_usage) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_CPU_USAGE { \
    "CPU_USAGE", \
    2, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_cpu_usage_t, seq) }, \
         { "cpu_usage", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_cpu_usage_t, cpu_usage) }, \
         } \
}
#endif

/**
 * @brief Pack a cpu_usage message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param seq  
 * @param cpu_usage  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_cpu_usage_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t seq, uint8_t cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CPU_USAGE_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, cpu_usage);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CPU_USAGE_LEN);
#else
    mavlink_cpu_usage_t packet;
    packet.seq = seq;
    packet.cpu_usage = cpu_usage;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CPU_USAGE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CPU_USAGE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
}

/**
 * @brief Pack a cpu_usage message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param seq  
 * @param cpu_usage  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_cpu_usage_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t seq,uint8_t cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CPU_USAGE_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, cpu_usage);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CPU_USAGE_LEN);
#else
    mavlink_cpu_usage_t packet;
    packet.seq = seq;
    packet.cpu_usage = cpu_usage;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CPU_USAGE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CPU_USAGE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
}

/**
 * @brief Encode a cpu_usage struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param cpu_usage C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_cpu_usage_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_cpu_usage_t* cpu_usage)
{
    return mavlink_msg_cpu_usage_pack(system_id, component_id, msg, cpu_usage->seq, cpu_usage->cpu_usage);
}

/**
 * @brief Encode a cpu_usage struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param cpu_usage C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_cpu_usage_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_cpu_usage_t* cpu_usage)
{
    return mavlink_msg_cpu_usage_pack_chan(system_id, component_id, chan, msg, cpu_usage->seq, cpu_usage->cpu_usage);
}

/**
 * @brief Send a cpu_usage message
 * @param chan MAVLink channel to send the message
 *
 * @param seq  
 * @param cpu_usage  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_cpu_usage_send(mavlink_channel_t chan, uint8_t seq, uint8_t cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CPU_USAGE_LEN];
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, cpu_usage);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CPU_USAGE, buf, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
#else
    mavlink_cpu_usage_t packet;
    packet.seq = seq;
    packet.cpu_usage = cpu_usage;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CPU_USAGE, (const char *)&packet, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
#endif
}

/**
 * @brief Send a cpu_usage message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_cpu_usage_send_struct(mavlink_channel_t chan, const mavlink_cpu_usage_t* cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_cpu_usage_send(chan, cpu_usage->seq, cpu_usage->cpu_usage);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CPU_USAGE, (const char *)cpu_usage, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
#endif
}

#if MAVLINK_MSG_ID_CPU_USAGE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_cpu_usage_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t seq, uint8_t cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 0, seq);
    _mav_put_uint8_t(buf, 1, cpu_usage);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CPU_USAGE, buf, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
#else
    mavlink_cpu_usage_t *packet = (mavlink_cpu_usage_t *)msgbuf;
    packet->seq = seq;
    packet->cpu_usage = cpu_usage;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CPU_USAGE, (const char *)packet, MAVLINK_MSG_ID_CPU_USAGE_MIN_LEN, MAVLINK_MSG_ID_CPU_USAGE_LEN, MAVLINK_MSG_ID_CPU_USAGE_CRC);
#endif
}
#endif

#endif

// MESSAGE CPU_USAGE UNPACKING


/**
 * @brief Get field seq from cpu_usage message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_cpu_usage_get_seq(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field cpu_usage from cpu_usage message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_cpu_usage_get_cpu_usage(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Decode a cpu_usage message into a struct
 *
 * @param msg The message to decode
 * @param cpu_usage C-struct to decode the message contents into
 */
static inline void mavlink_msg_cpu_usage_decode(const mavlink_message_t* msg, mavlink_cpu_usage_t* cpu_usage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    cpu_usage->seq = mavlink_msg_cpu_usage_get_seq(msg);
    cpu_usage->cpu_usage = mavlink_msg_cpu_usage_get_cpu_usage(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_CPU_USAGE_LEN? msg->len : MAVLINK_MSG_ID_CPU_USAGE_LEN;
        memset(cpu_usage, 0, MAVLINK_MSG_ID_CPU_USAGE_LEN);
    memcpy(cpu_usage, _MAV_PAYLOAD(msg), len);
#endif
}
