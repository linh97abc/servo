#pragma once
// MESSAGE CONFIG PACKING

#define MAVLINK_MSG_ID_CONFIG 32


typedef struct __mavlink_config_t {
 float Kp[4]; /*<  */
 float Ki[4]; /*<  */
 float Kd[4]; /*<  */
 uint8_t seq; /*<  */
} mavlink_config_t;

#define MAVLINK_MSG_ID_CONFIG_LEN 49
#define MAVLINK_MSG_ID_CONFIG_MIN_LEN 49
#define MAVLINK_MSG_ID_32_LEN 49
#define MAVLINK_MSG_ID_32_MIN_LEN 49

#define MAVLINK_MSG_ID_CONFIG_CRC 107
#define MAVLINK_MSG_ID_32_CRC 107

#define MAVLINK_MSG_CONFIG_FIELD_KP_LEN 4
#define MAVLINK_MSG_CONFIG_FIELD_KI_LEN 4
#define MAVLINK_MSG_CONFIG_FIELD_KD_LEN 4

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_CONFIG { \
    32, \
    "CONFIG", \
    4, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 48, offsetof(mavlink_config_t, seq) }, \
         { "Kp", NULL, MAVLINK_TYPE_FLOAT, 4, 0, offsetof(mavlink_config_t, Kp) }, \
         { "Ki", NULL, MAVLINK_TYPE_FLOAT, 4, 16, offsetof(mavlink_config_t, Ki) }, \
         { "Kd", NULL, MAVLINK_TYPE_FLOAT, 4, 32, offsetof(mavlink_config_t, Kd) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_CONFIG { \
    "CONFIG", \
    4, \
    {  { "seq", NULL, MAVLINK_TYPE_UINT8_T, 0, 48, offsetof(mavlink_config_t, seq) }, \
         { "Kp", NULL, MAVLINK_TYPE_FLOAT, 4, 0, offsetof(mavlink_config_t, Kp) }, \
         { "Ki", NULL, MAVLINK_TYPE_FLOAT, 4, 16, offsetof(mavlink_config_t, Ki) }, \
         { "Kd", NULL, MAVLINK_TYPE_FLOAT, 4, 32, offsetof(mavlink_config_t, Kd) }, \
         } \
}
#endif

/**
 * @brief Pack a config message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param seq  
 * @param Kp  
 * @param Ki  
 * @param Kd  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_config_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t seq, const float *Kp, const float *Ki, const float *Kd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONFIG_LEN];
    _mav_put_uint8_t(buf, 48, seq);
    _mav_put_float_array(buf, 0, Kp, 4);
    _mav_put_float_array(buf, 16, Ki, 4);
    _mav_put_float_array(buf, 32, Kd, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIG_LEN);
#else
    mavlink_config_t packet;
    packet.seq = seq;
    mav_array_memcpy(packet.Kp, Kp, sizeof(float)*4);
    mav_array_memcpy(packet.Ki, Ki, sizeof(float)*4);
    mav_array_memcpy(packet.Kd, Kd, sizeof(float)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIG_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONFIG;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
}

/**
 * @brief Pack a config message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param seq  
 * @param Kp  
 * @param Ki  
 * @param Kd  
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_config_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t seq,const float *Kp,const float *Ki,const float *Kd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONFIG_LEN];
    _mav_put_uint8_t(buf, 48, seq);
    _mav_put_float_array(buf, 0, Kp, 4);
    _mav_put_float_array(buf, 16, Ki, 4);
    _mav_put_float_array(buf, 32, Kd, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_CONFIG_LEN);
#else
    mavlink_config_t packet;
    packet.seq = seq;
    mav_array_memcpy(packet.Kp, Kp, sizeof(float)*4);
    mav_array_memcpy(packet.Ki, Ki, sizeof(float)*4);
    mav_array_memcpy(packet.Kd, Kd, sizeof(float)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_CONFIG_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_CONFIG;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
}

/**
 * @brief Encode a config struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param config C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_config_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_config_t* config)
{
    return mavlink_msg_config_pack(system_id, component_id, msg, config->seq, config->Kp, config->Ki, config->Kd);
}

/**
 * @brief Encode a config struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param config C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_config_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_config_t* config)
{
    return mavlink_msg_config_pack_chan(system_id, component_id, chan, msg, config->seq, config->Kp, config->Ki, config->Kd);
}

/**
 * @brief Send a config message
 * @param chan MAVLink channel to send the message
 *
 * @param seq  
 * @param Kp  
 * @param Ki  
 * @param Kd  
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_config_send(mavlink_channel_t chan, uint8_t seq, const float *Kp, const float *Ki, const float *Kd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_CONFIG_LEN];
    _mav_put_uint8_t(buf, 48, seq);
    _mav_put_float_array(buf, 0, Kp, 4);
    _mav_put_float_array(buf, 16, Ki, 4);
    _mav_put_float_array(buf, 32, Kd, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIG, buf, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
#else
    mavlink_config_t packet;
    packet.seq = seq;
    mav_array_memcpy(packet.Kp, Kp, sizeof(float)*4);
    mav_array_memcpy(packet.Ki, Ki, sizeof(float)*4);
    mav_array_memcpy(packet.Kd, Kd, sizeof(float)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIG, (const char *)&packet, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
#endif
}

/**
 * @brief Send a config message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_config_send_struct(mavlink_channel_t chan, const mavlink_config_t* config)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_config_send(chan, config->seq, config->Kp, config->Ki, config->Kd);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIG, (const char *)config, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
#endif
}

#if MAVLINK_MSG_ID_CONFIG_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_config_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t seq, const float *Kp, const float *Ki, const float *Kd)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint8_t(buf, 48, seq);
    _mav_put_float_array(buf, 0, Kp, 4);
    _mav_put_float_array(buf, 16, Ki, 4);
    _mav_put_float_array(buf, 32, Kd, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIG, buf, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
#else
    mavlink_config_t *packet = (mavlink_config_t *)msgbuf;
    packet->seq = seq;
    mav_array_memcpy(packet->Kp, Kp, sizeof(float)*4);
    mav_array_memcpy(packet->Ki, Ki, sizeof(float)*4);
    mav_array_memcpy(packet->Kd, Kd, sizeof(float)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_CONFIG, (const char *)packet, MAVLINK_MSG_ID_CONFIG_MIN_LEN, MAVLINK_MSG_ID_CONFIG_LEN, MAVLINK_MSG_ID_CONFIG_CRC);
#endif
}
#endif

#endif

// MESSAGE CONFIG UNPACKING


/**
 * @brief Get field seq from config message
 *
 * @return  
 */
static inline uint8_t mavlink_msg_config_get_seq(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  48);
}

