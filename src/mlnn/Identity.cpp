/*!
 * \file Identity.cpp
 * \brief 
 * \author tkornut
 * \date Mar 31, 2016
 */

#include <mlnn/Identity.hpp>

namespace mic {
namespace mlnn {


Identity::Identity(size_t inputs, size_t outputs, size_t batch_size) :
	Layer(inputs, outputs, batch_size, "identity") {

}

void Identity::forward(bool test) {

	y = x;

}

void Identity::backward() {

	dx = dy;

}

} /* namespace mlnn */
} /* namespace mic */
