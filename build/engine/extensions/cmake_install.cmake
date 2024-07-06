# Install script for directory: Z:/VS/Project/axmol/extensions

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files/SeamlessClient")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/GUI/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/assets-manager/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/Particle3D/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/physics-nodes/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/spine/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/DragonBones/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/fairygui/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/ImGui/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/Inspector/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/SDFGen/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/cocostudio/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/scripting/lua-bindings/cmake_install.cmake")
  include("Z:/VS/Project/SeamlessClient/build/engine/extensions/JSONDefault/cmake_install.cmake")

endif()

