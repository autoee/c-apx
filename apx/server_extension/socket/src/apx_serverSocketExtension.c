/*****************************************************************************
* \file      apx_serverSocketExtension.c
* \author    Conny Gustafsson
* \date      2019-09-04
* \brief     APX socket server extension (TCP+UNIX)
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
#ifdef _MSC_VER
#include <Windows.h>
#endif
#include "apx_serverSocketExtension.h"
#include "apx_socketServer.h"
#include "apx_server.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static apx_error_t apx_serverSocketExtension_init(struct apx_server_tag *apx_server, dtl_dv_t *config);
static void apx_serverSocketExtension_shutdown(void);
static apx_error_t apx_serverSocketExtension_configure(apx_socketServer_t *server, dtl_hv_t *cfg);


//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
static apx_socketServer_t *m_instance = (apx_socketServer_t*) 0; //singleton

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

apx_error_t apx_serverSocketExtension_register(struct apx_server_tag *apx_server, dtl_dv_t *config)
{
   apx_serverExtensionHandler_t handler = {apx_serverSocketExtension_init, apx_serverSocketExtension_shutdown};
   return apx_server_addExtension(apx_server, &handler, config);
}

#ifdef UNIT_TEST
void apx_serverSocketExtension_acceptTestSocket(testsocket_t *sock)
{
   if (m_instance != 0)
   {
      apx_socketServer_acceptTestSocket(m_instance, sock);
   }
}
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static apx_error_t apx_serverSocketExtension_init(struct apx_server_tag *apx_server, dtl_dv_t *config)
{
   if (m_instance == 0)
   {
      m_instance = apx_socketServer_new(apx_server);
      if (m_instance == 0)
      {
         return APX_MEM_ERROR;
      }
      if (config != 0)
      {
         if (dtl_dv_type(config) == DTL_DV_HASH)
         {
            return apx_serverSocketExtension_configure(m_instance, (dtl_hv_t*) config);
         }
         else
         {
            return APX_DV_TYPE_ERROR;
         }
      }
   }
   return APX_NO_ERROR;
}

static void apx_serverSocketExtension_shutdown(void)
{
   if (m_instance != 0)
   {
      apx_socketServer_stopAll(m_instance);
      apx_socketServer_delete(m_instance);
      m_instance = (apx_socketServer_t*) 0;
   }
}

static apx_error_t apx_serverSocketExtension_configure(apx_socketServer_t *server, dtl_hv_t *cfg)
{
   return APX_NO_ERROR;
}
