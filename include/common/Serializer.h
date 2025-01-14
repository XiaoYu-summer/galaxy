/*
 * 通用的序列化工具, 提供通用的二进制数字序列化／反序列化操作
 * 1. 定长(8bit,16bit,32bit,64bit)整型，浮点
 * 2. 指定长度(16bit,32bit)标识的字符串和二进制块
 *

   example:

		struct Obj : public NioMarshallable{
			int		a ;
			string	b ;
			void marshal(Serializer & s) const { 	s << a << b ;		}
			void unmarshal(Deserializer & ds ) { ds >> a >> b ; }
		}

		Obj o ;
		ByteBuffer bb ;
		Serializer s(&bb) ;
		s << o ;

		io.write(bb.data(),bb.size()) ;
		..

		Deserializer s(data,size) ;
		s >> o ;
		..

 */
#pragma once
#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>
#include <cstring>

#include "Byteorder.h"
#include "code/ErrorCode.h"

namespace Binary
{
    const uint32_t  MAX_BUFFER_SIZE = std::getenv("MAX_BUFFER_SIZE") ? std::stoi(std::getenv("MAX_BUFFER_SIZE")) : 2048;

	class Serializer;
	class Deserializer;

	struct BO {
		inline static uint16_t hton16(uint16_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le16(i) : h2be16(i); }
		inline static uint32_t hton32(uint32_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le32(i) : h2be32(i); }
		inline static uint64_t hton64(uint64_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le64(i) : h2be64(i); }
		inline static uint16_t ntoh16(uint16_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le16(i) : h2be16(i); }
		inline static uint32_t ntoh32(uint32_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le32(i) : h2be32(i); }
		inline static uint64_t ntoh64(uint64_t i, int bo) { return bo == LITTLE_ENDIAN ? h2le64(i) : h2be64(i); }
	};

	//template<typename Encoding >
	class Serializer
	{
	public:
		//  byteOrder: LITTLE_ENDIAN / BIG_ENDIAN
		inline Serializer(int byteOrder = LITTLE_ENDIAN)
		: bo_(byteOrder)
		{
            data_ = new char[MAX_BUFFER_SIZE]();
            memset(data_, 0, MAX_BUFFER_SIZE);
		}
		inline virtual ~Serializer()
        {
            if (data_)
            {
                delete[] data_;
                data_ = nullptr;
            }
        }

		inline const char* data() const
		{
			return data_;
		}
		inline size_t size() const
		{
			return size_;
		}

