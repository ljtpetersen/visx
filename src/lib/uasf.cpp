/* src/lib/uasf.cpp
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

#include <jp/visx.hpp>
#include <utility>
#include <math.h>
#include <stdio.h>

#ifndef __cplusplus
#error Not compiled using C++!
#endif

using namespace jp::visx::uasf;

// The invalid element has type UOPERATION_INVALID, and values NaN.
const UncertaintyTableElement UncertaintyTableElement::invalid_element = UncertaintyTableElement{UOPERATION_INVALID, NAN, NAN, NAN, NAN};

// The constructors for the UncertaintyTableElement only set the values.
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty) : UncertaintyTableElement(type, value, uncertainty, 0.0, 0.0) {}
// If the UncertaintyPair is NULL, set the values to 0.0.
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, const UncertaintyPair *value) : UncertaintyTableElement(type, value ? value->value : 0.0, value ? value->uncertainty : 0.0) {}
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty, double cumulative_value, double cumulative_uncertainty) : type_(type), value_(value), uncertainty_(uncertainty), cumulative_value_(cumulative_value), cumulative_uncertainty_(cumulative_uncertainty) {
	// Ensure the uncertainties are positive.
	uncertainty_ = fabs(uncertainty_);
	cumulative_uncertainty_ = fabs(cumulative_uncertainty_);
	simplifyUncertainty(cumulative_value_, cumulative_uncertainty_, &cumulative_value_, &cumulative_uncertainty_);
}

void UncertaintyTableElement::compute(UncertaintyPair *result_dest) const {
	// If the result is NULL, return. (There is no place to put the result.)
	if (!result_dest) {
		return;
	}
	double value_b, uncertainty_b;
	simplifyUncertainty(value_, uncertainty_, &value_b, &uncertainty_b);
	switch (type_) {
	// If the type is NUL, the result is the value and uncertainty.
	// The cumulative values for NUL are ignored.
	case UOPERATION_NUL:
		result_dest->value = value_b;
		result_dest->uncertainty = uncertainty_b;
		break;
	// If the type is ADD, the result is the sum of the value and the cumulative value.
	// The uncertainty is the sum of the uncertainty and the cumulative uncertainty.
	case UOPERATION_ADD:
		result_dest->value = value_b + cumulative_value_;
		result_dest->uncertainty = uncertainty_b + cumulative_uncertainty_;
		break;
	// If the type is SUB, the result is the difference between the cumulative value and
	// the value. The resulting uncertainty is the sum of the cumulative uncertainty and the
	// uncertainty.
	case UOPERATION_SUB:
		result_dest->value = cumulative_value_ - value_b;
		result_dest->uncertainty = uncertainty_b + cumulative_uncertainty_;
		break;
	// If the type is SUBO, the result is the opposite sign of if the operation is SUB.
	// The resulting uncertainty is the same.
	case UOPERATION_SUBO:
		result_dest->value = value_b - cumulative_value_;
		result_dest->uncertainty = uncertainty_b + cumulative_uncertainty_;
		break;
	// If the operation is MUL, the result is the product of the value and the cumulative
	// value. The resulting relative uncertainty is the sum of the relative uncertainties of
	// the cumulative and the value.
	case UOPERATION_MUL: {
		double res = result_dest->value = value_b * cumulative_value_;
		if (value_b == 0.0 && cumulative_value_ == 0.0) {
			result_dest->uncertainty = uncertainty_b * cumulative_uncertainty_;
		} else if (value_b == 0.0) {
			result_dest->uncertainty = (cumulative_uncertainty_ + cumulative_value_) * uncertainty_b;
		} else if (cumulative_value_ == 0.0) {
			result_dest->uncertainty = (value_b + uncertainty_b) * cumulative_uncertainty_;
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) + (uncertainty_b / value_b));
		}
	} break;
	// If the operation is DIV, the result is the quotient of the cumulative and the value.
	// The resulting uncertainty is calculated in the same manner as with MUL.
	case UOPERATION_DIV: {
		double res = result_dest->value = value_b != 0 ? cumulative_value_ / value_b : NAN;
		if (value_b == 0.0) {
			result_dest->uncertainty = NAN;
		} else if (cumulative_value_ == 0.0) {
			result_dest->uncertainty = (value_b + uncertainty_b == 0.0) ? DBL_MAX : (cumulative_uncertainty_ / (value_b + uncertainty_b));
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) + (uncertainty_b / value_b));
		}
	} break;
	// If the operation is DIVO, the result is the same as with DIV, but to the power of -1
	// (1/x). The uncertainty is calculated in the same way.
	case UOPERATION_DIVO: {
		double res = result_dest->value = cumulative_value_ != 0.0 ? value_b / cumulative_value_ : NAN;
		if (cumulative_value_ == 0.0) {
			result_dest->uncertainty = NAN;
		} else if (value_b == 0.0) {
			result_dest->uncertainty = (cumulative_value_ + cumulative_uncertainty_ == 0.0) ? DBL_MAX : (uncertainty_b / (cumulative_value_ + cumulative_uncertainty_));
		} else {
			result_dest->uncertainty = res * ((uncertainty_b / value_b) + (cumulative_uncertainty_ / cumulative_value_));
		}
	} break;
	// If the operation is POW, the result is the cumulative to the power of the value.
	// The relative resulting uncertainty is the product of the relative uncertainty of the
	// cumulative and the value. (z * dx/x)
	case UOPERATION_POW: {
		double res = result_dest->value = cumulative_value_ == 0.0 && value_b == 0.0 ? NAN : pow(cumulative_value_, value_b);
		if (isnan(res)) {
			result_dest->uncertainty = NAN;
		} else if (cumulative_value_ == 0.0) {
			result_dest->uncertainty = pow(cumulative_uncertainty_, value_b);
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) * value_b);
		}
	} break;
	// If the operation is POWO, the result is calculated in the same manner as POW, but
	// by switching cumulative with non-cumulative values.
	case UOPERATION_POWO: {
		double res = result_dest->value = value_b == 0.0 && cumulative_value_ == 0.0 ? NAN : pow(value_b, cumulative_value_);
		if (isnan(res)) {
			result_dest->uncertainty = NAN;
		} else if (value_b == 0.0) {
			result_dest->uncertainty = pow(uncertainty_b, cumulative_value_);
		} else {
			result_dest->uncertainty = res * ((uncertainty_b / value_b) * cumulative_value_);
		}
	} break;
	// If the operation is MULC, the uncertainty is ignored. The result is the value multiplied
	// by the cumulative value. The resulting uncertainty is the cumulative uncertainty
	// multiplied by the value.
	case UOPERATION_MULC:
		result_dest->value = cumulative_value_ * value_b;
		result_dest->uncertainty = cumulative_uncertainty_ * value_b;
		break;
	case UOPERATION_MULCO:
		result_dest->value = cumulative_value_ * value_b;
		result_dest->uncertainty = cumulative_value_ * uncertainty_b;
		break;
	// The DIVC operation is the same as the MULC operation, but instead of multiplication,
	// it uses division.
	case UOPERATION_DIVC:
		result_dest->value = value_b != 0.0 ? cumulative_value_ / value_b : NAN;
		result_dest->uncertainty = value_b != 0.0 ? cumulative_uncertainty_ / value_b : NAN;
		break;
	case UOPERATION_DIVCO:
		result_dest->value = cumulative_value_ != 0.0 ? value_b / cumulative_value_ : NAN;
		result_dest->uncertainty = cumulative_value_ != 0.0 ? uncertainty_b / cumulative_value_ : NAN;
		break;
	// If the operation is not valid, the resulting value is the same as the cumulative value;
	// the value_b and uncertainty_b values are discarded.
	default:
		result_dest->value = cumulative_value_;
		result_dest->uncertainty = cumulative_uncertainty_;
		break;
	}
	// The uncertainty should always be positive.
	result_dest->uncertainty = fabs(result_dest->uncertainty);
	// Simplify the uncertainty before returning.
	simplifyUncertainty(result_dest->value, result_dest->uncertainty, &result_dest->value, &result_dest->uncertainty);
}

UncertaintyTableElementType UncertaintyTableElement::getType(void) const {
	// Return the type.
	return type_;
}

double UncertaintyTableElement::getCumulative(void) const {
	// Return the cumulative value.
	return cumulative_value_;
}

void UncertaintyTableElement::getCumulative(UncertaintyPair *result_dest) const {
	// Ensure result_dest is a valid pointer.
	if (!result_dest) return;
	// Put the cumulative value and uncertainty into the result_dest pointer.
	result_dest->value = cumulative_value_;
	result_dest->uncertainty = cumulative_uncertainty_;
}

double UncertaintyTableElement::getCumulativeUncertainty(void) const {
	// Return the cumulative uncertainty.
	return cumulative_uncertainty_;
}

void UncertaintyTableElement::getValue(UncertaintyPair *result_dest) const {
	// Ensure result_dest is a valid pointer.
	if (!result_dest) return;
	// Put the value and uncertainty into the result_dest pointer.
	result_dest->value = value_;
	result_dest->uncertainty = uncertainty_;
}

double UncertaintyTableElement::getValue(void) const {
	// Return the value.
	return value_;
}

double UncertaintyTableElement::getUncertainty(void) const {
	// return the uncertainty.
	return uncertainty_;
}

void UncertaintyTableElement::setCumulative(double value, double uncertainty) {
	simplifyUncertainty(value, fabs(uncertainty), &cumulative_value_, &cumulative_uncertainty_);
}

void UncertaintyTableElement::setCumulative(const UncertaintyPair *value) {
	// Ensure the value is a valid pointer.
	if (!value) return;
	simplifyUncertainty(value->value, fabs(value->uncertainty), &cumulative_value_, &cumulative_uncertainty_);
}

void UncertaintyTableElement::setCumulative(double value) {
	simplifyUncertainty(value, cumulative_uncertainty_, &cumulative_value_, &cumulative_uncertainty_);
}

void UncertaintyTableElement::setCumulativeUncertainty(double value) {
	simplifyUncertainty(cumulative_value_, fabs(value), &cumulative_value_, &cumulative_uncertainty_);
}

void UncertaintyTableElement::setValue(double value, double uncertainty) {
	value_ = value;
	uncertainty_ = fabs(uncertainty);
}

void UncertaintyTableElement::setValue(const UncertaintyPair *value) {
	// Ensure value is a valid pointer.
	if (!value) return;
	value_ = value->value;
	uncertainty_ = fabs(value->uncertainty);
}

void UncertaintyTableElement::setValue(double value) {
	value_ = value;
}

void UncertaintyTableElement::setUncertainty(double value) {
	uncertainty_ = fabs(value);
}

void UncertaintyTableElement::setType(UncertaintyTableElementType type) {
	// Set the type.
	type_ = type;
}

void UncertaintyTableElement::setNotType(const UncertaintyTableElement &value) {
	// No need to check if value is valid (references should not be NULL).
	// Set the value.
	value_ = value.value_;
	// Ensure the uncertainty is positive before setting it.
	uncertainty_ = fabs(value.uncertainty_);
	// Set the cumulative value.
	cumulative_value_ = value.cumulative_value_;
	// Ensure the cumulative uncertainty is positive before setting it.
	cumulative_uncertainty_ = fabs(value.cumulative_uncertainty_);
}

UncertaintyTable::UncertaintyTable(size_t starting_capacity) : UncertaintyTable(starting_capacity, 0.0, 0.0) {}
UncertaintyTable::UncertaintyTable(size_t starting_capacity, double value, double uncertainty) {
	// Reserve `starting_capacity` elements.
	elements_.reserve(starting_capacity);
	// Add the starting value to the table.
	elements_.emplace_back(UOPERATION_NUL, value, uncertainty);
	this->compute(0);
}

// The second constructor calls the first constructor with default value `10`
// as starting capacity.
UncertaintyTable::UncertaintyTable(void) : UncertaintyTable(10) {}

size_t UncertaintyTable::getCapacity(void) const {
	// Return the capacity.
	return elements_.capacity();
}

void UncertaintyTable::getValue(size_t row, UncertaintyPair *result_dest) const {
	// If result_dest is invalid, return.
	if (!result_dest) return;
	// If the row is invalid, put NaN into the result_dest.
	else if (row >= elements_.size()) {
		UncertaintyTableElement::invalid_element.getValue(result_dest);
	// Otherwise, get the value of the row.
	} else {
		elements_[row].getValue(result_dest);
	}
}

double UncertaintyTable::getValue(size_t row) const {
	// If the row is invalid, return NaN.
	if (row >= elements_.size()) return NAN;
	// Otherwise, return the value.
	return elements_[row].getValue();
}

double UncertaintyTable::getUncertainty(size_t row) const {
	// If the row is invalid, return NaN.
	if (row >= elements_.size()) return NAN;
	// Otherwise, return the uncertainty.
	return elements_[row].getUncertainty();
}

const UncertaintyTableElement &UncertaintyTable::getElement(size_t row) const {
	// If the row is invalid, return an invalid_element.
	if (row >= elements_.size()) return UncertaintyTableElement::invalid_element;
	// Otherwise, return the element.
	return elements_[row];
}

void UncertaintyTable::add(UncertaintyTableElementType type, double value, double uncertainty) {
	// Add the value to the table.
	elements_.emplace_back(type, value, uncertainty);
	// Compute the table.
	this->compute(elements_.size() - 2);
}

void UncertaintyTable::add(UncertaintyTableElementType type, const UncertaintyPair *value) {
	// Add the value to the table.
	elements_.emplace_back(type, value);
	// Compute the table.
	this->compute(elements_.size() - 2);
}

void UncertaintyTable::remove(size_t row) {
	// If the row is not zero and it is a valid row, remove the row from the table.
	if (row < elements_.size() && row) {
		elements_.erase(elements_.begin() + row);
		this->compute(row - 1);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElementType type, double value, double uncertainty) {
	// If the row is zero, return.
	if (!row) return;
	// Otherwise, if the row is a valid row, add a row at that position.
	else if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, UncertaintyTableElement{type, value, uncertainty});
		this->compute(row - 1);
	// Otherwise, add a row to the end of the table.
	} else {
		elements_.push_back(UncertaintyTableElement{type, value, uncertainty});
		this->compute(elements_.size() - 2);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElementType type, const UncertaintyPair *value) {
	// If the row is zero, return.
	if (!row) return;
	// Otherwise, if the row is a valid row, add a row at that position.
	else if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, UncertaintyTableElement{type, value});
		this->compute(row - 1);
	// Otherwise, add a row to the end of the table.
	} else {
		elements_.push_back(UncertaintyTableElement{type, value});
		this->compute(elements_.size() - 2);
	}
}

void UncertaintyTable::swap(size_t row1, size_t row2) {
	// If the rows are all valid and non-zero, continue.
	if (!row1 || !row2 || row1 >= elements_.size() || row2 >= elements_.size()) return;
	// Copy the value of the first row into a temporary variable.
	UncertaintyTableElement el(elements_[row1]);
	// Copy the value of the second row into the first row.
	elements_[row1] = elements_[row2];
	// Copy the original value of the first row into the second row.
	elements_[row2] = el;
	// Compute the new resulting value. (Start from the lowest of the two rows).
	this->compute(row1 < row2 ? row1 - 1 : row2 - 1);
}

void UncertaintyTable::set(size_t row, const UncertaintyPair *value) {
	// If the row or the value pointer is invalid, return.
	if (row >= elements_.size() || !value) return;
	// Otherwise, set the value,
	elements_[row].setValue(value);
	// and compute the result (no need to compute from one less, as the cumulative
	// value is not changed).
	this->compute(row);
}

void UncertaintyTable::set(size_t row, double value) {
	// If the row is invalid, return.
	if (row >= elements_.size()) return;
	// Otherwise, set the value,
	elements_[row].setValue(value);
	// and compute the result (no need to compute from one less, as the cumulative
	// value is not changed).
	this->compute(row);
}

void UncertaintyTable::set(size_t row, double value, double uncertainty) {
	// If the row is invalid, return.
	if (row >= elements_.size()) return;
	// Otherwise, set the value,
	elements_[row].setValue(value, uncertainty);
	// and compute the result (no need to compute from one less, as the cumulative
	// value is not changed).
	this->compute(row);
}

void UncertaintyTable::setUncertainty(size_t row, double uncertainty) {
	// If the row is invalid, return.
	if (row >= elements_.size()) return;
	// Otherwise, set the value,
	elements_[row].setUncertainty(uncertainty);
	// and compute the result (no need to compute from one less, as the cumulative
	// value is not changed).
	this->compute(row);
}

void UncertaintyTable::setStartingValue(double value, double uncertainty) {
	// Set the starting value and compute from the start.
	elements_.front().setValue(value, uncertainty);
	this->compute(0);
}

void UncertaintyTable::setStartingValue(double value) {
	// Set the starting value and compute from the start.
	elements_.front().setValue(value);
	this->compute(0);
}

void UncertaintyTable::recompute(void) {
	this->compute(0);
}

void UncertaintyTable::setStartingValue(const UncertaintyPair *value) {
	// If the value is not a valid pointer, return.
	if (!value) return;
	// Otherwise, set the starting value and compute from the start.
	elements_.front().setValue(value);
	this->compute(0);
}

void UncertaintyTable::setStartingUncertainty(double uncertainty) {
	// Set the starting uncertainty and compute from the start.
	elements_.front().setUncertainty(uncertainty);
	this->compute(0);
}

double UncertaintyTable::getStartingValue(void) const {
	// Return the starting value.
	return elements_.front().getValue();
}

double UncertaintyTable::getStartingUncertainty(void) const {
	// Return the starting uncertainty.
	return elements_.front().getUncertainty();
}

void UncertaintyTable::getStartingValue(UncertaintyPair *value_dest) const {
	// Get the starting value.
	this->elements_.front().getValue(value_dest);
}

size_t UncertaintyTable::count(void) const {
	// Return the number of elements. (the starting value counts as an element.)
	return elements_.size();
}

UncertaintyTableElementType UncertaintyTable::getType(size_t row) const {
	// If the row is invalid, return an invalid operation.
	if (row >= elements_.size()) return UOPERATION_INVALID;
	// Otherwise, return the type.
	return elements_[row].getType();
}

void UncertaintyTable::add(const UncertaintyTableElement &element) {
	// Add the element to the back of the table and recompute.
	elements_.push_back(element);
	this->compute(elements_.size() - 2);
}

void UncertaintyTable::add(UncertaintyTableElement &&element) {
	// Add the element to the back of the table and recompute.
	elements_.push_back(element);
	this->compute(elements_.size() - 2);
}

void UncertaintyTable::addAt(size_t row, const UncertaintyTableElement &element) {
	// If the row is zero, return.
	if (!row) return;
	// Otherwise, if the row is valid, add the element at that position and recompute.
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, element);
		this->compute(row - 1);
	// Otherwise, add the element to the back of the table and recompute.
	} else {
		elements_.push_back(element);
		this->compute(elements_.size() - 2);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElement &&element) {
	// If the row is zero, return.
	if (!row) return;
	// Otherwise, if the row is valid, add the element at that position and recompute.
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, element);
		this->compute(row - 1);
	// Otherwise, add the element to the back of the table and recompute.
	} else {
		elements_.push_back(element);
		this->compute(elements_.size() - 2);
	}
}

void UncertaintyTable::set(size_t row, UncertaintyTableElement &&element) {
	// If the row is invalid, return.
	if (row >= elements_.size()) return;
	// Set the row.
	elements_[row] = element;
	// If the row is the first row, set the operation to NUL.
	if (!row) elements_[row++].setType(UOPERATION_NUL);
	// Compute.
	this->compute(row);
}

void UncertaintyTable::set(size_t row, const UncertaintyTableElement &element) {
	// If the row is invalid, return.
	if (row >= elements_.size()) return;
	// Set the row.
	elements_[row] = element;
	// If the row is the first row, set the operation to NUL.
	if (!row) elements_[row++].setType(UOPERATION_NUL);
	this->compute(row - 1);
}

void UncertaintyTable::compute(size_t starting_row) {
	// Declare an UncertaintyPair which will contain the current cumulative.
	UncertaintyPair current_cumulative;
	// If the row is invalid, return.
	if (starting_row >= count()) return;
	// Otherwise, get the first element and the end of the array.
	auto begin = elements_.begin() + starting_row, end = elements_.end();
	// If the first element is greater than or equal to the end, return.
	if (begin >= end) return;
	// Compute the first element.
	begin->compute(&current_cumulative);
	for (begin = begin + 1; begin < end; ++begin) {
		// If the UncertaintyPair contains an invalid value, set the remaining cumulatives,
		// as well as the result, to NaN.
		if (isnan(current_cumulative.uncertainty) || isnan(current_cumulative.value)) {
			for ( ; begin < end; ++begin) {
				begin->setNotType(UncertaintyTableElement::invalid_element);
			}
			break;
		}
		// Otherwise, set the cumulatives,
		begin->setCumulative(&current_cumulative);
		// and compute.
		begin->compute(&current_cumulative);
	}
	// Set the result.
	result_ = current_cumulative;
}

void UncertaintyTable::getResult(UncertaintyPair *result_dest) const {
	// Ensure the operator is valid.
	if (!result_dest) return;
	// Put the uncertainty and value into the result_dest.
	result_dest->uncertainty = result_.uncertainty;
	result_dest->value = result_.value;
}

double UncertaintyTable::getResult(void) const {
	// Return the result.
	return result_.value;
}

double UncertaintyTable::getResultingUncertainty(void) const {
	// Return the resulting uncertainty.
	return result_.uncertainty;
}

void UncertaintyTable::clear(void) {
	// Clear the table and add a first element.
	elements_.clear();
	elements_.emplace_back(UOPERATION_NUL, 0.0, 0.0);
	this->compute(0);
}

void jp::visx::uasf::simplifyUncertainty(double value, double uncertainty, double *value_dest, double *uncertainty_dest) {
	// I don't know how this works. I copied it over from some old code.
	// The comments I put on are what I think it does lol.

	// Check that the inputted values are not infinite or NaN.
	if (isinf(value) || isnan(value) || isinf(uncertainty) || isnan(uncertainty)) {
		*value_dest = NAN;
		*uncertainty_dest = NAN;
		return;
	}

	// Get the value of the value as a string.
	char value_str[10 + DBL_DIG] = "";
	sprintf(value_str, "%+." JP_STRMACRO(DBL_DIG) "e", value);
	{ // Make sure there is proper rounding.
		char uncertainty_str[14] = "";
		sprintf(uncertainty_str, "%+.1e", uncertainty);
		// If we should be rounding the uncertainty, make sure it is rounded
		// by adding 0.1 * the_exponent - 1 to it.
		if (uncertainty_str[3] == '5') {
			char new_uncertainty[9] = "";
			int exponent = atoi(uncertainty_str + 5);
			sprintf(new_uncertainty, "1e%d", exponent - 1);
			uncertainty += strtod(new_uncertainty, NULL);
		}
	}
	// Get the uncertainty (this should already have one sigfig).
	char uncertainty_str[9] = "";
	sprintf(uncertainty_str, "%+.0e", uncertainty);
	char *uncertainty_exp = uncertainty_str + 3, *value_exp = value_str + 4 + DBL_DIG;
	// Get the exponent values for the uncertainty and value.
	int uncertainty_expint = atoi(uncertainty_exp), value_expint = atoi(value_exp);
	// If the uncertainty is zero, do not change the value and return.
	if (uncertainty == 0.0) {
		*uncertainty_dest = 0.0;
		*value_dest = value;
		return;
	// Otherwise, if the uncertainty's exponent is greater than the value's
	// exponent, return with a value of zero.
	} else if (uncertainty_expint > value_expint) {
		*value_dest = 0.0;
		*uncertainty_dest = strtod(uncertainty_str, NULL);
	// Otherwise, if the uncertainty's and value's exponents are too far apart
	// (value > uncertainty) then return the original value with uncertainty zero.
	} else if (value_expint - uncertainty_expint > DBL_DIG) {
		*value_dest = strtod(value_str, NULL);
		*uncertainty_dest = 0.0;
	} else {
		// The final value will be multiplied by this afterwards.
		float em = 1.0;
		// Parse the uncertainty
		*uncertainty_dest = strtod(uncertainty_str, NULL);
		// IDK anymore.
		char *value_str2 = value_str + value_expint + 2 - uncertainty_expint, *value_str3 = value_str2;
		if (value_str2[1] > '4') {
			for ( ; value_str2[1] != 'e'; ++value_str2) { value_str2[1] = '0'; }
			value_str2 = value_str3;
			for ( ; value_str2[0] == '9'; --value_str2) { value_str2[0] = '0'; }
			if (value_str2[0] == '.') {
				if ((--value_str2)[0] == '9') {
					if (value_str[0] == '-') { em = -1.0; }
					value_str2[0] = '0';
					value_str[0] = '1';
				} else {
					++(value_str2[0]);
				}
			} else {
				++(value_str2[0]);
			}
		} else {
			for ( ; value_str2[1] != 'e'; ++value_str2) { value_str2[1] = '0'; }
		}
		*value_dest = strtod(value_str, NULL) * em;
	}
	*uncertainty_dest = fabs(*uncertainty_dest);
}

u64 jp::visx::uasf::sigFigCount(const char *s) {
	// This function is also imported from old code,
	// but I should be able to figure it out.
	u64 num = 0, zero_count = 0;
	bool has_comma_or_dot = false;
	for ( ; *s; ++s) {
		char c = *s;
		switch (c) {
		case ',':
		case '.':
			// If there is already a comma or dot, return 0 (invalid number).
			if (has_comma_or_dot) return 0;
			has_comma_or_dot = true;
			break;
		case '0':
			// If there has already been a comma or dot, and there has been another digit,
			// add the number of zeroes we have passed and the latest zero to the number
			// of digits. Then, zero the zero_count.
			if (has_comma_or_dot && num) {
				num += zero_count + 1;
				zero_count = 0;
			// Otherwise, if there have been other digits, add to the zero_count.
			} else if (num) ++zero_count;
			break;
		default:
			// If the character is not a number, return.
			if (c < '1' || c > '9') return 0;
			// Otherwise, add the zero_count + 1 to the number of figures.
			num += zero_count + 1;
			// Zero the zero count.
			zero_count = 0;
			break;
		}
	}
	// Return the number.
	return num;
}

u64 jp::visx::uasf::sigFigCount(const std::string &s) {
	return jp::visx::uasf::sigFigCount(s.c_str());
}

// If want C compatibility.
// Also works: set CMake value JP_CCOMPAT to true.
#ifdef JP_CCOMPAT

extern "C" {

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

typedef UncertaintyTable jp_visx_uasf_UncertaintyTable;

}

extern "C" jp_visx_uasf_UncertaintyTable *jp_visx_uasf_UncertaintyTable_new1(void);
extern "C" jp_visx_uasf_UncertaintyTable *jp_visx_uasf_UncertaintyTable_new2(size_t starting_capacity);
extern "C" size_t jp_visx_uasf_UncertaintyTable_getCapacity(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_getValue1(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyPair *result_dest);
extern "C" double jp_visx_uasf_UncertaintyTable_getValue2(jp_visx_uasf_UncertaintyTable *table, size_t row);
extern "C" double jp_visx_uasf_UncertaintyTable_getUncertainty(jp_visx_uasf_UncertaintyTable *table, size_t row);
extern "C" jp_visx_uasf_UncertaintyTableElementType jp_visx_uasf_UncertaintyTable_getType(jp_visx_uasf_UncertaintyTable *table, size_t row);
extern "C" void jp_visx_uasf_UncertaintyTable_add1(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty);
extern "C" void jp_visx_uasf_UncertaintyTable_add2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value);
extern "C" void jp_visx_uasf_UncertaintyTable_remove(jp_visx_uasf_UncertaintyTable *table, size_t row);
extern "C" void jp_visx_uasf_UncertaintyTable_clear(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_addAt1(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty);
extern "C" void jp_visx_uasf_UncertaintyTable_addAt2(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value);
extern "C" void jp_visx_uasf_UncertaintyTable_swap(jp_visx_uasf_UncertaintyTable *table, size_t row1, size_t row2);
extern "C" void jp_visx_uasf_UncertaintyTable_set1(jp_visx_uasf_UncertaintyTable *table, size_t row, const jp_visx_uasf_UncertaintyPair *value);
extern "C" void jp_visx_uasf_UncertaintyTable_set2(jp_visx_uasf_UncertaintyTable *table, size_t row, double value);
extern "C" void jp_visx_uasf_UncertaintyTable_set3(jp_visx_uasf_UncertaintyTable *table, size_t row, double value, double uncertainty);
extern "C" void jp_visx_uasf_UncertaintyTable_setUncertainty(jp_visx_uasf_UncertaintyTable *table, size_t row, double uncertainty);
extern "C" void jp_visx_uasf_UncertaintyTable_setStartingValue1(jp_visx_uasf_UncertaintyTable *table, double value, double uncertainty);
extern "C" void jp_visx_uasf_UncertaintyTable_setStartingValue2(jp_visx_uasf_UncertaintyTable *table, double value);
extern "C" void jp_visx_uasf_UncertaintyTable_setStartingValue3(jp_visx_uasf_UncertaintyTable *table, const jp_visx_uasf_UncertaintyPair *value);
extern "C" void jp_visx_uasf_UncertaintyTable_setStartingUncertainty(jp_visx_uasf_UncertaintyTable *table, double uncertainty);
extern "C" double jp_visx_uasf_UncertaintyTable_getStartingValue1(jp_visx_uasf_UncertaintyTable *table);
extern "C" double jp_visx_uasf_UncertaintyTable_getStartingUncertainty(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_getStartingValue2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyPair *value);
extern "C" size_t jp_visx_uasf_UncertaintyTable_count(jp_visx_uasf_UncertaintyTable *table);
extern "C" double jp_visx_uasf_UncertaintyTable_getResult1(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_getResult2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyPair *result_dest);
extern "C" double jp_visx_uasf_UncertaintyTable_getResultingUncertainty(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_recompute(jp_visx_uasf_UncertaintyTable *table);
extern "C" void jp_visx_uasf_UncertaintyTable_free(jp_visx_uasf_UncertaintyTable *table);
extern "C" u64 jp_visx_uasf_sigFigCount(const char *);

u64 jp_visx_uasf_sigFigCount(const char *c) {
	return jp::visx::uasf::sigFigCount(c);
}

UncertaintyTable *jp_visx_uasf_UncertaintyTable_new1(void) {
	return new UncertaintyTable();
}

UncertaintyTable *jp_visx_uasf_UncertaintyTable_new2(size_t starting_capacity) {
	return new UncertaintyTable(starting_capacity);
}

size_t jp_visx_uasf_UncertaintyTable_getCapacity(UncertaintyTable *table) {
	return table->getCapacity();
}

void jp_visx_uasf_UncertaintyTable_getValue1(UncertaintyTable *table, size_t row, UncertaintyPair *result_dest) {
	table->getValue(row, result_dest);
}

double jp_visx_uasf_UncertaintyTable_getValue2(UncertaintyTable *table, size_t row) {
	return table->getValue(row);
}

double jp_visx_uasf_UncertaintyTable_getUncertainty(UncertaintyTable *table, size_t row) {
	return table->getUncertainty(row);
}

jp_visx_uasf_UncertaintyTableElementType jp_visx_uasf_UncertaintyTable_getType(UncertaintyTable *table, size_t row) {
	return (jp_visx_uasf_UncertaintyTableElementType)table->getType(row);
}

void jp_visx_uasf_UncertaintyTable_add2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value) {
	table->add((UncertaintyTableElementType)type, (const UncertaintyPair *) value);
}

void jp_visx_uasf_UncertaintyTable_add1(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty) {
	table->add((UncertaintyTableElementType) type, value, uncertainty);
}

void jp_visx_uasf_UncertaintyTable_remove(UncertaintyTable *table, size_t row) {
	table->remove(row);
}

void jp_visx_uasf_UncertaintyTable_clear(UncertaintyTable *table) {
	table->clear();
}

void jp_visx_uasf_UncertaintyTable_addAt1(jp_visx_uasf_UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, double value, double uncertainty) {
	table->addAt(row, (UncertaintyTableElementType) type, value, uncertainty);
}

void jp_visx_uasf_UncertaintyTable_addAt2(UncertaintyTable *table, size_t row, jp_visx_uasf_UncertaintyTableElementType type, const jp_visx_uasf_UncertaintyPair *value) {
	table->addAt(row, (UncertaintyTableElementType) type, (const UncertaintyPair *) value);
}

void jp_visx_uasf_UncertaintyTable_swap(jp_visx_uasf_UncertaintyTable *table, size_t row1, size_t row2) {
	table->swap(row1, row2);
}

void jp_visx_uasf_UncertaintyTable_set1(jp_visx_uasf_UncertaintyTable *table, size_t row, const jp_visx_uasf_UncertaintyPair *value) {
	table->set(row, (const UncertaintyPair *)value);
}

void jp_visx_uasf_UncertaintyTable_set2(jp_visx_uasf_UncertaintyTable *table, size_t row, double value) {
	table->set(row, value);
}

void jp_visx_uasf_UncertaintyTable_set3(jp_visx_uasf_UncertaintyTable *table, size_t row, double value, double uncertainty) {
	table->set(row, value, uncertainty);
}

void jp_visx_uasf_UncertaintyTable_setUncertainty(UncertaintyTable *table, size_t row, double uncertainty) {
	table->setUncertainty(row, uncertainty);
}

void jp_visx_uasf_UncertaintyTable_setStartingValue1(UncertaintyTable *table, double value, double uncertainty) {
	table->setStartingValue(value, uncertainty);
}

void jp_visx_uasf_UncertaintyTable_setStartingValue2(UncertaintyTable *table, double value) {
	table->setStartingValue(value);
}

void jp_visx_uasf_UncertaintyTable_setStartingValue3(UncertaintyTable *table, const jp_visx_uasf_UncertaintyPair *value) {
	table->setStartingValue((const UncertaintyPair *)value);
}

void jp_visx_uasf_UncertaintyTable_setStartingUncertainty(UncertaintyTable *table, double value) {
	table->setStartingUncertainty(value);
}

size_t jp_visx_uasf_UncertaintyTable_count(UncertaintyTable *table) {
	return table->count();
}

double jp_visx_uasf_UncertaintyTable_getResult1(jp_visx_uasf_UncertaintyTable *table) {
	return table->getResult();
}

void jp_visx_uasf_UncertaintyTable_getResult2(jp_visx_uasf_UncertaintyTable *table, jp_visx_uasf_UncertaintyPair *result_dest) {
	table->getResult((UncertaintyPair *)result_dest);
}

double jp_visx_uasf_UncertaintyTable_getResultingUncertainty(UncertaintyTable *table) {
	return table->getResultingUncertainty();
}

void jp_visx_uasf_UncertaintyTable_recompute(UncertaintyTable *table) {
	table->recompute();
}

void jp_visx_uasf_UncertaintyTable_free(UncertaintyTable *table) {
	delete table;
}

#endif
