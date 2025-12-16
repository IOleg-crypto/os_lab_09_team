# Source Groups
set(PROTOCOL_SOURCES
    protocol/Protocol.h
)

set(SHM_SOURCES
    sharedmemory/SharedMemoryCore.cpp
    sharedmemory/SharedMemoryCore.h
)

set(PIPE_SOURCES
    pipe/PipeCore.cpp
    pipe/PipeCore.h
)

set(LOGIC_SOURCES
    VirtualBoard.cpp
    VirtualBoard.h
    IIpcCore.h
    ProcessManager.cpp
    ProcessManager.h
)

# Create the 'ipc_core' library
add_library(ipc_core STATIC
    ${LOGIC_SOURCES}
    ${PIPE_SOURCES}
    ${SHM_SOURCES}
    ${PROTOCOL_SOURCES}
)

# Include directories so internal files find each other
target_include_directories(ipc_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/protocol
)

# Qt dependency for ProcessManager (QProcess)
find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(ipc_core PUBLIC Qt6::Core)

# UUID library for Windows
if(WIN32)
    target_link_libraries(ipc_core PRIVATE Rpcrt4)
endif()
