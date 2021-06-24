cmake_minimum_required(VERSION 3.10)
project(desbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# nlohmann_json
find_package(nlohmann_json REQUIRED)
get_target_property(
  NLOHMANN_JSON_INC
  nlohmann_json::nlohmann_json
  INTERFACE_INCLUDE_DIRECTORIES
)

# numactl
find_library(NUMACTL_A NAMES libnuma.a PATHS ${CMAKE_INSTALL_RPATH})
if(NUMACTL_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libnuma.a not found")
endif()
string(REPLACE lib/libnuma.a include/ NUMACTL_INC ${NUMACTL_A})
if(NOT EXISTS ${NUMACTL_INC})
  message(FATAL_ERROR "numactl include dir doesn't exist")
endif()

# zlib
find_library(ZLIB_A NAMES libz.a PATHS ${CMAKE_INSTALL_RPATH})
if(ZLIB_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libz.a not found")
endif()
string(REPLACE lib/libz.a include/ ZLIB_INC ${ZLIB_A})
if(NOT EXISTS ${ZLIB_INC})
  message(FATAL_ERROR "zlib include dir doesn't exist")
endif()

# libprim
find_library(LIBPRIM_A NAMES libprim.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBPRIM_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libprim.a not found")
endif()
string(REPLACE lib/libprim.a include/ LIBPRIM_INC ${LIBPRIM_A})
if(NOT EXISTS ${LIBPRIM_INC})
  message(FATAL_ERROR "libprim include dir doesn't exist")
endif()

# libfactory
find_library(LIBFACTORY_A NAMES libfactory.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBFACTORY_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libfactory.a not found")
endif()
string(REPLACE lib/libfactory.a include/ LIBFACTORY_INC ${LIBFACTORY_A})
if(NOT EXISTS ${LIBFACTORY_INC})
  message(FATAL_ERROR "libfactory include dir doesn't exist")
endif()

# librnd
find_library(LIBRND_A NAMES librnd.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBRND_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "librnd.a not found")
endif()
string(REPLACE lib/librnd.a include/ LIBRND_INC ${LIBRND_A})
if(NOT EXISTS ${LIBRND_INC})
  message(FATAL_ERROR "librnd include dir doesn't exist")
endif()

# libfio
find_library(LIBFIO_A NAMES libfio.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBFIO_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libfio.a not found")
endif()
string(REPLACE lib/libfio.a include/ LIBFIO_INC ${LIBFIO_A})
if(NOT EXISTS ${LIBFIO_INC})
  message(FATAL_ERROR "libfio include dir doesn't exist")
endif()

# libstrop
find_library(LIBSTROP_A NAMES libstrop.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBSTROP_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libstrop.a not found")
endif()
string(REPLACE lib/libstrop.a include/ LIBSTROP_INC ${LIBSTROP_A})
if(NOT EXISTS ${LIBSTROP_INC})
  message(FATAL_ERROR "libstrop include dir doesn't exist")
endif()

# libsettings
find_library(LIBSETTINGS_A NAMES libsettings.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBSETTINGS_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libsettings.a not found")
endif()
string(REPLACE lib/libsettings.a include/ LIBSETTINGS_INC ${LIBSETTINGS_A})
if(NOT EXISTS ${LIBSETTINGS_INC})
  message(FATAL_ERROR "libsettings include dir doesn't exist")
endif()

# libdes
find_library(LIBDES_A NAMES libdes.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBDES_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libdes.a not found")
endif()
string(REPLACE lib/libdes.a include/ LIBDES_INC ${LIBDES_A})
if(NOT EXISTS ${LIBDES_INC})
  message(FATAL_ERROR "libdes include dir doesn't exist")
endif()

add_executable(
  desbench
  ${PROJECT_SOURCE_DIR}/src/main.cc
  ${PROJECT_SOURCE_DIR}/src/bench/EmptyComponent.cc
  ${PROJECT_SOURCE_DIR}/src/bench/BenchComponent.cc
  ${PROJECT_SOURCE_DIR}/src/bench/SimpleComponent.cc
  ${PROJECT_SOURCE_DIR}/src/bench/MemoryComponent.cc
  ${PROJECT_SOURCE_DIR}/src/bench/BenchComponent.h
  ${PROJECT_SOURCE_DIR}/src/bench/SimpleComponent.h
  ${PROJECT_SOURCE_DIR}/src/bench/EmptyComponent.h
  ${PROJECT_SOURCE_DIR}/src/bench/MemoryComponent.h
  )

target_include_directories(
  desbench
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${NLOHMANN_JSON_INC}
  ${NUMACTL_INC}
  ${ZLIB_INC}
  ${LIBPRIM_INC}
  ${LIBFACTORY_INC}
  ${LIBRND_INC}
  ${LIBFIO_INC}
  ${LIBSTROP_INC}
  ${LIBSETTINGS_INC}
  ${LIBDES_INC}
  )

target_link_libraries(
  desbench
  ${NUMACTL_A}
  ${ZLIB_A}
  ${LIBPRIM_A}
  ${LIBFACTORY_A}
  ${LIBRND_A}
  ${LIBFIO_A}
  ${LIBSTROP_A}
  ${LIBSETTINGS_A}
  ${LIBDES_A}
  ${LIBSETTINGS_A}
  ${LIBSTROP_A}
  ${LIBFIO_A}
  ${LIBRND_A}
  ${LIBFACTORY_A}
  ${LIBPRIM_A}
  ${ZLIB_A}
  ${NUMACTL_A}
  )

include(GNUInstallDirs)

install(
  TARGETS
  desbench
  )

