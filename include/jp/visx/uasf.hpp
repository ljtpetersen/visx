/* include/jp/visx/uasf.hpp
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

#ifndef JP_VISX_UASF_HPP
#define JP_VISX_UASF_HPP

// Make sure this file is compiled with C++
#ifndef __cplusplus
#error HPP File not compiled with C++.
#endif

#include "../def.h"
#include <vector>
#include <string>

namespace jp {
	namespace visx {
		namespace uasf { // UASF stands for Uncertainty and Significant Figures
			/* This enum contains the different types of operations. It starts at -1 (NUL).
			 * Here is a description of each value:
			 *		NUL: Directly sets the cumulative for the next value to the current
			 *			 value.
			 *		ADD: Add the cumulative and the value. Do the same for the uncertainties.
			 *		SUB: Substract the cumulative by the value. Sum the uncertainties
			 *		SUBO: Substract the value by the cumulative. Sum the uncertainties
			 *		MUL: Multiply the value and the cumulative. Add together the relative
			 *			 uncertainties.
			 *		DIV: Divide the cumulative by the value. Add together the relative
			 *			 uncertainties.
			 *		DIVO: Divide the value by the cumulative. Add together the relative
			 *			 uncertainties.
			 *		POW: Calculate the cumulative to the power of the value. Multiply the
			 *			 relative uncertainty by the value.
			 *		POWO: Calculate the value to the power of the cumulative. Multiply the
			 *			  relative uncertainty by the cumulative. This and the other's way
			 *			  of calculating the new uncertainty is not accurate. A more accurate
			 *			  method would be to get the minimum and maximum values.
			 *		MULC: Calculate the value multiplied by the cumulative. The uncertainty
			 *			  is the value multiplied by the cumulative uncertainty. This method
			 *			  discards the normal uncertainty.
			 *		DIVC: Works the same way as MULC, except with division instead.
			 *			  It is cumulative divided by value.
			 *		INVALID: This indicates an invalid UncertaintyTableElement.
			 */
			typedef enum {
				UOPERATION_NUL = -1,
				UOPERATION_ADD,
				UOPERATION_SUB,
				UOPERATION_SUBO,
				UOPERATION_MUL,
				UOPERATION_DIV,
				UOPERATION_DIVO,
				UOPERATION_POW,
				UOPERATION_POWO,
				UOPERATION_MULC,
				UOPERATION_MULCO,
				UOPERATION_DIVC,
				UOPERATION_DIVCO,
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
				/* This is a standard invalid element. To check if an element is invalid,
				 * compare its type with UOPERATION_INVALID.
				 */
				static const UncertaintyTableElement invalid_element;
				UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty);
				UncertaintyTableElement(UncertaintyTableElementType type, const UncertaintyPair *pair);
				UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty, double cumulative_value, double cumulative_uncertainty);
				/* This method computes the table element using the current value and
				 * cumulative value. It puts the result into result_dest. To see
				 * the calculation done for each type, see the enum
				 * UncertaintyTableElementType.
				 */
				void compute(UncertaintyPair *result_dest) const;
				// This method returns the type of the TableElement.
				UncertaintyTableElementType getType(void) const;
				// This method returns the cumulative_value_.
				double getCumulative(void) const;
				// This method puts the cumulative value and uncertainty into the
				// provided UncertaintyPair.
				void getCumulative(UncertaintyPair *result_dest) const;
				// This method returns the cumulative uncertainty of the element.
				double getCumulativeUncertainty(void) const;
				// This method puts the current value and uncertainty into the
				// provided UncertaintyPair.
				void getValue(UncertaintyPair *result_dest) const;
				// This method returns the current value.
				double getValue(void) const;
				// This method returns the current uncertainty.
				double getUncertainty(void) const;
				// This method sets the cumulative value and uncertainty.
				void setCumulative(double value, double uncertainty);
				// This method sets the cumulative value and uncertainty
				// with the provided UncertaintyPair.
				void setCumulative(const UncertaintyPair *value);
				// This method sets the cumulative value.
				void setCumulative(double value);
				// This method sets the cumulative uncertainty.
				void setCumulativeUncertainty(double uncertainty);
				// This method sets the value and uncertainty.
				void setValue(double value, double uncertainty);
				// This method sets the value and uncertainty using an
				// UncertaintyPair.
				void setValue(const UncertaintyPair *value);
				// This method sets the value.
				void setValue(double value);
				// This method sets the uncertainty.
				void setUncertainty(double uncertainty);
				// This method sets the type.
				void setType(UncertaintyTableElementType type);
				// This method copies everything except the type.
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
				// This constructor calls the other constructor with a starting capacity
				// of 10.
				UncertaintyTable(void);
				// This constructor allows the user to specify the starting capacity
				// of the vector.
				UncertaintyTable(size_t starting_capacity);
				UncertaintyTable(size_t starting_capacity, double starting_value, double starting_uncertainty);
				// This method returns the current capacity of the table.
				size_t getCapacity(void) const;
				// This method gets the current value of the table element on a specified
				// row. If the row is invalid, it puts NaN into the result_dest.
				// This method is equivalent to calling getValue on the corresponding
				// TableElement.
				void getValue(size_t row, UncertaintyPair *result_dest) const;
				// This method gets the current value of the table element on a specified
				// row. If the row is invalid, it returns NaN. This method is equivalent to
				// calling getValue on the corresponding TableElement.
				double getValue(size_t row) const;
				// This method gets the current uncertainty of the table element on a specified
				// row. If the row is invalid, it returns NaN. This method is equivalent to
				// calling getUncertainty on the corresponding TableElement.
				double getUncertainty(size_t row) const;
				// This method gets the type of the table element on a specified
				// row. If the row is invalid, it returns NaN. This method is equivalent to
				// calling getUncertainty on the corresponding TableElement.
				UncertaintyTableElementType getType(size_t row) const;
				// This method returns a constant reference to the specified row.
				// If the row is invalid, it returns UncertaintyTableElement::invalid_element.
				const UncertaintyTableElement &getElement(size_t row) const;
				// This method adds a row to the end of the table.
				void add(UncertaintyTableElementType type, double value, double uncertainty);
				// This method adds a row to the end of the table.
				void add(UncertaintyTableElementType type, const UncertaintyPair *value);
				// This method adds a row to the end of the table.
				void add(const UncertaintyTableElement &element);
				// This method adds a row to the end of the table.
				void add(UncertaintyTableElement &&element);
				// This method removes the specified row from the table.
				void remove(size_t row);
				// This method clears the table. (Removes all elements.)
				void clear(void);
				// This method adds to the table at the specified row. If the row is invalid,
				// it adds to the end of the table.
				void addAt(size_t row, UncertaintyTableElementType type, double value, double uncertainty);
				// This method adds to the table at the specified row. If the row is invalid,
				// it adds to the end of the table.
				void addAt(size_t row, UncertaintyTableElementType type, const UncertaintyPair *value);
				// This method adds to the table at the specified row. If the row is invalid,
				// it adds to the end of the table.
				void addAt(size_t row, const UncertaintyTableElement &element);
				// This method adds to the table at the specified row. If the row is invalid,
				// it adds to the end of the table.
				void addAt(size_t row, UncertaintyTableElement &&element);
				// This method swaps the values and types of the two rows (if they are
				// valid and not zero).
				void swap(size_t row1, size_t row2);
				// This method sets the value of the row.
				void set(size_t row, const UncertaintyPair *value);
				// This method sets the value of the row.
				void set(size_t row, double value);
				// This method sets the value of the row.
				void set(size_t row, double value, double uncertainty);
				// This method sets the row.
				void set(size_t row, UncertaintyTableElement &&element);
				// This method sets the row.
				void set(size_t row, const UncertaintyTableElement &element);
				// This method sets the uncertainty of the row.
				void setUncertainty(size_t row, double uncertainty);
				// This method sets the starting value.
				void setStartingValue(double value, double uncertainty);
				// This method sets the starting value.
				void setStartingValue(double value);
				// This method sets the starting value.
				void setStartingValue(const UncertaintyPair *value);
				// This method sets the starting uncertainty.
				void setStartingUncertainty(double uncertainty);
				// This method gets the starting value.
				double getStartingValue(void) const;
				// This method gets the starting uncertainty.
				double getStartingUncertainty(void) const;
				// This method gets the starting value.
				void getStartingValue(UncertaintyPair *value_dest) const;
				// This method gets the number of elements in the table.
				size_t count(void) const;
				// This method gets the last computed result of the table.
				double getResult(void) const;
				// This method gets the last computed result of the table.
				void getResult(UncertaintyPair *result_dest) const;
				// This method gets the last computed resulting uncertainty of the table.
				double getResultingUncertainty(void) const;
				// Recompute the resulting value from the start.
				void recompute(void);
			private:
				// This method computes the table starting from starting_row.
				// If starting_row >= count() then the method does nothing.
				void compute(size_t starting_row);
				std::vector<UncertaintyTableElement> elements_;
				UncertaintyPair result_;
			}; // class UncertaintyTable
			void simplifyUncertainty(double value, double uncertainty, double *value_dest, double *uncertainty_dest);
			u64 sigFigCount(const char *s);
			u64 sigFigCount(const std::string &s);
		} // namespace uasf
	} // namespace visx
} // namespace jp

#endif
