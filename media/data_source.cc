#include "media/data_source.h"

#include <glog/logging.h>

namespace media {

// static
const int DataSource::kReadError = -1;

DataSourceHost::~DataSourceHost() {}

DataSource::DataSource() : host_(NULL) {}

DataSource::~DataSource() {}

void DataSource::set_host(DataSourceHost* host) {
  DCHECK(host);
  DCHECK(!host_);
  host_ = host;
}

void DataSource::SetPlaybackRate(float playback_rate) {
  (void) playback_rate;
}

DataSourceHost* DataSource::host() { return host_; }

}  // namespace media
