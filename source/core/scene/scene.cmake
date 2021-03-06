include("${CMAKE_CURRENT_LIST_DIR}/animation/animation.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/bvh/bvh.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/camera/camera.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/entity/entity.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/light/light.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/material/material.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/prop/prop.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/shape/shape.cmake")

target_sources(core
	PRIVATE
	"${CMAKE_CURRENT_LIST_DIR}/scene.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_constants.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_loader.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_loader.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_ray.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_ray.inl"
	"${CMAKE_CURRENT_LIST_DIR}/scene_renderstate.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_worker.cpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_worker.hpp"
	"${CMAKE_CURRENT_LIST_DIR}/scene_worker.inl"
	)  