		inline void write(const void* s, size_t n)
        {
            if (!s || (size_ + n) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write data is null, data=" << s << ", or write not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_ << ", need size=" << n);
            }
            memcpy(data_, s, n);
            size_ += n;
        }
		inline void write_bool(bool b)
        {
            if ((size_ + sizeof(char)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write bool not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            const char v = b ? 1 : 0;
            *((char*)(data_ + size_)) = v;
            size_ += sizeof(char);
        }
		inline void write_byte(uint8_t u)
        {
            if ((size_ + sizeof(char)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write byte not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((char*)(data_ + size_)) = u;
            size_ += sizeof(char);
        }
		inline void write_int16(int16_t i)
        {
            if ((size_ + sizeof(uint16_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write int16_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint16_t*)(data_ + size_)) = BO::hton16((uint16_t)i, bo_);
            size_ += sizeof(uint16_t);
        }
		inline void write_int32(int32_t i)
        {
            if ((size_ + sizeof(uint32_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write int32_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint32_t*)(data_ + size_)) = BO::hton32((uint32_t)i, bo_);
            size_ += sizeof(uint32_t);
        }
		inline void write_int64(int64_t i)
        {
            if ((size_ + sizeof(uint64_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write int64_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint64_t*)(data_ + size_)) = BO::hton64((uint64_t)i, bo_);
            size_ += sizeof(uint64_t);
        }
		inline void write_uint16(uint16_t u)
        {
            if ((size_ + sizeof(uint16_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write uint16_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint16_t*)(data_ + size_)) = BO::hton16(u, bo_);
            size_ += sizeof(uint16_t);
        }
		inline void write_uint32(uint32_t u)
        {
            if ((size_ + sizeof(uint32_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write uint32_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint32_t*)(data_ + size_)) = BO::hton32(u, bo_);
            size_ += sizeof(uint32_t);
        }
		inline void write_uint64(uint64_t u)
        {
            if ((size_ + sizeof(uint64_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write uint64_t not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint64_t*)(data_ + size_)) = BO::hton64(u, bo_);
            size_ += sizeof(uint64_t);
        }
		//	real
		inline void write_float(float f)
        {
            if ((size_ + sizeof(uint32_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write float not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint32_t*)(data_ + size_)) = BO::hton32(*((uint32_t*)&f), bo_);
            size_ += sizeof(uint32_t);
        }
		inline void write_double(double d)
        {
            if ((size_ + sizeof(uint64_t)) > MAX_BUFFER_SIZE)
            {
                RUNTIME_EXCEPTION("write double not enough buffer, available buffer size=" << MAX_BUFFER_SIZE - size_);
            }
            *((uint64_t*)(data_ + size_)) = BO::hton64(*((uint64_t*)&d), bo_);
            size_ += sizeof(uint64_t);
        }

	private:
		char* data_;
		size_t size_ = 0;
		const int bo_ = 0;
	};

	//	只保存数据的引用，不改变数据
	class Deserializer
	{
	public:
		//  byteOrder: LITTLE_ENDIAN / BIG_ENDIAN
		inline virtual ~Deserializer() {}
		inline Deserializer(const void* data, size_t size, int byteOrder = LITTLE_ENDIAN)
		:data_(0),size_(0), bo_(byteOrder)
		{
			reset(data, size);
		}

		inline const char* data() const { return data_; }
		inline size_t size() const { return size_; }
		inline bool empty() const { return 0 == size_; }

		inline void skip(size_t k) const
		{
			if (size_ < k)
			{
                RUNTIME_EXCEPTION("skip not enough data, data size=" << size_ << ", skip size=" << k);
            }
			data_ += k;
			size_ -= k;
		}
		//	raw
		inline const char* read(size_t k) const
		{
			if (size_ < k)
			{
                RUNTIME_EXCEPTION("read not enough data, data size=" << size_ << ", read size=" << k);
            }
			const char* p = data_;
			data_ += k;
			size_ -= k;
			return p;
		}
		inline void read_raw(void * buffer, size_t n) const
		{
            if (!buffer)
            {
                RUNTIME_EXCEPTION("dest buffer is null!");
            }
			const char* p = read(n);
			memcpy(buffer, p, n);
		}

		//	fixed sized integer
		inline uint8_t read_byte() const
		{
			if (size_ < 1u)
			{
                RUNTIME_EXCEPTION("read byte not enough data, data size=" << size_);
            }
			uint8_t u = *((uint8_t*)data_);
			data_ += 1u;
			size_ -= 1u;
			return u;
		}
		inline bool read_bool() const
		{
			uint8_t u = read_byte();
			return u != 0;
		}
		inline int16_t read_int16() const
		{
			if (size_ < 2u)
			{
                RUNTIME_EXCEPTION("read int16 not enough data, data size=" << size_);
            }
			int16_t u = *((int16_t*)data_);
			u = (int16_t)BO::ntoh16((uint16_t)u, bo_);
			data_ += 2u;
			size_ -= 2u;
			return u;
		}
		inline int32_t read_int32() const
		{
			if (size_ < 4u)
			{
                RUNTIME_EXCEPTION("read int32 not enough data, data size=" << size_);
            }
			int32_t u = *((int32_t*)data_);
			u = (int32_t)BO::ntoh32((uint32_t)u, bo_);
			data_ += 4u;
			size_ -= 4u;
			return u;
		}
		inline int64_t read_int64() const
		{
			if (size_ < 8u)
			{
                RUNTIME_EXCEPTION("read int64() not enough data, data size=" << size_);
            }
			int64_t u = *((int64_t*)data_);
			u = (int64_t)BO::ntoh64((uint64_t)u, bo_);
			data_ += 8u;
			size_ -= 8u;
			return u;
		}
		inline uint16_t read_uint16() const
		{
			if (size_ < 2u)
			{
                RUNTIME_EXCEPTION("read uint16 not enough data, data size=" << size_);
            }
			uint16_t u = *((uint16_t*)data_);
			u = BO::ntoh16(u, bo_);
			data_ += 2u;
			size_ -= 2u;
			return u;
		}
		inline uint32_t read_uint32() const
		{
			if (size_ < 4u)
			{
                RUNTIME_EXCEPTION("read uint32 not enough data, data size=" << size_);
            }
			uint32_t u = *((uint32_t*)data_);
			u = BO::ntoh32(u, bo_);
			data_ += 4u;
			size_ -= 4u;
			return u;
		}
		inline uint64_t read_uint64() const
		{
			if (size_ < 8u)
			{
                RUNTIME_EXCEPTION("read uint64() not enough data, data size=" << size_);
            }
			uint64_t u = *((uint64_t*)data_);
			u = BO::ntoh64(u, bo_);
			data_ += 8u;
			size_ -= 8u;
			return u;
		}
		inline float read_float() const
		{
			if (size_ < 4u)
			{
                RUNTIME_EXCEPTION("read float not enough data, data size=" << size_);
            }
			uint32_t u32 = *((uint32_t*)data_);
			u32 = BO::ntoh32(u32, bo_);
			float f = *((float*)&u32);
			data_ += 4u;
			size_ -= 4u;
			return f;
		}
		inline double read_double() const
		{
			if (size_ < 8u)
			{
                RUNTIME_EXCEPTION("read double not enough data, data size=" << size_);
            }
			uint64_t u64 = *((uint64_t*)data_);
			u64 = BO::ntoh64(u64, bo_);
			double d = *((double*)&u64);
			data_ += 8u;
			size_ -= 8u;
			return d;
		}

		inline void reset(const void* data, size_t size) const
		{
			data_ = (const char*)data;
			size_ = size;
		}
	private:
		mutable const char* data_;
		mutable size_t size_;
		const int bo_;
	};

}
