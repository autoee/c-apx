/*****************************************************************************
* \file      message_server.c
* \author    Conny Gustafsson
* \date      2020-03-07
* \brief     Server that listens for messages forwarded by apx_control application
*
* Copyright (c) 2020 Conny Gustafsson
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
#include <assert.h>
#include "msocket_server.h"
#include "message_server.h"


//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
static void message_server_accept(void *arg, struct msocket_server_tag *srv, struct msocket_t *msocket);

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static msocket_server_t *m_server = NULL;
static apx_send_connection_t *m_apx_connection = (apx_send_connection_t*) NULL;

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
apx_error_t message_server_init(apx_send_connection_t *apx_connection)
{
   assert(apx_connection != 0);
   m_apx_connection = apx_connection;
   m_server = msocket_server_new(AF_UNIX, message_server_connection_vdelete);
   if (m_server != 0)
   {
      msocket_handler_t handler;
      memset(&handler, 0, sizeof(msocket_handler_t));
      handler.tcp_accept = message_server_accept;
      msocket_server_sethandler(m_server, &handler, (void*) 0);
   }
   return APX_NO_ERROR;
}

void message_server_start(const char *socket_path)
{
   msocket_server_unix_start(m_server, socket_path);
}

void message_server_cleanup_connection(message_server_connection_t *connection)
{
   if (connection != 0)
   {
      assert(m_server != 0);
      msocket_server_cleanup_connection(m_server, connection);
   }
}

void message_server_shutdown(void)
{
   if (m_server != 0)
   {
      m_apx_connection = (apx_send_connection_t*) NULL;
      msocket_server_delete(m_server);
      m_server = (msocket_server_t*) 0;
   }
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
static void message_server_accept(void *arg, struct msocket_server_tag *srv, struct msocket_t *msocket)
{
   if (m_apx_connection != 0)
   {
      message_server_connection_t *connection = message_server_connection_new(msocket, m_apx_connection);
      if (connection != 0)
      {
         message_server_connection_start(connection);
      }
      else
      {
         printf("message_connection_new failed\n");
      }
   }
}



