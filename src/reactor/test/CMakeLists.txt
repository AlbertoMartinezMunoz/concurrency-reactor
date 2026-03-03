add_executable(test-reactor test-reactor.cpp)
target_link_libraries(test-reactor reactor-lib gtest)
add_test(NAME test-reactor COMMAND test-reactor)
set_target_properties(test-reactor PROPERTIES FOLDER test)
