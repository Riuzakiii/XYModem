#pragma once
#ifdef TESTING_ENABLED
#include "gtest/gtest_prod.h"
#endif
#include "Devices/DeviceHandler.h" // for DeviceHandler
#include <memory>                  // for shared_ptr
#include <stddef.h>                // for size_t
#include <stdint.h>                // for uint32_t, uint8_t
namespace serial
{
class Serial;
} // namespace serial

namespace xymodem
{
/** DeviceHandler
 */

class SerialHandler : public DeviceHandler
{
public:
    explicit SerialHandler (std::shared_ptr<serial::Serial>& serialDevice_);
    SerialHandler (uint32_t baudrate,
                   uint32_t interByteTimeout,
                   uint32_t readTimeoutConstant,
                   uint32_t readTimeoutMultiplier,
                   uint32_t writeTimeoutConstant,
                   uint32_t writeTimeoutMultiplier);

    /** Write data to the device output buffer
     *  @param data the data bytes to write
     *  @param size number of bytes to write
     */
    size_t write (const uint8_t* data, size_t size) override;
    /** Empty the device input buffer and transfer its content to buffer.
     * If an ACK is in the buffer, the characters before it will be ignored.
     * Indeed, ACK is always the first thing we want if the device sent it.
     */
    void readAll() override;

    /** NOTE ON BUFFERS : buffers must be handled really carefully with serial.
     * Reading bytes one by one from the device buffer did not seem to gave
     * the actual buffer content, it is better to empty the buffer entirely each
     * time you access it. Flushing the device output/input buffer is apparently
     * bugged on Windows, flushInputBuffer() does the same by reading the entire
     * device input buffer. Flushing is done before writing to the device : when
     * done "immediately" after, the device feedback could have already been
     * received in the buffer and thus flushed.
     */

    /** Flush the device buffer by reading all its buffer. The function from the
     * serial lib was not working on Windows.
     */
    void flushDeviceInputBuffer() override;

    /**
     * @brief Return the number of available bytes in the device's buffer
     * @return size_t
     */
    [[nodiscard]] size_t available() const override;

    bool waitReadable() const override;

protected:
private:
    std::shared_ptr<serial::Serial> serialDevice;

#ifdef TESTING_ENABLED
    FRIEND_TEST (TestXYModemHelper, TestFlushLocalBuffer);
    FRIEND_TEST (TestXYModemHelper, TestReadNextByte);
#endif
};
} // namespace xymodem
