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

#ifndef NSGA2_HPP
#define NSGA2_HPP

#include "./NSGA2Base.hpp"
#include <type_traits>
namespace OptimT
{

#ifndef EIGEN_CORE_H    //Detects whether libEigen is included
#ifdef OptimT_GENETIC_USE_EIGEN     //If user hopes to use Eigen without including it, report an error
#error You must include Eigen before you define OptimT_GENETIC_USE_EIGEN! Include Eigen before OptimT.
#endif
#endif

/**
   *  @brief NSGA2 MOGA solver. Suitable for not too many objectives.
   *
   *  @tparam Var_t  Type of decisition variable.
   *  @tparam ObjNum Numbers of objectives.
   *  @tparam Fitness_t Type of fitness value.
   *  @tparam fOpt Whether greater fitness value means better.
   *  @tparam rOpt Whether the solver records fitness changelog.
   *  @tparam pfOpt Whether to protect the Pareto front from mutation.
   *  @tparam ...Args Type of other parameters.
  */
template<typename Var_t,
         size_t ObjNum,
         DoubleVectorOption DVO,
         FitnessOption isGreaterBetter,
         RecordOption Record,
         PFOption ProtectPF,
         class ...Args>
class NSGA2
    : public NSGA2Base<Var_t,
                    ObjNum,
                    stdVecD_t<ObjNum>,
                    isGreaterBetter,
                    Record,
                    ProtectPF,
                    Args...>
{
public:
    using Base_t = NSGA2Base<Var_t,
                    ObjNum,
                    stdVecD_t<ObjNum>,
                    isGreaterBetter,
                    Record,
                    ProtectPF,
                    Args...>;
    using Fitness_t = stdVecD_t<ObjNum>;
    OptimT_MAKE_GABASE_TYPES

    NSGA2() {
    };

    virtual ~NSGA2() {};

protected:

private:
};




#ifdef OptimT_GENETIC_USE_EIGEN

/**
 * @brief Partial specialization for NSGA2 using Eigen's array
 * 
 * @tparam Var_t 
 * @tparam ObjNum 
 * @tparam isGreaterBetter 
 * @tparam Record 
 * @tparam ProtectPF 
 * @tparam Args 
 */
template<typename Var_t,
         size_t ObjNum,
         FitnessOption isGreaterBetter,
         RecordOption Record,
         PFOption ProtectPF,
         class ...Args>
class NSGA2<Var_t,ObjNum,DoubleVectorOption::Eigen,isGreaterBetter,Record,ProtectPF,Args...>
    : public NSGA2Base<Var_t,
                    ObjNum,
                    EigenVecD_t<ObjNum>,
                    isGreaterBetter,
                    Record,
                    ProtectPF,
                    Args...>
{
public:
    using Base_t =NSGA2Base<Var_t,
                    ObjNum,
                    EigenVecD_t<ObjNum>,
                    isGreaterBetter,
                    Record,
                    ProtectPF,
                    Args...>;
    using Fitness_t = EigenVecD_t<ObjNum>;
    OptimT_MAKE_GABASE_TYPES

    using congestComposeFun = typename Base_t::congestComposeFun;
    using infoUnit = typename Base_t::infoUnit;

    NSGA2() {
        setCongestComposeFun();
    };

    virtual ~NSGA2() {};

    void setCongestComposeFun(congestComposeFun __ccFun=eigen_ccFun_liner) {
            this->_ccFun=__ccFun;
    }

    static double eigen_ccFun_liner(const Fitness_t * f,const ArgsType*) {
        return f->sum();
    };

    static double eigen_ccFun_sphere(const Fitness_t * f,const ArgsType*) {
        return std::sqrt(f->square().sum());
    }

    static double eigen_ccFun_max(const Fitness_t * f,const ArgsType*) {
        return f->maxCoeff();
    }

    template<int64_t p>
    static double eigen_ccFun_powered(const Fitness_t * f,const ArgsType*) {
        return std::pow(f->power(p).sum(),1.0/p);
    }

    virtual Fitness_t bestFitness() const {
        Fitness_t best=Base_t::_population.front()._Fitness;
        for(const Gene & i : Base_t::_population) {
            if(isGreaterBetter) {
                best=best.max(i._Fitness);
            } else {
                best=best.min(i._Fitness);
            }
        }
        return best;
    }

protected:

    virtual void customOptWhenInitialization() {
        this->prevFrontSize=-1;
        this->_pfGenes.clear();
        this->_pfGenes.reserve(Base_t::_option.populationSize*2);
        if(this->_ccFun==nullptr) {
            setCongestComposeFun();
        }
    }

    ///whether A strong domainates B
    static bool isStrongDomain(const Fitness_t * A,const Fitness_t * B) {
        if(isGreaterBetter) {
            return (*A>*B).all();
        } else {
            return (*A<*B).all();
        }
    } //isStrongDomain

    virtual void calculateDominatedNum(std::vector<infoUnit> & pop) const {
        const size_t popSizeBefore=pop.size();
        //calculate domainedByNum
#ifdef OptimT_NSGA2_DO_PARALLELIZE
        static const size_t thN=OtGlobal::threadNum();
#pragma omp parallel for
        for(size_t begIdx=0;begIdx<thN;begIdx++) {

            for(size_t ed=begIdx;ed<popSizeBefore;ed+=thN) {
                pop[ed].domainedByNum=0;
                for(size_t er=0;er<popSizeBefore;er++) {
                    if(er==ed)
                        continue;
                    pop[ed].domainedByNum+=
                            isStrongDomain(&(pop[er].iterator->_Fitness),
                                           &(pop[ed].iterator->_Fitness));
                }
            }
        }

#else
        for(size_t ed=0;ed<popSizeBefore;ed++) {
            pop[ed].domainedByNum=0;
            for(size_t er=0;er<popSizeBefore;er++) {
                if(er==ed)
                    continue;
                pop[ed].domainedByNum+=
                        isStrongDomain(&(pop[er].iterator->_Fitness),
                                       &(pop[ed].iterator->_Fitness));
            }
        }
#endif
    }

private:

};

#endif  // OptimT_GENETIC_USE_EIGEN
}

#endif // NSGA2_HPP
