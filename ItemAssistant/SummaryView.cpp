#include "StdAfx.h"
#include "SummaryView.h"
#include "DBManager.h"
#include "DataModel.h"

namespace aoia { namespace sv {

    SummaryView::SummaryView()
        : m_webview(_T(""))
    {
    }

    SummaryView::~SummaryView()
    {
    }

    LRESULT SummaryView::OnCreate( LPCREATESTRUCT createStruct )
    {
        m_webview.Create(m_hWnd);

        // Build the toolbar
        TBBUTTON buttons[1];
        buttons[0].iBitmap = 0;
        buttons[0].idCommand = ID_HELP;
        buttons[0].fsState = TBSTATE_ENABLED;
        buttons[0].fsStyle = TBSTYLE_BUTTON | BTNS_SHOWTEXT | BTNS_AUTOSIZE;
        buttons[0].dwData = NULL;
        buttons[0].iString = (INT_PTR)_T("Help");

        CImageList imageList;
        imageList.CreateFromImage(IDB_SUMMARY_VIEW, 16, 1, CLR_DEFAULT, IMAGE_BITMAP, LR_CREATEDIBSECTION | LR_DEFAULTSIZE);

        m_toolbar.Create(GetTopLevelWindow(), NULL, _T("SummaryViewToolBar"), 
            ATL_SIMPLE_TOOLBAR_PANE_STYLE | TBSTYLE_LIST, 
            TBSTYLE_EX_MIXEDBUTTONS);
        m_toolbar.SetButtonStructSize();
        m_toolbar.SetImageList(imageList);
        m_toolbar.AddButtons(ARRAYSIZE(buttons), buttons);
        m_toolbar.AutoSize();

        return 0;
    }

    LRESULT SummaryView::OnSize( UINT wParam, CSize newSize )
    {
        CRect r( CPoint( 0, 0 ), newSize );
        m_webview.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);

        return 0;
    }

    void SummaryView::OnActive( bool doActivation )
    {
        if (doActivation) {
            UpdateSummary();
        }
    }

    void SummaryView::UpdateSummary()
    {
        std::map<unsigned int, std::tstring> dimensions;
        if (!g_DBManager.getDimensions(dimensions)) {
            LOG("SummaryView::UpdateSummary() : Unable to get dimensional info.");
            return;
        }

        std::vector<DataModelPtr> models;
        for (std::map<unsigned int, std::tstring>::const_iterator it = dimensions.begin(); it != dimensions.end(); ++it)
        {
            models.push_back(DataModelPtr(new DataModel(it->first)));
        }

        for (std::vector<DataModelPtr>::const_iterator it = models.begin(); it != models.end(); ++it)
        {
            if ((*it)->getItemCount() == 0) {
                continue;   // skip empty dimensions
            }

            // TODO: output HTML into template
        }
    }

}}  // end of namespace
