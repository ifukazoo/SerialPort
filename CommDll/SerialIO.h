#ifndef _SERIAL_IO_H_
#define _SERIAL_IO_H_

#ifdef COMMDLL_EXPORTS
#define DLLAPI __declspec(dllexport)
#else
#define DLLAPI __declspec(dllimport)
#endif

class DLLAPI SerialIO
{
public:
	SerialIO();
	virtual ~SerialIO();

	bool Open(const TCHAR*, const TCHAR*);
	bool Close();

	enum code {
		SUCCESS,
		TIME_OUT,
		FAIL,
	};

	enum code ReadChunk(char**, int*, DWORD);
	enum code Read(char*, DWORD, DWORD*, DWORD);
	enum code Write(const char*, DWORD, DWORD*, DWORD);

private:
	void operator=(const SerialIO&);
	bool IsInitialized();
	bool CloseResource();

	HANDLE handle_;
	OVERLAPPED readov_;
	OVERLAPPED writeov_;
};
#endif

