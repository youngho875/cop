#ifndef _USEFUL_H_
#define _USEFUL_H_

#pragma warning(disable:4786)

#define		SAFE_DELETE(p)				{ if((p)) { delete ((p));     (p)=NULL; } }
#define		SAFE_RELEASE(p)				{ if((p)) { ((p))->Release(); (p)=NULL; } }
#define		SAFE_DELETE_ARRAY(p)	{ if((p)) { delete[] ((p));   (p)=NULL; } }
#define		SAFE_FREE(p)					{ if((p)) { free((p));				(p)=NULL; } }

#ifndef _WINDEF_
typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;
#endif

#endif