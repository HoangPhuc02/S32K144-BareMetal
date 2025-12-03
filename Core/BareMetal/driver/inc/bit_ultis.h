
/*
 * bit_utils.h
 *
 */

#ifndef BIT_UTILS_H_
#define BIT_UTILS_H_

/**
 * @brief Read a single bit from a register.
 *
 * @param reg Register to read from.
 * @param pos Bit position (0 = LSB).
 * @return 1 if the bit is set, 0 otherwise.
 */
#define READ_BIT(reg, pos)    (((reg) >> (pos)) & 0x1U)

/**
 * @brief Set a single bit in a register.
 *
 * @param reg Register to modify.
 * @param pos Bit position to set.
 */
#define SET_BIT(reg, pos)     ((reg) |= (1U << (pos)))

/**
 * @brief Clear a single bit in a register.
 *
 * @param reg Register to modify.
 * @param pos Bit position to clear.
 */
#define CLEAR_BIT(reg, pos)   ((reg) &= ~(1U << (pos)))

/**
 * @brief Toggle a single bit in a register.
 *
 * @param reg Register to modify.
 * @param pos Bit position to toggle.
 */
#define TOGGLE_BIT(reg, pos)  ((reg) ^= (1U << (pos)))

/**
 * @brief Write multiple bits in a register using a mask.
 *
 * This macro clears the bits specified by the mask and writes the new value.
 *
 * @param reg Register to modify.
 * @param mask Bit mask specifying which bits to update.
 * @param value New value to write (masked).
 */
#define WRITE_BITS(reg, mask, value) \
    ((reg) = ((reg) & ~(mask)) | ((value) & (mask)))

/**
 * @brief Read multiple bits from a register using a mask.
 *
 * @param reg Register to read from.
 * @param mask Bit mask specifying which bits to read.
 * @return The masked value.
 */
#define READ_BITS(reg, mask)  ((reg) & (mask))

#endif /* BIT_UTILS_H_ */

