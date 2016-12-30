CAR Version 3(A)
---------------------

####About

CAR Version 3 is the 'bare bones' CAR format. It stores files and not much else. The main benefit of Version 3 is that there is virtually no added space.

Option                     | Supported
-------------------------- | ---------
Basic Directory Storage    | <font color="#00FF00">YES</font>
Archive Checksum           | <font color="#FF9900">OPTIONAL</font>
Archive Size               | <font color="#00FF00">YES</font>
Raw Size Storage           | <font color="#00FF00">YES</font>
File Random Access         | <font color="#FF0000">NO</font>
File Addition              | <font color="#FF0000">NO</font>
Saves Empty Directories    | <font color="#FF0000">NO</font>
Can Store Symbolic Links   | <font color="#FF0000">NO</font>
Variable Length file names | <font color="#00FF00">YES</font>
File Checksum              | <font color="#FF0000">NO</font>
Saves UNIX Metadata[^1]    | <font color="#FF0000">NO</font>
Saves Extended Attributes  | <font color="#FF0000">NO</font>
Saves Directory Metadata   | <font color="#FF0000">NO</font>
Saves Arbitrary Metadata   | <font color="#FF0000">NO</font>
Extraction Speed / 10      | <font color="#00FF00">8</font>
Space Efficiency / 10	   | <font color="#00FF00">10</font>

####Data

All CAR v3 Files begin with the following 48-Byte header:
```
typedef struct {
	UInt32 magic;       // 'CAR' + a Null Byte
	UInt8  version[3];  // Set to 3.A
	UInt8  flags;       // File Parsing Flags
	UInt64 data;        // Data file offset
	UInt64 filecount;   // Number of Archived Files
	UInt64 extsize;     // Extracted Size
	UInt64 archsize;    // Archive Size
	UInt32 checksum;    // Archive Checksum
	UInt8  reserved[8]; // Fills out to 48 Bytes
} CARHeaderVersion3;
```

The following values are defined for the `flags` field in the CAR Header:

Name                 | Value
-------------------- | -----
Big-Endian           | `(1 << 0)`
Filenames are UTF-16 | `(1 << 1)`
Filenames are UTF-32 | `(1 << 2)`
Has Archive Checksum | `(1 << 3)`

Directly following this header, is the list of all files stored in the archive (This is the file's Table of Contents). `filecount` entries are placed back-to-back in the following format:
```
typedef struct {
	UInt16 nlen;      // Length of file's name
	VChar name[nlen]; // File's name (Including Null Byte)
	UInt64 size;      // Length of file data
	UInt64 offset;    // Offset in the data section
} CAREntryVersion3;
```

A file is stored as `size` bytes `offset` bytes into the data section. The data section begins at the file offset specified by the `data` field in the header.

Notes:
	1. Filenames are stored with all subdirectories listed out, separated with a `/`. All names are stored as non-relative paths with the first `/` representing the archive's root directory.
	2. `nlen` is the number of characters in the file's name, not necessarily the number of bytes. If the name encoding has been set to UTF-16 or UTF-32 in the header, `nlen` must be multiplied by 2 or 4 respectively to get the number of bytes in the file's name.

[^1]: UNIX Metadata includes Group and Owner ID Numbers; Creation, Access, and Modification Times; File Permissions (Mode); and File Type.
