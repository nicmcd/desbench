cmake_minimum_required(VERSION 3.20)
project(desbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# nlohmann_json
pkg_check_modules(nlohmann_json REQUIRED IMPORTED_TARGET nlohmann_json)
  get_target_property(
  NLOHMANN_JSON_INC
  PkgConfig::nlohmann_json
  INTERFACE_INCLUDE_DIRECTORIES
)

# numactl
pkg_check_modules(numactl REQUIRED IMPORTED_TARGET numa)
  get_target_property(
  NUMACTL_INC
  PkgConfig::numactl
  INTERFACE_INCLUDE_DIRECTORIES
)

# zlib
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
  get_target_property(
  ZLIB_INC
  PkgConfig::zlib
  INTERFACE_INCLUDE_DIRECTORIES
)

# libprim
pkg_check_modules(libprim REQUIRED IMPORTED_TARGET libprim)
  get_target_property(
  LIBPRIM_INC
  PkgConfig::libprim
  INTERFACE_INCLUDE_DIRECTORIES
)

# libfactory
pkg_check_modules(libfactory REQUIRED IMPORTED_TARGET libfactory)
  get_target_property(
  LIBFACTORY_INC
  PkgConfig::libfactory
  INTERFACE_INCLUDE_DIRECTORIES
)

# librnd
pkg_check_modules(librnd REQUIRED IMPORTED_TARGET librnd)
  get_target_property(
  LIBRND_INC
  PkgConfig::librnd
  INTERFACE_INCLUDE_DIRECTORIES
)

# libfio
pkg_check_modules(libfio REQUIRED IMPORTED_TARGET libfio)
  get_target_property(
  LIBFIO_INC
  PkgConfig::libfio
  INTERFACE_INCLUDE_DIRECTORIES
)

# libstrop
pkg_check_modules(libstrop REQUIRED IMPORTED_TARGET libstrop)
  get_target_property(
  LIBSTROP_INC
  PkgConfig::libstrop
  INTERFACE_INCLUDE_DIRECTORIES
)

# libsettings
pkg_check_modules(libsettings REQUIRED IMPORTED_TARGET libsettings)
  get_target_property(
  LIBSETTINGS_INC
  PkgConfig::libsettings
  INTERFACE_INCLUDE_DIRECTORIES
)

# libdes
pkg_check_modules(libdes REQUIRED IMPORTED_TARGET libdes)
  get_target_property(
  LIBDES_INC
  PkgConfig::libdes
  INTERFACE_INCLUDE_DIRECTORIES
)

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
  PkgConfig::nlohmann_json
  PkgConfig::numactl
  PkgConfig::zlib
  PkgConfig::libprim
  PkgConfig::libfactory
  PkgConfig::librnd
  PkgConfig::libfio
  PkgConfig::libstrop
  PkgConfig::libsettings
  PkgConfig::libdes
  )

include(GNUInstallDirs)

install(
  TARGETS
  desbench
  )

