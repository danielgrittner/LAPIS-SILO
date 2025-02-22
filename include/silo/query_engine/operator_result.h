#ifndef SILO_OPERATOR_RESULT_H
#define SILO_OPERATOR_RESULT_H

#include <type_traits>

#include <roaring/roaring.hh>

namespace silo::query_engine {

/// The return value of the Operator::evaluate method.
/// May return either a mutable or immutable bitmap.
struct OperatorResult {
  private:
   roaring::Roaring* mutable_bitmap;
   const roaring::Roaring* immutable_bitmap;

  public:
   explicit OperatorResult();
   explicit OperatorResult(const roaring::Roaring& bitmap);
   explicit OperatorResult(roaring::Roaring&& bitmap);

   // rule of five for manual memory management
   ~OperatorResult();
   OperatorResult(const OperatorResult& other) = delete;
   OperatorResult(OperatorResult&& other) noexcept;
   OperatorResult& operator=(const OperatorResult& other) = delete;
   OperatorResult& operator=(OperatorResult&& other) noexcept;

   roaring::Roaring& operator*();
   const roaring::Roaring& operator*() const;
   roaring::Roaring* operator->();
   const roaring::Roaring* operator->() const;

   bool isMutable() const;
};

}  // namespace silo::query_engine

#endif  // SILO_OPERATOR_RESULT_H
