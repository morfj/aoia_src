#pragma once


#include "shared/aopackets.h"
#include "shared/Thread.h"
#include "shared/Mutex.h"
#include "ItemAssistView.h"
#include "resource.h"


class PatternMatchView;


class FilterView
 :	public CDialogImpl<FilterView>
{
public:
   enum { IDD = IDD_PATTERN_MATCHER };

   FilterView() : m_pParent(NULL) { }

   void SetParent(PatternMatchView* parent);
   BOOL PreTranslateMsg(MSG* pMsg);
   void SetProgress(unsigned short percent);
   void UpdateToonList();

   BEGIN_MSG_MAP(FilterView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
      COMMAND_HANDLER(IDC_SHOW_ALL, BN_CLICKED, OnBnClickedShowAll)
      COMMAND_HANDLER(IDC_SHOW_PARTIALS, BN_CLICKED, OnBnClickedShowPartials)
      COMMAND_HANDLER(IDC_COMPLETABLE, BN_CLICKED, OnBnClickedCompletable)
      COMMAND_HANDLER(IDC_CHARCOMBO, CBN_SELCHANGE, OnCbnSelchangeCharcombo)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

protected:
   void UpdateFilterSettings();

   LRESULT OnCbnSelchangeCharcombo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedShowAll(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedShowPartials(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   LRESULT OnBnClickedCompletable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

private:
   PatternMatchView* m_pParent;
   std::tstring m_basequery;
};


class WebView
 :	public CDialogImpl<WebView>
{
public:
   enum { IDD = IDD_WEB };

   WebView() : m_pParent(NULL) { }

   void SetParent(PatternMatchView* parent);
   BOOL PreTranslateMsg(MSG* pMsg);

   BEGIN_MSG_MAP(WebView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_FORWARDMSG, OnForwardMsg)
		MSG_WM_SIZE(OnSize)
      DEFAULT_REFLECTION_HANDLER()
   END_MSG_MAP()

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnForwardMsg(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnSize(UINT wParam, CSize newSize);

   void Navigate(std::tstring const& url);
   void SetHTML(std::tstring const& html);

private:
   CAxWindow m_wndIE;
   CComQIPtr<IWebBrowser2> m_pWB2;

   PatternMatchView* m_pParent;
};


class AvailCalcThread : public Thread
{
public:
   AvailCalcThread() : m_index(0), m_term(false), m_toon(0) { }
   virtual ~AvailCalcThread() { }

   void SetOwner(PatternMatchView* owner) { m_pOwner = owner; }
   void SetToon(unsigned int toon = 0) { m_toon = toon; }
   unsigned int Toon() const { return m_toon; }
   void StopPlease() { if (IsRunning()) { m_term = true; } }

   virtual DWORD ThreadProc();

private:
   PatternMatchView* m_pOwner;
   int m_index;
   unsigned int m_toon;
   bool m_term;
};


#define WM_UPDATE_PBLIST WM_USER+1


class PatternMatchView
  : public ItemAssistView<PatternMatchView>
{
   typedef ItemAssistView<PatternMatchView> inherited;
public:
	DECLARE_WND_CLASS(NULL)

   PatternMatchView(void);
   virtual ~PatternMatchView(void);

	BEGIN_MSG_MAP_EX(PatternMatchView)
		MSG_WM_CREATE(OnCreate)
      MSG_WM_DESTROY(OnDestroy)
		MSG_WM_SIZE(OnSize)
      MESSAGE_HANDLER(WM_UPDATE_PBLIST, OnUpdatePbListView)
      NOTIFY_CODE_HANDLER_EX(LVN_COLUMNCLICK, OnColumnClick)
      NOTIFY_CODE_HANDLER_EX(LVN_ITEMACTIVATE, OnItemActivate)
      NOTIFY_CODE_HANDLER_EX(LVN_ITEMCHANGING, OnItemChanging)
      CHAIN_MSG_MAP(inherited)
      DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

   virtual void OnActive(bool doActivation);
   virtual bool PreTranslateMsg(MSG* pMsg);

   // Pocket boss list and access methods
   struct PbItem {
      unsigned int pbid;
      std::tstring pbname;
      float pbavailability;
   };
   typedef std::vector<boost::shared_ptr<PbItem> > PbList;
   PbList const& BossList() { return m_pblist; }
   void SetBossAvail(unsigned int pbid, float avail);

   void UpdatePbListView(unsigned int pbid = 0);

   PbList& PbListRef() { return m_pblist; }
   Mutex& PbListMutex() { return m_pblistMutex; }

   void SetFilterSettings(unsigned int toonid, float availfilter);

   static float CalcPbAvailability(unsigned int pbid, unsigned int toonid = 0);

protected:
   enum {
      AVAIL_TIMER = 1
   };

   LRESULT OnCreate(LPCREATESTRUCT createStruct);
   void OnDestroy();
	LRESULT OnSize(UINT wParam, CSize newSize);
   LRESULT OnColumnClick(LPNMHDR lParam);
   LRESULT OnItemActivate(LPNMHDR lParam);
   LRESULT OnUpdatePbListView(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
   LRESULT OnItemChanging(LPNMHDR lParam);

   void UpdateFilterProgress(unsigned short percent);

   void UpdateLayout(CSize newSize);
   static int CALLBACK CompareStr(LPARAM param1, LPARAM param2, LPARAM sort);

   void SetColumns(std::vector<std::tstring> &headings);
   void AddRow(unsigned int rowid, std::vector<std::tstring> &data);
   void UpdateRow(unsigned int rowid, std::vector<std::tstring> &data);

private:
   PbList m_pblist;

   float m_availfilter;
   unsigned int m_toonid;

   bool  m_sortDesc;
   int   m_sortColumn;

   CListViewCtrl     m_listview;
   FilterView        m_filterview;
   WebView           m_webview;
   CAccelerator      m_accelerators;
   AvailCalcThread   m_availCalc;
   Mutex             m_pblistMutex;
};
