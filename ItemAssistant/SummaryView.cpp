#include "StdAfx.h"
#include "SummaryView.h"

namespace aoia { namespace sv {

    SummaryView::SummaryView()
        : m_webview(_T(""))
    {
        m_datamodel.reset(new DataModel());
    }

    SummaryView::~SummaryView()
    {
    }

    LRESULT SummaryView::OnCreate( LPCREATESTRUCT createStruct )
    {
        m_webview.Create(m_hWnd);

        return 0;
    }

    LRESULT SummaryView::OnSize( UINT wParam, CSize newSize )
    {
        CRect r( CPoint( 0, 0 ), newSize );
        m_webview.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOZORDER | SWP_NOACTIVATE | SWP_DEFERERASE | SWP_NOSENDCHANGING);

        return 0;
    }

}}  // end of namespace
