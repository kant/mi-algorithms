/*!
 * \file rbm.cpp
 * \brief Contains RBM methods declaration.
 * \author tkornuta
 * \date Dec 22, 2015
 */

#include <rbm/rbm.hpp>

#include <cmath>
#include <data_utils/RandomGenerator.hpp>

#include <data_utils/functions.h>

#include <logger/Log.hpp>
using namespace mic::logger;

namespace mic {
namespace rbm {

RBM::RBM(size_t n_visible, size_t n_hidden, size_t b_size)
{

	num_input = n_visible;
	num_output = n_hidden;
	batch_size = b_size;

	// parameters
	// W - weights between input and output units.
	W.resize(num_output, num_input);
	W_delta.resize(num_output, num_input);
	//sigma.resize(num_input, 1); // => Eigen::VectorXd
	b.resize(num_input); // => Eigen::VectorXd
	b_delta.resize(num_input); // => Eigen::VectorXd
	c.resize(num_output); // => Eigen::VectorXd
	c_delta.resize(num_output); // => Eigen::VectorXd

	//temp storage
	h.resize(num_output, batch_size);
	//H2.resize(num_output, batch_size);
	hn.resize(num_output, batch_size);
	H.resize(num_output, batch_size);
	rv.resize(batch_size, num_input);
	rh.resize(num_output, batch_size);

	hidmeans.resize(num_output); // => Eigen::VectorXd
	hidmeans_inc.resize(num_output); // => Eigen::VectorXd
	hidmeans_inc_rep.resize(num_output, batch_size);
	sparsegrads.resize(num_output, num_input);

	v.resize(batch_size, num_input);
	vn.resize(batch_size, num_input);
	pc.resize(batch_size, num_input);

	ve.resize(batch_size, num_input);

	posprods.resize(num_output, num_input);
	negprods.resize(num_output, num_input);

	// Reset matrices/vectors.
	W.setZero();
	W_delta.setZero();
	//sigma.setZero();
	b.setZero();
	b_delta.setZero();
	c.setZero();
	c_delta.setZero();
	h.setZero();
	hn.setZero();
	H.setZero();
	rv.setZero();
	rh.setZero();
	hidmeans.setZero();
	hidmeans_inc.setZero();
	hidmeans_inc_rep.setZero();
	sparsegrads.setZero();
	v.setZero();
	vn.setZero();
	pc.setZero();
	ve.setZero();
	posprods.setZero();
	negprods.setZero();

	// Initialize W with random variables from 0.0f to 0.001f (normal distribution mi=0, variance=1).
	W.normRandReal(0.0, 0.001);

	if (learning_type == LTYPE::PCD) {
		LOG(LERROR) << "PCD not implemented!";
	}

}

RBM::RBM(size_t n_visible, size_t n_hidden, NTYPE v_type, NTYPE h_type, LTYPE l_type, size_t b_size) :
		RBM(n_visible, n_hidden, b_size)
{
	visible_type = v_type;
	hidden_type = h_type;
	learning_type = l_type;
}

RBM::~RBM() {

}

void RBM::up(mic::types::MatrixXfPtr in) { // h given v
	LOG(LTRACE)<<"RBM::up";

	// Copy input matrix to "RBM visible units".
	v = (*in.get());

	// Flatten!
	v.resize(batch_size, num_input);

	// Compute hidden units activation h by multiplication of visible units v and weight matrix W.
	h = W * (v.transpose());

	h.matrixRowVectorFunction(&_add, c);

	// Sigmoid
	h.elementwiseFunction(&_sigmoid);

	// Multiply activation  by visible in order to compute ...?
	posprods = h * v;

}

void RBM::down() { // v given h
	LOG(LTRACE)<<"RBM::down";

	H = h;
	//rh.elementwise_function(&_rand);
	rh.uniRandReal(0.0, 1.0);
	H.elementwise_function_matrix(&_compare, rh);

	vn = (H.transpose()) * W;

	vn.matrix_column_vector_function(&_add, b);
	vn.elementwise_function(&_sigmoid);

	if (learning_type == LTYPE::CD) {

		hn = W * vn.transpose();
		hn.matrix_row_vector_function(&_add, c);
		hn.elementwise_function(&_sigmoid);

		negprods = hn * vn;

		pc = vn;

	} else {
		LOG(LERROR) << "PCD not implemented!";
	}

}

void RBM::compute_statistics() {
	LOG(LTRACE)<<"RBM::compute_statistics";

	// Compute reconstruction error.
	ve = vn - v;
	err = ve.norm();
	err *= err / batch_size;

	// Compute sum and normalize it by batch size and number of outputs.
	hsum = h.sum() / (batch_size * num_output);


}

void RBM::adapt(float alpha, float decay, float sparsecost, float sparsetarget, float sparsedamping) {
	LOG(LTRACE)<<"RBM::adapt";

	// Reset deltas.
	W_delta.setZero();
	b_delta.setZero();
	c_delta.setZero();


	// hidmeans = sparsedamping*hidmeans + (1-sparsedamping)*poshidact/numcases;
	hidmeans.elementwise_function_scalar(&_mult, sparsedamping);
	// hidmeans *= (float)sparsedamping;

	hidmeans_inc.sum_rows(h);
	//hidmeans_inc = h.rowwise().sum();

	hidmeans_inc.elementwise_function_scalar(&_mult, (1.0f - sparsedamping) / batch_size);
	//hidmeans_inc *= (float)((1.0f - sparsedamping) / batch_size);

	hidmeans.elementwise_function_vector(&_add, hidmeans_inc);
	//hidmeans += hidmeans_inc;

	// sparsegrads = sparsecost*(repmat(hidmeans,numcases,1)-sparsetarget);
	hidmeans_inc = hidmeans;

	hidmeans_inc.elementwise_function_scalar(&_sub, sparsetarget);
	//hidmeans_inc.array() -= sparsetarget;

	hidmeans_inc.elementwise_function_scalar(&_mult, sparsecost);
	//hidmeans_inc *= (float)sparsecost;

	MatrixXf::repmat(hidmeans_inc_rep, hidmeans_inc, batch_size);

	sparsegrads  = hidmeans_inc_rep * v;

	hidmeans_inc.sum_rows(sparsegrads);
	//hidmeans_inc = sparsegrads.rowwise().sum();

	// Compute W increment.
	//W_delta = (posprods - negprods) - sparsegrads;
	W_delta = posprods;
	W_delta.elementwise_function_matrix(&_sub, negprods);
	W_delta.elementwise_function_matrix(&_sub, sparsegrads);

	W_delta.elementwise_function_scalar(&_mult, alpha / batch_size);
	//W_delta *= (alpha / batch_size);

	// Compute b update.
	//b_delta = (v-pc).colwise().sum();
	//b_delta *= (alpha / batch_size);

	b_delta.diff_cols(v, pc);
	b_delta.elementwise_function_scalar(&_mult, alpha / batch_size);

	// Compute c update.
	//c_delta = (h-hn).rowwise().sum();
	//c_delta -= hidmeans_inc;
	//c_delta *= (alpha / batch_size);

	c_delta.diff_rows(h, hn);
	c_delta.elementwise_function_vector(&_sub, hidmeans_inc);
	c_delta.elementwise_function_scalar(&_mult, alpha / batch_size);

	// Apply weight updates.
	//W += W_delta;
	//W *= (1.0f - decay);
	//b += b_delta;
	//c += c_delta;

	W.elementwise_function_matrix(&_add, W_delta);
	W.elementwise_function_scalar(&_mult, 1.0f - decay);
	b.elementwise_function_vector(&_add, b_delta);
	c.elementwise_function_vector(&_add, c_delta);

}


void RBM::serialize(std::ostream& os) const {

	os << "[" << num_input << " x " << num_output << " Encoder]" << std::endl;
	os << W << std::endl;
}



} /* namespace rbm */
} /* namespace mic */


