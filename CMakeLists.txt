cmake_minimum_required (VERSION 2.8)
    project (octopus)

    set (CMAKE_CXX_FLAGS "-g -Wall")
    set (CMAKE_C_FLAGS "-DTHREADED -g -Wall")

    add_subdirectory (MurmurHash)

    include_directories ("${PROJECT_SOURCE_DIR}/hash")

    include_directories ("${PROJECT_SOURCE_DIR}/MurmurHash")

    include_directories ("${PROJECT_SOURCE_DIR}/tree")

    add_executable (node_exec
                hkey.c
                zk_common.c
                config.c
                node.c
                nodes.c
                router.c
                broker.c
                zookeeper.c zk_updater.c main.c)

    target_link_libraries (node_exec MurmurHash zookeeper_mt procps czmq 
                       tcmalloc)
