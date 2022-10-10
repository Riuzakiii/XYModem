#pragma once
#include <array>
#include <vector>
#include <algorithm>

namespace xymodem
{
template<typename T, std::size_t size>
class RingBuffer
{
public:
    RingBuffer() = default;

    int write(const std::vector<T>& values)
    {
        int available1 = writeHead >= readHead ? size - writeHead : readHead - writeHead; //after writingHead
        int available2 = writeHead >= readHead ? readHead : 0; //before writingHead
        int written = 0;

        if(available1 + available2 >= values.size())
        {
            int write1 = 0;
            int write2 = 0;
            if(available1 > 0)
            {
                write1 = std::min(static_cast<int>(values.size()), available1);
                std::copy(values.data(), std::next(values.data(), write1), std::next(buffer.data(), writeHead));
                writeHead += write1;
                writeHead %= size;
                written += write1;
            }


            if(available2 > 0 && written < values.size())
            {
                write2 = std::min(static_cast<int>(values.size()) - written, available2);
                std::copy(std::next(values.data(), written), std::next(values.data(), write1 + write2), std::next(buffer.data(), writeHead));
                writeHead += write2;
                writeHead %= size;
                written += write2;
            }
            return written;
        }
        return 0;
    }

    std::vector<T> read(int num = 1)
    {
        int available1 = readHead >= writeHead ? size - readHead : writeHead - readHead; //after readHead
        int available2 = readHead >= writeHead ? writeHead : 0; //before readHead
        int read = 0;

        if(available1 + available2 > 0)
        {
            std::vector<T> result(num);
            int read1 = 0;
            int read2 = 0;

            if(available1 > 0)
            {
                read1 = std::min(num, available1);
                std::copy(std::next(buffer.data(), readHead), std::next(buffer.data(), readHead + read1), result.data());
                readHead += read1;
                readHead %= size;
                read += read1;
            }

            if(available2 > 0)
            {
                read2 = std::min(num - read1, available2);
                std::copy(std::next(buffer.data(), readHead), std::next(buffer.data(), readHead + read2), std::next(result.data(), read1));
                readHead += read2;
                readHead %= size;
                read += read2;
            }
            result.resize(read);
            return result;
        }
        return {};
    }

    int available()
    {
        return writeHead >= readHead ? (writeHead-readHead) : size - readHead + writeHead;
    }
private:
    std::array<T, size> buffer;
    int writeHead = 0;
    int readHead = 0;
};
}
