#include "../include/nested_loop_join_executor.h"

NestedLoopJoinExecutor::NestedLoopJoinExecutor(
    AbstractExecutor *left_child_executor,
    AbstractExecutor *right_child_executor, const std::string join_key)
    : left_(left_child_executor),
      right_(right_child_executor),
      join_key_(join_key){};

void NestedLoopJoinExecutor::Init() {
  left_->Init();
  right_->Init();
  nextOuterTuple = true;
  innerTupleFound = false;
}

bool NestedLoopJoinExecutor::Next(Tuple *tuple) {
  if (nextOuterTuple || innerTupleFound) {
    if (innerTupleFound) {
      Tuple inner_tuple = *tuple;
      while (left_->Next(&inner_tuple)) {
        if (JoinKeyMatch(&inner_tuple, tuple)) {
          *tuple = Tuple(inner_tuple);
          nextOuterTuple = false;
          innerTupleFound = true;
          return true;
        }
      }
      left_->Init();
      nextOuterTuple = true;
      innerTupleFound = false;
      if (Next(tuple)) return true;
    } else {
      while (right_->Next(tuple)) {
        Tuple inner_tuple;
        while (left_->Next(&inner_tuple)) {
          if (JoinKeyMatch(&inner_tuple, tuple)) {
            *tuple = Tuple(inner_tuple);
            nextOuterTuple = false;
            innerTupleFound = true;
            return true;
          }
        }
        left_->Init();
        innerTupleFound = false;
      }
      nextOuterTuple = true;
      innerTupleFound = false;
    }
  }
  return false;
}

bool NestedLoopJoinExecutor::JoinKeyMatch(const Tuple *inner_tuple, const Tuple *outer_tuple) {
  if ("id" == join_key_ && inner_tuple->id == outer_tuple->id) return true;
  if ("val1" == join_key_ && inner_tuple->val1 == outer_tuple->val1) return true;
  if ("val2" == join_key_ && inner_tuple->val2 == outer_tuple->val2) return true;
  return false;
}