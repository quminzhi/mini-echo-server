cmake_minimum_required(VERSION 3.18.4)

#########################
# add hostinfo app
#########################

set(CMAKE_BUILD_TYPE Debug)

set(HOSTINFO hostinfo-app)
add_executable(${HOSTINFO} ${HOSTINFO}.c)
target_link_libraries(${HOSTINFO} PUBLIC
  # link libraries needed
  hostinfo
) 

set(ECHOCLI echo-cli)
add_executable(${ECHOCLI} ${ECHOCLI}.c)
target_link_libraries(${ECHOCLI} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(ECHOSERV echo-serv)
add_executable(${ECHOSERV} ${ECHOSERV}.c)
target_link_libraries(${ECHOSERV} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(ECHOSERVPROC echo-serv-proc)
add_executable(${ECHOSERVPROC} ${ECHOSERVPROC}.c)
target_link_libraries(${ECHOSERVPROC} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(MULTIPLEX io-multiplex)
add_executable(${MULTIPLEX} ${MULTIPLEX}.c)
target_link_libraries(${MULTIPLEX} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(ECHOSERVERMIO echo-serv-mio)
add_executable(${ECHOSERVERMIO} ${ECHOSERVERMIO}.c)
target_link_libraries(${ECHOSERVERMIO} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(ECHOSERVERTHD echo-serv-thd)
add_executable(${ECHOSERVERTHD} ${ECHOSERVERTHD}.c)
target_link_libraries(${ECHOSERVERTHD} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
) 

set(ECHOSERVERPOOL echo-serv-pool)
add_executable(${ECHOSERVERPOOL} ${ECHOSERVERPOOL}.c)
target_link_libraries(${ECHOSERVERPOOL} PUBLIC
  # link libraries needed
  inet-helper
  libexternal 
  sbuf
) 
