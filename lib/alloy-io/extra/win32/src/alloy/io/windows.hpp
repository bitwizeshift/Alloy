#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
# define NOMINMAX 1
#endif
#ifndef VC_EXTRALEAN
# define VC_EXTRALEAN 1
#endif
#ifndef UNICODE
# define UNICODE 1
#endif

#if WINVER < 0x0501
# undef WINVER
# define WINVER 0x0501
#endif
#if _WIN32_WINNT < 0x0501
# undef _WIN32_WINNT
# define _WIN32_WINNT 0x0501
#endif

#define DIRECTINPUT_VERSION 0x0800

#define OEMRESOURCE

#include <wctype.h>
#include <windows.h>
#include <dinput.h>
#include <xinput.h>
#include <dbt.h>