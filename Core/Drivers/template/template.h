/**
 * @file    template.h
 * @brief   [Brief description of module]
 * @details [Detailed description of what this module does, its purpose,
 *          and any important information about its usage]
 * 
 * @author  [Author Name]
 * @date    [Date]
 * @version [Version Number]
 * 
 * @note    [Important notes about this module]
 * @warning [Important warnings or precautions]
 * 
 * @par Change Log:
 * - Version X.X (Date): [Description of changes]
 * 
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/**
 * @defgroup [ModuleName]_Definitions [Module Name] Definitions
 * @{
 */

/** @brief [Brief description of constant/macro] */
#define CONSTANT_NAME       (value)

/** @brief [Description of enumeration type] */
typedef enum {
    ENUM_VALUE_1 = 0U,    /**< Description of value 1 */
    ENUM_VALUE_2 = 1U,    /**< Description of value 2 */
    ENUM_VALUE_3 = 2U     /**< Description of value 3 */
} enum_type_name_t;

/**
 * @brief [Description of structure]
 */
typedef struct {
    uint32_t member1;     /**< Description of member1 */
    uint8_t member2;      /**< Description of member2 */
    bool member3;         /**< Description of member3 */
} struct_type_name_t;

/** @} */ /* End of [ModuleName]_Definitions */

/*******************************************************************************
 * API Function Prototypes
 ******************************************************************************/

/**
 * @defgroup [ModuleName]_Functions [Module Name] Functions
 * @{
 */

/**
 * @brief [Brief one-line description of function]
 * @details [Detailed description of what the function does, how it works,
 *          and any important behavioral details]
 * 
 * @param[in]     param1 [Description of input parameter]
 * @param[in]     param2 [Description of input parameter]
 * @param[out]    param3 [Description of output parameter]
 * @param[in,out] param4 [Description of input/output parameter]
 * 
 * @return [Description of return value]
 * @retval value1 [Description of specific return value]
 * @retval value2 [Description of specific return value]
 * 
 * @note [Additional important notes]
 * @warning [Important warnings about usage]
 * 
 * @pre [Preconditions that must be met]
 * @post [Postconditions after function execution]
 * 
 * @par Example:
 * @code
 * // Example usage code
 * Function_Name(param1, param2);
 * @endcode
 * 
 * @see [Related functions or references]
 */
void Function_Name(uint32_t param1, uint8_t param2);

/**
 * @brief [Brief description]
 * 
 * @param[in] param [Description]
 * 
 * @return [Description]
 * 
 * @par Example:
 * @code
 * result = Function_Name2(value);
 * @endcode
 */
uint32_t Function_Name2(uint32_t param);

/**
 * @brief [Brief description of initialization function]
 * @details [Detailed description of initialization process]
 * 
 * @return None
 * 
 * @note [Important initialization notes]
 * 
 * @par Example:
 * @code
 * Module_Init();
 * @endcode
 */
void Module_Init(void);

/**
 * @brief [Brief description of configuration function]
 * 
 * @param[in] config Pointer to configuration structure
 * 
 * @return None
 * 
 * @note config must not be NULL
 * 
 * @par Example:
 * @code
 * struct_type_name_t config = {
 *     .member1 = 100,
 *     .member2 = 5,
 *     .member3 = true
 * };
 * Module_Configure(&config);
 * @endcode
 */
void Module_Configure(const struct_type_name_t *config);

/** @} */ /* End of [ModuleName]_Functions */

#endif /* TEMPLATE_H */

/*******************************************************************************
 * EOF
 ******************************************************************************/
