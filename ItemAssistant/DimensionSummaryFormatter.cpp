#include "stdafx.h"
#include "DimensionSummaryFormatter.h"

namespace aoia { namespace sv {

    DimensionSummaryFormatter::DimensionSummaryFormatter(DataModelPtr model, std::tstring const& dimensionName)
        : m_model(model)
        , m_dimensionName(dimensionName)
    {
    }


    std::tstring DimensionSummaryFormatter::toString() const
    {
        std::tostringstream out;

        out << "<h2>" << m_dimensionName << "</h2>";
        out << "<table><tr>";

        for (unsigned int j = 0; j < m_model->getColumnCount(); ++j)
        {
            out << "<th>" << m_model->getColumnName(j) << "</th>";
        }

        out << "</tr>";

        for (unsigned int i = 0; i < m_model->getItemCount(); ++i)
        {
            out << "<tr>";
            for (unsigned int j = 0; j < m_model->getColumnCount(); ++j)
            {
                out << "<td>" << m_model->getItemProperty(i, j) << "</td>";
            }
            out << "</tr>";
        }

        out << "</table>";

        return out.str();
    }

}}  // namespace
