#include "irqn.h"

/**
 * @brief IRQn auto-assignment bitfield
 * @note each IRQn is represented by a single bit
 */
uint8_t irqn_bitfield[IRQN_AA_AVAILABLE_COUNT / 8] = {0};

#define IRQN_BITFIELD_GET_FIELD_INDEX(irqn) (irqn / 8)
#define IRQN_BITFIELD_GET_FIELD_BIT(irqn) (irqn % 8)
#define IRQN_BITFIELD_GET_FIELD_MASK(irqn) (1 << IRQN_BITFIELD_GET_FIELD_BIT(irqn))

/**
 * @brief get IRQn bitfield value
 */
inline bool irqn_bitfield_get(size_t irqn_index)
{
    return (irqn_bitfield[IRQN_BITFIELD_GET_FIELD_INDEX(irqn_index)] & IRQN_BITFIELD_GET_FIELD_MASK(irqn_index)) != 0;
}

/**
 * @brief set IRQn bitfield value
 */
inline void irqn_bitfield_set(size_t irqn_index, bool value)
{
    if (value)
    {
        irqn_bitfield[IRQN_BITFIELD_GET_FIELD_INDEX(irqn_index)] |= IRQN_BITFIELD_GET_FIELD_MASK(irqn_index);
    }
    else
    {
        irqn_bitfield[IRQN_BITFIELD_GET_FIELD_INDEX(irqn_index)] &= ~IRQN_BITFIELD_GET_FIELD_MASK(irqn_index);
    }
}

/**
 * @brief get next available IRQn
 * @param irqn_index next free irqn index. only valid if function returns true
 * @return true if next free irqn was found, false otherwise
 */
inline bool irqn_bitfield_next(size_t &irqn_index)
{
    for (size_t i = 0; i < IRQN_AA_AVAILABLE_COUNT; i++)
    {
        if (!irqn_bitfield_get(i))
        {
            irqn_index = i;
            return true;
        }
    }

    return false;
}

/**
 * @brief translate irqn bitfield index to IRQn
 * @param irqn_index irqn bitfield index
 * @return IRQn number
 */
inline IRQn_Type irqn_bitfield_index_to_irqn(size_t irqn_index)
{
    // offset IRQn index by first irqn number
    irqn_index += IRQN_AA_FIRST_IRQN;

    // assert that irqn is in range
    CORE_ASSERT(irqn_index >= IRQN_AA_FIRST_IRQN && irqn_index <= IRQN_AA_LAST_IRQN, "irqn out of range");

    // return index as IRQn
    return (IRQn_Type)irqn_index;
}

/**
 * @brief translate IRQn to irqn bitfield index
 * @param irqn IRQn number
 * @return irqn bitfield index
 */
inline size_t irqn_to_bitfield_index(IRQn_Type irqn)
{
    // offset IRQn index by first irqn number
    size_t irqn_index = (size_t)irqn;
    irqn_index -= IRQN_AA_FIRST_IRQN;

    // assert that irqn is in range
    CORE_ASSERT(irqn_index >= 0 && irqn_index < IRQN_AA_AVAILABLE_COUNT, "irqn index out of range");

    // return IRQn bitfield index
    return irqn_index;
}

//
// IRQn auto-assignment public API
//
en_result_t _irqn_aa_get(IRQn_Type &irqn)
{
    // get next available IRQn index
    size_t irqn_index;
    if (!irqn_bitfield_next(irqn_index))
    {
        // no more IRQn available
        return Error;
    }

    // set IRQn bitfield value to true
    irqn_bitfield_set(irqn_index, true);

    // translate IRQn index to IRQn and done
    irqn = irqn_bitfield_index_to_irqn(irqn_index);
    return Ok;
}

en_result_t _irqn_aa_resign(IRQn_Type &irqn)
{
    // translate IRQn to IRQn index
    size_t irqn_index = irqn_to_bitfield_index(irqn);

    // set IRQn bitfield value to false
    irqn_bitfield_set(irqn_index, false);

    // done
    return Ok;
}
