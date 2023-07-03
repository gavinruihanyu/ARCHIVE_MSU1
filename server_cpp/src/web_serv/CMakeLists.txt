# CMakeList.txt : CMake project for web_serv, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
if (WIN32)
	set (lib_dir "C:/local")
	set (boostname boost)

	#boost linking
	set( boost_DIR ${lib_dir}/boost_1_79_0)
	find_package(${boostname} 1.79.0 COMPONENTS thread REQUIRED)
	set(Boost_INCLUDE_DIR ${lib_dir}/boost_1_79_0)
	set(Boost_LIBRARY_DIR ${lib_dir}/boost_1_79_0/stage/lib)
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY_DIR})
endif (WIN32)

if (UNIX)
	set(lib_dir "/usr/local")

	#boost linking
	find_package(Boost COMPONENTS thread REQUIRED)
	include_directories(${Boost_INCLUDE_DIRS})
endif(UNIX)

#mongodb -unix compatable
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)
set(LIBMONGOCXX_INCLUDE_DIR ${lib_dir}/include/mongocxx/v_noabi)
set(LIBBSONCXX_INCLUDE_DIR ${lib_dir}/include/bsoncxx/v_noabi)
include_directories(${LIBMONGOCXX_INCLUDE_DIR})
include_directories(${LIBBSONCXX_INCLUDE_DIR})
link_directories(${LIBMONGOCXX_LIBRARY_DIR})
link_directories(${LIBBSONCXX_LIBRARY_DIR})

#openssl -unix compatable
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
link_directories(${OPENSSL_LIBRARY_DIR})

#link own libraries
set(SERVER_DIR server)
set(ROUTER_DIR router)
set(CLIENT_DIR client)
set(HELPERS_DIR helpers)

add_library(
	helpers_lib
	${HELPERS_DIR}/helpers.hpp
	${HELPERS_DIR}/helpers.cpp
)
include_directories(${HELPERS_DIR})

add_library(
	client_lib
	${CLIENT_DIR}/client.hpp
	${CLIENT_DIR}/client.cpp
)
include_directories(${CLIENT_DIR})

add_library(
	server_lib
	${SERVER_DIR}/server.hpp
	${SERVER_DIR}/server.cpp
 )
include_directories(${SERVER_DIR})

add_library(
	router_lib
	${ROUTER_DIR}/router.hpp
	${ROUTER_DIR}/router.cpp
 )
include_directories(${ROUTER_DIR})

target_link_libraries( router_lib ${LIBMONGOCXX_LIBRARIES})
target_link_libraries( router_lib ${LIBSONCXX_LIBRARIES})
target_link_libraries( router_lib ${Boost_LIBRARIES})
target_link_libraries( router_lib ${OPENSSL_LIBRARIES})
target_link_libraries( router_lib client_lib)

target_link_libraries( server_lib ${LIBMONGOCXX_LIBRARIES})
target_link_libraries( server_lib ${LIBSONCXX_LIBRARIES})
target_link_libraries( server_lib ${Boost_LIBRARIES})
target_link_libraries( server_lib ${OPENSSL_LIBRARIES})
target_link_libraries( server_lib router_lib) #push router as a preq to servers compilation

target_link_libraries( server_lib helpers_lib)

# Add source to this project's executable.
add_executable (
	web_serv
	"web_serv.cpp"
 )
target_link_libraries( web_serv ${Boost_LIBRARIES})
target_link_libraries( web_serv ${OPENSSL_LIBRARIES})
target_link_libraries( web_serv ${LIBMONGOCXX_LIBRARIES})
target_link_libraries( web_serv ${LIBSONCXX_LIBRARIES})
target_link_libraries( web_serv server_lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET web_serv PROPERTY CXX_STANDARD 20)
endif()