/**
 * @file
 *
 * Sink/Source wrapper FILE operations
 */

/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Contributed by Qualcomm Connected Experiences, Inc.,
 *    with authorization from the AllSeen Alliance, Inc.
 *    
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *    
 *        http://www.apache.org/licenses/LICENSE-2.0
 *    
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 *    
 *    Pursuant to Section 1 of the License, the work of authorship constituting
 *    a Work and any Contribution incorporated in the Work shall mean only that
 *    Contributor's code submissions authored by that Contributor.  Any rights
 *    granted under the License are conditioned upon acceptance of these
 *    clarifications.
 ******************************************************************************/
#include <qcc/platform.h>

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <qcc/Debug.h>
#include <qcc/FileStream.h>
#include <qcc/String.h>

using namespace std;
using namespace qcc;

/** @internal */
#define QCC_MODULE  "STREAM"

QStatus qcc::DeleteFile(qcc::String fileName)
{
    if (unlink(fileName.c_str())) {
        return ER_OS_ERROR;
    } else {
        return ER_OK;
    }
}

FileSource::FileSource(qcc::String fileName) :
    fd(open(fileName.c_str(), O_RDONLY)), event(new Event(fd, Event::IO_READ)), ownsFd(true), locked(false)
{
#ifndef NDEBUG
    if (0 > fd) {
        QCC_DbgHLPrintf(("open(\"%s\") failed: %d - %s", fileName.c_str(), errno, strerror(errno)));
    }
#endif
}

FileSource::FileSource() :
    fd(0), event(new Event(fd, Event::IO_READ)), ownsFd(false), locked(false)
{
}

FileSource::FileSource(const FileSource& other) :
    fd(dup(other.fd)), event(new Event(fd, Event::IO_READ)), ownsFd(true), locked(other.locked)
{
}

FileSource FileSource::operator=(const FileSource& other)
{
    if (&other != this) {
        if (ownsFd && (0 <= fd)) {
            close(fd);
        }
        fd = dup(other.fd);
        delete event;
        event = new Event(fd, Event::IO_READ);
        ownsFd = true;
        locked = other.locked;
    }
    return *this;
}

FileSource::~FileSource()
{
    if (ownsFd && (0 <= fd)) {
        close(fd);
    }
    delete event;
}

QStatus FileSource::PullBytes(void* buf, size_t reqBytes, size_t& actualBytes, uint32_t timeout)
{
    QCC_UNUSED(timeout);
    QCC_DbgTrace(("FileSource::PullBytes(buf = %p, reqBytes = %u, actualBytes = <>)",
                  buf, reqBytes));
    if (0 > fd) {
        return ER_INIT_FAILED;
    }
    if (reqBytes == 0) {
        actualBytes = 0;
        return ER_OK;
    }
    ssize_t ret = read(fd, buf, reqBytes);
    if (0 > ret) {
        QCC_LogError(ER_FAIL, ("read returned error (%d)", errno));
        return ER_FAIL;
    } else {
        actualBytes = ret;
        return (0 == ret) ? ER_EOF : ER_OK;
    }
}

bool FileSource::Lock(bool block)
{
    if (fd < 0) {
        return false;
    }
    if (!locked) {
        int ret = flock(fd, block ? LOCK_EX : LOCK_EX | LOCK_NB);
        if (ret && errno != EWOULDBLOCK) {
            QCC_LogError(ER_OS_ERROR, ("Lock fd %d failed with '%s'", fd, strerror(errno)));
        }
        locked = (ret == 0);
    }
    return locked;
}

void FileSource::Unlock()
{
    if (fd >= 0 && locked) {
        flock(fd, LOCK_UN);
        locked = false;
    }
}

