
#include "StdAfx.h"
#include "BotExportView.h"


BotExportView::BotExportView(void)
{
}


BotExportView::~BotExportView(void)
{
}


LRESULT BotExportView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	RECT rect;

	GetClientRect(&rect);
	dc.DrawText(_T("Bot Export Plugin"), -1, &rect, NULL);

	return 0;
}


LRESULT BotExportView::OnCreate(LPCREATESTRUCT createStruct)
{
	return 0;
}


void BotExportView::OnAOMessage(AO::Header* pMsg)
{
}
