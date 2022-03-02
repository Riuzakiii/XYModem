#pragma once
#include <string>
#include <vector>
#include "fmt/core.h"
#include "fmt/ranges.h"

namespace xymodem
{
/** DeviceHandler
 */

struct DeviceHandler
{
public:
    /** Displays the content of the buffer
     */
    std::string showBuffer ();

    /** Write data to the device output buffer
     *  @param data the data bytes to write
     *  @param size number of bytes to write
     */
    virtual size_t write (const uint8_t* data, size_t size) = 0;

    /**
     * @brief Read all available characters and put them in inputBuffer
     */
    virtual void readAll () = 0;

    /** Read next byte from front of inputBuffer and removes it from the buffer
     */
    [[nodiscard]] uint8_t readNextByte ();

    /** Flush inputBuffer
     */
    virtual void flushLocalBuffer ();
    /** Flush the device buffer by reading all its buffer.
     */
    virtual void flushDeviceInputBuffer () = 0;
    /** Uses flushLocalBuffer and flushDeviceBuffer to flush all buffers.
     * @see flushLocalBuffer
     * @see flushDeviceInputBuffer
     */
    void flushAllInputBuffers ();

    [[nodiscard]] size_t getInputBufferSize ();
    [[nodiscard]] bool isInputBufferEmpty () const;
    [[nodiscard]] uint8_t getInputBufferFront () const;

    /**
     * @brief Return the number of available bytes in the device's buffer
     * @return size_t 
     */
    [[nodiscard]] virtual size_t available () const = 0;

    /**
     * @brief Wait until a byte is received
     * @return true if there was not interruption
     */
    virtual bool waitReadable() const = 0;

protected:
    DeviceHandler () = default;
    std::vector<uint8_t> inputBuffer;
};
}
