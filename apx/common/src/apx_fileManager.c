/*****************************************************************************
* \file      apx_fileManager.c
* \author    Conny Gustafsson
* \date      2020-01-27
* \brief     New APX file manager
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
#include <string.h>
#include <assert.h>
#include <stdio.h> //DEBUG only
#include "apx_connectionBase.h"
#include "apx_portDataRef.h"
#include "apx_nodeData.h"

#ifdef MEM_LEAK_CHECK
#include "CMemLeak.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// PRIVATE CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
static apx_error_t apx_fileManager_processCmdMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen);
static apx_error_t apx_fileManager_processDataMsg(apx_fileManager_t *self, uint32_t address, const uint8_t *msgBuf, int32_t msgLen);
static apx_error_t apx_fileManager_processFileInfoMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen);
static apx_error_t apx_fileManager_processFileOpenMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen);
static void apx_fileManager_freeAllocatedMemory(void *arg, uint8_t *ptr, uint32_t size);
//////////////////////////////////////////////////////////////////////////////
// PRIVATE VARIABLES
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////
apx_error_t apx_fileManager_create(apx_fileManager_t *self, uint8_t mode, struct apx_connectionBase_tag *parentConnection)
{
   if (self != 0)
   {
      apx_error_t result;
      self->parentConnection = parentConnection;
      apx_fileManagerReceiver_create(&self->receiver);
      result = apx_fileManagerReceiver_reserve(&self->receiver, RMF_MAX_CMD_BUF_SIZE); //reserve minimum of 1KB in the receive buffer
      if (result == APX_NO_ERROR)
      {
         result = apx_fileManagerShared_create(&self->shared);
         if (result == APX_NO_ERROR)
         {
            self->shared.arg = (void*) self;
            self->shared.freeAllocatedMemory = apx_fileManager_freeAllocatedMemory;
            result = apx_fileManagerWorker_create(&self->worker, &self->shared, mode);
            if (result != APX_NO_ERROR)
            {
               apx_fileManagerShared_destroy(&self->shared);
               apx_fileManagerReceiver_destroy(&self->receiver);
            }
         }
         else
         {
            apx_fileManagerReceiver_destroy(&self->receiver);
         }
      }
      return result;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

void apx_fileManager_destroy(apx_fileManager_t *self)
{
   if (self != 0)
   {
      apx_fileManagerWorker_destroy(&self->worker);
      apx_fileManagerShared_destroy(&self->shared);
      apx_fileManagerReceiver_destroy(&self->receiver);
   }
}

void apx_fileManager_start(apx_fileManager_t *self)
{
   if (self != 0)
   {
      apx_fileManagerShared_connect(&self->shared);
      apx_fileManagerWorker_start(&self->worker);
   }
}

void apx_fileManager_stop(apx_fileManager_t *self)
{
   if (self != 0)
   {
      apx_fileManagerWorker_stop(&self->worker);
   }
}

apx_file_t* apx_fileManager_findFileByAddress(apx_fileManager_t *self, uint32_t address)
{
   if (self != 0)
   {
      return apx_fileManagerShared_findFileByAddress(&self->shared, address);
   }
   return (apx_file_t*) 0;
}

apx_file_t* apx_fileManager_findRemoteFileByName(apx_fileManager_t *self, const char *name)
{
   if (self != 0)
   {
      return apx_fileManagerShared_findRemoteFileByName(&self->shared, name);
   }
   return (apx_file_t*) 0;
}

void apx_fileManager_setTransmitHandler(apx_fileManager_t *self, apx_transmitHandler_t *handler)
{
   if (self != 0)
   {
      apx_fileManagerWorker_setTransmitHandler(&self->worker, handler);
   }
}

void apx_fileManager_copyTransmitHandler(apx_fileManager_t *self, apx_transmitHandler_t *handler)
{
   if (self != 0)
   {
      apx_fileManagerWorker_copyTransmitHandler(&self->worker, handler);
   }
}

/**
 * Server Mode
 */
