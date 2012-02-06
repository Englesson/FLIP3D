#ifndef UNCONDITIONED_CG_SOLVER_H
#define UNCONDITIONED_CG_SOLVER_H

#include "Sparse_Matrix.h"
#include "Array3D.h"
#include <cmath>

struct Uncondioned_CG_Solver
{
	VectorN d; //Search vector
	VectorN r;
	VectorN tmp;
	double beta, alpha;
		
	Uncondioned_CG_Solver() {}

	Uncondioned_CG_Solver(int dimx, int dimy, int dimz)
	{
		init(dimx,dimy,dimz);
	}

	void init(int dimx, int dimy, int dimz)
	{
		d.init(dimx, dimy, dimz);
		r.init(dimx, dimy, dimz);
		tmp.init(dimx, dimy, dimz);
	}

	void solve(const Sparse_Matrix & A,const VectorN & b,int maxiterations, double tol, VectorN & x, Array3c & marker);

};

void Uncondioned_CG_Solver::solve(const Sparse_Matrix & A,const VectorN & b,int maxiterations, double tol, VectorN & x, Array3c &marker)
{
	b.copy_to(r);
	double rnorm = vectorN_norm2(r); //vectorN_norm2 WORKS!!! checked with matlab
	if(std::sqrt(rnorm) < tol)
		return;


	x.zero();
	tmp.zero();
	b.copy_to(d);
	int i = 0;
	double rnextnorm = 0;
	write_system_to_matlab(A,b,marker); //Debug

	while(true)
	{
		//Calc alpha(i): alpha(i) = dot(r,r) / dot( d(i), A*d(i) );
		mtx_mult_vectorN(A,d,tmp,marker); //Seems to work
		//tmp == A*d(i)
		alpha = rnorm/vectorN_dot(d,tmp); //vectorN_dot might be wrong
		
		//Calc new position x(i+1): x(i+1) = x(i) + alpha(i)*d(i);
		vectorN_add_scale(x, d, alpha); //void
		i++;

		if(std::sqrt(rnorm) < tol || i == maxiterations)
		{
			std::cout << std::scientific;
			std::cout << "CG: " << i << " iterations, " << "norm_squared = " << rnorm <<  "\n";
			x.write_file_to_matlab("x.txt",marker);
			break;
		}

		//----------------------------------------------------------------------------//
		// Calculate new residual and search vector
		//----------------------------------------------------------------------------//

		//Calc new residual r(i+1) = r(i) - alpha(i)*A*d(i);
		vectorN_sub_scale(r, tmp, alpha); // tmp = A*d(i)
		rnextnorm = vectorN_norm2(r); //r = r(i+1)

		//Calc beta(i+1) = dot(r(i+1),r(i+1)) / dot(r(i),r(i));
		beta = rnextnorm / rnorm;

		//Calc new search vector d(i+1) = r(i+1) + beta(i+1)*d(i);
		vectorN_scale_add(d, r, beta);
		rnorm = rnextnorm;
	}
}









#endif