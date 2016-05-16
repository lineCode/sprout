#pragma once

namespace rapidjson {

class CrtAllocator;

template <typename BaseAllocator> class MemoryPoolAllocator;

template <typename Encoding, typename Allocator> class GenericValue;

template<typename CharType> struct UTF8;

using Value = GenericValue<UTF8<char>, MemoryPoolAllocator<CrtAllocator>>;

}

namespace json {

using Value = rapidjson::Value;

}