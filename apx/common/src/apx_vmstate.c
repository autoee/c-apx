/*****************************************************************************
* \file      apx_vmState.c
* \author    Conny Gustafsson
* \date      2019-03-05
* \brief     APX virtual machine state
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
#include "apx_vmstate.h"
#include "apx_vmdefs.h"
#include "pack.h"
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

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
void apx_vmInnerState_create(apx_vmInnerState_t *self)
{
   if (self != 0)
   {
      self->valueType = APX_VALUE_TYPE_NONE;
      self->arrayIdx = -1;
      self->arrayLen = -1;
      self->key = (adt_str_t*) 0;
   }
}

void apx_vmInnerState_destroy(apx_vmInnerState_t *self)
{
   if (self != 0)
   {
      if (self->key != 0)
      {
         adt_str_delete(self->key);
      }
   }
}

apx_vmInnerState_t* apx_vmInnerState_new(void)
{
   apx_vmInnerState_t *self = (apx_vmInnerState_t*) malloc(sizeof(apx_vmInnerState_t));
   if (self != 0)
   {
      apx_vmInnerState_create(self);
   }
   return self;

}

void apx_vmInnerState_delete(apx_vmInnerState_t *self)
{
   if (self != 0)
   {
      apx_vmInnerState_destroy(self);
      free(self);
   }
}

void apx_vmInnerState_vdelete(void *arg)
{
   apx_vmInnerState_delete((apx_vmInnerState_t*) arg);
}

void apx_vmstate_create(apx_vmstate_t *self)
{
   if (self != 0)
   {
     adt_stack_create(&self->stack, apx_vmInnerState_vdelete);
     self->inner = (apx_vmInnerState_t*) 0;
     self->hasValidWriteData = false;
     self->hasValidReadData = false;
     self->data.write.pBegin = (uint8_t*) 0;
     self->data.write.pEnd = (uint8_t*) 0;
     self->data.write.pNext = (uint8_t*) 0;
     self->data.read.pBegin = (const uint8_t*) 0;
     self->data.read.pEnd = (const uint8_t*) 0;
     self->data.read.pNext = (const uint8_t*) 0;
   }
}

void apx_vmstate_destroy(apx_vmstate_t *self)
{
   if (self != 0)
   {
     adt_stack_destroy(&self->stack);
     if (self->inner != 0)
     {
        apx_vmInnerState_delete(self->inner);
     }
   }
}

apx_vmstate_t* apx_vmstate_new(void)
{
   apx_vmstate_t *self = (apx_vmstate_t*) malloc(sizeof(apx_vmstate_t));
   if (self != 0)
   {
      apx_vmstate_create(self);
   }
   return self;
}

void apx_vmstate_delete(apx_vmstate_t *self)
{
   if (self != 0)
   {
      apx_vmstate_destroy(self);
      free(self);
   }
}

apx_error_t apx_vmstate_setWriteData(apx_vmstate_t *self, uint8_t *pData, uint32_t dataLen)
{
   if (self != 0 && (pData != 0) && (dataLen > 0))
   {
      self->data.write.pBegin = pData;
      self->data.write.pEnd = pData+dataLen;
      self->data.write.pNext = pData;
      self->hasValidWriteData = true;
      self->hasValidReadData = false;
      return APX_NO_ERROR;
   }
   if (self != 0)
   {
      self->hasValidWriteData = false;
      self->hasValidReadData = false;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * Returns position of the write pointer
 */
uint8_t* apx_vmstate_getWritePtr(apx_vmstate_t *self)
{
   if ( (self != 0) && (self->hasValidWriteData) )
   {
      return self->data.write.pNext;
   }
   return (uint8_t*) 0;
}

/**
 * Packs uint8 value into write buffer
 */
apx_error_t apx_vmstate_packU8(apx_vmstate_t *self, uint8_t u8Value)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         if (self->data.write.pNext < self->data.write.pEnd)
         {
            *self->data.write.pNext++ = u8Value;
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}


/**
 * Packs uint16 value into write buffer
 */
apx_error_t apx_vmstate_packU16(apx_vmstate_t *self, uint16_t u16Value)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         if (self->data.write.pNext+1u < self->data.write.pEnd)
         {
            packU16LE(self->data.write.pNext, u16Value);
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * Packs uint16 value into write buffer
 */
apx_error_t apx_vmstate_packU32(apx_vmstate_t *self, uint32_t u32Value)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         if (self->data.write.pNext+3u < self->data.write.pEnd)
         {
            packU32LE(self->data.write.pNext, u32Value);
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * Packs U8 data into an U8 dynamic array (255 array elements maximum)
 */
apx_error_t apx_vmstate_packU8U8DynArray(apx_vmstate_t *self, uint8_t *pArrayValues, uint8_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = ((uint32_t) sizeof(uint8_t)) + ((uint32_t) arrayLen);
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            *self->data.write.pNext++ = arrayLen;
            memcpy(self->data.write.pNext, pArrayValues, arrayLen);
            self->data.write.pNext+=arrayLen;
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * Packs U8 data into an U16 dynamic array (65535 array elements maximum)
 */
apx_error_t apx_vmstate_packU8U16DynArray(apx_vmstate_t *self, uint8_t *pArrayValues, uint16_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = ((uint32_t) sizeof(uint16_t)) + ((uint32_t) arrayLen);
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            packU16LE(self->data.write.pNext, (uint32_t) arrayLen);
            memcpy(self->data.write.pNext, pArrayValues, arrayLen);
            self->data.write.pNext+=arrayLen;
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * Packs U8 data into an U32 dynamic array (65535 array elements maximum)
 */
apx_error_t apx_vmstate_packU8U32DynArray(apx_vmstate_t *self, uint8_t *pArrayValues, uint32_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = ((uint32_t) sizeof(uint32_t)) + ((uint32_t) arrayLen);
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            packU32LE(self->data.write.pNext, (uint32_t) arrayLen);
            memcpy(self->data.write.pNext, pArrayValues, arrayLen);
            self->data.write.pNext+=arrayLen;
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_error_t apx_vmstate_packU8DynArray(apx_vmstate_t *self, uint8_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = UINT8_SIZE;
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            *self->data.write.pNext++ = arrayLen;
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_error_t apx_vmstate_packU16DynArray(apx_vmstate_t *self, uint16_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = UINT16_SIZE;
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            packU16LE(self->data.write.pNext, (uint32_t) arrayLen);
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_error_t apx_vmstate_packU32DynArray(apx_vmstate_t *self, uint32_t arrayLen)
{
   if ( self != 0)
   {
      if (self->hasValidWriteData)
      {
         uint32_t writeLen = UINT32_SIZE;
         uint32_t bytesFree = (uint32_t) (self->data.write.pEnd-self->data.write.pNext);
         if (writeLen <= bytesFree)
         {
            packU32LE(self->data.write.pNext, (uint32_t) arrayLen);
            return APX_NO_ERROR;
         }
         else
         {
            return APX_BUFFER_BOUNDARY_ERROR;
         }
      }
      else
      {
         return APX_MISSING_BUFFER_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

