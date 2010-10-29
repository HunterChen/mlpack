/** @file dense_point.h
 *
 *  An implementation of dense points.
 *
 *  @author Dongryeol Lee (dongryel@cc.gatech.edu)
 */

#ifndef CORE_TABLE_DENSE_POINT_H
#define CORE_TABLE_DENSE_POINT_H

#include <armadillo>
#include <boost/serialization/serialization.hpp>
#include "abstract_point.h"
#include "memory_mapped_file.h"

namespace core {
namespace table {

extern core::table::MemoryMappedFile *global_m_file_;

class DenseConstPoint: public core::table::AbstractPoint {
  protected:

    double *ptr_;

    int n_rows_;

  public:

    void Print() const {
      printf("Vector of length: %d\n", n_rows_);
      for(int i = 0; i < n_rows_; i++) {
        printf("%g ", ptr_[i]);
      }
      printf("\n");
    }

    const double *ptr() const {
      return ptr_;
    }

    virtual ~DenseConstPoint() {
      // A const point is always defined as an alias to a part of an
      // already-existing memory block, so you do not free it.
      Reset();
    }

    void Reset() {
      ptr_ = NULL;
      n_rows_ = 0;
    }

    DenseConstPoint() {
      Reset();
    }

    int length() const {
      return n_rows_;
    }

    double operator[](int i) const {
      return ptr_[i];
    }

    void Alias(double *ptr_in, int length_in) {
      ptr_ = ptr_in;
      n_rows_ = length_in;
    }

    void Alias(const DenseConstPoint &point_in) {
      ptr_ = const_cast<double *>(point_in.ptr());
      n_rows_ = point_in.length();
    }
};

class DensePoint: public DenseConstPoint {

  private:

    friend class boost::serialization::access;

    bool is_alias_;

  private:
    void DestructPtr_() {
      if(DenseConstPoint::ptr_ != NULL && is_alias_ == false) {
        if(core::table::global_m_file_) {
          core::table::global_m_file_->Deallocate(ptr_);
        }
        else {
          delete[] DenseConstPoint::ptr_;
        }
      }
    }

  public:

    template<class Archive>
    void save(Archive &ar, const unsigned int version) const {

      // First the length of the point.
      ar & n_rows_;
      for(int i = 0; i < n_rows_; i++) {
        double element = ptr_[i];
        ar & element;
      }
    }

    template<class Archive>
    void load(Archive &ar, const unsigned int version) {
      // Load the length of the point.
      int length;
      ar & length;

      // Allocate the point.
      ptr_ = (core::table::global_m_file_) ?
             (double *)
             core::table::global_m_file_->Allocate(sizeof(double) * length) :
             new double[length];
      for(int i = 0; i < length; i++) {
        ar & (ptr_[i]);
      }
    }
    BOOST_SERIALIZATION_SPLIT_MEMBER()

    void Reset() {
      DenseConstPoint::Reset();
      is_alias_ = false;
    }

    DensePoint() {
      Reset();
    }

    virtual ~DensePoint() {
      DestructPtr_();
      Reset();
    }

    double &operator[](int i) {
      return DenseConstPoint::ptr_[i];
    }

    void Init(int length_in) {
      DenseConstPoint::ptr_ =
        (core::table::global_m_file_) ?
        (double *)
        core::table::global_m_file_->Allocate(sizeof(double) * length_in) :
        new double[length_in];
      DenseConstPoint::n_rows_ = length_in;
      is_alias_ = false;
    }

    void Init(const std::vector<double> &vector_in) {
      DenseConstPoint::ptr_ =
        (core::table::global_m_file_) ?
        (double *)
        core::table::global_m_file_->Allocate(
          sizeof(double) * vector_in.size()) :
        new double[vector_in.size()];
      DenseConstPoint::n_rows_ = vector_in.size();
      for(unsigned int i = 0; i < vector_in.size(); i++) {
        ptr_[i] = vector_in[i];
      }
      is_alias_ = false;
    }

    void CopyValues(const DenseConstPoint &point_in) {
      memcpy(
        DenseConstPoint::ptr_, point_in.ptr(),
        sizeof(double) * point_in.length());
      DenseConstPoint::n_rows_ = point_in.length();
      is_alias_ = false;
    }

    void Copy(const DenseConstPoint &point_in) {
      DestructPtr_();
      DenseConstPoint::ptr_ =
        (core::table::global_m_file_) ?
        (double *) core::table::global_m_file_->Allocate(
          sizeof(double) * point_in.length()) :
        new double[point_in.length()];
      CopyValues(point_in);
    }

    void SetZero() {
      memset(ptr_, 0, sizeof(double) * n_rows_);
    }

    void Alias(double *ptr_in, int length_in) {
      DenseConstPoint::ptr_ = ptr_in;
      DenseConstPoint::n_rows_ = length_in;
      is_alias_ = true;
    }

    void Add(
      double scale_factor, const core::table::DenseConstPoint &point_in) {
      for(int i = 0; i < point_in.length(); i++) {
        ptr_[i] += scale_factor * point_in[i];
      }
    }

    void operator+=(const core::table::DenseConstPoint &point_in) {
      for(int i = 0; i < point_in.length(); i++) {
        ptr_[i] += point_in[i];
      }
    }

    void operator/=(double scale_factor) {
      for(int i = 0; i < DenseConstPoint::n_rows_; i++) {
        ptr_[i] /= scale_factor;
      }
    }

    void operator*=(double scale_factor) {
      for(int i = 0; i < DenseConstPoint::n_rows_; i++) {
        ptr_[i] *= scale_factor;
      }
    }
};
};
};

#endif
