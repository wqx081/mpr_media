#ifndef MEDIA_BASE_AUDIO_DECODER_CONFIG_H_
#define MEDIA_BASE_AUDIO_DECODER_CONFIG_H_

#include "base/macros.h"
#include "base/ref_counted.h"
#include "media/audio/channel_layout.h"


namespace media {

enum AudioCodec {
  // These values are histogrammed over time; do not change their ordinal
  // values.  When deleting a codec replace it with a dummy value; when adding a
  // codec, do so at the bottom (and update kAudioCodecMax).
  kUnknownAudioCodec = 0,
  kCodecAAC,
  kCodecMP3,
  kCodecPCM,
  kCodecVorbis,
  kCodecFLAC,
  kCodecAMR_NB,
  kCodecAMR_WB,
  kCodecPCM_MULAW,
  kCodecGSM_MS,
  kCodecPCM_S16BE,
  kCodecPCM_S24BE,
  kCodecOpus,
  // DO NOT ADD RANDOM AUDIO CODECS!
  //
  // The only acceptable time to add a new codec is if there is production code
  // that uses said codec in the same CL.

  kAudioCodecMax = kCodecOpus  // Must equal the last "real" codec above.
};

// TODO(dalecurtis): FFmpeg API uses |bytes_per_channel| instead of
// |bits_per_channel|, we should switch over since bits are generally confusing
// to work with.
class AudioDecoderConfig {
 public:
  // Constructs an uninitialized object. Clients should call Initialize() with
  // appropriate values before using.
  AudioDecoderConfig();

  // Constructs an initialized object. It is acceptable to pass in NULL for
  // |extra_data|, otherwise the memory is copied.
  AudioDecoderConfig(AudioCodec codec, int bits_per_channel,
                     ChannelLayout channel_layout, int samples_per_second,
                     const uint8_t* extra_data, size_t extra_data_size,
                     bool is_encrypted);

  ~AudioDecoderConfig();

  // Resets the internal state of this object.
  void Initialize(AudioCodec codec, int bits_per_channel,
                  ChannelLayout channel_layout, int samples_per_second,
                  const uint8_t* extra_data, size_t extra_data_size,
                  bool is_encrypted,
                  bool record_stats);

  // Deep copies |audio_config|.
  void CopyFrom(const AudioDecoderConfig& audio_config);

  // Returns true if this object has appropriate configuration values, false
  // otherwise.
  bool IsValidConfig() const;

  // Returns true if all fields in |config| match this config.
  // Note: The contents of |extra_data_| are compared not the raw pointers.
  bool Matches(const AudioDecoderConfig& config) const;

  AudioCodec codec() const;
  int bits_per_channel() const;
  ChannelLayout channel_layout() const;
  int samples_per_second() const;
  int bytes_per_frame() const;

  // Optional byte data required to initialize audio decoders such as Vorbis
  // codebooks.
  uint8_t* extra_data() const;
  size_t extra_data_size() const;

  // Whether the audio stream is potentially encrypted.
  // Note that in a potentially encrypted audio stream, individual buffers
  // can be encrypted or not encrypted.
  bool is_encrypted() const;

 private:
  AudioCodec codec_;
  int bits_per_channel_;
  ChannelLayout channel_layout_;
  int samples_per_second_;
  int bytes_per_frame_;

  std::unique_ptr<uint8_t[]> extra_data_;
  size_t extra_data_size_;

  bool is_encrypted_;

  DISALLOW_COPY_AND_ASSIGN(AudioDecoderConfig);
};

}  // namespace media

#endif  // MEDIA_BASE_AUDIO_DECODER_CONFIG_H_