/*
 * Copyright (c) 2000, 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AVFORMAT_ASF_H
#define AVFORMAT_ASF_H

#include <stdint.h>
#include "avformat.h"
#include "metadata.h"

#define PACKET_SIZE 3200

typedef struct {
    int num;
    unsigned char seq;
    /* use for reading */
    AVPacket pkt;
    int frag_offset;
    int timestamp;
    int64_t duration;

    int ds_span;                /* descrambling  */
    int ds_packet_size;
    int ds_chunk_size;

    int64_t packet_pos;

    uint16_t stream_language_index;

} ASFStream;

typedef uint8_t ff_asf_guid[16];

typedef struct {
    ff_asf_guid guid;                  ///< generated by client computer
    uint64_t file_size;         /**< in bytes
                                 *   invalid if broadcasting */
    uint64_t create_time;       /**< time of creation, in 100-nanosecond units since 1.1.1601
                                 *   invalid if broadcasting */
    uint64_t play_time;         /**< play time, in 100-nanosecond units
                                 * invalid if broadcasting */
    uint64_t send_time;         /**< time to send file, in 100-nanosecond units
                                 *   invalid if broadcasting (could be ignored) */
    uint32_t preroll;           /**< timestamp of the first packet, in milliseconds
                                 *   if nonzero - subtract from time */
    uint32_t ignore;            ///< preroll is 64bit - but let's just ignore it
    uint32_t flags;             /**< 0x01 - broadcast
                                 *   0x02 - seekable
                                 *   rest is reserved should be 0 */
    uint32_t min_pktsize;       /**< size of a data packet
                                 *   invalid if broadcasting */
    uint32_t max_pktsize;       /**< shall be the same as for min_pktsize
                                 *   invalid if broadcasting */
    uint32_t max_bitrate;       /**< bandwidth of stream in bps
                                 *   should be the sum of bitrates of the
                                 *   individual media streams */
} ASFMainHeader;


typedef struct {
    uint32_t packet_number;
    uint16_t packet_count;
} ASFIndex;


typedef struct {
    uint32_t seqno;
    int is_streamed;
    int asfid2avid[128];                 ///< conversion table from asf ID 2 AVStream ID
    ASFStream streams[128];              ///< it's max number and it's not that big
    uint32_t stream_bitrates[128];       ///< max number of streams, bitrate for each (for streaming)
    char stream_languages[128][6];       ///< max number of streams, language for each (RFC1766, e.g. en-US)
    /* non streamed additonnal info */
    uint64_t nb_packets;                 ///< how many packets are there in the file, invalid if broadcasting
    int64_t duration;                    ///< in 100ns units
    /* packet filling */
    unsigned char multi_payloads_present;
    int packet_size_left;
    int packet_timestamp_start;
    int packet_timestamp_end;
    unsigned int packet_nb_payloads;
    int packet_nb_frames;
    uint8_t packet_buf[PACKET_SIZE];
    ByteIOContext pb;
    /* only for reading */
    uint64_t data_offset;                ///< beginning of the first data packet
    uint64_t data_object_offset;         ///< data object offset (excl. GUID & size)
    uint64_t data_object_size;           ///< size of the data object
    int index_read;

    ASFMainHeader hdr;

    int packet_flags;
    int packet_property;
    int packet_timestamp;
    int packet_segsizetype;
    int packet_segments;
    int packet_seq;
    int packet_replic_size;
    int packet_key_frame;
    int packet_padsize;
    unsigned int packet_frag_offset;
    unsigned int packet_frag_size;
    int64_t packet_frag_timestamp;
    int packet_multi_size;
    int packet_obj_size;
    int packet_time_delta;
    int packet_time_start;
    int64_t packet_pos;

    int stream_index;


    int64_t last_indexed_pts;
    ASFIndex* index_ptr;
    uint32_t nb_index_count;
    uint32_t nb_index_memory_alloc;
    uint16_t maximum_packet;

    ASFStream* asf_st;                   ///< currently decoded stream
} ASFContext;

extern const ff_asf_guid ff_asf_header;
extern const ff_asf_guid ff_asf_file_header;
extern const ff_asf_guid ff_asf_stream_header;
extern const ff_asf_guid ff_asf_ext_stream_header;
extern const ff_asf_guid ff_asf_audio_stream;
extern const ff_asf_guid ff_asf_audio_conceal_none;
extern const ff_asf_guid ff_asf_audio_conceal_spread;
extern const ff_asf_guid ff_asf_video_stream;
extern const ff_asf_guid ff_asf_jfif_media;
extern const ff_asf_guid ff_asf_video_conceal_none;
extern const ff_asf_guid ff_asf_command_stream;
extern const ff_asf_guid ff_asf_comment_header;
extern const ff_asf_guid ff_asf_codec_comment_header;
extern const ff_asf_guid ff_asf_codec_comment1_header;
extern const ff_asf_guid ff_asf_data_header;
extern const ff_asf_guid ff_asf_head1_guid;
extern const ff_asf_guid ff_asf_head2_guid;
extern const ff_asf_guid ff_asf_extended_content_header;
extern const ff_asf_guid ff_asf_simple_index_header;
extern const ff_asf_guid ff_asf_ext_stream_embed_stream_header;
extern const ff_asf_guid ff_asf_ext_stream_audio_stream;
extern const ff_asf_guid ff_asf_metadata_header;
extern const ff_asf_guid ff_asf_marker_header;
extern const ff_asf_guid ff_asf_my_guid;
extern const ff_asf_guid ff_asf_language_guid;
extern const ff_asf_guid ff_asf_content_encryption;
extern const ff_asf_guid ff_asf_ext_content_encryption;
extern const ff_asf_guid ff_asf_digital_signature;

