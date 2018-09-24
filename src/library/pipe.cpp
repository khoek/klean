/*
Copyright (c) 2016 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Jared Roesch
*/

#include "library/pipe.h"
#include "util/exception.h"


#if defined(LEAN_WINDOWS) && !defined(LEAN_CYGWIN)
#include <io.h>
#include <fcntl.h>
#else
#include <unistd.h>
#endif

namespace lean {

pipe::pipe() {
    int fds[2];

    #if defined(LEAN_WINDOWS) && !defined(LEAN_CYGWIN)
    if (::pipe(fds, 4096, O_BINARY) == -1) {
        throw exception("unable to create pipe");
    }
    #else
    if (::pipe(fds) == -1) {
        throw exception("unable to create pipe");
    }
    #endif

    m_read_fd = fds[0];
    m_write_fd = fds[1];
}

}
