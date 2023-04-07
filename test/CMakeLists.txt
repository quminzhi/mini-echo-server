cmake_minimum_required(VERSION 3.18.4)

set(CMAKE_BUILD_TYPE Debug)

#################################
# Add a test for shared libraries
#################################

# # add a test for foo 
# set(FOOTEST foo-test)
# set(SOURCES foo-test.cpp)
# add_executable(${FOOTEST} ${SOURCES})
# target_link_libraries(${FOOTEST} PUBLIC 
#   gtest_main 
#   foo 
# )
# add_test(NAME ${FOOTEST} COMMAND "${FOOTEST}")

# # add a test for external link_libraries
# set(EXTERNALTEST external-test)
# set(SOURCES external-test.cpp)
# add_executable(${EXTERNALTEST} ${SOURCES})
# target_link_libraries(${EXTERNALTEST} PUBLIC 
#   gtest_main 
#   libexternal 
# )
# add_test(NAME ${EXTERNALTEST} COMMAND "${EXTERNALTEST}")
