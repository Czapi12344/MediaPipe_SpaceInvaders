cmake_minimum_required(VERSION 3.3.1)

project(SpaceInvaders)

find_package(OpenCV 4.0 REQUIRED)
find_package(SFML 2.5 REQUIRED COMPONENTS graphics)

include_directories(
	${OpenCV_INCLUDE_DIRS}
	${SFML}
)

add_executable(SpaceInvaders
	src/main.cpp
	src/Entity/Player.h src/Entity/Player.cpp
	src/Entity/Bullet.h src/Entity/Bullet.cpp
	src/Entity/MonsterCluster.h src/Entity/MonsterCluster.cpp
	src/Entity/Monster.h src/Entity/Monster.cpp
	src/GameProcessing/Collisions.h src/GameProcessing/Collisions.cpp
	src/GameProcessing/Score.h src/GameProcessing/Score.cpp
	src/Entity/EntityState.h
	src/HandTracking/HandTracker.h src/HandTracking/HandTracker.cpp
)

target_link_libraries(SpaceInvaders
	PUBLIC
		${OpenCV_LIBS}
		sfml-graphics
	PRIVATE 
		mediapipe
		mpformats
)

# Mediapipe dependencies

find_program(BAZEL NAMES bazel)

# Check if Bazel executable was found
if(BAZEL)
    message(STATUS "Bazel found: ${BAZEL}")
else()
	message(FATAL_ERROR "Bazel not found.\nInstall Bazel using following commands:\n"
	"wget -c https://github.com/bazelbuild/bazelisk/releases/download/v1.16.0/bazelisk-linux-amd64; "
	"chmod +x bazelisk-linux-amd64; "
	"sudo mv bazelisk-linux-amd64 /usr/local/bin/bazel;"
	)
endif()

set(MP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mediapipe")
set(MP_BIN_DIR "${MP_DIR}/bazel-bin")
set(MP_BAZEL_DIR "${MP_DIR}/bazel-mediapipe")

message(STATUS "Building Mediapipe")
execute_process(
    COMMAND ${BAZEL} build -c opt --define MEDIAPIPE_DISABLE_GPU=1 mediapipe/examples/desktop/libmp:libmp.so
    WORKING_DIRECTORY ${MP_DIR}
	RESULT_VARIABLE mpbuild_res
)

if(mpbuild_res EQUAL "1")
    message(FATAL_ERROR "Error during mediapipe build.")
else ()
	message(STATUS "Mediapipe built successfully.")
endif()

add_library(mediapipe SHARED IMPORTED)
target_include_directories(mediapipe INTERFACE ${MP_DIR}/mediapipe/examples/desktop/libmp)

# Protobuf
set(protobuf_source_dir "${MP_BAZEL_DIR}/external/com_google_protobuf")
include("${MP_DIR}/third_party/libprotobuf.cmake")

# MediaPipe Formats
file(GLOB_RECURSE MP_FORMATS_SRC "${MP_BIN_DIR}/mediapipe/framework/formats/*.cc")
add_library(mpformats STATIC ${MP_FORMATS_SRC})
target_include_directories(mpformats PUBLIC "${MP_BIN_DIR}")
target_link_libraries(mpformats libprotobuf)

set_target_properties(mediapipe PROPERTIES
	IMPORTED_LOCATION ${MP_BIN_DIR}/mediapipe/examples/desktop/libmp/libmp.so
)

# Mediapipe Modules
file(COPY "${MP_BIN_DIR}/mediapipe/modules" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/mediapipe")
file(COPY "${MP_BAZEL_DIR}/mediapipe/modules/hand_landmark/handedness.txt" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/mediapipe/modules/hand_landmark")
