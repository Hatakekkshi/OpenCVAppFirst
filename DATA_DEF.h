
#pragma once

#define SAFE_DELETE_ARRAY(pArr) do{if((pArr)) delete []pArr;}while 0;

#if defined UNICODE || defined _UNICODE
#define  tstring wstring
#else
#define  tstring string
#endif