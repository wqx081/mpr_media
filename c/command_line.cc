#include "c/command_line.h"

#include <glog/logging.h>

namespace media {

namespace {

void InitParseContext(OptionParseContext* context,
                      const OptionGroupDefinition* groups,
                      int nb_groups) {
  static const OptionGroupDefinition global_group = {"global", "", 0}; 
  memset(context, 0, sizeof(*context));
  context->nb_groups = nb_groups;
  context->groups    = static_cast<OptionGroupList *>(av_mallocz_array(context->nb_groups,
                                        sizeof(*context->groups)));
  DCHECK(context->groups);
  for (int i = 0; i < context->nb_groups; ++i) {
    context->groups[i].group_def = &groups[i];
  }
  context->global_opts.group_def = &global_group;
  context->global_opts.arg       = "";

  InitOpts();
}

} // namespace init
int SplitCommandLine(OptionParseContext* context,
                     int argc,
                     char* argv[],
                     const OptionDefinition* options,
                     const OptionGroupDefinition* groups,
                     int nb_groups) {
  
  return true;
}

} // namespace media
