cmake_minimum_required(VERSION 3.6)
project(ride.concurrency)

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
endif()

include_directories(include)

set(LIB_SOURCES
        src/pool.cpp
        src/worker.cpp
)

set(LIB_HEADERS
        include/ride/concurrency/thread_pool.hpp

        include/ride/concurrency/container/deque.hpp
        include/ride/concurrency/container/list.hpp
        include/ride/concurrency/container/queue.hpp
        include/ride/concurrency/container/stack.hpp

        include/ride/concurrency/container/detail/bidirectional_container.hpp
        include/ride/concurrency/container/detail/bidirectional_operations.hpp
        include/ride/concurrency/container/detail/container.hpp
        include/ride/concurrency/container/detail/container_types.hpp
        include/ride/concurrency/container/detail/emplacer.hpp
        include/ride/concurrency/container/detail/forward_container.hpp
        include/ride/concurrency/container/detail/forward_operations.hpp
        include/ride/concurrency/container/detail/operations.hpp
        include/ride/concurrency/container/detail/safe_container.hpp

        include/ride/concurrency/detail/abstract_job.hpp
        include/ride/concurrency/detail/action_job.hpp
        include/ride/concurrency/detail/barrier.hpp
        include/ride/concurrency/detail/gate.hpp
        include/ride/concurrency/detail/job.hpp
        include/ride/concurrency/detail/job_traits.hpp
        include/ride/concurrency/detail/pass_keys.hpp
        include/ride/concurrency/detail/pool.hpp
        include/ride/concurrency/detail/special_job.hpp
        include/ride/concurrency/detail/worker.hpp
        include/ride/concurrency/detail/worker_factory.hpp

        include/ride/concurrency/sample/pausable_thread_pool.hpp
        include/ride/concurrency/sample/static_thread_pool.hpp
)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES} ${LIB_HEADERS})
