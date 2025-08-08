include(create_executable)

set(sorting_comparison_executable task-1-sort-comparison)
create_executable(${sorting_comparison_executable} ${sorting_algorithms_static_lib})
