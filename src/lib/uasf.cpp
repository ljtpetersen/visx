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

using namespace jp::visx::uasf;

const UncertaintyTableElement UncertaintyTableElement::invalid_element = UncertaintyTableElement{UOPERATION_INVALID, NAN, NAN, NAN, NAN};
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty) : type_(type), value_(value), uncertainty_(uncertainty) {}
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, const UncertaintyPair *value) : type_(type), value_(value ? value->value : 0.0), uncertainty_(value ? value->uncertainty : 0.0) {}
UncertaintyTableElement::UncertaintyTableElement(UncertaintyTableElementType type, double value, double uncertainty, double cumulative_value, double cumulative_uncertainty) : type_(type), value_(value), uncertainty_(uncertainty), cumulative_value_(cumulative_value), cumulative_uncertainty_(cumulative_uncertainty) {}
void UncertaintyTableElement::compute(UncertaintyPair *result_dest) const {
	if (!result_dest) {
		return;
	}
	switch (type_) {
	case UOPERATION_NUL:
		result_dest->value = value_;
		result_dest->uncertainty = uncertainty_;
		break;
	case UOPERATION_ADD:
		result_dest->value = value_ + cumulative_value_;
		result_dest->uncertainty = uncertainty_ + cumulative_uncertainty_;
		break;
	case UOPERATION_SUB:
		result_dest->value = cumulative_value_ - value_;
		result_dest->uncertainty = uncertainty_ + cumulative_uncertainty_;
		break;
	case UOPERATION_SUBO:
		result_dest->value = cumulative_value_ - value_;
		result_dest->uncertainty = uncertainty_ + cumulative_uncertainty_;
		break;
	case UOPERATION_MUL: {
		double res = result_dest->value = value_ * cumulative_value_;
		if (value_ == 0 && cumulative_value_ == 0) {
			result_dest->uncertainty = uncertainty_ * cumulative_uncertainty_;
		} else if (value_ == 0) {
			result_dest->uncertainty = (cumulative_uncertainty_ + cumulative_value_) * uncertainty_;
		} else if (cumulative_value_ == 0) {
			result_dest->uncertainty = (value_ + uncertainty_) * cumulative_uncertainty_;
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) + (uncertainty_ / value_));
		}
	} break;
	case UOPERATION_DIV: {
		double res = result_dest->value = value_ != 0 ? cumulative_value_ / value_ : NAN;
		if (value_ == 0) {
			result_dest->uncertainty = NAN;
		} else if (cumulative_value_ == 0) {
			result_dest->uncertainty = (value_ + uncertainty_ == 0) ? DBL_MAX : (cumulative_uncertainty_ / (value_ + uncertainty_));
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) + (uncertainty_ / value_));
		}
	} break;
	case UOPERATION_DIVO: {
		double res = result_dest->value = cumulative_value_ != 0 ? value_ / cumulative_value_ : NAN;
		if (cumulative_value_ == 0) {
			result_dest->uncertainty = NAN;
		} else if (value_ == 0) {
			result_dest->uncertainty = (cumulative_value_ + cumulative_uncertainty_ == 0) ? DBL_MAX : (uncertainty_ / (cumulative_value_ + cumulative_uncertainty_));
		} else {
			result_dest->uncertainty = res * ((uncertainty_ / value_) + (cumulative_uncertainty_ / cumulative_value_));
		}
	} break;
	case UOPERATION_POW: {
		double res = result_dest->value = cumulative_value_ == 0 && value_ == 0 ? NAN : pow(cumulative_value_, value_);
		if (isnan(res)) {
			result_dest->uncertainty = NAN;
		} else if (cumulative_value_ == 0) {
			result_dest->uncertainty = pow(cumulative_uncertainty_, value_);
		} else {
			result_dest->uncertainty = res * ((cumulative_uncertainty_ / cumulative_value_) * value_);
		}
	} break;
	case UOPERATION_MULC:
		result_dest->value = cumulative_value_ * value_;
		result_dest->uncertainty = cumulative_uncertainty_ * value_;
		break;
	case UOPERATION_DIVC:
		result_dest->value = value_ != 0 ? cumulative_value_ / value_ : NAN;
		result_dest->uncertainty = value_ != 0 ? cumulative_uncertainty_ / value_ : NAN;
		break;
	default:
		result_dest->value = cumulative_value_;
		result_dest->uncertainty = cumulative_uncertainty_;
		break;
	}
	result_dest->uncertainty = fabs(result_dest->uncertainty);
}

