/*****************************************************************************
* \file      apx_serverExtension.c
* \author    Conny Gustafsson
* \date      my_date
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
#include <malloc.h>
#include <string.h>
#include "apx_serverExtension.h"
#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////
void apx_serverExtension_create(apx_serverExtension_t *self, const char *name, const apx_serverExtensionHandler_t *handler, dtl_dv_t *config)
{
   if ( (self != 0) && (name != 0) && (handler != 0) )
   {
      self->name = STRDUP(name);
      memcpy(&self->handler, handler, sizeof(apx_serverExtensionHandler_t));
      self->config = config;
   }
}

void apx_serverExtension_destroy(apx_serverExtension_t *self)
{
   if (self != 0)
   {
      if (self->name != 0)
      {
         free(self->name);
      }
   }
}

apx_serverExtension_t* apx_serverExtension_new(const char *name, const apx_serverExtensionHandler_t *handler, dtl_dv_t *config)
{
   apx_serverExtension_t *self = (apx_serverExtension_t*) malloc(sizeof(apx_serverExtension_t));
   if (self != 0)
   {
      apx_serverExtension_create(self, name, handler, config);
   }
   return self;
}


void apx_serverExtension_delete(apx_serverExtension_t *self)
{
   if (self != 0)
   {
      apx_serverExtension_destroy(self);
      free(self);
   }
}

void apx_serverExtension_vdelete(void *arg)
{
   apx_serverExtension_delete((apx_serverExtension_t*) arg);
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////


