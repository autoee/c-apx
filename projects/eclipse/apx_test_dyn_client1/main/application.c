/*****************************************************************************
* \file      application.c
* \author    Conny Gustafsson
* \date      2019-10-13
* \brief     Description
*
* Copyright (c) 2019 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/
//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "apx_eventListener.h"
#include "apx_nodeData.h"
#include "application.h"
#include "pack.h"

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static void onClientConnected(void *arg, apx_clientConnectionBase_t *clientConnection);
static void onClientDisconnected(void *arg, apx_clientConnectionBase_t *clientConnection);
static void onLogEvent(void *arg, apx_logLevel_t level, const char *label, const char *msg);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
static apx_client_t *m_client = NULL;
static apx_nodeData_t *m_nodeData = NULL;
static const char *m_nodeName;
static uint16_t m_wheelBasedVehicleSpeed;

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void application_init(const char *apx_definition, const char *tcp_addr, uint16_t tcp_port)
{
   apx_clientEventListener_t handlerTable;
   apx_error_t result;
   memset(&handlerTable, 0, sizeof(handlerTable));
   handlerTable.clientConnected = onClientConnected;
   handlerTable.clientDisconnected = onClientDisconnected;
   handlerTable.logEvent = onLogEvent;

   m_wheelBasedVehicleSpeed = 0u;

   m_client = apx_client_new();
   if (m_client != 0)
   {
      apx_client_registerEventListener(m_client, &handlerTable);
      result = apx_client_createLocalNode_cstr(m_client, apx_definition);
      if (result != APX_NO_ERROR)
      {
         printf("apx_client_createLocalNode_cstr returned %d\n", (int) result);
         return;
      }
      m_nodeData = apx_client_getDynamicNode(m_client, 0);
      m_nodeName = STRDUP(apx_nodeData_getName(m_nodeData));
      result = apx_client_connectTcp(m_client, tcp_addr, tcp_port);
      //result = apx_client_connectUnix(m_client, "/tmp/apx_server.socket");
      if (result != APX_NO_ERROR)
      {
         printf("apx_client_connectTcp returned %d\n", (int) result);
         return;
      }
   }
}

void application_run(void)
{
   if ( (strcmp(m_nodeName, "TestNode1")==0) )
   {
      uint8_t tmp[2];
      m_wheelBasedVehicleSpeed++;
      packLE(&tmp[0], m_wheelBasedVehicleSpeed, sizeof(m_wheelBasedVehicleSpeed));
      apx_nodeData_setOutPortData(m_nodeData, &tmp[0], 1, sizeof(m_wheelBasedVehicleSpeed), false);
   }
   else if ( (strcmp(m_nodeName, "TestNode2")==0) )
   {

   }
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void onClientConnected(void *arg, apx_clientConnectionBase_t *clientConnection)
{
   printf("onClientConnected\n");
}

static void onClientDisconnected(void *arg, apx_clientConnectionBase_t *clientConnection)
{
   printf("onClientDisconnected\n");
}

static void onLogEvent(void *arg, apx_logLevel_t level, const char *label, const char *msg)
{
   printf("onLogEvent\n");
}
