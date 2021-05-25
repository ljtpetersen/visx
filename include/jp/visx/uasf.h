/* include/jp/visx/uasf.h
 *
 * This file is part of the VisX project (https://github.com/ljtpetersen/visx).
 * Copyright (c) 2021 James Petersen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef JP_VISX_UASF_H
#define JP_VISX_UASF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../def.h"

typedef enum {
	JP_VISX_UASF_UOPERATION_NUL = -1,
	JP_VISX_UASF_UOPERATION_ADD,
	JP_VISX_UASF_UOPERATION_SUB,
	JP_VISX_UASF_UOPERATION_SUBO,
	JP_VISX_UASF_UOPERATION_MUL,
	JP_VISX_UASF_UOPERATION_DIV,
	JP_VISX_UASF_UOPERATION_DIVO,
	JP_VISX_UASF_UOPERATION_POW,
	JP_VISX_UASF_UOPERATION_POWO,
	JP_VISX_UASF_UOPERATION_MULC,
	JP_VISX_UASF_UOPERATION_DIVC,
	JP_VISX_UASF_UOPERATION_INVALID,
} jp_visx_uasf_UncertaintyTableElementType;

typedef struct {
	double value,
		   uncertainty;
} jp_visx_uasf_UncertaintyPair;

// This only emulates the function of the UncertaintyTable class.
// This is because there is no function in the UncertaintyTable which provides
// access to a UncertaintyTableElement pointer.
typedef void jp_visx_uasf_UncertaintyTable;

jp_visx_uasf_UncertaintyTable *jp_visx_uasf_UncertaintyTable_new1(void);
jp_visx_uasf_UncertaintyTable *jp_visx_uasf_UncertaintyTable_new2(size_t starting_capacity);
size_t jp_visx_uasf_UncertaintyTable_getCapacity(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_getValue1(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyPair *result_dest);
double jp_visx_uasf_UncertaintyTable_getValue2(jp_visx_uasf_UncertaintyTable *table, size_t row);
double jp_visx_uasf_UncertaintyTable_getUncertainty(jp_visx_uasf_UncertaintyTable *table, size_t row);
jp_visx_uasf_UncertaintyTableElementType jp_visx_uasf_UncertaintyTable_getType(jp_visx_uasf_UncertaintyTable *table, size_t row);
void jp_visx_uasf_UncertaintyTable_add1(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty);
void jp_visx_uasf_UncertaintyTable_add2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value);
void jp_visx_uasf_UncertaintyTable_remove(jp_visx_uasf_UncertaintyTable *table, size_t row);
void jp_visx_uasf_UncertaintyTable_clear(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_addAt1(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty);
void jp_visx_uasf_UncertaintyTable_addAt2(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value);
void jp_visx_uasf_UncertaintyTable_swap(jp_visx_uasf_UncertaintyTable *table, size_t row1, size_t row2);
void jp_visx_uasf_UncertaintyTable_set1(jp_visx_uasf_UncertaintyTable *table, size_t row, const jp_visx_uasf_UncertaintyPair *value);
void jp_visx_uasf_UncertaintyTable_set2(jp_visx_uasf_UncertaintyTable *table, size_t row, double value);
void jp_visx_uasf_UncertaintyTable_set3(jp_visx_uasf_UncertaintyTable *table, size_t row, double value, double uncertainty);
void jp_visx_uasf_UncertaintyTable_setUncertainty(jp_visx_uasf_UncertaintyTable *table, size_t row, double uncertainty);
void jp_visx_uasf_UncertaintyTable_setStartingValue1(jp_visx_uasf_UncertaintyTable *table, double value, double uncertainty);
void jp_visx_uasf_UncertaintyTable_setStartingValue2(jp_visx_uasf_UncertaintyTable *table, double value);
void jp_visx_uasf_UncertaintyTable_setStartingValue3(jp_visx_uasf_UncertaintyTable *table, const jp_visx_uasf_UncertaintyPair *value);
void jp_visx_uasf_UncertaintyTable_setStartingUncertainty(jp_visx_uasf_UncertaintyTable *table, double uncertainty);
double jp_visx_uasf_UncertaintyTable_getStartingValue1(jp_visx_uasf_UncertaintyTable *table);
double jp_visx_uasf_UncertaintyTable_getStartingUncertainty(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_getStartingValue2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyPair *value);
size_t jp_visx_uasf_UncertaintyTable_count(jp_visx_uasf_UncertaintyTable *table);
double jp_visx_uasf_UncertaintyTable_getResult1(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_getResult2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyPair *result_dest);
double jp_visx_uasf_UncertaintyTable_getResultingUncertainty(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_recompute(jp_visx_uasf_UncertaintyTable *table);
void jp_visx_uasf_UncertaintyTable_free(jp_visx_uasf_UncertaintyTable *table);

#ifdef __cplusplus
}
#endif

#endif

