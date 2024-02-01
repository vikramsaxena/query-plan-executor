#include "../include/hash_join_executor.h"

HashJoinExecutor::HashJoinExecutor(AbstractExecutor *left_child_executor,
                                   AbstractExecutor *right_child_executor,
                                   SimpleHashFunction *hash_fn)
    : left_(left_child_executor),
      right_(right_child_executor),
      hash_fn_(hash_fn) {}

void HashJoinExecutor::Init() {
  left_->Init();
  ht.clearHashTable();
  Tuple tuple;
  while (left_->Next(&tuple)) ht.Insert(hash_fn_->GetHash(tuple), tuple);
  right_->Init();
  performNextProbe = true;
  tupleIndex = 0;
}

bool HashJoinExecutor::Next(Tuple *tuple) {
  if (performNextProbe || 0 < tupleIndex) {
    if (0 < tupleIndex) {
      std::vector<Tuple> t;
      ht.GetValue(hash_fn_->GetHash(*tuple), &t);
      if (0 < t.size()) {
        if (tupleIndex < t.size()) {
          *tuple = t[tupleIndex++];
          performNextProbe = false;
          return true;
        }
        performNextProbe = true;
        tupleIndex = 0;
        if (Next(tuple)) return true;
      }
    } else {
      while (right_->Next(tuple)) {
        std::vector<Tuple> t;
        ht.GetValue(hash_fn_->GetHash(*tuple), &t);
        if (0 < t.size()) {
          if (tupleIndex < t.size()) {
            *tuple = t[tupleIndex++];
            performNextProbe = false;
            return true;
          }
          performNextProbe = true;
          tupleIndex = 0;
        }
      }
      performNextProbe = true;
      tupleIndex = 0;
    }
  }
  return false;
}
