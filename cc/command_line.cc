#include "cc/command_line.h"

namespace media {

CommandLine::CommandLine() {
  memset(&octx_, 0, sizeof(octx_));
}

CommandLine::~CommandLine() {}

/*
 *  prepare_app_arguments()
 *  init_parse_context(octx, groups, nb_groups)
 */
base::Status CommandLine::Initialize(int argc, char* argv[]) {
  (void) argc;
  (void) argv;
  return base::Status::OK();
}

/*
 * split_commandline
 * Parse
 */
base::Status CommandLine::Parse() {
  return base::Status::OK();
}

} // namespace media
