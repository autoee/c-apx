#ifndef APX_FILE_MAP2_H_
#define APX_FILE_MAP2_H_

//////////////////////////////////////////////////////////////////////////////
// INCLUDES
//////////////////////////////////////////////////////////////////////////////
#include <stdint.h>
#include "adt_list.h"
#include "adt_ary.h"
#include "apx_file.h"

//////////////////////////////////////////////////////////////////////////////
// CONSTANTS AND DATA TYPES
//////////////////////////////////////////////////////////////////////////////
typedef struct apx_fileMap_tag
{
   adt_list_t fileList; //list of apx_file_t automatically sorted by address
   apx_file_t *lastFile; //Last accessed file (for caching repeated access requests)
} apx_fileMap_t;


//////////////////////////////////////////////////////////////////////////////
// GLOBAL FUNCTION PROTOTYPES
//////////////////////////////////////////////////////////////////////////////
void apx_fileMap_create(apx_fileMap_t *self);
void apx_fileMap_destroy(apx_fileMap_t *self);
int8_t apx_fileMap_insertFile(apx_fileMap_t *self, apx_file_t *pFile);
int8_t apx_fileMap_removeFile(apx_fileMap_t *self, apx_file_t *pFile);
apx_file_t *apx_fileMap_findByAddress(apx_fileMap_t *self, uint32_t address);
apx_file_t *apx_fileMap_findByName(apx_fileMap_t *self, const char *name);
int32_t apx_fileMap_length(const apx_fileMap_t *self);
void apx_fileMap_clear_weak(apx_fileMap_t *self);
adt_list_t *apx_fileMap_getList(apx_fileMap_t *self);
bool apx_fileMap_exist(apx_fileMap_t *self, apx_file_t *file);
adt_ary_t *apx_fileMap_makeFileInfoArray(apx_fileMap_t *self);



#endif //APX_FILE_MAP_H_
