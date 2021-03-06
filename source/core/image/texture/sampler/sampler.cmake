target_sources(core
  PRIVATE
  "${CMAKE_CURRENT_LIST_DIR}/address_mode.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/bilinear.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_2d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_3d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_linear_2d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_linear_2d.inl"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_linear_3d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_linear_3d.inl"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_nearest_2d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_nearest_2d.inl"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_nearest_3d.hpp"
  "${CMAKE_CURRENT_LIST_DIR}/sampler_nearest_3d.inl"
) 
