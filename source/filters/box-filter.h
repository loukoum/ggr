#pragma once

class BoxFilter;

#include "filters/filter.h"

class BoxFilter : public Filter {
public:
	BoxFilter();
	BoxFilter(const BoxFilter& other);
	BoxFilter(float radius);

	float evaluate1D(float centeredSamplePoint) const;
	float evaluate2D(const Point2& centeredSamplePoint) const;
	Sample2D sample(const Sample2D& sample) const;
	std::unique_ptr<Filter> clone() const;
};