extern const AVMetadataConv ff_asf_metadata_conv[];

#define ASF_PACKET_FLAG_ERROR_CORRECTION_PRESENT 0x80 //1000 0000


//   ASF data packet structure
//   =========================
//
//
//  -----------------------------------
// | Error Correction Data             |  Optional
//  -----------------------------------
// | Payload Parsing Information (PPI) |
//  -----------------------------------
// | Payload Data                      |
//  -----------------------------------
// | Padding Data                      |
//  -----------------------------------


// PPI_FLAG - Payload parsing information flags
#define ASF_PPI_FLAG_MULTIPLE_PAYLOADS_PRESENT 1

#define ASF_PPI_FLAG_SEQUENCE_FIELD_IS_BYTE  0x02 //0000 0010
#define ASF_PPI_FLAG_SEQUENCE_FIELD_IS_WORD  0x04 //0000 0100
#define ASF_PPI_FLAG_SEQUENCE_FIELD_IS_DWORD 0x06 //0000 0110
#define ASF_PPI_MASK_SEQUENCE_FIELD_SIZE     0x06 //0000 0110

#define ASF_PPI_FLAG_PADDING_LENGTH_FIELD_IS_BYTE  0x08 //0000 1000
#define ASF_PPI_FLAG_PADDING_LENGTH_FIELD_IS_WORD  0x10 //0001 0000
#define ASF_PPI_FLAG_PADDING_LENGTH_FIELD_IS_DWORD 0x18 //0001 1000
#define ASF_PPI_MASK_PADDING_LENGTH_FIELD_SIZE     0x18 //0001 1000

#define ASF_PPI_FLAG_PACKET_LENGTH_FIELD_IS_BYTE  0x20 //0010 0000
#define ASF_PPI_FLAG_PACKET_LENGTH_FIELD_IS_WORD  0x40 //0100 0000
#define ASF_PPI_FLAG_PACKET_LENGTH_FIELD_IS_DWORD 0x60 //0110 0000
#define ASF_PPI_MASK_PACKET_LENGTH_FIELD_SIZE     0x60 //0110 0000

// PL_FLAG - Payload flags
#define ASF_PL_FLAG_REPLICATED_DATA_LENGTH_FIELD_IS_BYTE   0x01 //0000 0001
#define ASF_PL_FLAG_REPLICATED_DATA_LENGTH_FIELD_IS_WORD   0x02 //0000 0010
#define ASF_PL_FLAG_REPLICATED_DATA_LENGTH_FIELD_IS_DWORD  0x03 //0000 0011
#define ASF_PL_MASK_REPLICATED_DATA_LENGTH_FIELD_SIZE      0x03 //0000 0011

#define ASF_PL_FLAG_OFFSET_INTO_MEDIA_OBJECT_LENGTH_FIELD_IS_BYTE  0x04 //0000 0100
#define ASF_PL_FLAG_OFFSET_INTO_MEDIA_OBJECT_LENGTH_FIELD_IS_WORD  0x08 //0000 1000
#define ASF_PL_FLAG_OFFSET_INTO_MEDIA_OBJECT_LENGTH_FIELD_IS_DWORD 0x0c //0000 1100
#define ASF_PL_MASK_OFFSET_INTO_MEDIA_OBJECT_LENGTH_FIELD_SIZE     0x0c //0000 1100

#define ASF_PL_FLAG_MEDIA_OBJECT_NUMBER_LENGTH_FIELD_IS_BYTE  0x10 //0001 0000
#define ASF_PL_FLAG_MEDIA_OBJECT_NUMBER_LENGTH_FIELD_IS_WORD  0x20 //0010 0000
#define ASF_PL_FLAG_MEDIA_OBJECT_NUMBER_LENGTH_FIELD_IS_DWORD 0x30 //0011 0000
#define ASF_PL_MASK_MEDIA_OBJECT_NUMBER_LENGTH_FIELD_SIZE     0x30 //0011 0000

#define ASF_PL_FLAG_STREAM_NUMBER_LENGTH_FIELD_IS_BYTE  0x40 //0100 0000
#define ASF_PL_MASK_STREAM_NUMBER_LENGTH_FIELD_SIZE     0xc0 //1100 0000

#define ASF_PL_FLAG_PAYLOAD_LENGTH_FIELD_IS_BYTE  0x40 //0100 0000
#define ASF_PL_FLAG_PAYLOAD_LENGTH_FIELD_IS_WORD  0x80 //1000 0000
#define ASF_PL_MASK_PAYLOAD_LENGTH_FIELD_SIZE     0xc0 //1100 0000

#define ASF_PL_FLAG_KEY_FRAME 0x80 //1000 0000

extern AVInputFormat ff_asf_demuxer;
int ff_guidcmp(const void *g1, const void *g2);
void ff_get_guid(ByteIOContext *s, ff_asf_guid *g);

#endif /* AVFORMAT_ASF_H */
