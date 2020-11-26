// Anbox - The Android in a Box runtime environment
// Copyright 2018 Canonical Ltd.  All rights reserved.

#ifndef ANBOX_SDK_AUDIO_FORMAT_H_
#define ANBOX_SDK_AUDIO_FORMAT_H_

#include "anbox-platform-sdk/common.h"

/**
 * @brief Audio pcm sub formats.
 *
 * All of these are in native byte order.
 */
typedef enum {
    /** DO NOT CHANGE - PCM signed 16 bits. */
    AUDIO_FORMAT_PCM_SUB_16_BIT          = 0x1,
    /** DO NOT CHANGE - PCM unsigned 8 bits. */
    AUDIO_FORMAT_PCM_SUB_8_BIT           = 0x2,
    /** PCM signed .31 fixed point. */
    AUDIO_FORMAT_PCM_SUB_32_BIT          = 0x3,
    /** PCM signed 8.23 fixed point. */
    AUDIO_FORMAT_PCM_SUB_8_24_BIT        = 0x4,
    /** PCM single-precision floating point. */
    AUDIO_FORMAT_PCM_SUB_FLOAT           = 0x5,
    /** PCM signed .23 fixed point packed in 3 bytes. */
    AUDIO_FORMAT_PCM_SUB_24_BIT_PACKED   = 0x6,
} AnboxAudioPcmSubFormat;

/* The audio_format_*_sub_fmt_t declarations are not currently used */

/**
 * @brief Audio format is a 32-bit word that consists of:
 *   main format field (upper 8 bits)
 *   sub format field (lower 24 bits).
 *
 * The main format indicates the main codec type. The sub format field
 * indicates options and parameters for each format. The sub format is mainly
 * used for instance to indicate the requested bitrate or profile.
 * It can also be used for certain formats to give informations not present in
 * the encoded audio stream (e.g. octet alignement for AMR).
 */
typedef enum {
    /** Invalid Audio format. */
    AUDIO_FORMAT_INVALID             = 0xFFFFFFFFUL,
    /** Default Audio format. */
    AUDIO_FORMAT_DEFAULT             = 0,
    /* DO NOT CHANGE */
    /** PCM format, reserved*/
    AUDIO_FORMAT_PCM                 = 0x00000000UL,

    /* Aliases */
    /* note != AudioFormat.ENCODING_PCM_16BIT */
    /** PCM signed 16 bits. */
    AUDIO_FORMAT_PCM_16_BIT          = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_16_BIT),
    /* note != AudioFormat.ENCODING_PCM_8BIT */
    /** PCM unsigned 8 bits. */
    AUDIO_FORMAT_PCM_8_BIT           = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_8_BIT),
    /** PCM signed .31 fixed point. */
    AUDIO_FORMAT_PCM_32_BIT          = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_32_BIT),
    /** PCM signed 8.23 fixed point. */
    AUDIO_FORMAT_PCM_8_24_BIT        = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_8_24_BIT),
    /** PCM single-precision floating point. */
    AUDIO_FORMAT_PCM_FLOAT           = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_FLOAT),
    /** PCM signed .23 fixed point packed in 3 bytes. */
    AUDIO_FORMAT_PCM_24_BIT_PACKED   = (AUDIO_FORMAT_PCM |
                                        AUDIO_FORMAT_PCM_SUB_24_BIT_PACKED),
} AnboxAudioFormat;
#endif
