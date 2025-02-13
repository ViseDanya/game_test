#include <vector>

class StreamBuffer
{
    public:
    StreamBuffer(std::vector<std::byte>& buffer) : buffer(buffer) {}; 

    template <typename T>
    void write(const T& input) 
    {
        const std::byte* inputAsBytes = reinterpret_cast<const std::byte*>(&input);
        buffer.insert(buffer.end(), inputAsBytes, inputAsBytes + sizeof(T));
    }

    private:
    std::vector<std::byte>& buffer;
};