#pragma once

#include "variant_map.hpp"

namespace memory {

template<typename T>
bool Variant_map::query(const std::string& key, T& value) const {
	auto i = map_.find(key);

	if (map_.end() == i) {
		return false;
	}

	auto& variant = i->second;

	if (Variant::Type::Uint != variant.type) {
		return false;
	}

	value = static_cast<T>(variant.uint_value);

	return true;
}

inline bool Variant_map::query(const std::string& key, bool& value) const {
	auto i = map_.find(key);

	if (map_.end() == i) {
		return false;
	}

	auto& variant = i->second;

	if (Variant::Type::Bool != variant.type) {
		return false;
	}

	value = variant.bool_value;

	return true;
}

inline bool Variant_map::query(const std::string& key, int32_t& value) const {
	auto i = map_.find(key);

	if (map_.end() == i) {
		return false;
	}

	auto& variant = i->second;

	if (Variant::Type::Int != variant.type) {
		return false;
	}

	value = variant.int_value;

	return true;
}

inline bool Variant_map::query(const std::string& key, uint32_t& value) const {
	auto i = map_.find(key);

	if (map_.end() == i) {
		return false;
	}

	auto& variant = i->second;

	if (Variant::Type::Uint != variant.type) {
		return false;
	}

	value = variant.uint_value;

	return true;
}

inline bool Variant_map::query(const std::string& key, float& value) const {
	auto i = map_.find(key);

	if (map_.end() == i) {
		return false;
	}

	auto& variant = i->second;

	if (Variant::Type::Float != variant.type) {
		return false;
	}

	value = variant.float_value;

	return true;
}

template<typename T>
void Variant_map::set(const std::string& key, T value) {
	map_[key] = Variant(static_cast<uint32_t>(value));
}

inline void Variant_map::set(const std::string& key, bool value) {
	map_[key] = Variant(value);
}

inline void Variant_map::set(const std::string& key, int32_t value) {
	map_[key] = Variant(value);
}

inline void Variant_map::set(const std::string& key, uint32_t value) {
	map_[key] = Variant(value);
}

inline void Variant_map::set(const std::string& key, float value) {
	map_[key] = Variant(value);
}

inline void Variant_map::inherit(const Variant_map& other, const std::string& key) {
	auto i = other.map_.find(key);

	if (other.map_.end() == i) {
		return;
	}

	map_[key] = i->second;
}

inline void Variant_map::inherit_except(const Variant_map& other, const std::string& key) {
	for (auto i : other.map_) {
		if (i.first != key) {
			map_.insert(map_.end(), i);
		}
	}
}

}
