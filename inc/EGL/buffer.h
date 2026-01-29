#ifndef EGL_BUFFER_H
#define EGL_BUFFER_H

#include <string>
#include <stdexcept>
#include <cstdint>
#include <vector>

namespace egl {

/**
 * @brief Enum to indicate the type of Buffer.
 */
enum class BufferType {
    Array,              ///< Vertex attributes
    AtomicCounter,      ///< Atomic counter storage
    CopyRead,           ///< Buffer copy source
    CopyWrite,          ///< Buffer copy destination
    DispatchIndirect,   ///< Indirect compute dispatch commands
    DrawIndirect,       ///< Indirect command arguments
    ElementArray,       ///< Vertex array indices
    PixelPack,          ///< Pixel read target
    PixelUnpack,        ///< Texture data source
    Query,              ///< Query result buffer
    ShaderStorage,      ///< Read-write storage for shaders
    Texture,            ///< Texture data buffer
    TransformFeedback,  ///< Transform feedback buffer
    Uniform             ///< Uniform block storage
};

/**
 * @brief Enum to indicate Buffer Usage.
 * 
 * The Usage of a buffer can be split into two parts as follows:
 * 
 * The frequency of usage may be one of these:
 *
 * Stream
 * - The data store contents will be modified once and used at most a few times.
 * 
 * Static
 * - The data store contents will be modified once and used many times.
 *
 * Dynamic
 * - The data store contents will be modified repeatedly and used many times.
 *
 * The nature of usage may be one of these:
 *
 * Draw
 * - The data store contents are modified by the application, and used as the source for GL drawing and image specification commands.
 *
 * Read
 * - The data store contents are modified by reading data from the GL, and used to return that data when queried by the application.
 *
 * Copy
 * - The data store contents are modified by reading data from the GL, and used as the source for GL drawing and image specification commands.
 * 
 */
enum class BufferUsage {
    StreamDraw,
    StreamRead,
    StreamCopy,
    StaticDraw,
    StaticRead,
    StaticCopy,
    DynamicDraw,
    DynamicRead,
    DynamicCopy
};

/**
 * @brief Enum flags to indicate Buffer map usage.
 */
enum class MapUsage : uint32_t {
    None                    = 0,
    Read                    = 1 << 0, ///< Indicates that the returned pointer may be used to read buffer object data.
    Write                   = 1 << 1, ///< Indicates that the returned pointer may be used to modify buffer object data.
    Persistent              = 1 << 2, ///< Indicates that the mapping is to be made in a persistent fassion and that the client intends to hold and use the returned pointer during subsequent GL operation.
    Coherent                = 1 << 3, ///< Indicates that a persistent mapping is also to be coherent. Coherent maps guarantee that the effect of writes to a buffer's data store by either the client or server will eventually become visible to the other without further intervention from the application.
    InvalidRange            = 1 << 4, ///< Indicates that the previous contents of the specified range may be discarded. Data within this range are undefined with the exception of subsequently written data.
    InvalidateBuffer        = 1 << 5, ///< Indicates that the previous contents of the entire buffer may be discarded. Data within the entire buffer are undefined with the exception of subsequently written data.
    FlushExplicit           = 1 << 6, ///< Indicates that one or more discrete subranges of the mapping may be modified. When this flag is set, modifications to each subrange must be explicitly flushed by calling glFlushMappedBufferRange.
    Unsynchronized          = 1 << 7  ///< Indicates that the GL should not attempt to synchronize pending operations on the buffer prior to returning from glMapBufferRange or glMapNamedBufferRange.
};

inline MapUsage operator|(MapUsage a, MapUsage b) {
    return static_cast<MapUsage>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline MapUsage operator&(MapUsage a, MapUsage b) {
    return static_cast<MapUsage>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

inline MapUsage& operator|=(MapUsage& a, MapUsage b) {
    a = a | b;
    return a;
}

/**
 * @brief Enum falgs for explicit Buffer usage in setStorage.
 */
enum class BufferFlag : uint32_t {
    None            = 0,
    DynamicStorage  = 1 << 0, ///< Indicates that the contents of the data store may be updated after creation through calls to glBufferSubData.
    MapRead         = 1 << 1, ///< Indicates that the data store may be mapped by the client for read access and a pointer in the client's address space obtained that may be read from.
    MapWrite        = 1 << 2, ///< Indicates that the data store may be mapped by the client for write access and a pointer in the client's address space obtained that may be written through.
    MapPersistent   = 1 << 3, ///< Indicates that the client may request that the server read from or write to the buffer while it is mapped. The client's pointer to the data store remains valid so long as the data store is mapped, even during execution of drawing or dispatch commands.
    MapCoherent     = 1 << 4, ///< Indicates thar shared access to buffers that are simultaneously mapped for client access and are used by the server will be coherent, so long as that mapping is performed using glMapBufferRange.
    ClientStorage   = 1 << 5  ///< When all other criteria for the buffer storage allocation are met, this bit may be used by an implementation to determine whether to use storage that is local to the server or to the client to serve as the backing store for the buffer.
};

inline BufferFlag operator|(BufferFlag a, BufferFlag b) {
    return static_cast<BufferFlag>(
        static_cast<uint32_t>(a) | static_cast<uint32_t>(b)
    );
}

inline BufferFlag operator&(BufferFlag a, BufferFlag b) {
    return static_cast<BufferFlag>(
        static_cast<uint32_t>(a) & static_cast<uint32_t>(b)
    );
}

inline BufferFlag& operator|=(BufferFlag& a, BufferFlag b) {
    a = a | b;
    return a;
}

unsigned int toGLenum(BufferType type);
unsigned int toGLenum(BufferUsage usage);
unsigned int toGLenum(MapUsage usage);
unsigned int toGLenum(BufferFlag flag);
bool validateMapUsage(MapUsage usage, std::string& error);
bool validateBufferFlag(BufferFlag flag, std::string& error);

class Buffer {
protected:
    unsigned int _id = 0;
    bool _mapped = false;
    MapUsage _mapUsage = MapUsage::None;
    BufferFlag _flags = BufferFlag::None;
    BufferType _type;

    void _delete();
    void _check();

public:
    Buffer() = delete;
    Buffer(BufferType type);
    Buffer(Buffer&& other);
    Buffer(const Buffer& other) = delete;
    ~Buffer() noexcept;

    void bind() const;
    int64_t size() const;

    BufferType getType() const;

    void setData(int64_t size, const void* data, BufferUsage usage); // size in bytes
    template <typename T>
    void setData(std::vector<T> data, BufferUsage usage) { setData(data.size() * sizeof(T), (void*)data.data(), usage); }

    void setStorage(int64_t size, const void* data, BufferFlag flags); // size in bytes
    template <typename T>
    void setStorage(std::vector<T> data, BufferFlag flags) { setStorage(data.size() * sizeof(T), (void*)data.data(), flags); }

    void setSubData(int64_t offset, int64_t size, const void* data); // offset and size in bytes

    void getSubData(int64_t offset, int64_t size, void* data); // data must be at least be as big as size

    void* map(int64_t offset, int64_t length, MapUsage access); // get a pointer to the buffer data for a specific usage
    void unmap();

    Buffer& operator=(Buffer&& other);
    Buffer& operator=(const Buffer& other) = delete;
};

}

#endif