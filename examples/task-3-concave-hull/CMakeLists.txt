include(create_executable)

set(hull_building_demo task-3-concave-hull-demo)
create_executable(${hull_building_demo} Qt5::Core Qt5::Widgets ${hull_building_demo_static_lib})
