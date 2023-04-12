#ifndef MERRORCODES_H
#define MERRORCODES_H



//**************************************************************************
//*** Error Codes
//**************************************************************************

//--------------------------------------------------------------------------
// generic error codes that all methods may use
												//	Description
												//	-----------
const UInt32 noErr				= 0x00000000;	//	no error has occured
											
const UInt32 dataErr			= 0x00000001;	//	a generic data error has occured (avoid using this)
											
const UInt32 lowMemoryErr		= 0x00000002;	//	there isn't sufficient memory to continue

const UInt32 bufferOverflowErr	= 0x00000003;	//	doing that will overflow an internal buffer

const UInt32 bufferUnderflowErr	= 0x00000004;	//	doing that will underflow an internal buffer

const UInt32 timeOutErr			= 0x00000006;	//  The operation timed out before it could complete

const UInt32 wrongThreadErr		= 0x00000007;	//  The function was called from the wrong thread

const UInt32 operationBusyErr	= 0x00000008;	//  The function was called before the previous operation
												//  completed.  Wait until the previous operation completes 
												//  before calling again.

const UInt32 listFullErr		= 0x00000009;	//  There are too many added; adding one more would overflow.
												//  The object was not added

const UInt32 notInListErr		= 0x00000010;	//	The object you tried to remove from a list was not 
												//	in the list (and therefore was not removed)

const UInt32 alreadyInListErr	= 0x000000011;	//	The object you tried to add to a list is already in that list	

//--------------------------------------------------------------------------
// error codes returned by M_File methods
												// Equivalent Error Codes
												//	Windows - see "System Error Codes" in MSDN
												//	Mac - man 2 open, man 2 read, man 2 write, etc.
												
												//	Windows							Mac
												//	-------							------						
const UInt32 fileNotFoundErr	= 0x00000500;	//	ERROR_FILE_NOT_FOUND 			ENOENT
												//	ERROR_PATH_NOT_FOUND			ELOOP
												//	ERROR_INVALID_DRIVE
												//	ERROR_PATH_BUSY

const UInt32 badPathErr			= 0x00000501;	//	ERROR_BAD_PATHNAME				ENOTDIR
												//	ERROR_INVALID_NAME

const UInt32 fileNameErr		= 0x00000502;	//	ERROR_BUFFER_OVERFLOW			ENAMETOOLONG
												//	ERROR_FILENAME_EXCED_RANGE

const UInt32 tooManyFilesErr	= 0x00000503;	//	ERROR_TOO_MANY_OPEN_FILES 		EMFILE
												//	ERROR_NO_MORE_SEARCH_HANDLES

const UInt32 accessDeniedErr	= 0x00000504;	//	ERROR_ACCESS_DENIED				EACCES
												//	ERROR_SHARING_VIOLATION
												//	ERROR_WRITE_PROTECT

const UInt32 diskFullErr		= 0x00000505;	//	ERROR_HANDLE_DISK_FULL			ENOSPC
												//	ERROR_DISK_FULL

const UInt32 fileExistsErr		= 0x00000506;	//	ERROR_FILE_EXISTS				EEXIST

const UInt32 cantCreateErr		= 0x00000507;	//	ERROR_CANNOT_MAKE				EDQUOT
												//									ENFILE

const UInt32 alreadyOpenErr		= 0x00000508;	//	The file is already open (M_File::Open()).

const UInt32 badFileHandle		= 0x00000509;	//	ERROR_INVALID_HANDLE			EBADF
												//	ERROR_INVALID_HANDLE_STATE

const UInt32 fileIOErr			= 0x00000510;	//	ERROR_READ_FAULT				EIO
												//	ERROR_WRITE_FAULT				EFBIG
												//	ERROR_NET_WRITE_FAULT
												//	ERROR_NOT_READY
					
const UInt32 fileParameterErr	= 0x00000511;	//	ERROR_INVALID_PARAMETER			EINVAL
												//									EFAULT

const UInt32 fileOtherErr		= 0x00000512;	//	Default M_File error code.
												//	(Any other M_File error not covered by a specific
												//	error code.

//--------------------------------------------------------------------------
// error codes returned by PresetDocument methods
												//	Description
												//	-----------
const UInt32 fileLengthErr		= 0x00000600;	//	file length is not what was expected

const UInt32 headerDataErr		= 0x00000601;	//	Error in the file header Data

const UInt32 presetChunkDataErr	= 0x00000602;	//	Error in the preset chunk Data

const UInt32 fileOpErr			= 0x00000603;	//	Generic file operation error

const UInt32 invalidPresetErr	= 0x00000604;	//  the document contains the wrong kind of preset data.


//--------------------------------------------------------------------------
// BucketNet method error codes
												//  Description
												//  -----------
const UInt32 invalidBNMsg		= 0x00000700;	//  The BNMsg given had a problem

const UInt32 bnmsgQFull			= 0x00000701;	//  The BNMsg Queue is full


typedef UInt32 stdErr;


#endif