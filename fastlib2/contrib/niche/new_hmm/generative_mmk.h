#ifndef GENERATIVE_MMK_H
#define GENERATIVE_MMK_H

#include "multinomial.h"
#include "isotropic_gaussian.h"
#include "hmm.h"
#include "mlpack/kde/bandwidth_lscv.h"
#include "mlpack/kde/dataset_scaler.h"
#include "mlpack/kde/dualtree_kde.h"
#include "mlpack/kde/naive_kde.h"

#define INSIDE_GENERATIVE_MMK_IMPL_H

double GenerativeMMK(double lambda,
		     const Multinomial &x,
		     const Multinomial &y);

double GenerativeMMK(double lambda,
		     const IsotropicGaussian &x,
		     const IsotropicGaussian &y);

template <typename TDistribution>
double GenerativeMMK(double lambda, int n_T,
		     const HMM<TDistribution> &hmm_a,
		     const HMM<TDistribution> &hmm_b);

double GenerativeMMK(double lambda, double rho, int n_T,
		     const HMM<Multinomial> &hmm_a,
		     const HMM<Multinomial> &hmm_b);

template <typename TDistribution>
void GenerativeMMKBatch(double lambda, int n_T,
			const ArrayList<HMM<TDistribution> > &hmms,
			Matrix *p_kernel_matrix);

void GenerativeMMKBatch(double lambda, double rho, int n_T,
			const ArrayList<HMM<Multinomial> > &hmms,
			Matrix *p_kernel_matrix);

double KDEGenerativeMMK(double lambda,
			const Matrix &samples1,
			const Matrix &samples2);

void KDEGenerativeMMKBatch(double lambda,
			   const ArrayList<Matrix> &samplings,
			   Matrix *p_kernel_matrix);

void ScaleSamplingsToCube(ArrayList<Matrix> *p_samplings);



#include "generative_mmk_impl.h"
#undef INSIDE_GENERATIVE_MMK_IMPL_H

#endif /* GENERATIVE_MMK_H */
