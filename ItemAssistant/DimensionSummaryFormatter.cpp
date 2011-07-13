#include "stdafx.h"
#include "DimensionSummaryFormatter.h"

namespace aoia { namespace sv {

    DimensionSummaryFormatter::DimensionSummaryFormatter( DataModelPtr model )
        : m_model(model)
    {
    }

    DimensionSummaryFormatter::~DimensionSummaryFormatter()
    {
    }

    std::tstring DimensionSummaryFormatter::toString() const
    {
        return _T("");
    }

}}  // namespace