FileSink::FileSink(qcc::String fileName, Mode mode)
    : fd(-1), event(new Event(fd, Event::IO_WRITE)), ownsFd(true), locked(false)
{
#ifdef QCC_OS_ANDROID
    /* Android uses per-user groups so user and group permissions are the same */
    mode_t fileMode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    mode_t dirMode = S_IRWXU | S_IRWXG | S_IXOTH;
    if (WORLD_READABLE & mode) {
        fileMode |= S_IROTH;
        dirMode |= S_IROTH;
    }
    if (WORLD_WRITABLE & mode) {
        fileMode |= S_IWOTH;
        dirMode |= S_IWOTH;
    }
#else
    /* Default for plain posix is user permissions only */
    mode_t fileMode = S_IRUSR | S_IWUSR;
    mode_t dirMode = S_IRWXU | S_IXGRP | S_IXOTH;
    if (WORLD_READABLE & mode) {
        fileMode |= S_IRGRP | S_IROTH;
        dirMode |= S_IRGRP | S_IROTH;
    }
    if (WORLD_WRITABLE & mode) {
        fileMode |= S_IWGRP | S_IWOTH;
        dirMode |= S_IWGRP | S_IWOTH;
    }
#endif

    /* Create the intermediate directories */
    size_t begin = 0;
    for (size_t end = fileName.find("/", begin); end != String::npos; end = fileName.find("/", begin)) {

        /* Skip consecutive slashes */
        if (begin == end) {
            ++begin;
            continue;
        }

        /* Get the directory path */
        String p = fileName.substr(0, end);

        /* Only try to create the directory if it doesn't already exist */
        struct stat sb;
        if (0 > stat(p.c_str(), &sb)) {
            if (0 > mkdir(p.c_str(), dirMode)) {
                QCC_LogError(ER_OS_ERROR, ("mkdir(%s) failed with '%s'", p.c_str(), strerror(errno)));
                return;
            }
        }
        begin = end + 1;
    }

    /* Create and open the file */
    fd = open(fileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC, fileMode);
    if (0 > fd) {
        QCC_LogError(ER_OS_ERROR, ("open(%s) failed with '%s'", fileName.c_str(), strerror(errno)));
    }
}

FileSink::FileSink()
    : fd(1), event(new Event(fd, Event::IO_WRITE)), ownsFd(false), locked(false)
{
}

FileSink::FileSink(const FileSink& other) :
    fd(dup(other.fd)), event(new Event(fd, Event::IO_WRITE)), ownsFd(true), locked(other.locked)
{
}

FileSink FileSink::operator=(const FileSink& other)
{
    if (&other != this) {
        if (ownsFd && (0 <= fd)) {
            close(fd);
        }
        fd = dup(other.fd);
        delete event;
        event = new Event(fd, Event::IO_WRITE);
        ownsFd = true;
        locked = other.locked;
    }
    return *this;
}

FileSink::~FileSink()
{
    if (ownsFd && (0 <= fd)) {
        close(fd);
    }
    delete event;
}

QStatus FileSink::PushBytes(const void* buf, size_t numBytes, size_t& numSent)
{
    if (0 > fd) {
        return ER_INIT_FAILED;
    }

    ssize_t ret = write(fd, buf, numBytes);
    if (0 <= ret) {
        numSent = ret;
        return ER_OK;
    } else {
        QCC_LogError(ER_FAIL, ("write failed (%d)", errno));
        return ER_FAIL;
    }
}

bool FileSink::Lock(bool block)
{
    if (fd < 0) {
        return false;
    }
    if (!locked) {
        int ret = flock(fd, block ? LOCK_EX : LOCK_EX | LOCK_NB);
        if (ret && errno != EWOULDBLOCK) {
            QCC_LogError(ER_OS_ERROR, ("Lock fd %d failed with '%s'", fd, strerror(errno)));
        }
        locked = (ret == 0);
    }
    return locked;
}

void FileSink::Unlock()
{
    if (fd >= 0 && locked) {
        flock(fd, LOCK_UN);
        locked = false;
    }
}
