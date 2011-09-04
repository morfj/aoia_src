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
		__int64 totalCredits = 0;
		int totalLevels = 0;
		int totalAiLevels = 0;

        out << "<h2>" << m_dimensionName << "</h2>";
        out << "<table id=\"summarytbl\"><tr>";

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
                if (j == 0) {
                    out << "<td class=\"leftalign\">";
                }
                else {
                    out << "<td class=\"rightalign\">";
                }

                std::tstring propertyValue = m_model->getItemProperty(i, j);
                out << propertyValue << "</td>";

                if (propertyValue.compare(_T("-")) != 0)
                {
                    if (j == 1) {
                        // levels
                        try {
                            totalLevels += boost::lexical_cast<unsigned int>(m_model->getItemProperty(i, j));
                        }
                        catch(boost::bad_lexical_cast &)
                        {
                            // no credits, continue to next toon
                        }
                    }
                    else if (j == 2) {
                        // AI levels
                        try{
                            totalAiLevels += boost::lexical_cast<unsigned int>(m_model->getItemProperty(i, j));
                        }
                        catch(boost::bad_lexical_cast &)
                        {
                            // no credits, continue to next toon
                        }
                    }
                    else if (j == 3) {
                        // credits
                        try{
                            totalCredits += boost::lexical_cast<unsigned int>(m_model->getItemProperty(i, j));
                        }
                        catch(boost::bad_lexical_cast &)
                        {
                            // no credits, continue to next toon
                        }
                    }
                }
            }
            out << "</tr>";
        }

        out << "<tr>";
        for (unsigned int j = 0; j < m_model->getColumnCount(); ++j)
        {
            if (j == 0) {
				out << "<td class=\"leftalign\">SUM</td>";
			}
			else if(j ==1){
				// Level
				out << "<td class=\"rightalign\">" << totalLevels << "</td>";
			}
			else if(j == 2){
				// AI level
				out << "<td class=\"rightalign\">" << totalAiLevels << "</td>";
			}
			else if(j == 3){
				// AI level
				out << "<td class=\"rightalign\">" << totalCredits << "</td>";
			}
		}
		out << "</tr>";

        out << "</table>";

        return out.str();
    }

}}  // namespace
