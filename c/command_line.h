#ifndef C_COMMAND_LINE_H_
#define C_COMMAND_LINE_H_

#include <stdint.h>
#include <stdio.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
} // extern "C"

namespace media {

struct SpecifierOption {
  char* specifier;
  union {
    uint8_t* str;
    int      i;
    int64_t  i64;
    float    f;
    double   dbl;
  } u;
};

#define HAS_ARG    0x0001
#define OPT_BOOL   0x0002
#define OPT_EXPERT 0x0004
#define OPT_STRING 0x0008
#define OPT_VIDEO  0x0010
#define OPT_AUDIO  0x0020
#define OPT_INT    0x0080
#define OPT_FLOAT  0x0100
#define OPT_SUBTITLE 0x0200
#define OPT_INT64  0x0400
#define OPT_EXIT   0x0800
#define OPT_DATA   0x1000
#define OPT_PERFILE  0x2000     /* the option is per-file (currently ffmpeg-only).
                                   implied by OPT_OFFSET or OPT_SPEC */
#define OPT_OFFSET 0x4000       /* option is specified as an offset in a passed optctx */
#define OPT_SPEC   0x8000       /* option is to be stored in an array of SpecifierOpt.
                                   Implies OPT_OFFSET. Next element after the offset is
                                   an int containing element count in the array. */
#define OPT_TIME  0x10000
#define OPT_DOUBLE 0x20000
#define OPT_INPUT  0x40000
#define OPT_OUTPUT 0x80000

struct OptionDefinition {
  const char* name;
  int flags;
  union {
    void* dst_ptr;
    int (*func_arg)(void*, const char*, const char*);
    size_t off;
  } u;
  const char* help;
  const char* argname;
};

/**
 * An option extracted from the commandline.
 * Cannot use AVDictionary because of options like -map which can be 
 * used multiple times.
 */
struct Option {
  const OptionDefinition* opt;
  const char* key;
  const char* val;
};

struct OptionGroupDefinition {
  /* group name*/
  const char* name;

  /* Option to be used as group separator. Can be NULL for groups which
   * are terminated by a non-option argument. (e.g. ffmpeg output files)
   */
  const char* sep;

  /* Option flags that must be set on each option that is applied to this group */
  int flags;
};

struct OptionGroup {
  const OptionGroupDefinition* group_def;
  const char* arg;

  Option* opts;
  int nb_opts;

  AVDictionary* codec_opts;
  AVDictionary* format_opts;
  AVDictionary* resample_opts;
  AVDictionary* sws_dict;
  AVDictionary* swr_opts;
};

/**
 * A list of option groups that all have the same group type
 * (e.g. input files or output files)
 */
struct OptionGroupList {
  const OptionGroupDefinition* group_def;

  OptionGroup* groups;
  int          nb_groups;
};

struct OptionParseContext {
  OptionGroup global_opts;

  OptionGroupList* groups;
  int              nb_groups;

  // parsing state
  OptionGroup cur_group;
};


// Functions
void RegisterExit(void (*cb)(int ret));
void ExitProgram(int ret);
void InitOpts();
void UninitOpts();

void LogCallbackHelp(void* ptr, int level, const char* fmt, va_list vl);

int OptCpuFlags(void* optctx, const char* opt, const char* arg);
int OptDefault(void* optctx, const char* opt, const char* arg);
int OptLoglevel(void* optctx, const char* opt, const char* arg);
int OptReport(const char* opt);
int OptMaxAlloc(void* optctx, const char* opt, const char* arg);
int OptCodecDebug(void* optctx, const char* opt, const char* arg);
int OptTimeLimit(void* optctx, const char* opt, const char* arg);

/**
 * Parse a string and return its corresponding value as a double.
 *
 * @param context  the context of the value to be set
 * @param numstr   the string to be parsed
 * @param type     the type (OPT_INT64 or OPT_FLOAT)
 * @param min      the minimum valid accepted value
 * @param max      the maximum valid accepted value
 */
//TODO(wqx): Don't let me die
double ParseNumberOrDie(const char* context, 
                        const char* numstr, 
                        int type,
                        double min, double max);

/**
 * Parse a string specifying a time and return its corresponding value as
 * a number of microseconds.
 *
 * @param context  the context of the value to be set 
 * @param timestr  the string to be parsed
 * @param is_duration a flag which tells how to interpret timestr
 */
//TODO(wqx): Don't let me die
int64_t ParseTimeOrDie(const char* context, const char* timestr, int is_duration);

/**
 * Parse the command line arguments.
 *
 * @param optctx   an opaque options context
 * @param argc     number of command line arguments
 * @param argv     values of command line arguments
 * @param options  Array with the definitions required to interpret every option
 *                 of the form: -option_name [argument]
 * @param parse_arg_function  Name of the function called to process every argument
 *                 without a leading option name flag. NULL if such arguments do
 *                 not have to be processed.
 */  
void ParseOptions(void* optctx, 
                  int argc, 
                  char** argv, 
                  const OptionDefinition* options,
                  void (*parse_arg_function)(void* optctx, const char*));

/**
 * Parse one given option.
 *  
 * @return on success 1 if arg was consumed, 0 otherwise; negative number on error 
 */
int ParseOption(void* optctx, 
                const char* opt, 
                const char* arg, 
                const OptionDefinition* options);


int ParseOptGroup(void* optctx, OptionGroup* g);

/**
 * Split the commandline into an intermediate form convenient for further
 * processing.
 *
 * 
 */
int SplitCommandLine(OptionParseContext* ctx,
                     int argc,
                     char* argv[],
                     const OptionDefinition* options,
                     const OptionGroupDefinition* groups,
                     int nb_groups);

void UninitParseContext(OptionParseContext* ctx);
void ParseLoglevel(int argc, char** argv, const OptionDefinition* options);
int LocateOption(int argc, char** argv, const OptionDefinition* options,
                 const char* optname);

/**
 * Check if the given stream matches a stream specifier.
 *
 * @param format Corresponding format context.
 * @param stream Stream from stream to be checked.
 * @param spec   A stream specifier of the [v|a|s|d]:[\<stream index\>] form.
 *
 * @return 1 if the stream matches, 0 if it doesn't, <0 on error
 */
int CheckStreamSpecifier(AVFormatContext* format, 
                         AVStream* stream, 
                         const char* spec);

/**
 * Filter out options for given codec.
 *
 * Create a new options dictionary containing only the options from
 * opts which apply to the codec with ID codec_id.
 *
 * @param opts     dictionary to place options in
 * @param codec_id ID of the codec that should be filtered for
 * @param s Corresponding format context.
 * @param st A stream from s for which the options should be filtered.
 * @param codec The particular codec for which the options should be filtered.
 *              If null, the default one is looked up according to the codec id.
 * @return a pointer to the created dictionary
 */
AVDictionary* FilterCodecOpts(AVDictionary* opts,
                              enum AVCodecID codec_id,
                              AVFormatContext* s,
                              AVStream* stream,
                              AVCodec* codec);

/**
 * Setup AVCodecContext options for avformat_find_stream_info().
 *
 * Create an array of dictionaries, one dictionary for each stream
 * contained in s.
 * Each dictionary will contain the options from codec_opts which can
 * be applied to the corresponding stream codec context.
 *
 * @return pointer to the created array of dictionaries, NULL if it
 * cannot be created
 */
AVDictionary** SetupFindStreamInfoOpts(AVFormatContext* format,
                                       AVDictionary* codec_opts);


} // namespace media
#endif // C_COMMAND_LINE_H_
