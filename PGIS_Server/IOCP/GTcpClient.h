#pragma once

extern BOOL bGTcpCltIsActive;

void GTcpClt_CloseClients(void);
void GTcpClt_FreeClients(void);
void GTcpClt_EndThread(void);

void GTcpClt_Create(void);
void GTcpClt_Destroy(void);
