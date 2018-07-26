target_sources(core
    PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/emission.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/emission.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking_multi.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking_multi.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking_single.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking_single.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/tracking.hpp"
    "${CMAKE_CURRENT_LIST_DIR}/volume_integrator.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/volume_integrator.hpp"
    )
