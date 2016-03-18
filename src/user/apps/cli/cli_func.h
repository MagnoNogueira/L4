#ifndef CLI_FUNC_H
#define CLI_FUNC_H

#include "sys_type.h"

/* ---------------------------------------------------------------------------------- */
/* local subroutines for test & set functions                                         */
/* ---------------------------------------------------------------------------------- */
UI8_T CLI_FUNC_ipHextoStr(UI32_T, UI8_T*);


/* -------------------------------------------------- */
/* Transform 0x???????? IP address to xxx.xxx.xxx.xxx */
/* -------------------------------------------------- */
UI8_T CLI_FUNC_UI32_TtoStr(UI32_T, UI8_T *);


/* -------------------------------------------------- */
/* Transform integer n to a string */
/* -------------------------------------------------- */
UI8_T CLI_FUNC_UI8_TtoStr(UI8_T, UI8_T *);


/* -------------------------------------------------- */
/* Transform integer n to a string */
/* -------------------------------------------------- */
void CLI_FUNC_reverse(UI8_T *buf);

#endif
