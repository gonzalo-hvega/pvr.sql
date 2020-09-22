cmake_minimum_required(VERSION 3.5)
PROJECT(netsockets)

SET(SOURCES socket.cc http.cc)

ADD_DEFINITIONS(-D_GNU_SOURCE)

ADD_LIBRARY(netsockets STATIC ${SOURCES})
