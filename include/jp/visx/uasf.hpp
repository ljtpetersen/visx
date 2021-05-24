/* src/lib/uasf.hpp
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

#ifndef JP_VISX_H
#define JP_VISX_H

// Make sure this file is compiled with C++
#ifndef __cplusplus
#error HPP File not compiled with C++.
#endif

#include "../def.h"
#include <utility>
#include <vector>
#include <math.h>

namespace jp {
	namespace visx {
		namespace uasf { // UASF stands for Uncertainty and Significant Figures
			typedef enum {
				UOPERATION_NUL = -1,
				UOPERATION_ADD,
				UOPERATION_SUB,
				UOPERATION_SUBO,
				UOPERATION_MUL,
				UOPERATION_DIV,
				UOPERATION_DIVO,
				UOPERATION_POW,
				UOPERATION_MULC,
				UOPERATION_DIVC,
				UOPERATION_INVALID
			} UncertaintyTableElementType;
			typedef struct {
				double value,
					   uncertainty;
			} UncertaintyPair;
			/* The UncertaintyTableElement is a single element in an UncertaintyTable.
			 * It has a double value and uncertainty, as well as cumulative uncertainty
			 * and cumulative value, which represent the result of the previous operation.
			 * It also has a type (see UncertaintyTableElementType).
			 */
			class UncertaintyTableElement {
			public:
				static const UncertaintyTableElement invalid_element;
				UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty);
				UncertaintyTableElement(UncertaintyTableElementType type, const UncertaintyPair *pair);
				UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty, double cumulative_value, double cumulative_uncertainty);
				void compute(UncertaintyPair *result_dest) const;
				UncertaintyTableElementType getType(void) const;
				double getCumulative(void) const;
				void getCumulative(UncertaintyPair *result_dest) const;
				double getCumulativeUncertainty(void) const;
				void getValue(UncertaintyPair *result_dest) const;
				double getValue(void) const;
				double getUncertainty(void) const;
				void setCumulative(double value, double uncertainty);
				void setCumulative(const UncertaintyPair *value);
				void setCumulative(double value);
				void setCumulativeUncertainty(double uncertainty);
				void setValue(double value, double uncertainty);
				void setValue(const UncertaintyPair *value);
				void setValue(double value);
				void setUncertainty(double uncertainty);
				void setType(UncertaintyTableElementType type);
				void setNotType(const UncertaintyTableElement &value);
			private:
				UncertaintyTableElementType type_;
				double						value_,
											uncertainty_,
											cumulative_value_,
											cumulative_uncertainty_;
			};
			/* The UncertaintyTable class has a list of elements (UncertaintyTableElement)
			 * It also has an output value and an output uncertainty.
			 * Uncertainty Tables use doubles (long doubles aren't well supported on windows).
			 */
			class UncertaintyTable {
			public:
				UncertaintyTable(size_t starting_capacity = 10);
				size_t getCapacity(void) const;
				void getValue(size_t row, UncertaintyPair *result_dest) const;
				double getValue(size_t row) const;
				double getUncertainty(size_t row) const;
				UncertaintyTableElementType getType(size_t row);
				const UncertaintyTableElement &getElement(size_t row) const;
				void add(UncertaintyTableElementType type, double value, double uncertainty);
				void add(UncertaintyTableElementType type, const UncertaintyPair *value);
				void add(const UncertaintyTableElement &element);
				void add(UncertaintyTableElement &&element);
				void remove(size_t row);
				void addAt(size_t row, UncertaintyTableElementType type, double value, double uncertainty);
				void addAt(size_t row, UncertaintyTableElementType type, const UncertaintyPair *value);
				void addAt(size_t row, const UncertaintyTableElement &element);
				void addAt(size_t row, UncertaintyTableElement &&element);
				void swap(size_t row1, size_t row2);
				void set(size_t row, const UncertaintyPair *value);
				void set(size_t row, double value);
				void set(size_t row, double value, double uncertainty);
				void set(size_t row, UncertaintyTableElement &&element);
				void setUncertainty(size_t row, double uncertainty);
				void setStartingValue(double value, double uncertainty);
				void setStartingValue(double value);
				void setStartingValue(const UncertaintyPair *value);
				void setStartingUncertainty(double uncertainty);
				double getStartingValue(void) const;
				double getStartingUncertainty(void) const;
				void getStartingValue(UncertaintyPair *value_dest) const;
				size_t count(void) const;
				double getResult(void) const;
				void getResult(UncertaintyPair *result_dest) const;
				double getResultingUncertainty(void) const;
			private:
				void compute(size_t starting_row);
				std::vector<UncertaintyTableElement> elements_;
				UncertaintyPair result_;
			}; // class UncertaintyTable
		} // namespace uasf
	} // namespace visx
} // namespace jp

#endif
