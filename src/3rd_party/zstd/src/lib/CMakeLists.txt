# ################################################################
# Copyright (c) 2015-present, Yann Collet, Facebook, Inc.
# All rights reserved.
#
# This source code is licensed under both the BSD-style license (found in the
# LICENSE file in the root directory of this source tree) and the GPLv2 (found
# in the COPYING file in the root directory of this source tree).
# ################################################################

project(libzstd)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Define library directory, where sources and header files are located
include_directories(${LIBRARY_DIR} ${LIBRARY_DIR}/common)

set(Sources
        ${LIBRARY_DIR}/common/entropy_common.c
        ${LIBRARY_DIR}/common/fse_decompress.c
        ${LIBRARY_DIR}/common/threading.c
        ${LIBRARY_DIR}/common/pool.c
        ${LIBRARY_DIR}/common/zstd_common.c
        ${LIBRARY_DIR}/common/error_private.c
        ${LIBRARY_DIR}/common/xxhash.c
        ${LIBRARY_DIR}/compress/hist.c
        ${LIBRARY_DIR}/compress/fse_compress.c
        ${LIBRARY_DIR}/compress/huf_compress.c
        ${LIBRARY_DIR}/compress/zstd_compress.c
        ${LIBRARY_DIR}/compress/zstdmt_compress.c
        ${LIBRARY_DIR}/compress/zstd_fast.c
        ${LIBRARY_DIR}/compress/zstd_double_fast.c
        ${LIBRARY_DIR}/compress/zstd_lazy.c
        ${LIBRARY_DIR}/compress/zstd_opt.c
        ${LIBRARY_DIR}/compress/zstd_ldm.c
        ${LIBRARY_DIR}/decompress/huf_decompress.c
        ${LIBRARY_DIR}/decompress/zstd_decompress.c
        ${LIBRARY_DIR}/decompress/zstd_decompress_block.c
        ${LIBRARY_DIR}/decompress/zstd_ddict.c
        #${LIBRARY_DIR}/dictBuilder/cover.c
        #${LIBRARY_DIR}/dictBuilder/fastcover.c
        #${LIBRARY_DIR}/dictBuilder/divsufsort.c
        #${LIBRARY_DIR}/dictBuilder/zdict.c
        #${LIBRARY_DIR}/deprecated/zbuff_common.c
        #${LIBRARY_DIR}/deprecated/zbuff_compress.c
        #${LIBRARY_DIR}/deprecated/zbuff_decompress.c
)

set(Headers
        ${LIBRARY_DIR}/zstd.h
        ${LIBRARY_DIR}/common/debug.h
        ${LIBRARY_DIR}/common/pool.h
        ${LIBRARY_DIR}/common/threading.h
        ${LIBRARY_DIR}/common/bitstream.h
        ${LIBRARY_DIR}/common/error_private.h
        ${LIBRARY_DIR}/common/zstd_errors.h
        ${LIBRARY_DIR}/common/fse.h
        ${LIBRARY_DIR}/common/huf.h
        ${LIBRARY_DIR}/common/mem.h
        ${LIBRARY_DIR}/common/zstd_internal.h
        ${LIBRARY_DIR}/compress/hist.h
        ${LIBRARY_DIR}/compress/zstd_compress_internal.h
        ${LIBRARY_DIR}/compress/zstd_fast.h
        ${LIBRARY_DIR}/compress/zstd_double_fast.h
        ${LIBRARY_DIR}/compress/zstd_lazy.h
        ${LIBRARY_DIR}/compress/zstd_opt.h
        ${LIBRARY_DIR}/compress/zstd_ldm.h
        ${LIBRARY_DIR}/compress/zstdmt_compress.h
        ${LIBRARY_DIR}/decompress/zstd_decompress_internal.h
        ${LIBRARY_DIR}/decompress/zstd_decompress_block.h
        ${LIBRARY_DIR}/decompress/zstd_ddict.h
        #${LIBRARY_DIR}/dictBuilder/zdict.h
        #${LIBRARY_DIR}/dictBuilder/cover.h
        #${LIBRARY_DIR}/deprecated/zbuff.h
)


add_library(libzstd_static STATIC ${Sources} ${Headers})
if (ZSTD_MULTITHREAD_SUPPORT)
    set_property(TARGET libzstd_static APPEND PROPERTY COMPILE_DEFINITIONS "ZSTD_MULTITHREAD")
    target_link_libraries(libzstd_static ${THREADS_LIBS})
endif ()

set(STATIC_LIBRARY_BASE_NAME zstd)

set_target_properties(
    libzstd_static
    PROPERTIES
    OUTPUT_NAME ${STATIC_LIBRARY_BASE_NAME}
)

