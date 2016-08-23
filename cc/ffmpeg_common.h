#ifndef CC_FFMPEG_COMMON_H_
#define CC_FFMPEG_COMMON_H_

extern "C" {

#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>

#include <libavutil/avutil.h>
#include <libavutil/parseutils.h>
#include <libavutil/avstring.h>
#include <libavutil/avassert.h>
#include <libavutil/opt.h>
#include <libavutil/dict.h>
#include <libavutil/eval.h>
#include <libavutil/fifo.h>
#include <libavutil/pixfmt.h>
#include <libavutil/rational.h>
#include <libavutil/threadmessage.h>

#include <libswresample/swresample.h>

#include "libavutil/parseutils.h"
#include "libavutil/pixdesc.h"
#include "libavutil/eval.h"
#include "libavutil/dict.h"
#include "libavutil/opt.h"
#include "libavutil/cpu.h"
#include "libavutil/ffversion.h"
#include "libavutil/version.h"

#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include "libpostproc/postprocess.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/bprint.h"
#include "libavutil/display.h"
#include "libavutil/mathematics.h"
#include "libavutil/imgutils.h"

#include "libavdevice/avdevice.h"

#include "libavformat/avformat.h"
#include "libavdevice/avdevice.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/channel_layout.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/fifo.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/dict.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/avstring.h"
#include "libavutil/imgutils.h"
#include "libavutil/timestamp.h"
#include "libavutil/bprint.h"
#include "libavutil/time.h"
#include "libavutil/threadmessage.h"

# include "libavfilter/avfilter.h"
# include "libavfilter/buffersrc.h"
# include "libavfilter/buffersink.h"

#include "libavutil/samplefmt.h"
#include "libavutil/log.h"

#include <libavutil/avutil.h>
#include <libavutil/intreadwrite.h>

} // extern "C"
#endif // CC_FFMPEG_COMMON_H_
