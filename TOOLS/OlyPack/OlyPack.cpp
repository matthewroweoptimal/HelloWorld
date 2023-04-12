// OyPack.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\types.h>
#include <sys\stat.h>
#include "crc16.h"
#include "Upgrade.h"
#include <share.h>
#include <thread>
#include <chrono>

#define DEFAULT_BOOTLOADER_START			0x00004000
#define DEFAULT_BOOTLOADER_VECTOR_TABLE		DEFAULT_BOOTLOADER_START
#define DEFAULT_BOOTLOADER_STACK_PTR		0x23000000
#define DEFAULT_BOOTLOADER_VERSION_MAJOR	0
#define DEFAULT_BOOTLOADER_VERSION_MINOR	0
#define DEFAULT_BOOTLOADER_VERSION_SUB		0

#define DEFAULT_UPGRADE_START				0x00068000
#define DEFAULT_UPGRADE_VECTOR_TABLE		DEFAULT_BOOTLOADER_START
#define DEFAULT_UPGRADE_STACK_PTR			0x23000000
#define DEFAULT_UPGRADE_VERSION_MAJOR		0
#define DEFAULT_UPGRADE_VERSION_MINOR		0
#define DEFAULT_UPGRADE_VERSION_SUB			0

#define BUFFER_SIZE						0x4000



bool ExtractOption(int argc, _TCHAR* argv[], const _TCHAR *pszCodes, _TCHAR *pszValue, int nMaxSize);
void Log(TCHAR *pszLogLine);

bool g_bLogFile = false;
int g_nMajorVersion = 1;
int g_nMinorVersion = 0;
int g_nLogFile = -1;

