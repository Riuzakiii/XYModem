#pragma once
#include <stddef.h> // for size_t
#include <stdint.h> // for uint8_t
#include <string>
#include <vector>

namespace xymodem
{
/** DeviceHandler
 */

struct DeviceHandler
{
public:
    virtual ~DeviceHandler() = default;

    /** Write data to the device output buffer
     *  @param data the data bytes to write
     *  @param size number of bytes to write
     */
    virtual size_t write (const uint8_t* data, size_t size) = 0;

    /** Real n bytes from buffer
     */
    [[nodiscard]] virtual std::vector<uint8_t> read (int n) = 0;

    /** Flush the device buffer
     */
    virtual void flushInputBuffer() = 0;

    /**
     * @brief Return the number of available bytes in the device's buffer
     * @return size_t
     */
    [[nodiscard]] virtual size_t available() const = 0;

    /**
     * @brief Wait until a byte is received
     * @return true if there was not interruption
     */
    virtual bool waitReadable() const = 0;

protected:
    DeviceHandler() = default;
};
} // namespace xymodem
