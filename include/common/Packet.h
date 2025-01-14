#pragma once
#include <string>
#include <iostream>
#include <stdexcept>
#include "Serializer.h"

namespace Binary
{
class Pack : public Serializer
{
private:
	Pack (const Pack & o);
	Pack & operator = (const Pack& o);
public:
	Pack(int byteOrder = LITTLE_ENDIAN): Serializer(byteOrder){}

	virtual ~Pack() {}

	Pack & push(const void * s, size_t n) { write(s,n); return *this; }
	Pack & push_uint8(uint8_t u8)	 { return push(&u8, 1); }
	Pack & push_uint16(uint16_t u16) { write_uint16(u16); return *this; }
	Pack & push_uint32(uint32_t u32) { write_uint32(u32); return *this; }
	Pack & push_uint64(uint64_t u64) { write_uint64(u64); return *this; }

	void reserve(size_t n) {}
};

class Unpack : public Deserializer
{
public:

	Unpack(const void * data = 0, size_t size = 0, int byteOrder = LITTLE_ENDIAN): Deserializer(data, size, byteOrder) {}
	virtual ~Unpack() {}
	
	void finish() const
    {
		if (!empty())
        {
            RUNTIME_EXCEPTION("too much data , size=" << size());
        }
	}

	uint8_t pop_uint8() const
    {
		return read_byte();
	}

	uint16_t pop_uint16() const
    {
		return read_uint16();
	}

	uint32_t pop_uint32() const
    {
		return read_uint32();
	}

	uint64_t pop_uint64() const
    {
		return read_uint64();
	}
};

// base type helper
inline void WriteArray(Pack & p, const void * s, size_t n)
{
    p.push(s, n);
}

inline Pack & operator << (Pack & p, bool sign)
{
	p.push_uint8(sign ? 1 : 0);
	return p;
}

inline Pack & operator << (Pack & p, uint8_t  u8)
{
	p.push_uint8(u8);
	return p;
}

inline Pack & operator << (Pack & p, int8_t  i8)
{
	p.push_uint8(i8);
	return p;
}

inline Pack & operator << (Pack & p, uint16_t  u16)
{
	p.push_uint16(u16);
	return p;
}

inline Pack & operator << (Pack & p, int16_t  i16)
{
	p.push_uint16(i16);
	return p;
}

inline Pack & operator << (Pack & p, uint32_t  u32)
{
	p.push_uint32(u32);
	return p;
}
inline Pack & operator << (Pack & p, uint64_t  u64)
{
	p.push_uint64(u64);
	return p;
}

inline Pack & operator << (Pack & p, int64_t  i64)
{
	p.push_uint64((uint64_t)i64);
	return p;
}

inline Pack & operator << (Pack & p, int32_t  i32)
{
	p.push_uint32((uint32_t)i32);
	return p;
}

inline void ReadArray(const Unpack & p, void * s, size_t n)
{
    p.read_raw(s, n);
}

inline const Unpack & operator >> (const Unpack & p, uint32_t & u32)
{
	u32 =  p.pop_uint32();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, uint64_t & u64)
{
	u64 =  p.pop_uint64();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int64_t & i64)
{
	i64 =  (int64_t)p.pop_uint64();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int32_t & i32)
{
	i32 =  (int32_t)p.pop_uint32();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, uint8_t & u8)
{
	u8 =  p.pop_uint8();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int8_t & i8)
{
	i8 =  p.pop_uint8();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, uint16_t & u16)
{
	u16 =  p.pop_uint16();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, int16_t & i16)
{
	i16 =  p.pop_uint16();
	return p;
}

inline const Unpack & operator >> (const Unpack & p, bool & sign)
{
	sign =  (p.pop_uint8() == 0) ? false : true;
	return p;
}

} // namespace