UncertaintyTableElementType UncertaintyTableElement::getType(void) const {
	return type_;
}

double UncertaintyTableElement::getCumulative(void) const {
	return cumulative_value_;
}

void UncertaintyTableElement::getCumulative(UncertaintyPair *result_dest) const {
	result_dest->value = cumulative_value_;
	result_dest->uncertainty = cumulative_uncertainty_;
}

double UncertaintyTableElement::getCumulativeUncertainty(void) const {
	return cumulative_uncertainty_;
}

void UncertaintyTableElement::getValue(UncertaintyPair *result_dest) const {
	result_dest->value = value_;
	result_dest->uncertainty = uncertainty_;
}

double UncertaintyTableElement::getValue(void) const {
	return value_;
}

double UncertaintyTableElement::getUncertainty(void) const {
	return uncertainty_;
}

void UncertaintyTableElement::setCumulative(double value, double uncertainty) {
	cumulative_value_ = value;
	cumulative_uncertainty_ = uncertainty;
}

void UncertaintyTableElement::setCumulative(const UncertaintyPair *value) {
	cumulative_value_ = value->value;
	cumulative_uncertainty_ = value->uncertainty;
}

void UncertaintyTableElement::setCumulative(double value) {
	cumulative_value_ = value;
}

void UncertaintyTableElement::setCumulativeUncertainty(double value) {
	cumulative_uncertainty_ = value;
}

void UncertaintyTableElement::setValue(double value, double uncertainty) {
	value_ = value;
	uncertainty_ = uncertainty;
}

void UncertaintyTableElement::setValue(const UncertaintyPair *value) {
	value_ = value->value;
	uncertainty_ = value->uncertainty;
}

void UncertaintyTableElement::setValue(double value) {
	value_ = value;
}

void UncertaintyTableElement::setUncertainty(double value) {
	uncertainty_ = value;
}

void UncertaintyTableElement::setType(UncertaintyTableElementType type) {
	type_ = type;
}

void UncertaintyTableElement::setNotType(const UncertaintyTableElement &value) {
	value_ = value.value_;
	uncertainty_ = value.uncertainty_;
	cumulative_value_ = value.cumulative_value_;
	cumulative_uncertainty_ = value.cumulative_uncertainty_;
}

UncertaintyTable::UncertaintyTable(size_t starting_capacity) : elements_() {
	elements_.reserve(starting_capacity);
	elements_.emplace_back(UOPERATION_NUL, 0.0, 0.0);
}

size_t UncertaintyTable::getCapacity(void) const {
	return elements_.capacity();
}

void UncertaintyTable::getValue(size_t row, UncertaintyPair *result_dest) const {
	if (row >= elements_.size()) {
		result_dest->uncertainty = NAN;
		result_dest->value = NAN;
	} else {
		const UncertaintyTableElement &val = elements_[row];
		result_dest->value = val.getValue();
		result_dest->uncertainty = val.getUncertainty();
	}
}

double UncertaintyTable::getValue(size_t row) const {
	if (row >= elements_.size()) return NAN;
	return elements_[row].getValue();
}

double UncertaintyTable::getUncertainty(size_t row) const {
	if (row >= elements_.size()) return NAN;
	return elements_[row].getUncertainty();
}

const UncertaintyTableElement &UncertaintyTable::getElement(size_t row) const {
	if (row >= elements_.size()) return UncertaintyTableElement::invalid_element;
	return elements_[row];
}

void UncertaintyTable::add(UncertaintyTableElementType type, double value, double uncertainty) {
	elements_.emplace_back(type, value, uncertainty);
	compute(elements_.size() - 2);
}

void UncertaintyTable::add(UncertaintyTableElementType type, const UncertaintyPair *value) {
	elements_.emplace_back(type, value);
	compute(elements_.size() - 2);
}

void UncertaintyTable::remove(size_t row) {
	if (row < elements_.size() && row) {
		elements_.erase(elements_.begin() + row);
		compute(row - 1);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElementType type, double value, double uncertainty) {
	if (!row) return;
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, UncertaintyTableElement{type, value, uncertainty});
		compute(row - 1);
	} else {
		elements_.push_back(UncertaintyTableElement{type, value, uncertainty});
		compute(elements_.size() - 2);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElementType type, const UncertaintyPair *value) {
	if (!row) return;
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, UncertaintyTableElement{type, value});
		compute(row - 1);
	} else {
		elements_.push_back(UncertaintyTableElement{type, value});
		compute(elements_.size() - 2);
	}
}

