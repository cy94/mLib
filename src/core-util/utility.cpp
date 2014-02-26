
namespace Utility
{
	//
	// These hash functions are taken from http://www.burtleburtle.net/bob/hash/doobs.html
	//

	__forceinline void hashMix(UINT &a, UINT &b, UINT &c)
	{
		a -= b; a -= c; a ^= (c>>13);
		b -= c; b -= a; b ^= (a<<8);
		c -= a; c -= b; c ^= (b>>13);
		a -= b; a -= c; a ^= (c>>12);
		b -= c; b -= a; b ^= (a<<16);
		c -= a; c -= b; c ^= (b>>5);
		a -= b; a -= c; a ^= (c>>3);
		b -= c; b -= a; b ^= (a<<10);
		c -= a; c -= b; c ^= (b>>15);
	}

	UINT hash32(const BYTE *k, UINT length)
	{
		UINT a, b, c, LocalLength;

		/* Set up the internal state */
		LocalLength = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = 0x9b9773e9;

		/*---------------------------------------- handle most of the key */
		while (LocalLength >= 12)
		{
			a += (k[0] + ((UINT)k[1]<<8) + ((UINT)k[2]<<16) + ((UINT)k[3]<<24));
			b += (k[4] + ((UINT)k[5]<<8) + ((UINT)k[6]<<16) + ((UINT)k[7]<<24));
			c += (k[8] + ((UINT)k[9]<<8) + ((UINT)k[10]<<16)+ ((UINT)k[11]<<24));
			hashMix(a, b, c);
			k += 12;
			LocalLength -= 12;
		}

		/*------------------------------------- handle the last 11 bytes */
		c += length;
		switch(LocalLength)              /* all the case statements fall through */
		{
		case 11: c += ((UINT)k[10]<<24);
		case 10: c += ((UINT)k[9]<<16);
		case 9 : c += ((UINT)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 : b += ((UINT)k[7]<<24);
		case 7 : b += ((UINT)k[6]<<16);
		case 6 : b += ((UINT)k[5]<<8);
		case 5 : b += k[4];
		case 4 : a += ((UINT)k[3]<<24);
		case 3 : a += ((UINT)k[2]<<16);
		case 2 : a += ((UINT)k[1]<<8);
		case 1 : a += k[0];
			/* case 0: nothing left to add */
		}
		hashMix(a, b, c);
		/*-------------------------------------------- report the result */
		return c;
	}

	UINT64 hash64(const BYTE *k, UINT length)
	{
		UINT a, b, c, LocalLength;

		/* Set up the internal state */
		LocalLength = length;
		a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
		c = 0x9b9773e9;

		/*---------------------------------------- handle most of the key */
		while (LocalLength >= 12)
		{
			a += (k[0] + ((UINT)k[1]<<8) + ((UINT)k[2]<<16) + ((UINT)k[3]<<24));
			b += (k[4] + ((UINT)k[5]<<8) + ((UINT)k[6]<<16) + ((UINT)k[7]<<24));
			c += (k[8] + ((UINT)k[9]<<8) + ((UINT)k[10]<<16)+ ((UINT)k[11]<<24));
			hashMix(a, b, c);
			k += 12;
			LocalLength -= 12;
		}

		/*------------------------------------- handle the last 11 bytes */
		c += length;
		switch(LocalLength)              /* all the case statements fall through */
		{
		case 11: c += ((UINT)k[10]<<24);
		case 10: c += ((UINT)k[9]<<16);
		case 9 : c += ((UINT)k[8]<<8);
			/* the first byte of c is reserved for the length */
		case 8 : b += ((UINT)k[7]<<24);
		case 7 : b += ((UINT)k[6]<<16);
		case 6 : b += ((UINT)k[5]<<8);
		case 5 : b += k[4];
		case 4 : a += ((UINT)k[3]<<24);
		case 3 : a += ((UINT)k[2]<<16);
		case 2 : a += ((UINT)k[1]<<8);
		case 1 : a += k[0];
			/* case 0: nothing left to add */
		}
		hashMix(a, b, c);
		/*-------------------------------------------- report the result */
		return UINT64(c) + UINT64(UINT64(a) << 32);
	}

    bool fileExists(const std::string &filename)
    {
        std::ifstream file(filename);
        return (!file.fail());
    }

    std::string getNextLine(std::ifstream &file)
    {
        std::string line;
        getline(file, line);
        return std::string(line.c_str());
    }

    Vector<std::string> getFileLines(std::ifstream &file, UINT minLineLength)
    {
		Vector<std::string> result;
        std::string line;
        while(!file.fail())
        {
            getline(file, line);
            if(!file.fail() && line.length() >= minLineLength) result.pushBack(line.c_str());
        }
		return result;
    }

    Vector<std::string> getFileLines(const std::string &filename, UINT minLineLength)
    {
        std::ifstream file(filename);
        MLIB_ASSERT_STR(!file.fail(), std::string("Failed to open ") + filename);
        return getFileLines(file, minLineLength);
    }

	Vector<BYTE> getFileData(const std::string &filename)
	{
		FILE *inputFile = Utility::checkedFOpen(filename.c_str(), "rb");
		UINT fileSize = Utility::getFileSize(filename);
		Vector<BYTE> result(fileSize);
		Utility::checkedFRead(result.ptr(), sizeof(BYTE), fileSize, inputFile);
		fclose(inputFile);
		return result;
	}

	void copyFile(const std::string &sourceFile, const std::string &destFile)
	{
		Vector<BYTE> data = getFileData(sourceFile);
		FILE *file = Utility::checkedFOpen(destFile.c_str(), "wb");
		Utility::checkedFWrite(data.ptr(), sizeof(BYTE), data.size(), file);
		fclose(file);
	}

#ifdef WIN32
	void copyStringToClipboard(const std::string &S)
	{
		OpenClipboard(NULL);
		EmptyClipboard();

		HGLOBAL globalHandle;
		size_t bytesToCopy = S.length() + 1;
		globalHandle = GlobalAlloc(GMEM_MOVEABLE, bytesToCopy);
		if(globalHandle != NULL)
		{
			BYTE *stringPointer = (BYTE*)GlobalLock(globalHandle); 
			memcpy(stringPointer, S.c_str(), bytesToCopy); 
			GlobalUnlock(globalHandle);
			SetClipboardData(CF_TEXT, globalHandle);
		}
		CloseClipboard();
	}

	std::string loadStringFromClipboard()
	{
		std::string result;

		OpenClipboard(NULL);
		HGLOBAL globalHandle = GetClipboardData(CF_TEXT);
		if(globalHandle != NULL)
		{
			const char *stringPointer = (const char *)GlobalLock(globalHandle);
			if(stringPointer != NULL)
			{
				result = stringPointer;
				GlobalUnlock(GlobalHandle);
			}
		}
		CloseClipboard();

		return result;
	}

    UINT getFileSize(const std::string &filename)
    {
        BOOL success;
        WIN32_FILE_ATTRIBUTE_DATA fileInfo;
        success = GetFileAttributesExA(filename.c_str(), GetFileExInfoStandard, (void*)&fileInfo);
        MLIB_ASSERT_STR(success && fileInfo.nFileSizeHigh == 0, std::string("GetFileAttributesEx failed on ") + filename);
        return fileInfo.nFileSizeLow;
    }
    
    // Create a process with the given command line, and wait until it returns
    int runCommand(const std::string &executablePath, const std::string& commandLine, bool block)
    {
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory( &si, sizeof(si) );
        si.cb = sizeof(si);
        ZeroMemory( &pi, sizeof(pi) );

        std::string fullCommandLine = executablePath + " " + commandLine;
		char* fullCommandLinePtr = new char[fullCommandLine.length()+1];
		strcpy(fullCommandLinePtr, fullCommandLine.c_str());

        // Start the child process. 
        if( !CreateProcessA( NULL,  // No module name (use command line)
            fullCommandLinePtr,		// Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory 
            &si,            // Pointer to STARTUPINFO structure
            &pi )           // Pointer to PROCESS_INFORMATION structure
        ) 
        {
            MLIB_ERROR("CreateProcess failed");
            return -1;
        }
		SAFE_DELETE_ARRAY(fullCommandLinePtr);

        if(block)
        {
            // Wait until child process exits.
            WaitForSingleObject( pi.hProcess, INFINITE );
        }

        // Close process and thread handles. 
        CloseHandle( pi.hProcess );
        CloseHandle( pi.hThread );
        return 0;
    }

    void makeDirectory(const std::string &directory)
    {
        CreateDirectoryA(directory.c_str(), NULL);
    }
#endif

#ifdef LINUX
	void copyStringToClipboard(const std::string &S)
	{
		
	}

	std::string loadStringFromClipboard()
	{
		return "";
	}

	UINT getFileSize(const std::string &filename)
	{
		return 0;
	}

	// Create a process with the given command line, and wait until it returns
	int runCommand(const std::string &executablePath, const std::string &commandLine, bool block)
	{
		return 0;
	}

	void makeDirectory(const std::string &directory)
	{
		mkdir(directory.ptr(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}
#endif
}
