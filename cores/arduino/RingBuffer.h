#pragma once
#include <stdint.h>

/**
 * @brief generic ring buffer
 * @note in scenarios where there is always one consumer and one producer, the buffer is thread safe (and thus may be used from within an interrupt handler)
 */
template <typename TElement>
class RingBuffer
{
public:
    /**
     * @brief Construct a new Ring Buffer object with the given capacity
     */
    RingBuffer(size_t capacity)
    {
        this->buffer = new TElement[capacity];
        this->_capacity = capacity;
        clear();
    }

    /**
     * @brief Construct a new Ring Buffer object with the given buffer and capacity
     * @note the buffer must be at least the size of the capacity
     * @note the buffer will be freed when the RingBuffer is destroyed
     */
    RingBuffer(TElement *buffer, size_t capacity)
    {
        this->buffer = buffer;
        this->_capacity = capacity;
        clear();
    }

    /**
     * @brief Destroy the Ring Buffer object, freeing the buffer
     */
    ~RingBuffer()
    {
        delete[] this->buffer;
    }

    /**
     * @brief Get the number of elements in the buffer
     */
    size_t count()
    {
        return this->_count;
    }

    /**
     * @brief Get the capacity of the buffer
     */
    size_t capacity()
    {
        return this->_capacity;
    }

    /**
     * @brief Test if the buffer is full
     */
    bool isFull()
    {
        return this->count() >= this->capacity();
    }

    /**
     * @brief Test if the buffer is empty
     */
    bool isEmpty()
    {
        return this->count() == 0;
    }

    /**
     * @brief Get the next element in the buffer without removing it
     */
    TElement peek()
    {
        return this->isEmpty() ? 0 : this->buffer[this->_ri];
    }

    /**
     * @brief Push an element onto the buffer
     * @param element the element to push
     * @param force if true, a element will be overwritten case the buffer is full
     * @return true if the element was pushed, false if the buffer is full
     */
    bool push(TElement element, bool force = false)
    {
        if (!force && this->isFull())
        {
            return false;
        }

        // add the new element
        this->_push(element);
        return true;
    }

    /**
     * @brief Pop an element from the buffer
     * @param element the element to pop
     * @return true if the element was popped, false if the buffer is empty
     */
    bool pop(TElement &element)
    {
        if (this->isEmpty())
        {
            return false;
        }

        element = this->_pop();
        return true;
    }

    /**
     * @brief Clear the buffer
     */
    void clear()
    {
        this->_count = 0;
        this->_wi = 0;
        this->_ri = 0;
    }

    /**
     * @brief update the write index and count
     * @note this is a internal operation that is made public to allow for DMA transfers into the buffer
     */
    void _update_write_index(size_t writtenCount)
    {
        this->_wi = (this->_wi + writtenCount) % (this->_capacity);

        // increment count atomically
        __sync_fetch_and_add(&this->_count, writtenCount);
    }

    /**
     * @brief get the internal data buffer
     * @note this is a internal operation that is made public to allow for DMA transfers into the buffer
     */
    volatile uint8_t *getBuffer()
    {
        return this->buffer;
    }

private:
    void _push(TElement element)
    {
        this->buffer[this->_wi] = element;
        _update_write_index(1);
    }

    TElement _pop()
    {
        TElement element = this->buffer[this->_ri];
        this->_ri = (this->_ri + 1) % (this->_capacity);

        // decrement count atomically
        __sync_fetch_and_sub(&this->_count, 1);
        return element;
    }

    /**
     * @brief the data buffer
     */
    volatile uint8_t *buffer;

    /**
     * @brief the length of the data buffer
     */
    volatile size_t _capacity;

    /**
     * @brief the number of elements in the buffer
     */
    volatile size_t _count;

    /**
     * @brief the write index
     * @note increment after push
     */
    volatile size_t _wi;

    /**
     * @brief the read index
     * @note increment after pop
     */
    volatile size_t _ri;
};
