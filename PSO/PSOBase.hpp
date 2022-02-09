/*
 Copyright © 2022  TokiNoBug
This file is part of OptimTemplates.

    OptimTemplates is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OptimTemplates is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OptimTemplates.  If not, see <https://www.gnu.org/licenses/>.

*/

#ifndef OptimT_PSOBASE_HPP
#define OptimT_PSOBASE_HPP

#include "PSOOption.hpp"
#include "PSOAbstrcat.hpp"

namespace OptimT {

///Abstrcat base class for most PSO solvers
template<class Var_t,size_t DIM,class Fitness_t,RecordOption Record,class Arg_t=void>
class PSOBase : public PSOAbstract<Var_t,Fitness_t,Record,Arg_t>
{
public:
    using Base_t = PSOAbstract<Var_t,Fitness_t,Record,Arg_t>;
    OptimT_MAKE_PSOABSTRACT_TYPES

public:
    PSOBase() {};
    virtual ~PSOBase() {};

    static constexpr size_t dimensions() {
        return DIM;
    }

protected:
    static const size_t dims=DIM;

};

///partial specialization for PSOBase with dynamic dimensions
template<class Var_t,class Fitness_t,RecordOption Record,class Arg_t>
class PSOBase<Var_t,Dynamic,Fitness_t,Record,Arg_t>
        : public PSOAbstract<Var_t,Fitness_t,Record,Arg_t>
{
public:
    using Base_t = PSOAbstract<Var_t,Fitness_t,Record,Arg_t>;
    OptimT_MAKE_PSOABSTRACT_TYPES

    inline size_t dimensions() const {
        return dims;
    }

    inline void setDimensions(size_t d) {
        dims=d;
    }

protected:
    size_t dims;

};

}
#endif // PSOBASE_HPP
