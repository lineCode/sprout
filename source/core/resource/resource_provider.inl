#ifndef SU_CORE_RESOURCE_PROVIDER_INL
#define SU_CORE_RESOURCE_PROVIDER_INL

#include "resource_provider.hpp"

namespace resource {

template <typename T>
Provider<T>::Provider(std::string const& name) noexcept : Identifiable<T>(name) {}

template <typename T>
Provider<T>::~Provider() noexcept {}

}  // namespace resource

#endif
