#include "../include/aggregation_executor.h"

AggregationExecutor::AggregationExecutor(AbstractExecutor *child_executor,
                                         AggregationType aggr_type)
    : child_(child_executor), aggr_type_(aggr_type){};

void AggregationExecutor::Init() { child_->Init(); }

bool AggregationExecutor::Next(Tuple *tuple) {
    int count = 0, sum = 0, max = 0, min = 0;
    while (child_->Next(tuple)) {
        count++;
        sum += tuple->val1;
        if (max < tuple->val1) {
            max = tuple->val1;
        }
        if (min > tuple->val1) {
            min = tuple->val1;
        }
    }
    if (0 < count) {
        tuple->id = 0;
        switch (aggr_type_) {
            case AggregationType::COUNT:
                tuple->val1 = count;
                tuple->val2 = "COUNT";
                return true;
            case AggregationType::SUM:
                tuple->val1 = sum;
                tuple->val2 = "SUM";
                return true;
            case AggregationType::MAX:
                tuple->val1 = max;
                tuple->val2 = "MAX";
                return true;
            case AggregationType::MIN:
                tuple->val1 = min;
                tuple->val2 = "MIN";
                return true;
            default:
                break;
        }
    }
    return false;
}
