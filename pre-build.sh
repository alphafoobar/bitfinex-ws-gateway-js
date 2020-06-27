#!/usr/bin/env bash

echo "CMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
echo "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
echo "DEPENDENCY_BASE_DIR=${DEPENDENCY_BASE_DIR}"

if [[ -z "${DEPENDENCY_BASE_DIR}" ]]
then
    export DEPENDENCY_BASE_DIR=$(pwd)
fi

export CJSON_VERSION=1.7.12
export LWS_VERSION=4.0.1
export CMOCKA_VERSION=1.1.5
export SOURCE_DIR=${DEPENDENCY_BASE_DIR}/src-dependencies
export LIBS_DIR=${DEPENDENCY_BASE_DIR}/libraries

echo "SOURCE_DIR=${SOURCE_DIR}"
if [[ -d "${SOURCE_DIR}" ]]
then
    rm -rf ${SOURCE_DIR}
fi
mkdir -p ${SOURCE_DIR}

echo "LIBS_DIR=${LIBS_DIR}"
if [[ -d "${LIBS_DIR}" ]]
then
    rm -rf ${LIBS_DIR}
fi
mkdir -p ${LIBS_DIR}

rm -rf ${SOURCE_DIR}/lws && \
    wget https://github.com/warmcat/libwebsockets/archive/v${LWS_VERSION}.tar.gz -O /tmp/libwebsockets.tar.gz && \
    rm -rf ${SOURCE_DIR}/libwebsockets && \
    mkdir -p ${SOURCE_DIR}/libwebsockets && \
    tar --strip=1 -xf /tmp/libwebsockets.tar.gz -C ${SOURCE_DIR}/libwebsockets && \
    rm /tmp/libwebsockets.tar.gz && \
    cd ${SOURCE_DIR}/libwebsockets && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_BUILD_TYPE=Release \
        -DLWS_IPV6=ON \
        -DLWS_WITHOUT_BUILTIN_GETIFADDRS=ON \
        -DLWS_WITHOUT_EXTENSIONS=ON \
        -DLWS_WITHOUT_TESTAPPS=ON \
        -DLWS_WITH_STATIC=ON \
        -DLWS_WITH_ZIP_FOPS=OFF \
        -DLWS_WITH_ZLIB=OFF \
        -DLWS_WITH_SHARED=OFF \
        -DOPENSSL_ROOT_DIR=/usr/local/opt/openssl \
        -DCMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE=/usr/local/opt/openssl \
        -DCMAKE_INSTALL_PREFIX=${LIBS_DIR}/libwebsockets/${LWS_VERSION} && \
    make install

cd ${SOURCE_DIR}
wget https://github.com/DaveGamble/cJSON/archive/v${CJSON_VERSION}.tar.gz -O /tmp/cjson.tar.gz && \
    rm -rf ${SOURCE_DIR}/cjson && \
    mkdir -p ${SOURCE_DIR}/cjson && \
    tar --strip=1 -xf /tmp/cjson.tar.gz -C ${SOURCE_DIR}/cjson && \
    rm /tmp/cjson.tar.gz && \
    cd ${SOURCE_DIR}/cjson && \
    cmake . \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DENABLE_CJSON_UTILS=Off \
        -DENABLE_CJSON_TEST=Off \
        -DBUILD_SHARED_AND_STATIC_LIBS=On \
        -DCMAKE_INSTALL_PREFIX=${LIBS_DIR}/cJSON/${CJSON_VERSION} && \
    make install

cd ${SOURCE_DIR}
wget https://gitlab.com/cmocka/cmocka/-/archive/cmocka-${CMOCKA_VERSION}/cmocka-cmocka-${CMOCKA_VERSION}.tar.gz -O /tmp/cmocka.tar.gz && \
    rm -rf ${SOURCE_DIR}/cmocka && \
    mkdir -p ${SOURCE_DIR}/cmocka && \
    tar --strip=1 -xf /tmp/cmocka.tar.gz -C ${SOURCE_DIR}/cmocka && \
    rm /tmp/cmocka.tar.gz && \
    cd ${SOURCE_DIR}/cmocka && \
    mkdir -p build && \
    cd build && \
    cmake .. \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_INSTALL_PREFIX=${LIBS_DIR}/cmocka/${CMOCKA_VERSION} \
        -DWITH_STATIC_LIB=On \
        -DCMAKE_BUILD_TYPE=Debug && \
    make install

