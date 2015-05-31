add_executable(test_equation test_equation.c)
set_target_properties(test_equation PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_equation stack state m)
add_test(test_equation test_equation)