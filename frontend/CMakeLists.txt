set(FRONTEND_LIST WaveUtil.cc)
add_library(frontend STATIC ${FRONTEND_LIST})
target_link_libraries(frontend PUBLIC utils)