/**
 * @brief Get field Kp from config message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_config_get_Kp(const mavlink_message_t* msg, float *Kp)
{
    return _MAV_RETURN_float_array(msg, Kp, 4,  0);
}

/**
 * @brief Get field Ki from config message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_config_get_Ki(const mavlink_message_t* msg, float *Ki)
{
    return _MAV_RETURN_float_array(msg, Ki, 4,  16);
}

/**
 * @brief Get field Kd from config message
 *
 * @return  
 */
static inline uint16_t mavlink_msg_config_get_Kd(const mavlink_message_t* msg, float *Kd)
{
    return _MAV_RETURN_float_array(msg, Kd, 4,  32);
}

/**
 * @brief Decode a config message into a struct
 *
 * @param msg The message to decode
 * @param config C-struct to decode the message contents into
 */
static inline void mavlink_msg_config_decode(const mavlink_message_t* msg, mavlink_config_t* config)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_config_get_Kp(msg, config->Kp);
    mavlink_msg_config_get_Ki(msg, config->Ki);
    mavlink_msg_config_get_Kd(msg, config->Kd);
    config->seq = mavlink_msg_config_get_seq(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_CONFIG_LEN? msg->len : MAVLINK_MSG_ID_CONFIG_LEN;
        memset(config, 0, MAVLINK_MSG_ID_CONFIG_LEN);
    memcpy(config, _MAV_PAYLOAD(msg), len);
#endif
}
