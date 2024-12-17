add_executable(test_vec test_vec.c)
add_test(
    NAME test_vec
    COMMAND "./test_vec"
)

add_executable(test_htable test_htable.c)
add_test(
    NAME test_htable
    COMMAND "./test_htable"
)

add_executable(test_str test_str.c)
add_test(
    NAME test_str
    COMMAND "./test_str"
)

add_executable(test_binheap test_binheap.c)
add_test(
    NAME test_binheap
    COMMAND "./test_binheap"
)