/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "AbstractSingleDiskAdaptor.h"
#include "File.h"
#include "SingleFileAllocationIterator.h"
#ifdef HAVE_POSIX_FALLOCATE
# include "FallocFileAllocationIterator.h"
#endif // HAVE_POSIX_FALLOCATE
#include "DiskWriter.h"

namespace aria2 {

AbstractSingleDiskAdaptor::AbstractSingleDiskAdaptor():
  totalLength(0), _readOnly(false) {}

AbstractSingleDiskAdaptor::~AbstractSingleDiskAdaptor() {}

void AbstractSingleDiskAdaptor::initAndOpenFile()
{
  diskWriter->initAndOpenFile(getFilePath(), totalLength);
}

void AbstractSingleDiskAdaptor::openFile()
{
  diskWriter->openFile(getFilePath(), totalLength);
}

void AbstractSingleDiskAdaptor::closeFile()
{
  diskWriter->closeFile();
}

void AbstractSingleDiskAdaptor::openExistingFile()
{
  diskWriter->openExistingFile(getFilePath(), totalLength);
}

void AbstractSingleDiskAdaptor::writeData(const unsigned char* data, size_t len, off_t offset)
{
  diskWriter->writeData(data, len, offset);
}

ssize_t AbstractSingleDiskAdaptor::readData(unsigned char* data, size_t len, off_t offset)
{
  return diskWriter->readData(data, len, offset);
}

bool AbstractSingleDiskAdaptor::fileExists()
{
  return File(getFilePath()).exists();
}

uint64_t AbstractSingleDiskAdaptor::size()
{
  return diskWriter->size();
}

void AbstractSingleDiskAdaptor::truncate(uint64_t length)
{
  diskWriter->truncate(length);
}

FileAllocationIteratorHandle AbstractSingleDiskAdaptor::fileAllocationIterator()
{
#ifdef HAVE_POSIX_FALLOCATE
  if(_fallocate) {
    SharedHandle<FallocFileAllocationIterator> h
      (new FallocFileAllocationIterator(this, size(), totalLength));
    return h;
  } else
#endif // HAVE_POSIX_FALLOCATE
    {
      SingleFileAllocationIteratorHandle h
	(new SingleFileAllocationIterator(this, size(), totalLength));
      h->init();
      return h;
    }
}

void AbstractSingleDiskAdaptor::enableDirectIO()
{
  diskWriter->enableDirectIO();
}

void AbstractSingleDiskAdaptor::disableDirectIO()
{
  diskWriter->disableDirectIO();
}

bool AbstractSingleDiskAdaptor::directIOAllowed() const
{
  return diskWriter->directIOAllowed();
}

void AbstractSingleDiskAdaptor::enableReadOnly()
{
  diskWriter->enableReadOnly();
  _readOnly = true;
}

void AbstractSingleDiskAdaptor::disableReadOnly()
{
  diskWriter->disableReadOnly();
  _readOnly = false;
}

void AbstractSingleDiskAdaptor::cutTrailingGarbage()
{
  if(File(getFilePath()).size() > totalLength) {
    diskWriter->truncate(totalLength);
  }
}
  
void AbstractSingleDiskAdaptor::setDiskWriter(const DiskWriterHandle& diskWriter)
{
  this->diskWriter = diskWriter;
}

DiskWriterHandle AbstractSingleDiskAdaptor::getDiskWriter() const
{
  return diskWriter;
}

void AbstractSingleDiskAdaptor::setTotalLength(const uint64_t& totalLength)
{
  this->totalLength = totalLength;
}

uint64_t AbstractSingleDiskAdaptor::getTotalLength() const
{
  return totalLength;
}

} // namespace aria2
