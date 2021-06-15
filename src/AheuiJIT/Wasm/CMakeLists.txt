add_executable(AheuiJITWasm export.h export.cpp)
target_link_libraries(AheuiJITWasm AheuiJIT)
set_target_properties(AheuiJITWasm PROPERTIES LINK_FLAGS " --bind \
    -s ASYNCIFY \
    -s ALLOW_MEMORY_GROWTH=0 \
    -s INITIAL_MEMORY=167772160 \
    -s SINGLE_FILE=1 \
    -s IMPORTED_MEMORY \
    -s DISABLE_EXCEPTION_CATCHING=1 \
    -s 'ASYNCIFY_IMPORTS=[\"_runWasm\",\"_setWasm\"]' \
    -s EXPORTED_FUNCTIONS='[\"_initRuntime\", \"_runAheuiCode\",\"_setConfig\"]' \
    -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]' \
    --post-js ${CMAKE_CURRENT_SOURCE_DIR}/post.js \
    --pre-js ${CMAKE_CURRENT_SOURCE_DIR}/pre.js")
