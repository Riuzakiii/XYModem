#pragma once
#include <string>

namespace xymodem
{
/** This class represents a file. Derived classes have to be made to handle the file depending on the platform.
 */
class File
{
public:
    File() = default;
    virtual ~File() = default;

    /** Read the next blockSize bytes from the data file
     * @param blockSizeBytes Block size in bytes
     * @return The bytes read from the file in a std::string
     */
    [[nodiscard]] virtual std::string getNextFileBlock(const std::intmax_t blockSizeBytes) = 0;

    [[nodiscard]] virtual std::string getFilename() const;
    [[nodiscard]] virtual std::intmax_t getFilesize() const;
    [[nodiscard]] virtual std::intmax_t getLastModificationDate() const;

    /** Erase the content of the file.
     */
    virtual void erase();

    /** Must be true for getNextFileBlock to work.
     */
    [[nodiscard]] virtual bool isOpened() const = 0;

    /** Prepare the file for reading/writing operations.
     */

    virtual void open();

    /** Release access to the file.
     */
    virtual void close();
protected:
    // No copies allowed
    File(const File&) = delete;
    std::string filename;
    std::intmax_t filesize = 0;
    std::intmax_t lastModificationDate = 0;
};
}
