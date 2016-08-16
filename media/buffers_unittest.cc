#include "base/string_util.h"
#include "media/buffers.h"
#include <gtest/gtest.h>

namespace media {

namespace {

// Simple implementation of Buffer to test base class functionality.
class TestBuffer : public Buffer {
 public:
  TestBuffer()
      : Buffer(base::TimeDelta(), base::TimeDelta()) {
  }

  // Sets |data_| and |size_| members for testing purposes. Does not take
  // ownership of |data|.
  TestBuffer(const uint8_t* data, int size)
      : Buffer(base::TimeDelta(), base::TimeDelta()),
        data_(data),
        size_(size) {
  }

  // Buffer implementation.
  virtual const uint8_t* GetData() const override { return data_; }
  virtual int GetDataSize() const override { return size_; }

 protected:
  virtual ~TestBuffer() {}

 private:
  const uint8_t* data_;
  int size_;

  DISALLOW_COPY_AND_ASSIGN(TestBuffer);
};

}  // namespace

TEST(BufferTest, Timestamp) {
  const base::TimeDelta kZero;
  const base::TimeDelta kTimestampA = base::TimeDelta::FromMicroseconds(1337);
  const base::TimeDelta kTimestampB = base::TimeDelta::FromMicroseconds(1234);

  scoped_ref_ptr<TestBuffer> buffer = new TestBuffer();
  EXPECT_TRUE(buffer->GetTimestamp() == kZero);

  buffer->SetTimestamp(kTimestampA);
  EXPECT_TRUE(buffer->GetTimestamp() == kTimestampA);

  buffer->SetTimestamp(kTimestampB);
  EXPECT_TRUE(buffer->GetTimestamp() == kTimestampB);
}

TEST(BufferTest, Duration) {
  const base::TimeDelta kZero;
  const base::TimeDelta kDurationA = base::TimeDelta::FromMicroseconds(1337);
  const base::TimeDelta kDurationB = base::TimeDelta::FromMicroseconds(1234);

  scoped_ref_ptr<TestBuffer> buffer = new TestBuffer();
  EXPECT_TRUE(buffer->GetDuration() == kZero);

  buffer->SetDuration(kDurationA);
  EXPECT_TRUE(buffer->GetDuration() == kDurationA);

  buffer->SetDuration(kDurationB);
  EXPECT_TRUE(buffer->GetDuration() == kDurationB);
}

TEST(BufferTest, IsEndOfStream) {
  const uint8_t kData[] = { 0x00, 0xFF };
  const int kDataSize = arraysize(kData);

  scoped_ref_ptr<TestBuffer> buffer = new TestBuffer(NULL, 0);
  EXPECT_TRUE(buffer->IsEndOfStream());

  buffer = new TestBuffer(kData, kDataSize);
  EXPECT_FALSE(buffer->IsEndOfStream());
}

}  // namespace media
