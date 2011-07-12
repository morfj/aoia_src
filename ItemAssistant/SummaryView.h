#ifndef SUMMARYVIEW_H
#define SUMMARYVIEW_H

#include <PluginSDK/ItemAssistView.h>
#include "DataModel.h"
#include "WebView.h"

namespace aoia { namespace sv {

    class SummaryView
        : public ItemAssistView<SummaryView>
    {
    public:
        SummaryView(void);
        virtual ~SummaryView(void);

        BEGIN_MSG_MAP_EX(SummaryView)
            MSG_WM_CREATE(OnCreate)
            MSG_WM_SIZE(OnSize)
            REFLECT_NOTIFICATIONS()
            DEFAULT_REFLECTION_HANDLER()
        END_MSG_MAP();

    protected:
        LRESULT OnCreate(LPCREATESTRUCT createStruct);
        LRESULT OnSize(UINT wParam, CSize newSize);

    private:
        aoia::WebView m_webview;
        DataModelPtr m_datamodel;
    };

}}  // end of namespace

#endif // SUMMARYVIEW_H
