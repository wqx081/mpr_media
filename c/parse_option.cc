#include "c/command_line.h"

namespace media {

enum OptGroup {
  GROUP_OUTFILE = 0,
  GROUP_INFILE,
};

static const OptionGroupDefinition kGroups[] = {
  {"output file", nullptr, OPT_OUTPUT },  
  {"input file", "i", OPT_INPUT },
};

bool FFmpegParseOptions(int argc, char** argv) {
  (void) argc;
  (void) argv;

  OptionParseContext option_parse_context;
  
  memset(&option_parse_context, 0, sizeof(option_parse_context));
  //
  // Split the commandline into an internal representation
  // ret = SplitCommandLine(&option_parse_context, 
  //                        argc, argv,
  //                        kGroups, FF_ARRAY_ELEMS(kGroups));
  //                        

  // Apply global options
  // ParseOptGroup(nullptr, &option_parse_context.global_opts);

  // Open input files
  // OpenFiles(&option_parse_context.groups[GROUP_INFILE], "input", OpenInputFile);

  // Create the complex filtergraphs
  // InitComplexFilters();

  // Open Output Files
  // OpenFiles(&option_parse_context.groups[GROUP_OUTFILE], "output", OpenOutputFile);

  // Configure the complex filtergraphs
  // ConfigureComplexFilters();
  //
  return true;
}

} // namespace media