void UncertaintyTable::swap(size_t row1, size_t row2) {
	if (!row1 || !row2 || row1 >= elements_.size() || row2 >= elements_.size()) return;
	UncertaintyTableElement el(elements_[row1]);
	elements_[row1] = elements_[row2];
	elements_[row2] = el;
	compute(row1 < row2 ? row1 - 1 : row2 - 1);
}

void UncertaintyTable::set(size_t row, const UncertaintyPair *value) {
	if (row >= elements_.size()) return;
	elements_[row].setValue(value);
	compute(row - 1);
}

void UncertaintyTable::set(size_t row, double value) {
	if (row >= elements_.size()) return;
	elements_[row].setValue(value);
	compute(row - 1);
}

void UncertaintyTable::set(size_t row, double value, double uncertainty) {
	if (row >= elements_.size()) return;
	elements_[row].setValue(value, uncertainty);
	compute(row - 1);
}

void UncertaintyTable::setUncertainty(size_t row, double uncertainty) {
	if (row >= elements_.size()) return;
	elements_[row].setUncertainty(uncertainty);
	compute(row - 1);
}

void UncertaintyTable::setStartingValue(double value, double uncertainty) {
	elements_.front().setValue(value, uncertainty);
	compute(0);
}

void UncertaintyTable::setStartingValue(double value) {
	elements_.front().setValue(value);
	compute(0);
}

void UncertaintyTable::setStartingValue(const UncertaintyPair *value) {
	elements_.front().setValue(value);
	compute(0);
}

void UncertaintyTable::setStartingUncertainty(double uncertainty) {
	elements_.front().setUncertainty(uncertainty);
	compute(0);
}

double UncertaintyTable::getStartingValue(void) const {
	return elements_.front().getValue();
}

double UncertaintyTable::getStartingUncertainty(void) const {
	return elements_.front().getUncertainty();
}

void UncertaintyTable::getStartingValue(UncertaintyPair *value_dest) const {
	elements_.front().getValue(value_dest);
}

size_t UncertaintyTable::count(void) const {
	return elements_.size();
}

UncertaintyTableElementType UncertaintyTable::getType(size_t row) {
	return elements_[row].getType();
}

void UncertaintyTable::add(const UncertaintyTableElement &element) {
	elements_.push_back(element);
	compute(elements_.size() - 2);
}

void UncertaintyTable::add(UncertaintyTableElement &&element) {
	elements_.push_back(element);
	compute(elements_.size() - 2);
}

void UncertaintyTable::addAt(size_t row, const UncertaintyTableElement &element) {
	if (!row) return;
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, element);
		compute(row - 1);
	} else {
		elements_.push_back(element);
		compute(elements_.size() - 2);
	}
}

void UncertaintyTable::addAt(size_t row, UncertaintyTableElement &&element) {
	if (!row) return;
	if (row < elements_.size()) {
		elements_.insert(elements_.begin() + row, element);
		compute(row - 1);
	} else {
		elements_.push_back(element);
		compute(elements_.size() - 2);
	}
}

void UncertaintyTable::set(size_t row, UncertaintyTableElement &&element) {
	if (row >= elements_.size()) return;
	if (!row) element.setType(UOPERATION_NUL);
	elements_[row] = element;
	compute(row - 1);
}

void UncertaintyTable::compute(size_t starting_row) {
	UncertaintyPair current_cumulative;
	auto begin = elements_.begin() + starting_row, end = elements_.end();
	if (begin >= end) return;
	begin->compute(&current_cumulative);
	for (begin = begin + 1; begin < end; ++begin) {
		if (isnan(current_cumulative.uncertainty) || isnan(current_cumulative.value)) {
			for ( ; begin < end; ++begin) {
				begin->setNotType(UncertaintyTableElement::invalid_element);
			}
			break;
		}
		begin->setCumulative(&current_cumulative);
		begin->compute(&current_cumulative);
	}
	result_ = current_cumulative;
}

void UncertaintyTable::getResult(UncertaintyPair *result_dest) const {
	result_dest->uncertainty = result_.uncertainty;
	result_dest->value = result_.value;
}

double UncertaintyTable::getResult(void) const {
	return result_.value;
}

double UncertaintyTable::getResultingUncertainty(void) const {
	return result_.uncertainty;
}
