set(target dental)

set(inc_dir ${CMAKE_SOURCE_DIR}/include/${target})
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_SOURCE_DIR} *.cxx *.cpp *.c *.hxx *.hpp *.h)
file(GLOB_RECURSE INCS ${inc_dir}/*.hxx ${inc_dir}/*.hpp ${inc_dir}/*.h)

add_executable(${target} ${INCS} ${SRCS})

# string(TOUPPER ${target} upper_target)
# target_compile_definitions(${target} PRIVATE ${upper_target}_LIB)

# target_link_libraries(${target} PRIVATE zjcommloger zjgraphic debug glfw3d.lib optimized glfw3.lib libEGL.dll.lib libGLESv2.dll.lib)
