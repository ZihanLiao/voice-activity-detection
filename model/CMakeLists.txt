set(MODEL_LIST torch_vad_model.cc vad_informer.cc vad_api.cc)
add_library(model STATIC ${MODEL_LIST})
target_link_libraries(model PUBLIC ${TORCH_LIBRARIES})