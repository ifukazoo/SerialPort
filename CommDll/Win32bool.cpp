#include <Windows.h>
#include <iostream>
#include "Win32Bool.h"


static void PrintErrorMsg(DWORD, const char *, int);
static LPTSTR GetLastErrorMsg(DWORD);

namespace win32bool {

bool CreateFile(
	LPCTSTR lpFileName,                         // �t�@�C����
	DWORD dwDesiredAccess,                      // �A�N�Z�X���[�h
	DWORD dwShareMode,                          // ���L���[�h
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, // �Z�L�����e�B�L�q�q
	DWORD dwCreationDisposition,                // �쐬���@
	DWORD dwFlagsAndAttributes,                 // �t�@�C������
	HANDLE hTemplateFile,                       // �e���v���[�g�t�@�C���̃n���h��
	HANDLE* handle)
{
	HANDLE got = ::CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (got != INVALID_HANDLE_VALUE) {
		*handle = got;
	}
	return got != INVALID_HANDLE_VALUE;
}

bool CreateEvent(
	LPSECURITY_ATTRIBUTES lpEventAttributes, // �Z�L�����e�B�L�q�q
	BOOL bManualReset,                       // ���Z�b�g�̃^�C�v
	BOOL bInitialState,                      // �������
	LPCTSTR lpName,                           // �C�x���g�I�u�W�F�N�g�̖��O
	HANDLE* handle)
{
	HANDLE got = ::CreateEvent(lpEventAttributes, bManualReset, bInitialState, lpName);
	if (got) {
		*handle = got;
	}
	return got != NULL;
}

bool WaitForSingleObject(
	HANDLE hHandle,        // �I�u�W�F�N�g�̃n���h��
	DWORD dwMilliseconds,   // �^�C���A�E�g����
	DWORD* lpReason)
{
	DWORD reason = ::WaitForSingleObject(hHandle, dwMilliseconds);
	if (reason == WAIT_ABANDONED || reason == WAIT_OBJECT_0 || reason == WAIT_TIMEOUT) {
		*lpReason = reason;
		return true;
	}
	return false;
}

bool FormatMessage(
	DWORD dwFlags,      // ���͌��Ə������@�̃I�v�V����
	LPCVOID lpSource,   // ���b�Z�[�W�̓��͌�
	DWORD dwMessageId,  // ���b�Z�[�W���ʎq
	DWORD dwLanguageId, // ���ꎯ�ʎq
	LPTSTR lpBuffer,    // ���b�Z�[�W�o�b�t�@
	DWORD nSize,        // ���b�Z�[�W�o�b�t�@�̍ő�T�C�Y
	DWORD* written,
	va_list *Arguments) {  // �����̃��b�Z�[�W�}���V�[�P���X����Ȃ�z��
	DWORD number = ::FormatMessage(dwFlags, lpSource, dwMessageId, dwLanguageId, lpBuffer, nSize, Arguments);
	if (0 <= number)
	{
		*written = number;
	}
	return !(number <= 0);
}

bool TryWin32(BOOL success, const char * function, int linenumber)
{
	if (!success) {
		DWORD dwLastError = ::GetLastError();
		PrintErrorMsg(dwLastError, function, linenumber);
	}
	return success != FALSE;
}

bool TryWin32AsyncIO(BOOL success, const char * function, int linenumber)
{
	if (!success) {
		DWORD dwLastError = ::GetLastError();
		if (dwLastError != ERROR_IO_PENDING) {
			PrintErrorMsg(dwLastError, function, linenumber);
			return false;
		}
	}
	return true;
}

bool TryClearCommError(
	HANDLE hFile,     // �ʐM�f�o�C�X�̃n���h��
	LPDWORD lpErrors, // �G���[�R�[�h���󂯎��ϐ��ւ̃|�C���^
	LPCOMSTAT lpStat,  // �ʐM��ԃo�b�t�@�ւ̃|�C���^
	const char * function,
	int linenumber)
{

	if (!::ClearCommError(hFile, lpErrors, lpStat)) {
		DWORD dwLastError = ::GetLastError();
		PrintErrorMsg(dwLastError, function, linenumber);
		return false;
	}
	if (*lpErrors) {
		std::wcout << "reason =[" << *lpErrors << "] func=" << function << " line=" << linenumber << std::endl;
		return false;
	}
	return true;
}

} // namespace win32bool

static void PrintErrorMsg(DWORD dwLastError, const char * function, int linenumber)
{
	LPTSTR msg = GetLastErrorMsg(dwLastError);
	if (msg) {
		std::wcout << "reason = [" << msg << "]" << "func=" << function << " line=" << linenumber << std::endl;
	}
	::LocalFree(msg);
}

static LPTSTR GetLastErrorMsg(DWORD dwLastError)
{
	LPTSTR lpMsgBuf = NULL;
	if (FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwLastError,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		reinterpret_cast<LPTSTR>(&lpMsgBuf),
		0,
		NULL)) {
		return lpMsgBuf;
	}
	return NULL;
}