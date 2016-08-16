#include "media/bit_reader.h"

#include <gtest/gtest.h>

namespace media {

TEST(BitReaderTest, NormalOperationTest) {
  uint8_t value8;
  uint64_t value64;
  // 0101 0101 1001 1001 repeats 4 times
  uint8_t buffer[] = {0x55, 0x99, 0x55, 0x99, 0x55, 0x99, 0x55, 0x99};
  BitReader reader1(buffer, 6);  // Initialize with 6 bytes only

  EXPECT_TRUE(reader1.ReadBits(1, &value8));
  EXPECT_EQ(value8, 0);
  EXPECT_TRUE(reader1.ReadBits(8, &value8));
  EXPECT_EQ(value8, 0xab);  // 1010 1011
  EXPECT_TRUE(reader1.ReadBits(7, &value64));
  EXPECT_TRUE(reader1.ReadBits(32, &value64));
  EXPECT_EQ(value64, 0x55995599u);
  EXPECT_FALSE(reader1.ReadBits(1, &value8));
  value8 = 0xff;
  EXPECT_TRUE(reader1.ReadBits(0, &value8));
  EXPECT_EQ(value8, 0);

  BitReader reader2(buffer, 8);
  EXPECT_TRUE(reader2.ReadBits(64, &value64));
  EXPECT_EQ(value64, 0x5599559955995599ull);
  EXPECT_FALSE(reader2.ReadBits(1, &value8));
  EXPECT_TRUE(reader2.ReadBits(0, &value8));
}

TEST(BitReaderTest, ReadBeyondEndTest) {
  uint8_t value8;
  uint8_t buffer[] = {0x12};
  BitReader reader1(buffer, sizeof(buffer));

  EXPECT_TRUE(reader1.ReadBits(4, &value8));
  EXPECT_FALSE(reader1.ReadBits(5, &value8));
  EXPECT_FALSE(reader1.ReadBits(1, &value8));
  EXPECT_TRUE(reader1.ReadBits(0, &value8));
}

}  // namespace media
