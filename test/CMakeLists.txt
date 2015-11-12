add_executable(test_position test_position.c ../src/position.c)
set_target_properties(test_position PROPERTIES COMPILE_FLAGS -DTEST)
add_test(test_position test_position)
