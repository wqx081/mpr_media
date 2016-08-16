#ifndef MPR_MEDIA_FFMPEG_FFMPEG_H_
#define MPR_MEDIA_FFMPEG_FFMPEG_H_

extern "C" {

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/log.h>
#include <libavutil/mathematics.h>
#include <libavutil/opt.h>

} // extern "C"

namespace media {

void PrintFFmpegError(const char* filename, int err);

struct InputStream {
  int file_index;
  AVStream *st;
  int discard;
  int user_set_discard;
  int decoding_needed;

  AVCodecContext* dec_ctx;
  AVCodec *dec;
  AVFrame *decoded_frame;
  //AVFrame *filter_frame;

  int64_t start;
  int64_t next_dts;
  int64_t dts;

  int64_t next_pts;
  int64_t pts;
  int     wrap_correction_done;

  int64_t min_pts;
  int64_t max_pts;
  int64_t nb_samples;

  double ts_scale;
  int saw_first_ts;
  int showed_multi_packet_warning;
  AVDictionary *decoder_opts;
  AVRational framerate;
  int top_field_first;
  int guess_layout_max;

  int autorotate;
  int resample_height;
  int resample_width;
  int resample_pix_fmt;

  int resample_sample_fmt;
  int resample_sample_rate;
  int resample_channels;
  uint64_t resample_channel_layout;

  int fix_sub_duration;
  struct {
    int got_output;
    int ret;
    AVSubtitle subtitle;
  } prev_sub;

  struct sub2video {
    int64_t last_pts;
    int64_t end_pts;
    AVFrame *frame;
    int w, h;
  } sub2video;

  int dr1;

  // stats
  uint64_t data_size;
  uint64_t nb_packets;
  uint64_t frames_decoded;
  uint64_t samples_decoded;
};

struct InputFile {
  AVFormatContext* ctx;
  int eof_reached;
  int eagain;
  int ist_index;
  int loop;
  int64_t duration;

  AVRational time_base;
  int64_t input_ts_offset;

  int64_t ts_offset;
  int64_t last_ts;
  int64_t start_time;
  int seek_timestamp;
  int64_t recording_time;
  int nb_streams;

  int nb_streams_warn;
  int rate_emu;
  int accurate_seek;
};

// Output

enum OSTFinished {
  ENCODER_FINISHED = 1,
  MUXER_FINISHED = 2,
};

struct OutputStream {
  int file_index;
  int index;
  int source_index;
  AVStream* st;
  int encoding_needed;
  int frame_number;

  InputStream* sync_ist;
  int64_t sync_opts;

  int64_t first_pts;
  int64_t last_mux_dts;
  AVBitStreamFilterContext* bitstream_filters;
  AVCodecContext* enc_ctx;
  AVCodec* enc;
  int64_t max_frames;
  AVFrame *filtered_frame;
  AVFrame *last_frame;
  int last_dropped;
  int last_nb0_frames[3];

  // Video Only
  AVRational frame_rate;
  int is_cfr;
  int force_fps;
  int top_field_first;
  int rotate_overridden;

  AVRational frame_aspect_ratio;

  // Forced key first

  // Audio only
  int* audio_channels_map;
  int audio_channels_mapped;


  // log
  // char* logfile_path;
  // FILE* logfile;
  //

  AVDictionary* encoder_opts;
  AVDictionary* sws_dict;
  AVDictionary* swr_opts;
  AVDictionary* resample_opts;
  char *apad;
  OSTFinished finished;        /* no more packets should be written for this stream */
  int unavailable;                     /* true if the steram is unavailable (possibly temporarily) */
  int stream_copy;
  const char *attachment_filename;
  int copy_initial_nonkeyframes;
  int copy_prior_start;
  char *disposition;

  int keep_pix_fmt;

  AVCodecParserContext *parser;

  // stats
  uint64_t data_size;
  uint64_t packets_written;
  uint64_t frames_encoded;
  uint64_t samples_encoded;
  
  int quality;
  int pict_type;

  int64_t error[4];
};

struct OutputFile {
  AVFormatContext* ctx;
  AVDictionary* opts;
  int ost_index;
  int64_t recording_time;
  int64_t start_time;
  uint64_t limit_filesize;

  int shortest;
};

} // namespace media
#endif // MPR_MEDIA_FFMPEG_FFMPEG_H_
