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

#ifndef OptimT_MATRIXDYNAMICSIZE_H
#define OptimT_MATRIXDYNAMICSIZE_H

#include <stdint.h>
#include <memory>
#include <type_traits>

namespace OptimT
{
///col-major matrix with basic types only
template<class Scalar_t,class allocator_t=std::allocator<Scalar_t>>
class MatrixDynamicSize
{
public:
    MatrixDynamicSize() {
        rowNum=0;
        colNum=0;
        _capacity=0;
        dataPtr=nullptr;
    };

    MatrixDynamicSize(size_t r,size_t c) {
        rowNum=r;
        colNum=c;
        _capacity=r*c;
        dataPtr=alloc.allocate(_capacity);
        if(isClass)
            for(size_t i=0;i<_capacity;i++) {
                alloc.construct(dataPtr+i);
            }
    }

    ///Deep copy function
    explicit MatrixDynamicSize(const MatrixDynamicSize & src) {
        resize(src.rowNum,src.colNum);
        for(size_t i=0;i<size();i++) {
            dataPtr[i]=src.dataPtr[i];
        }
    }

    ///Deep copy
    const MatrixDynamicSize & operator=(const MatrixDynamicSize & src) {
        resize(src.rowNum,src.colNum);
        for(size_t i=0;i<size();i++) {
            dataPtr[i]=src.dataPtr[i];
        }
        return *this;
    }

    ~MatrixDynamicSize() {
        if(dataPtr!=nullptr) {
            alloc.deallocate(dataPtr,_capacity);
            if(isClass)
                for(size_t i=0;i<_capacity;i++) {
                    alloc.destroy(dataPtr+i);
                }
        }
    };

    using iterator = Scalar_t*;
    using citerator = const Scalar_t*;

    inline iterator begin() {
        return dataPtr;
    }

    inline iterator end() {
        return dataPtr+size();
    }

    inline size_t size() const {
        return rowNum*colNum;
    }

    void resize(size_t r,size_t c) {
        if(r*c!=size()) {
            if(r*c>_capacity) {
                if(dataPtr!=nullptr) {
                    if(isClass)
                        for(size_t i=0;i<_capacity;i++) {
                            alloc.destroy(dataPtr+i);
                        }
                    alloc.deallocate(dataPtr,_capacity);
                }
                dataPtr=alloc.allocate(r*c);
                _capacity=r*c;
                rowNum=r;
                colNum=c;
                for(size_t i=0;i<_capacity;i++) {
                    alloc.construct(dataPtr+i);
                }
            }
            else {
                rowNum=r;
                colNum=c;
            }
        }
        else {
            //conservative resize
            rowNum=r;
            colNum=c;
        }
    }

    inline size_t rows() const {
        return rowNum;
    }

    inline size_t cols() const {
        return colNum;
    }

    inline size_t capacity() const {
        return _capacity;
    }

    inline Scalar_t & operator()(size_t n) const {
        return dataPtr[n];
    }

    inline Scalar_t & operator()(size_t r,size_t c) const {
        return dataPtr[rowNum*c+r];
    }

    inline Scalar_t * data() {
        return dataPtr;
    }

    inline const Scalar_t * cdata() const {
        return dataPtr;
    }

protected:
    Scalar_t * dataPtr;
    size_t rowNum;
    size_t colNum;
    size_t _capacity;

private:
    static const bool isClass=std::is_class<Scalar_t>::value;
    static allocator_t alloc;

};



}

#endif // MATRIXDYNAMICSIZE_H
