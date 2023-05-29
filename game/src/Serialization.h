//
// Created by I3artek on 27/05/2023.
//
//https://gafferongames.com/post/reading_and_writing_packets/
//https://gafferongames.com/post/serialization_strategies/

#ifndef SPLATTER_SERIALIZATION_H
#define SPLATTER_SERIALIZATION_H

#include <cstdint>
#include <cstring>
#include <cmath>
#include <vector>
#include <errno.h>

#define REALLOC_C 2

inline int max_i(int a, int b) { return a > b ? a : b; }

#define check_for_space(buffer, b_size, required)                   \
    if(b_size < required)                                           \
        {                                                           \
            size_t new_size = max_i(REALLOC_C * b_size, required);  \
            buffer = (uint32_t*)realloc(buffer, new_size);          \
            if(ENOMEM == errno)                                     \
            return false;                                           \
            b_size = new_size;                                      \
        }

#define check_for_enough_data(available, required)                  \
    if(available < required)                                        \
    {                                                               \
        return false;                                               \
    }

#define simple_assert(cond) if(!(cond)) return false;

#define serialize_int32( stream, value)                             \
    do                                                              \
    {                                                               \
        int32_t int32_value;                                        \
        if ( stream->IsWriting )                                    \
        {                                                           \
            int32_value = (int32_t) value;                          \
        }                                                           \
        if ( !stream->SerializeInt32( int32_value ) )               \
        {                                                           \
            return false;                                           \
        }                                                           \
        if ( stream->IsReading )                                    \
        {                                                           \
            value = int32_value;                                    \
        }                                                           \
     } while (0)

#define serialize_uint32( stream, value)                             \
    do                                                              \
    {                                                               \
        uint32_t uint32_value;                                        \
        if ( stream->IsWriting )                                    \
        {                                                           \
            uint32_value = (uint32_t) value;                          \
        }                                                           \
        if ( !stream->SerializeUInt32( uint32_value ) )               \
        {                                                           \
            return false;                                           \
        }                                                           \
        if ( stream->IsReading )                                    \
        {                                                           \
            value = uint32_value;                                    \
        }                                                           \
     } while (0)

#define serialize_float32( stream, value )                          \
  do                                                                \
  {                                                                 \
      if ( !serialize_float_internal( stream, value ) )             \
      {                                                             \
          return false;                                             \
      }                                                             \
} while (0)

#define serialize_char_vector( stream, vector, bytes )              \
  do                                                                \
    {                                                               \
        uint32_t size;                                              \
        if ( stream->IsWriting )                                    \
        {                                                           \
            size = vector.size();                                   \
        }                                                           \
        if ( !stream->SerializeUInt32( size ) )                     \
        {                                                           \
            return false;                                           \
        }                                                           \
        if ( !stream->SerializeCharVector( vector, size ))          \
        {                                                           \
            return false;                                           \
        }                                                           \
                                                                    \
     } while (0)

#define serialize_vector( stream, vector )                          \
    do                                                              \
        {                                                           \
        uint32_t size;                                              \
        if(stream->IsWriting)                                       \
        {                                                           \
            size = vector.size();                                   \
        }                                                           \
        serialize_int32(stream, size);                              \
        if(stream->IsReading)                                       \
        {                                                           \
            if(vector.capacity() < size)                            \
            {                                                       \
            vector.resize(size);                                    \
            }                                                       \
        }                                                           \
        for(int i = 0; i < size; i++)                               \
        {                                                           \
            if (stream->IsWriting)                                  \
            {                                                       \
                vector.push_back({});                               \
            }                                                       \
            if(!vector[i].Serialize(stream))                        \
            {                                                       \
                return false;                                       \
            }                                                       \
        }                                                           \
    } while(0)

template <typename Stream>
bool serialize_float_internal( Stream & stream,
                               float & value )
{
    union FloatInt
    {
        float float_value;
        uint32_t int_value;
    };
    FloatInt tmp;
    if ( stream->IsWriting )
    {
        tmp.float_value = value;
    }
    bool result = stream->SerializeUInt32( tmp.int_value );
    if ( stream->IsReading )
    {
        value = tmp.float_value;
    }
    return result;
}

class BitReader
{
private:
    uint64_t scratch = 0;
    int scratch_bits = 0;
    int total_bits;
    int num_bits_read = 0;
    int word_index = 0;
    uint32_t * buffer;
public:
    BitReader(const uint8_t * buffer, int bytes)
    {
        this->buffer = (uint32_t*)buffer;
        total_bits = bytes * 8;
    }

    uint32_t ReadBits(int bits)
    {
        if(scratch_bits < bits)
        {
            uint64_t temp = buffer[word_index++];
            temp <<= scratch_bits;
            scratch |= temp;
            scratch_bits = 32;
        }
        uint32_t temp = 0;
        temp |= scratch;
        temp <<= (32 - bits);
        temp >>= (32 - bits);
        scratch >>= bits;
        scratch_bits -= bits;
        total_bits -= bits;
        return temp;
    }

    void ReadBytes(char* arr, int bytes)
    {
        memcpy(arr, buffer + word_index, bytes);
        word_index += ceil((double)bytes / 4);
    }

    bool WouldReadPastEnd(int bits)
    {
        return bits > total_bits;
    }
};

class BitWriter
{
private:
    uint64_t scratch = 0;
    int scratch_bits = 0;
    int word_index = 0;
    uint32_t* buffer;
public:
    BitWriter(uint8_t * buffer, int bytes)
    {
        this->buffer = (uint32_t*)buffer;
    }

