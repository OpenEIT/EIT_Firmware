/*
 * Copyright (C) 2007-2009 IAR Systems AB.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/* $Rev: 21495 $ */

// Elf constants and structures
#include "LxElfException.h"


LxException::
~LxException()
{
}

std::string LxException::
GetMessage() const
{
  return "An unexpected error occured";
}

LxMessageException::
LxMessageException(const std::string & m)
  : mMessage(m)
{
}

std::string LxMessageException::
GetMessage() const
{
  return mMessage;
}


LxFileException::
LxFileException(const std::string & f, Type type)
  : mType(type), mFileName(f)
{
}

std::string LxFileException::
GetMessage() const
{
  std::string fileName = mFileName.empty() ?
                         "ELF file" :
                         "'" + mFileName + "'";

  switch(mType)
  {
  case kFileOpenError:
    return "Could not open " + fileName;
  case kFileReadError:
    return "Could not read " + fileName;
  case kFileWriteError:
    return "Could not write to " + fileName;
  case kWrongElfType:
    return fileName + " is of an invalid ELF type";
  case kWrongElfClass:
    return fileName + " is not a 32 bit ELF file";
  case kParseError:
    return "Error when parsing " + fileName;
  case kSymTableNotFound:
    return fileName + "does not contain a aymbol table.";
  default:
    return "Error accessing " + fileName;
  }
}



LxSymbolException::
LxSymbolException(const std::string & symbol, Type type)
  : mSymbol(symbol), mType(type)
{
}

std::string LxSymbolException::
GetMessage() const
{
  std::string symbol = "'" + mSymbol + "'";
  switch(mType)
  {
  case kStringNotFound:
    return "The string " + symbol + " was not found in the string table";
  case kSymbolNotFound:
    return "The symbol " + symbol + " was not found in the symbol table";
  default:
    return "The symbol " + symbol + " was not found";
  }
}



std::string LxSaveException::
GetMessage() const
{
  return "An error occured when generating the output file";
}

LxChecksumException::
LxChecksumException(Type type)
  : mType(type)
{
}

std::string LxChecksumException::
GetMessage() const
{
  switch(mType)
  {
  case kSymbolInSeveralSections:
    return "The checksum symbol must not span over several ELF sections";
  case kWrongSymbolType:
    return "The checksum symbol is of an invalid type";
  case kWrongSymbolSize:
    return "The checksum symbol is of an invalid size";
  case kChecksumError:
    return "An error occurred when calculating or storing the checksum";
  case kChecksumRangeError:
    return "The checksum range contains uninitialized data";
  case kChecksumOverlapError:
    return "Checksums have dependencies";
  case kMultipleSectionsForSymbol:
    return "The checksum extends over more than one ELF section";
  case kNoSectionForAddress:
    return "No matching section found for absolute address";
  default:
    return "Checksum error";
  }
}

LxFillException::
LxFillException(Type type)
  : mType(type)
{
}

std::string LxFillException::
GetMessage() const
{
  switch(mType)
  {
  case kPatternMissing0x:
    return "Fill pattern must begin with \"0x\".";
  case kOddNumberOfPatternChars:
    return "Fill pattern must have an even number of characters.";
  case kRangeError:
    return "Error in fill ranges.";
  default:
    return "An error occurred during fill.";
  }
}

std::string LxStripException::
GetMessage() const
{
  return "Stripping the elf file failed";
}

