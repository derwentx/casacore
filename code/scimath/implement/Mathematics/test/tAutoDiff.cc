//# tAutoDiff.cc: test program for AutoDiff
//# Copyright (C) 1995,1996
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <trial/Mathematics/AutoDiff.h>
#include <trial/Mathematics/AutoDiffMath.h>
#include <trial/Mathematics/AutoDiffIO.h>

#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/Lattices/IPosition.h>

#include <iostream.h>

Int main()
{
  uInt nerr = 0;
   // try {

	// test the constructors
	{
	    AutoDiff<Float> a;
	    if (a.value() != 0 || a.derivatives().nelements() != 0) {
		cerr << "AutoDiff<Float> a; failed a = " << a << endl;
		nerr++;
	    }
   
	    AutoDiff<Float> b(1.0);
	    if (b.value() != 1.0 || b.derivatives().nelements() != 0) {
		cerr << "AutoDiff<Float> b(1.0); failed b = " << b << endl;
		nerr++;
	    }

	    Vector<Float> g(3);
	    g = 0.0;
	    g(1) = 1.0;
	    AutoDiff<Float> x(2.0, 3, 1);
	    if (x.value() != 2.0 || ! allEQ(x.derivatives().ac(),g.ac())) {
		cerr << "AutoDiff<Float> x(2.0, 3, 1); failed x = " << x << endl;
		nerr++;
	    }

	    AutoDiff<Float> y(x);
	    if (y.value() != x.value() || ! allEQ(y.derivatives().ac(),x.derivatives().ac())) {
		cerr << "AutoDiff<Float> y(x); failed y = " << y << " x = " << x << endl;
		nerr++;
	    }
 
	    g(0) = 1.0;
	    g(1) = -1.0;
	    g(2) = 0.5;
	    Float val = 5.0;
	    AutoDiff<Float> z(val, g);
	    if (z.value() != val || ! allEQ(z.derivatives().ac(),g.ac())) {
		cerr << "AutoDiff<Float> z(val, g); failed z = " << z 
		     << " val = " << val << " g = " << g.ac() << endl;
		nerr++;
	    }
	}

	// test the assignment operators
	{
	    AutoDiff<Float> x(3.0,1,0);
	    x = 1.0;
	    if (x.value() != 1.0 || x.derivatives().nelements()!=0) {
		cerr << "assigntment to constant failed x : " << x << endl;
		nerr++;
	    }

	    AutoDiff<Float> y(2.0, 3, 1);
	    x = y;
	    if (x.value() != y.value() || ! allEQ(x.derivatives().ac(), y.derivatives().ac())) {
		cerr << "assignment to other failed x : " << x << " y : " << y << endl;
		nerr++;
	    }
	}

	// test the class member operators
	{
	  AutoDiff<Float> x(3.0,2,0);
	  AutoDiff<Float> y(2.0,2,1);
	  AutoDiff<Float> z;
	  z = x;
	  z *= y;
	  // verify result
	  if (z.value() != (x.value() * y.value()) ||
	      z.derivatives()(0) != y.value() ||
	      z.derivatives()(1) != x.value()) {
	    cerr << "*= operator failed" << endl;
	    nerr++;
	  }

	  z = x;
	  z /= y;
	  // verify result
	  if (z.value() != (x.value() / y.value()) ||
	      z.derivatives()(0) != (1.0/y.value()) ||
	      z.derivatives()(1) != (-x.value()/(y.value()*y.value()))) {
	    cerr << "/= operator failed" << endl;
	    nerr++;
	  }

	  z = x;
	  z += y;
	  // verify result
	  if (z.value() != (x.value() + y.value()) ||
	      z.derivatives()(0) != 1 ||
	      z.derivatives()(1) != 1) {
	    cerr << "+= operator failed" << endl;
	    nerr++;
	  }

	  z = x;
	  z -= y;
	  // verify result
	  if (z.value() != (x.value() - y.value()) ||
	      z.derivatives()(0) != 1 ||
	      z.derivatives()(1) != -1) {
	    cerr << "-= operator failed" << endl;
	    nerr++;
	  }
	}

	// other class members
	{
	  AutoDiff<Float> x;
	  if (x.nDerivatives() != 0) {
	    cerr << "wrong number of elements, should be 0" << endl;
	    nerr++;
	  }
	  if (!x.isConstant()) {
	    cerr << "x should be const, isConstant reports False" << endl;
	    nerr++;
	  }
	  AutoDiff<Float> y(1.0,1,0);
	  y.resize(3);
	  if (y.nDerivatives() != 3) {
	    cerr << "resize failed" << endl;
	    nerr++;
	  }
	  Vector<Float> grad(3);
	  grad(0) = 1.;
	  grad(1) = 2.;
	  grad(2) = 3.;
	  y.derivatives() = grad;
	  if (!allEQ(y.derivatives().ac(),grad.ac())) {
	    cerr << "derivatives assignment failed" << endl;
	    nerr++;
	  }
	  y.value() = 4.0;
	  if (y.value() != 4.0) {
	    cerr << "value assignment failed" << endl;
	    nerr++;
	  }
	  if (y.isConstant()) {
	    cerr << "y should not be const, isConstant reports True" << endl;
	    nerr++;
	  }
	}

	// AutoDIffMath tests
	{
	  AutoDiff<Float> x(3.0,1,0);
	  AutoDiff<Float> y;

	  y = +x;
	  if (y.value() != x.value() ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac())) {
	    cerr << "operator+(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = -x;
	  if (y.value() != -x.value() ||
	      !allEQ(y.derivatives().ac(), -x.derivatives().ac())) {
	    cerr << "operator-(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }
	  
	  y = x + x;
	  if (y.value() != (Float(2.0) * x.value()) ||
	      !allEQ(y.derivatives().ac(), Float(2.0) * x.derivatives().ac())) {
	    cerr << "operator+(const AutoDiff<T> &, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }
	  y = x - x;
	  if (y.value() != 0.0 ||
	      !allEQ(y.derivatives().ac(), Float(0.0))) {
	    cerr << "operator-(const AutoDiff<T> &, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }
	  y = x * x;
	  if (y.value() != (x.value() * x.value()) ||
	      !allEQ(y.derivatives().ac(), Float(2.0) * x.value() * x.derivatives().ac())) {
	    cerr << "operator*(const AutoDiff<T> &, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = x / x;
	  if (y.value() != 1 ||
	      !allEQ(y.derivatives().ac(), Float(0.0))) {
	    cerr << "operator/(const AutoDiff<T> &, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = x + Float(1.0);
	  if (y.value() != (x.value() + Float(1.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac())) {
	    cerr << "operator+(const AutoDiff<T> &,const T&) failed" << endl;
	    nerr++;
	  }

	  y = x - Float(1.0);
	  if (y.value() != (x.value() - Float(1.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac())) {
	    cerr << "operator-(const AutoDiff<T> &,const T&) failed" << endl;
	    nerr++;
	  }

	  y = x * Float(2.0);
	  if (y.value() != (x.value() * Float(2.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac()*Float(2.0))) {
	    cerr << "operator*(const AutoDiff<T> &,const T&) failed" << endl;
	    nerr++;
	  }

	  y = x / Float(2.0);
	  if (y.value() != (x.value() / Float(2.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac()/Float(2.0))) {
	    cerr << "operator/(const AutoDiff<T> &,const T&) failed" << endl;
	    nerr++;
	  }

	  y = Float(1.0) + x;
	  if (y.value() != (x.value() + Float(1.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac())) {
	    cerr << "operator+(,const T&, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = Float(1.0) - x;
	  if (y.value() != (Float(1.0) - x.value()) ||
	      !allEQ(y.derivatives().ac(), -x.derivatives().ac())) {
	    cerr << "operator-(const T&, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = Float(2.0) * x;
	  if (y.value() != (x.value() * Float(2.0)) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac()*Float(2.0))) {
	    cerr << "operator*(const T&, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  y = Float(2.0) / x;
	  if (y.value() != (Float(2.0) / x.value()) ||
	      !allEQ(y.derivatives().ac(), -x.derivatives().ac()*Float(2.0)/(x.value()*x.value()))) {
	    cerr << "operator/(const T&, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // trancendentals
	  x.value() = 0.5;
	  // acos(x) : derivative = -1/sqrt(1-x*x)
	  y = acos(x);
	  if (y.value() != Float(acos(x.value())) ||
	      !allEQ(y.derivatives().ac(),
		     -x.derivatives().ac()/Float(sqrt(1.0 - x.value()*x.value())))) {
	    cerr << "acos(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // asin(x) : derivative = 1/sqrt(1-x*x)
	  y = asin(x);
	  if (y.value() != Float(asin(x.value())) ||
	      !allEQ(y.derivatives().ac(),
		     x.derivatives().ac()/Float(sqrt(1.0 - x.value()*x.value())))) {
	    cerr << "asin(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // atan(x) : derivative = 1/(1+x*x)
	  y = atan(x);
	  if (y.value() != Float(atan(x.value())) ||
	      !allEQ(y.derivatives().ac(), 
		     x.derivatives().ac()/Float(1.0 + x.value()*x.value()))) {
	    cerr << "atan(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // atan2(x, y) : derivative = d(atan(x/y))
	  //                          = (1/(1+(x/y)*(x/y))) * (dx/y - x*dy/y**2)
	  AutoDiff<Float> w(3.0, 2, 0);
	  AutoDiff<Float> z(2.5, 2, 1);
	  y = atan2(w, z);
	  if (y.value() != Float(atan2(w.value(), z.value())) ||
	      !allEQ(y.derivatives().ac(), 
		     (w.derivatives().ac()/z.value() - 
		      w.value()*z.derivatives().ac()/(z.value()*z.value())) / 
		     Float(1.0 + w.value()*w.value()/(z.value()*z.value())))) {
	    cerr << "atan2(const AutoDiff<T> &, const AutoDiff<T> &g) failed" << endl;
	    nerr++;
	  }

	  // cos(x) : derivative = -sin(x)
	  y = cos(x);
	  if (y.value() != Float(cos(x.value())) ||
	      !allEQ(y.derivatives().ac(),-Float(sin(x.value()))*x.derivatives().ac())) {
	    cerr << "cos(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // cosh(x) : derivative = sinh(x)
	  y = cosh(x);
	  if (y.value() != Float(cosh(x.value())) ||
	      !allEQ(y.derivatives().ac(), Float(sinh(x.value()))*x.derivatives().ac())) {
	    cerr << "cosh(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // exp(x) : derivative = exp(x)
	  y = exp(x);
	  if (y.value() != Float(exp(x.value())) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac() * Float(exp(x.value())))) {
	    cerr << "exp(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // log(x) : derivative = 1/x
	  y = log(x);
	  if (y.value() != Float(log(x.value())) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac() / x.value())) {
	    cerr << "log(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // log10(x) : derivative = (1/x) / log(10)
	  y = log10(x);
	  if (y.value() != Float(log10(x.value())) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac() / Float((x.value()*log(10))))) {
	    cerr << "log10(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // pow(x,y) : derivative = y*pow(x,y-1)*dx + pow(x,y)*log(x)*dy
	  y = pow(w,z);
	  if (y.value() != Float(pow(w.value(), z.value()))
 ||
	      !allEQ(y.derivatives().ac(),
		     (Float(z.value()*pow(w.value(),z.value()-1))*w.derivatives().ac() +
		      Float(pow(w.value(),z.value())*log(w.value()))*z.derivatives().ac()))) {
	    cerr << "pow(const AutoDiff<T> &, const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // pow(x,const) : derivative = const*pow(x,const-1)*dx
	  y = pow((AutoDiff<Float>&)x,Float(2.5));
	  if (y.value() != Float(pow(x.value(),2.5)) ||
	      !allEQ(y.derivatives().ac(), 
		     Float(2.5*pow(x.value(),1.5))*x.derivatives().ac())) {
	    cerr << "pow(const AutoDiff<T> &, const double &) failed" << endl;
	    nerr++;
	  }

	  // sin(x) : derivative = cos(x)
	  y = sin(x);
	  if (y.value() != Float(sin(x.value())) ||
	      !allEQ(y.derivatives().ac(), Float(cos(x.value()))*x.derivatives().ac())) {
	    cerr << "sin(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // sinh(x) : derivative = cosh(x)
	  y = sinh(x);
	  if (y.value() != Float(sinh(x.value())) ||
	      !allEQ(y.derivatives().ac(), Float(cosh(x.value()))*x.derivatives().ac())) {
	    cerr << "sinh(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // sqrt(x) : derivative = 0.5/sqrt(x)
	  y = sqrt(x);
	  if (y.value() != Float(sqrt(x.value())) ||
	      !allEQ(y.derivatives().ac(), x.derivatives().ac()*Float(0.5/sqrt(x.value())))) {
	    cerr << "sqrt(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // tan(x) : derivative = sec(x)*sec(x) = 1/(cos(x)*cos(x))
	  y = tan(x);
	  if (y.value() != Float(tan(x.value())) ||
	      !allEQ(y.derivatives().ac(), 
		     x.derivatives().ac()/Float(cos(x.value())*cos(x.value())))) {
	    cerr << "tan(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	  // tanh(x) : derivative = sech(x)*sech(x) = 1/(cosh(x)*cosh(x))
	  y = tanh(x);
	  if (y.value() !=Float(tanh(x.value())) ||
	      !allEQ(y.derivatives().ac(), 
		     x.derivatives().ac()/Float(cosh(x.value())*cosh(x.value())))) {
	    cerr << "sinh(const AutoDiff<T> &) failed" << endl;
	    nerr++;
	  }

	}
//    } catch (AipsError x) {
//      cout << "Exception : " << x.getMesg() << endl;
//    } end_try;
    if (nerr != 0) {
      cout << "There were " << nerr << " errors" << endl;
    } else {
      cout << "ok" << endl;
    }
    return nerr;
}

