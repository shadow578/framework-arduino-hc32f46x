#include "../test.h"
#include <RingBuffer.h>

/**
 * test ring buffer initializes the internal buffer and is able to free it
 */
TEST(RingBuffer, SmokeTest)
{
  auto rb = new RingBuffer<uint8_t>(10);

  EXPECT_EQ(rb->capacity(), 10) << "Capacity should be 10";
  EXPECT_EQ(rb->count(), 0) << "Count should be 0 after initialization";
  EXPECT_TRUE(rb->isEmpty()) << "Buffer should be empty after initialization";
  EXPECT_FALSE(rb->isFull()) << "Buffer should not be full after initialization";

  EXPECT_EQ(rb->peek(), 0) << "Peek should return 0 when buffer is empty";

  uint8_t dummy;
  EXPECT_FALSE(rb->pop(dummy)) << "Pop should return false when buffer is empty";

  delete rb;
}

/**
 * test ring buffer pushes, peeks, and pops elements
 */
TEST(RingBuffer, PushPeekPop)
{
  auto rb = new RingBuffer<uint8_t>(10);

  for (uint8_t i = 0; i < 10; i++)
  {
    EXPECT_TRUE(rb->push(i)) << "Push should return true when buffer is not full";
    EXPECT_EQ(rb->count(), i + 1) << "Count should increase after push";
  }

  EXPECT_TRUE(rb->isFull()) << "Buffer should be full after 10 pushes";
  EXPECT_FALSE(rb->isEmpty()) << "Buffer should not be empty after 10 pushes";

  for (uint8_t i = 0; i < 10; i++)
  {
    EXPECT_EQ(rb->peek(), i) << "Peek should return the pushed value";

    uint8_t actual;
    EXPECT_TRUE(rb->pop(actual)) << "Pop should return true when buffer is not empty";
    EXPECT_EQ(actual, i) << "Pop should return the pushed value";
  }

  EXPECT_FALSE(rb->isFull()) << "Buffer should not be full after 10 pops";
  EXPECT_TRUE(rb->isEmpty()) << "Buffer should be empty after 10 pops";

  delete rb;
}

/**
 * test clear empties the buffer
 */
TEST(RingBuffer, Clear)
{
  auto rb = new RingBuffer<uint8_t>(10);

  for (uint8_t i = 0; i < 10; i++)
  {
    rb->push(i);
  }

  EXPECT_TRUE(rb->isFull()) << "Buffer should be full after 10 pushes";
  EXPECT_FALSE(rb->isEmpty()) << "Buffer should not be empty after 10 pushes";

  rb->clear();

  EXPECT_FALSE(rb->isFull()) << "Buffer should not be full after clear";
  EXPECT_TRUE(rb->isEmpty()) << "Buffer should be empty after clear";

  delete rb;
}

/**
 * test ring buffer does not overwrite elements when not forced
 */
TEST(RingBuffer, DontOverwriteIfNotForced)
{
  auto rb = new RingBuffer<uint8_t>(4);

  for (uint8_t i = 0; i < 4; i++)
  {
    EXPECT_TRUE(rb->push(i)) << "Push should return true when buffer is not full";
  }

  EXPECT_EQ(rb->count(), 4) << "Count should be 4 after 4 pushes";
  EXPECT_TRUE(rb->isFull()) << "Buffer should be full after 4 pushes";
  EXPECT_FALSE(rb->isEmpty()) << "Buffer should not be empty after 4 pushes";

  // buffer is now full, pushing should return false
  EXPECT_FALSE(rb->push(9)) << "Push should return false when buffer is full and not forced";

  // buffer should still have 4 elements
  EXPECT_EQ(rb->count(), 4) << "Count should be 4 after failed push";

  uint8_t actual;
  for (const uint8_t expected : {0, 1, 2, 3})
  {
    EXPECT_TRUE(rb->pop(actual)) << "Pop should return true when buffer is not empty";
    EXPECT_EQ(actual, expected) << "Pop should return the pushed value";
  }

  delete rb;
}

/**
 * test ring buffer overwrites elements when forced
 */
TEST(RingBuffer, OverwriteIfForced)
{
  auto rb = new RingBuffer<uint8_t>(4);

  for (uint8_t i = 0; i < 4; i++)
  {
    EXPECT_TRUE(rb->push(i)) << "Push should return true when buffer is not full";
  }

  EXPECT_EQ(rb->count(), 4) << "Count should be 4 after 4 pushes";
  EXPECT_TRUE(rb->isFull()) << "Buffer should be full after 4 pushes";
  EXPECT_FALSE(rb->isEmpty()) << "Buffer should not be empty after 4 pushes";

  // buffer is now full, pushing with force should return true and overwrite the oldest element
  EXPECT_TRUE(rb->push(8, /*force*/ true)) << "Push should return true when buffer is full and forced";

  // pushing should also set 'didOverrun' to true
  bool didOverrun;
  EXPECT_TRUE(rb->push(9, /*force*/ true, didOverrun)) << "Push should return true when buffer is full and forced";
  EXPECT_TRUE(didOverrun) << "Push should set didOverrun to true when buffer is full and forced";

  // buffer should still have 4 elements
  EXPECT_EQ(rb->count(), 4) << "Count should be 4 after forced push";

  uint8_t actual;
  for (const uint8_t expected : {2, 3, 8, 9})
  {
    EXPECT_TRUE(rb->pop(actual)) << "Pop should return true when buffer is not empty";
    EXPECT_EQ(actual, expected) << "Pop should return the pushed value";
  }

  delete rb;
}


/**
 * test _update_write_index updates the write index and count normally
 * and when overrunning 
 */
TEST(RingBuffer, UpdateWriteIndex)
{
  auto rb = new RingBuffer<uint8_t>(4);

  // writing 4 elements with _update_write_index should increase the count to 4
  // it should also not overrun, so should return false
  EXPECT_FALSE(rb->_update_write_index(4)) << "Update write index should not overrun when writing 4 elements";
  EXPECT_EQ(rb->count(), 4) << "Count should be 4 after writing 4 elements";

  // after writing 1 more element with _update_write_index:
  // - the count() remains at 4
  // - the function should return true, indicating that it overran
  EXPECT_TRUE(rb->_update_write_index(1)) << "Update write index should overrun when writing 1 more element";
  EXPECT_EQ(rb->count(), 4) << "Count should still be 4 after writing 1 more element with overrun";

  delete rb;
}