void apx_fileManager_headerReceived(apx_fileManager_t *self)
{
   ///TODO: actually retrieve the NumHeader size from the parsed header data
   apx_fileManagerWorker_setNumHeaderSize(&self->worker, 32u);
   apx_fileManagerWorker_sendHeaderAckMsg(&self->worker);
}

/**
 * Client Mode
 */
void apx_fileManager_headerAccepted(apx_fileManager_t *self)
{
   adt_ary_t *localFiles = apx_fileManagerShared_getLocalFileList(&self->shared);
   if (localFiles != 0)
   {
      int32_t len = adt_ary_length(localFiles);
      int32_t i;
      for (i=0; i < len; i++)
      {
         apx_fileInfo_t *fileInfo = (apx_fileInfo_t*) adt_ary_value(localFiles, i);
         apx_fileManagerWorker_sendFileInfoMsg(&self->worker, fileInfo);
      }
      adt_ary_destructor_enable(localFiles, false);
      adt_ary_delete(localFiles);
   }
}

apx_error_t apx_fileManager_requestOpenFile(apx_fileManager_t *self, uint32_t address)
{
   if ( (self != 0) && ( (address & RMF_ADDRESS_MASK_INTERNAL) != RMF_INVALID_ADDRESS))
   {
      apx_file_t *file = apx_fileManagerShared_findFileByAddress(&self->shared, address);
      if (file != 0)
      {
         if (apx_file_isRemoteFile(file) )
         {
            apx_fileManagerWorker_sendFileOpenMsg(&self->worker, address);
            return APX_NO_ERROR;
         }
         else
         {
            return APX_NOT_IMPLEMENTED_ERROR;
         }
      }
      return APX_FILE_NOT_FOUND_ERROR;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

void apx_fileManager_setConnectionId(apx_fileManager_t *self, uint32_t connectionId)
{
   if (self != 0)
   {
      apx_fileManagerShared_setConnectionId(&self->shared, connectionId);
   }
}

int32_t apx_fileManager_getNumLocalFiles(apx_fileManager_t *self)
{
   if (self != 0)
   {
      return apx_fileManagerShared_getNumLocalFiles(&self->shared);
   }
   return -1;
}

int32_t apx_fileManager_getNumRemoteFiles(apx_fileManager_t *self)
{
   if (self != 0)
   {
      return apx_fileManagerShared_getNumRemoteFiles(&self->shared);
   }
   return -1;
}

uint16_t apx_fileManager_getNumPendingWorkerMessages(apx_fileManager_t *self)
{
   if (self != 0)
   {
      return apx_fileManagerWorker_getNumPendingMessages(&self->worker);
   }
   return 0u;
}

apx_file_t *apx_fileManager_fileInfoNotify(apx_fileManager_t *self, const apx_fileInfo_t *fileInfo)
{
   if (self != 0)
   {
      apx_file_t *file = apx_fileManagerShared_createRemoteFile(&self->shared, fileInfo);
      if (file != 0)
      {
         apx_error_t rc;
         apx_file_setFileManager(file, self);
         rc = apx_fileManagerReceiver_reserve(&self->receiver, fileInfo->length);
         if (rc != APX_NO_ERROR)
         {
            printf("[FILE-MANAGER] Failed to reserve memory for receive buffer, error code: %d\n", (int) rc);
            return (apx_file_t*) 0;
         }
      }
      return file;
   }
   return (apx_file_t*) 0;
}

apx_error_t apx_fileManager_messageReceived(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen)
{
   if ( (self != 0) && (msgBuf != 0) && (msgLen > 0) )
   {
      rmf_msg_t msg;
      int32_t result = rmf_unpackMsg(msgBuf, msgLen, &msg);
      if (result > 0)
      {
         apx_error_t retval = apx_fileManagerReceiver_write(&self->receiver, msg.address, msg.data, msg.dataLen, msg.more_bit);
         if (retval == APX_NO_ERROR)
         {
            apx_fileManagerReception_t completeMsg;
            result = apx_fileManagerReceiver_checkComplete(&self->receiver, &completeMsg);
            if (result == APX_NO_ERROR)
            {
               if (completeMsg.startAddress == RMF_CMD_START_ADDR)
               {
#if APX_DEBUG_ENABLE
                  printf("[FILE-MANAGER] APX Command: len=%u\n", (unsigned int) completeMsg.msgSize);
#endif
                  retval = apx_fileManager_processCmdMsg(self, completeMsg.msgBuf, completeMsg.msgSize);
               }
               else if (msg.address < RMF_CMD_START_ADDR)
               {
#if APX_DEBUG_ENABLE
                  printf("[FILE-MANAGER] Data Write: addr=0x%08X; len=%u\n", (unsigned int) completeMsg.startAddress, (unsigned int) completeMsg.msgSize);
#endif
                  retval = apx_fileManager_processDataMsg(self, completeMsg.startAddress, completeMsg.msgBuf, completeMsg.msgSize);
               }
               else
               {
                  retval = APX_INVALID_ADDRESS_ERROR;
               }
            }
         }
         return retval;
      }
      else if (result < 0)
      {
         assert(0); //This should actually never happen
         return APX_VALUE_ERROR;
      }
      else
      {
         return APX_DATA_NOT_PROCESSED_ERROR;
      }
   }
   return APX_INVALID_ARGUMENT_ERROR;
}



apx_error_t apx_fileManager_writeConstData(apx_fileManager_t *self, uint32_t address, uint32_t len, apx_file_read_const_data_func *readFunc, void *arg)
{
   if (self != 0)
   {
      return apx_fileManagerWorker_sendConstData(&self->worker, address, len, readFunc, arg);
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

apx_file_t *apx_fileManager_createLocalFile(apx_fileManager_t *self, const apx_fileInfo_t *fileInfo)
{
   if (self != 0)
   {
      apx_file_t *file = apx_fileManagerShared_createLocalFile(&self->shared, fileInfo);
      if (file != 0)
      {
         apx_file_setFileManager(file, self);
      }
      return file;
   }
   return (apx_file_t*) 0;
}

/**
 * Sends a fileInfo structure to remote side.
 * Note that fileInfo must be a cloned instance since this function takes ownership of the fileInfo pointer and will delete it later.
 */
apx_error_t apx_fileManager_sendFileInfo(apx_fileManager_t *self, apx_fileInfo_t *fileInfo)
{
   if (self != 0)
   {
      apx_fileManagerWorker_sendFileInfoMsg(&self->worker, fileInfo);
      return APX_NO_ERROR;
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

/**
 * The variable data must have been previously allocated through the apx_allocator_alloc function using the allocator in parent connection
 */
apx_error_t apx_fileManager_writeDynamicData(apx_fileManager_t *self, uint32_t address, apx_size_t len, uint8_t *data)
{
   if ( (self != 0) && (data != 0) && (len <= APX_MAX_FILE_SIZE) )
   {
      if (address >= RMF_CMD_START_ADDR)
      {
         return APX_INVALID_ADDRESS_ERROR;
      }
      return apx_fileManagerWorker_sendDynamicData(&self->worker, address, len, data);
   }
   return APX_INVALID_ARGUMENT_ERROR;
}

void apx_fileManager_disconnectNotify(apx_fileManager_t *self)
{
   if (self != 0)
   {
      apx_fileManagerShared_disconnect(&self->shared);
   }
}

#ifdef UNIT_TEST
bool apx_fileManager_run(apx_fileManager_t *self)
{
   if (self != 0)
   {
      return apx_fileManagerWorker_run(&self->worker);
   }
   return false;
}

int32_t apx_fileManager_numPendingMessages(apx_fileManager_t *self)
{
   if (self != 0)
   {
      return apx_fileManagerWorker_numPendingMessages(&self->worker);
   }
   return -1;
}
#endif //UNIT_TEST
//////////////////////////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
//////////////////////////////////////////////////////////////////////////////

static apx_error_t apx_fileManager_processCmdMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen)
{
   assert(self != 0);
   uint32_t cmdType;
   int32_t result;
   result = rmf_deserialize_cmdType(msgBuf, msgLen, &cmdType);
   if (result > 0)
   {
      apx_error_t retval = APX_NO_ERROR;
      msgBuf+=RMF_CMD_TYPE_LEN;
      msgLen-=RMF_CMD_TYPE_LEN;
      switch(cmdType)
      {
      case RMF_CMD_FILE_INFO:
         retval = apx_fileManager_processFileInfoMsg(self, msgBuf, msgLen);
      break;
      case RMF_CMD_FILE_OPEN:
         retval = apx_fileManager_processFileOpenMsg(self, msgBuf, msgLen);
      break;
      case RMF_CMD_HEARTBEAT_RQST:
         ///TODO: implement
         break;
      case RMF_CMD_HEARTBEAT_RSP:
         ///TODO: implement
         break;
      case RMF_CMD_PING_RQST:
         ///TODO: implement
         break;
      case RMF_CMD_PING_RSP:
         ///TODO: implement
         break;

      default:
         printf("[APX_FILE_MANAGER] not implemented cmdType: %d\n", cmdType);
      }
      return retval;
   }
   //If we end up here it means that the message has been received complete but it actually
   //contains so few bytes in it we cannot determine what type of command we are dealing with.
   return APX_INVALID_MSG_ERROR;
}


static apx_error_t apx_fileManager_processDataMsg(apx_fileManager_t *self, uint32_t address, const uint8_t *msgBuf, int32_t msgLen)
{
   assert(self != 0);
   if (self->parentConnection != 0)
   {
      apx_file_t *file = apx_fileManager_findFileByAddress(self, (address | RMF_REMOTE_ADDRESS_BIT) );
      if (file != 0)
      {
         if (apx_file_isOpen(file))
         {
            uint32_t offset;
            uint32_t startAddress = apx_file_getStartAddress(file) & RMF_ADDRESS_MASK_INTERNAL;
            assert(address >= startAddress);
            assert(address < startAddress+apx_file_getFileSize(file));
            offset = address - startAddress;
            return apx_connectionBase_fileWriteNotify(self->parentConnection, file, offset, msgBuf, msgLen);
         }
      }
      return APX_INVALID_ADDRESS_ERROR;
   }
   return APX_NULL_PTR_ERROR;
}

static apx_error_t apx_fileManager_processFileInfoMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen)
{
   rmf_fileInfo_t cmdFileInfo;
   int32_t result = rmf_deserialize_cmdFileInfo(msgBuf, msgLen, &cmdFileInfo);
   if (result > 0)
   {
      assert(self->parentConnection != 0);
      return apx_connectionBase_fileInfoNotify(self->parentConnection, &cmdFileInfo);
   }
   else
   {
      return APX_INVALID_MSG_ERROR;
   }
   return APX_NO_ERROR;
}

static apx_error_t apx_fileManager_processFileOpenMsg(apx_fileManager_t *self, const uint8_t *msgBuf, int32_t msgLen)
{
   rmf_cmdOpenFile_t cmdOpenFile;
   int32_t result = rmf_deserialize_cmdOpenFile(msgBuf, msgLen, &cmdOpenFile);
   if (result > 0)
   {
      if (self->parentConnection != 0)
      {
         return apx_connectionBase_fileOpenNotify(self->parentConnection, cmdOpenFile.address);
      }
      else
      {
         return APX_NULL_PTR_ERROR;
      }
   }
   else
   {
      return APX_INVALID_MSG_ERROR;
   }
   return APX_NO_ERROR;
}

static void apx_fileManager_freeAllocatedMemory(void *arg, uint8_t *ptr, uint32_t size)
{
   apx_fileManager_t *self = (apx_fileManager_t*) arg;
   if ( (self != 0) && (self->parentConnection != 0) )
   {
      apx_connectionBase_free(self->parentConnection, ptr, (size_t) size);
   }
}