    void WriteBits(uint32_t value, int bits)
    {
        uint64_t temp = value;
        temp <<= 32 + bits;
        temp >>= 32 + bits;
        temp <<= scratch_bits;
        scratch |= temp;
        scratch_bits += bits;
        if(scratch_bits >= 32)
        {
            buffer[word_index++] |= scratch;
            scratch >>= 32;
            scratch_bits -= 32;
        }
    }

    void WriteBytes(char* arr, int bytes)
    {
        memcpy(buffer + word_index, arr, bytes);
        word_index += ceil((double)bytes / 4);
    }

    void Flush()
    {
        buffer[word_index++] |= scratch;
        scratch >>= 32;
        scratch_bits -= 32;
    }
};

class Stream
{
public:
    enum { IsWriting = 0 };
    enum { IsReading = 0 };
    virtual bool SerializeInt32(int32_t & value) = 0;
    virtual bool SerializeUInt32(uint32_t & value) = 0;
    //virtual bool SerializeBytes(char* arr, uint32_t size);
    virtual bool SerializeCharVector(std::vector<char>& vec, uint32_t size) = 0;
};

class WriteStream32 : public Stream
{
private:
	std::vector<uint32_t> buffer;

public:
	enum { IsWriting = 1 };
	enum { IsReading = 0 };

	WriteStream32(std::vector<char> vector, uint32_t size = 0)
	{
	}

	WriteStream32()
	{
	}

	bool SerializeInt32(int32_t& value) override
	{
		uint32_t uvalue = value - INT32_MIN;
		return SerializeUInt32(uvalue);
	}

	bool SerializeUInt32(uint32_t& value) override
	{
		buffer.push_back(value);
		return true;
	}

	bool SerializeCharVector(std::vector<char>& vec, uint32_t size) override
	{
		if (size % 4 != 0)
			return false;
		buffer.resize(buffer.size() + size / 4);
		std::memcpy((buffer.data() + buffer.size()), vec.data(), size);
		return true;
	}

	uint32_t* GetBuffer()
	{
		return buffer.data();
	}

	uint32_t GetSize() const
	{
		return buffer.size();
	}
};

class ReadStream32 : public Stream
{
private:
	std::vector<uint32_t> buffer;
	uint32_t head;
public:
	enum { IsWriting = 0 };
	enum { IsReading = 1 };

	ReadStream32(std::vector<char>& vector, uint32_t size = 0)
	{
		buffer.resize(size / 4);
		std::memcpy(buffer.data(), vector.data(), vector.size());
		head = 0;
	}

	ReadStream32(uint32_t* buf, uint32_t s)
	{
		buffer.resize(s);
		std::memcpy(buffer.data(), buf, s * 4);
		head = 0;
	}


	bool SerializeInt32(int32_t& value) override
	{
		uint32_t uvalue;
		if (!SerializeUInt32(uvalue))
		{
			return false;
		}
		value = uvalue + INT32_MIN;
		return true;
	}

	bool SerializeUInt32(uint32_t& value) override
	{
		check_for_enough_data(buffer.size() - head, 1)
			value = buffer[head++];
		return true;
	}

	bool SerializeCharVector(std::vector<char>& vec, uint32_t size) override
	{
		if (size % 4 != 0)
			return false;
		check_for_enough_data(buffer.size() - head, size / 4);
		if (vec.capacity() < size)
		{
			vec.resize(size);
		}
		std::memcpy(vec.data(), buffer.data() + head, size);
		head += size / 4;
		return true;
	}
};

class WriteStream : Stream
{
public:

    enum { IsWriting = 1 };
    enum { IsReading = 0 };

    WriteStream( uint8_t * buffer, int bytes ) : m_writer( buffer, bytes ) {}

    bool SerializeInt32(int32_t & value) override
    {
        uint32_t unsigned_value = value - INT32_MIN;
        m_writer.WriteBits( unsigned_value, 32 );
        return true;
    }

    bool SerializeUInt32(uint32_t & value) override
    {
        m_writer.WriteBits( value, 32 );
        return true;
    }

    bool SerializeBytes(char* arr, uint32_t size)
    {
        m_writer.WriteBytes(arr, size);
        return true;
    }

    bool SerializeCharVector(std::vector<char>& vec, uint32_t size) override
    {
        return SerializeBytes(vec.data(), vec.size());
    }

private:

    BitWriter m_writer;
};

class ReadStream : Stream
{
public:

    enum { IsWriting = 0 };
    enum { IsReading = 1 };

    ReadStream( const uint8_t * buffer, int bytes ) : m_reader( buffer, bytes ) {}

    bool SerializeInt32(int32_t & value) override
    {
        if ( m_reader.WouldReadPastEnd(32) )
        {
            return false;
        }
        uint32_t unsigned_value = m_reader.ReadBits( 32 );
        value = (int32_t) unsigned_value + INT32_MIN;
        return true;
    }

    bool SerializeUInt32(uint32_t & value) override
    {
        if ( m_reader.WouldReadPastEnd(32) )
        {
            return false;
        }
        value = m_reader.ReadBits( 32 );
        return true;
    }

    bool SerializeBytes(char* arr, uint32_t size)
    {
        m_reader.ReadBytes(arr, size);
        return true;
    }

    bool SerializeCharVector(std::vector<char>& vec, uint32_t size) override
    {
        vec.resize(size);
        return SerializeBytes(vec.data(), vec.size());
    }

private:

    BitReader m_reader;
};

#endif //SPLATTER_SERIALIZATION_H
