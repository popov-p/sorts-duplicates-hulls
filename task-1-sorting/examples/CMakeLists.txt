include(create_executable)

set(sorting_comparison_executable sort-comparison-exec)
create_executable(${sorting_comparison_executable} ${sorting_algorithms_static_lib})
