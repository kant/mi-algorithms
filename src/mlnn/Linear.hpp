/*!
 * \file Linear.hpp
 * \brief 
 * \author tkornut
 * \date Mar 31, 2016
 */

#ifndef SRC_MLNN_LINEAR_HPP_
#define SRC_MLNN_LINEAR_HPP_

#include <mlnn/Layer.hpp>

namespace mic {
namespace mlnn {

/*!
 * \author krocki
 */
class Linear : public mic::mlnn::Layer {
public:
	Linear(size_t inputs, size_t outputs, size_t batch_size);

	virtual ~Linear() {};

	void forward(bool test = false);

	void backward();

	void resetGrads();

	void applyGrads(double alpha, double decay = 0);

protected:
	mic::types::MatrixXf W;
	mic::types::VectorXf b;

	mic::types::MatrixXf dW;
	mic::types::MatrixXf db;

	mic::types::MatrixXf mW;
	mic::types::MatrixXf mb;
};

} /* namespace mlnn */
} /* namespace mic */

#endif /* SRC_MLNN_LINEAR_HPP_ */