int _tmain(int argc, _TCHAR* argv[])
{
	_TCHAR szWork[_MAX_PATH];
	_TCHAR szLogFilepath[_MAX_PATH];
	_TCHAR szSrcFilepath[_MAX_PATH];
	_TCHAR szDestFilepath[_MAX_PATH];
	_TCHAR szLogLine[_MAX_PATH];
	unsigned int dwCodeStart = 0xFFFFFFFF;
	unsigned int dwVectorTable = 0xFFFFFFFF;
	unsigned int dwStackPointer = 0xFFFFFFFF;
	int nSrcFile = -1;
	int nDestFile = -1;
	unsigned char* pBuffer = (unsigned char*)malloc(BUFFER_SIZE);
	unsigned int dwFileSize = 0;
	unsigned short wCrc;
	OLY_REGION rgn;
	int nVersionMajor = -1;
	int nVersionMinor = -1;
	int nVersionSub = -1;
	unsigned int dwWritten = 0;
	int nRc = 1;	//	return code 1= fail, 0 = good

	memset(szLogFilepath, 0, sizeof(szLogFilepath));
	memset(szSrcFilepath, 0, sizeof(szSrcFilepath));
	memset(szDestFilepath, 0, sizeof(szDestFilepath));
/*
	for (int i = 0; i < argc; i++)
	{
		wprintf(_T("argv[%d] = %s\n"), i, argv[i]);
	}
*/

	//	Determine log file name	and open if defined
	if (ExtractOption(argc, argv, _T("Ll"), szLogFilepath, _countof(szLogFilepath)))
	{
		wprintf(_T("Opening log file '%s'\n"), szLogFilepath);
		_tsopen_s(&g_nLogFile, szLogFilepath, _O_CREAT | _O_TRUNC | _O_WRONLY | _O_U8TEXT, _SH_DENYNO, _S_IREAD | _S_IWRITE);
	}

	Log(_T("************************************************\r\n"));
	swprintf_s(szLogLine, _T("OLY Code Image File Packing Tool V%d.%d\r\n"), g_nMajorVersion, g_nMinorVersion);
	Log(szLogLine);
	swprintf_s(szLogLine, _T("Date of Tool Build:%s %s\r\n"), _T(__DATE__), _T(__TIME__));
	Log(szLogLine);
	swprintf_s(szLogLine, _T("Log File: %s\r\n"), szLogFilepath);
	Log(szLogLine);

	if (!pBuffer)
		Log(_T("Memory error, can't allocate CRC/copy bufffer!\r\n"));

	//	Determine if App or secondary boot loader
	//	Assumed to be upgrade file unless there is a "-B" or "-b" option
	bool bSecondaryBootLoader = ExtractOption(argc, argv, L"Bb", NULL, 0);
	if (bSecondaryBootLoader)
		Log(_T("Image type: Secondary boot loader\r\n"));
	else
		Log(_T("Image type: Upgrade firmware (default)\r\n"));

	//	Determine if there is a code start override
	if (ExtractOption(argc, argv, _T("Ss"), szWork, _countof(szWork)))
		_stscanf_s(szWork, _T("%X"), &dwCodeStart);
	if (0xFFFFFFFF!=dwCodeStart)
	{
		swprintf_s(szLogLine, _T("Code start specified:  %08X\r\n"), dwCodeStart);
		Log(szLogLine);
	}
	else
	{
		dwCodeStart = bSecondaryBootLoader?DEFAULT_BOOTLOADER_START:DEFAULT_UPGRADE_START;
		swprintf_s(szLogLine, _T("No code start specified (default):  %08X\r\n"), dwCodeStart);
		Log(szLogLine);
	}

	//	Determine if there is a vector table override
	if (ExtractOption(argc, argv, _T("Tt"), szWork, _countof(szWork)))
		_stscanf_s(szWork, _T("%X"), &dwVectorTable);
	if (0xFFFFFFFF!=dwVectorTable)
	{
		swprintf_s(szLogLine, _T("Vector Table specified:  %08X\r\n"), dwVectorTable);
		Log(szLogLine);
	}
	else
	{
		dwVectorTable = bSecondaryBootLoader?DEFAULT_BOOTLOADER_VECTOR_TABLE:DEFAULT_UPGRADE_VECTOR_TABLE;
		swprintf_s(szLogLine, _T("No vector Table specified (default):  %08X\r\n"), dwVectorTable);
		Log(szLogLine);
	}

	//	Determine if there is a stack pointer overridetable override
	if (ExtractOption(argc, argv, _T("Pp"), szWork, _countof(szWork)))
		_stscanf_s(szWork, _T("%X"), &dwStackPointer);
	if (0xFFFFFFFF!=dwStackPointer)
	{
		swprintf_s(szLogLine, _T("Stack pointer specified:  %08X\r\n"), dwStackPointer);
		Log(szLogLine);
	}
	else
	{
		dwStackPointer = bSecondaryBootLoader?DEFAULT_BOOTLOADER_STACK_PTR:DEFAULT_UPGRADE_STACK_PTR;
		swprintf_s(szLogLine, _T("No stack pointer specified (default):  %08X\r\n"), dwStackPointer);
		Log(szLogLine);
	}

	//	Determine if there is a version override
	if (ExtractOption(argc, argv, _T("Vv"), szWork, _countof(szWork)))
		_stscanf_s(szWork, _T("%d.%d.%d"), &nVersionMajor, &nVersionMinor, &nVersionSub);
	if (-1!=nVersionMajor)
	{
		swprintf_s(szLogLine, _T("Major version specified:  %d\r\n"), nVersionMajor);
		Log(szLogLine);
	}
	else
	{
		nVersionMajor = bSecondaryBootLoader?DEFAULT_BOOTLOADER_VERSION_MAJOR:DEFAULT_UPGRADE_VERSION_MAJOR;
		swprintf_s(szLogLine, _T("No major version specified (default):  %d\r\n"), nVersionMajor);
		Log(szLogLine);
	}
	if (-1!=nVersionMinor)
	{
		swprintf_s(szLogLine, _T("Minor version specified:  %d\r\n"), nVersionMinor);
		Log(szLogLine);
	}
	else
	{
		nVersionMinor = bSecondaryBootLoader?DEFAULT_BOOTLOADER_VERSION_MINOR:DEFAULT_UPGRADE_VERSION_MINOR;
		swprintf_s(szLogLine, _T("No minor version specified (default):  %d\r\n"), nVersionMinor);
		Log(szLogLine);
	}
	if (-1!=nVersionSub)
	{
		swprintf_s(szLogLine, _T("Sub version specified:  %d\r\n"), nVersionSub);
		Log(szLogLine);
	}
	else
	{
		nVersionSub = bSecondaryBootLoader?DEFAULT_BOOTLOADER_VERSION_SUB:DEFAULT_UPGRADE_VERSION_SUB;
		swprintf_s(szLogLine, _T("No sub version specified (default):  %d\r\n"), nVersionSub);
		Log(szLogLine);
	}

	//	Open source bin file
	if (ExtractOption(argc, argv, _T("Ii"), szSrcFilepath, _countof(szSrcFilepath)))
	{
		_tsopen_s(&nSrcFile, szSrcFilepath, _O_RDONLY | _O_BINARY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		if (-1==nSrcFile)
		{
			swprintf_s(szLogLine, _T("Can not open input file '%s'\r\n"), szSrcFilepath);
			Log(szLogLine);
		}
		else
		{
			swprintf_s(szLogLine, _T("Opening input file '%s'\r\n"), szSrcFilepath);
			Log(szLogLine);
		}
	}
	else
		Log(_T("No input file specified!  Please specify filename with -I option\r\n")); 

	//	Open destination bin file
	if (ExtractOption(argc, argv, _T("Oo"), szDestFilepath, _countof(szDestFilepath)))
	{
		_tsopen_s(&nDestFile, szDestFilepath, _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		if (-1==nDestFile)
		{
			swprintf_s(szLogLine, _T("Can not open output file '%s'\r\n"), szDestFilepath);
			Log(szLogLine);
		}
		else
		{
			swprintf_s(szLogLine, _T("Opening output file '%s'\r\n"), szDestFilepath);
			Log(szLogLine);
		}
	}
	else
		Log(_T("No output file specified!  Please specify filename with -O option\r\n")); 

	//	Calculate CRC
	if (-1!=nSrcFile)
	{
		struct stat fileStatus;
		if (!fstat(nSrcFile, &fileStatus))
			dwFileSize = fileStatus.st_size;
		if (!dwFileSize)
		{
			Log(_T("Input file is zero length!\r\n")); 
			_close(nSrcFile);
			nSrcFile = -1;
		}
		else
		{
			swprintf_s(szLogLine, _T("Input file is %d (0x%08X) bytes long.\r\n"), dwFileSize, dwFileSize);
			Log(szLogLine);
		}
	}
	else
		Log(_T("Can't calculate CRC with no valid input file!\r\n")); 

	if ((-1!=nSrcFile) && pBuffer)
	{
		unsigned int dwChunk = BUFFER_SIZE;
		unsigned int dwRemaining = dwFileSize;
		crc16_data_t crc;
		crc16_init(&crc);
		while (dwRemaining) {
			if (dwRemaining<dwChunk)
				dwChunk = dwRemaining;
			unsigned int dwRead = _read(nSrcFile, pBuffer, dwChunk);
			if (dwChunk!=dwRead)
			{
				swprintf_s(szLogLine, _T("Failed reading input file (%d)!\r\n"), errno);
				Log(szLogLine);
				_close(nSrcFile);
				nSrcFile = -1;
				break;
			}
			dwRemaining -= dwChunk;
			if (dwChunk&1)
				*(pBuffer+dwChunk) = 0xFF;
			crc16_update(&crc, pBuffer, (dwChunk+1)&0xFFFFFFFE);
		}
		crc16_finalize(&crc, &wCrc);
		swprintf_s(szLogLine, _T("CRC calclulated: %04X\r\n"), (unsigned int)wCrc);
		Log(szLogLine);
	}

	//	Fill in header values
	memset(&rgn, 0, sizeof(rgn));
	rgn.address = dwCodeStart;
	rgn.length = dwFileSize;
	rgn.stackPtr = dwStackPointer;
	rgn.vectorTable = dwVectorTable;
	rgn.version.major = nVersionMajor;
	rgn.version.minor = nVersionMinor;
	rgn.version.sub = nVersionSub;
	rgn.crc = wCrc;
	rgn.type = bSecondaryBootLoader?OLY_REGION_SECONDARY_BOOT:OLY_REGION_APPLICATION;

	//	Write header
	if (-1!=nDestFile)
	{
		if (sizeof(rgn)!=_write(nDestFile, &rgn, sizeof(rgn)))
		{
			Log(_T("Failed writing output file header!\r\n")); 
			_close(nDestFile);
			nDestFile = -1;
		}
		else
			dwWritten += sizeof(rgn);
	}
	else if (-1!=nSrcFile)
	{
		_close(nSrcFile);
		nSrcFile = -1;
	}

	//	Write code data
	if ((-1!=nDestFile) && (-1!=nSrcFile))
	{
		_lseek(nSrcFile, 0, SEEK_SET);

		unsigned int dwChunk = BUFFER_SIZE;
		unsigned int dwRemaining = dwFileSize;
		while (dwRemaining) {
			if (dwRemaining<dwChunk)
				dwChunk = dwRemaining;
			if (dwChunk!=_read(nSrcFile, pBuffer, dwChunk))
			{
				Log(_T("Failed reading input file!\r\n")); 
				_close(nSrcFile);
				nSrcFile = -1;
				break;
			}
			if (dwChunk&1)
			{
				*(pBuffer+dwChunk) = 0xFF;
				dwChunk += 1;
			}
			if (dwChunk!=_write(nDestFile, pBuffer, dwChunk))
			{
				Log(_T("Failed writing output file!\r\n")); 
				_close(nDestFile);
				nDestFile = -1;
				break;
			}
			else
				dwWritten += dwChunk;
			dwRemaining -= dwChunk;
		}
	}

	//	Close source file
	if (-1!=nSrcFile)
	{
		_close(nSrcFile);
		nSrcFile = -1;
	}

	//	Close destination file
	if (-1!=nDestFile)
	{
		swprintf_s(szLogLine, _T("Output file successfully written with %d (0x%08X) bytes\r\n"), dwWritten, dwWritten);
		Log(szLogLine);
		_close(nDestFile);
		nDestFile = -1;
		nRc = 0;
	}
	else
	{
		Log(_T("Writing output file failed!\r\n"));
	}

	if (-1!=g_nLogFile)
	{
		Log(_T("Closing Log File.\r\n"));
		Log(_T("************************************************\r\n"));
		_close(g_nLogFile);
		g_nLogFile = -1;
	}


	if (nRc)
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	else
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	exit(nRc);

	return 0;
}


//	Looks for a specific option and extracts value
//	returns true if option found
bool ExtractOption(int argc, //	 Number of args
	_TCHAR* argv[],			//	List of args
	const _TCHAR* pszCodes,	//	List of code characters
	_TCHAR* pszValue,		//	Where to put value
	int nMaxSize)			//	size of value buffer
{
	//	clear return buffer
	if (pszValue && nMaxSize)
		memset(pszValue, 0, nMaxSize * sizeof(TCHAR));

	if (argv && pszCodes && (wcslen(pszCodes) > 0))
	{
		//	 Look at all options
		for (int nArg = 1; nArg < argc; nArg++)
		{
			_TCHAR* pszArg = argv[nArg];
			int nArgLength = 0;
			if (pszArg)
				nArgLength = _tcslen(pszArg);

			if (nArgLength > 1)
			{
				TCHAR code = *(pszArg + 1);

				//	Look for option indicator
				if (L'-' == *pszArg)
				{
					//	Search if any codes present
					for (unsigned int dwCode = 0; dwCode < _tcslen(pszCodes); dwCode++)
					{
						if (code == *(pszCodes + dwCode))
						{
							//	copy value?
							if (pszValue && nMaxSize)
							{
								//	Limit value size to buffer
								int nCopyLength = nMaxSize - 1;

								//	Limit value size to arg length
								nArgLength = _tcslen(argv[nArg + 1]);	// SC : Fixed ARG Copying Code, wasn't working
								if (nArgLength < nCopyLength)
									nCopyLength = nArgLength;

								//	If anything left copy it
								if (nArgLength >= 0)
								{
//									wprintf(_T("ARG VALUE = %s\n"), argv[nArg + 1]);
									_tcsncpy_s(pszValue, nMaxSize, argv[nArg + 1], nCopyLength);
								}
							}
							return(true);
						}
					}
				}
			}
		}
	}

	return(false);
}


//	Log a string to the log file if open.
void Log(TCHAR *pszLogLine)
{
	if (pszLogLine)
	{
		if (-1!=g_nLogFile)
		{
			_write(g_nLogFile, pszLogLine, _tcslen(pszLogLine)*sizeof(TCHAR));
		}
		_tprintf(pszLogLine);
		fflush(stdout);
	}
}