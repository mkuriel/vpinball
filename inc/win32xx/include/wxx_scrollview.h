// Win32++   Version 8.6
// Release Date: 2nd November 2018
//
//      David Nash
//      email: dnash@bigpond.net.au
//      url: https://sourceforge.net/projects/win32-framework
//
//
// Copyright (c) 2005-2018  David Nash
//
// Permission is hereby granted, free of charge, to
// any person obtaining a copy of this software and
// associated documentation files (the "Software"),
// to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify,
// merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom
// the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice
// shall be included in all copies or substantial portions
// of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF
// ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
// PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
// ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.
//
////////////////////////////////////////////////////////


////////////////////////////////////////////////////////
//
// The CScrollView class adds scrolling to a view window. Inherit your
// view window from CScrollView, and use the SetScrollSizes function
// to set up the scrolling. Override OnDraw and perform the drawing to
// the window as usual.
//
// Use SetScrollSizes to specify the scrolling sizes.
// Use SetScollSizes(CSize(0,0)) to disable scrolling.
//
// The default scrolling background is white. Use the SetScrollBkgnd
// to set a different brush color.
//
/////////////////////////////////////////////////////////



#ifndef _WIN32XX_SCROLLVIEW_H_
#define _WIN32XX_SCROLLVIEW_H_

#include "wxx_appcore0.h"

#ifndef WHEEL_DELTA
  #define WHEEL_DELTA                     120
#endif

#ifndef GET_WHEEL_DELTA_WPARAM
  #define GET_WHEEL_DELTA_WPARAM(wparam)  ((short)HIWORD(wparam))
#endif

namespace Win32xx
{

    ///////////////////////////////////////////////
    // The CScrollView class adds scrolling to a view window. Inherit your
    // view window from CScrollView, and use the SetScrollSizes function
    // to set up the scrolling. Override OnDraw and perform the drawing to
    // the window as usual.
    class CScrollView : public CWnd
    {
    public:
        CScrollView();
        virtual ~CScrollView();

        CBrush GetScrollBkgnd() const    { return m_bkgndBrush; }
        CPoint GetScrollPosition() const { return m_currentPos; }
        CSize GetTotalScrollSize() const { return m_totalSize; }
        BOOL IsHScrollVisible() const    { return (GetStyle() &  WS_HSCROLL) != FALSE; }
        BOOL IsVScrollVisible() const    { return (GetStyle() &  WS_VSCROLL) != FALSE; }
        void SetScrollPosition(POINT pt);
        void SetScrollSizes(CSize totalSize = CSize(0,0), CSize pageSize = CSize(0,0), CSize lineSize = CSize(0,0));
        void SetScrollBkgnd(CBrush bkgndBrush) { m_bkgndBrush = bkgndBrush; }

    protected:
        virtual void    FillOutsideRect(CDC& dc, HBRUSH brush);
        virtual BOOL    OnEraseBkgnd(CDC& dc);
        virtual LRESULT OnHScroll(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnMouseWheel(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnPaint(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnVScroll(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual LRESULT OnWindowPosChanging(UINT msg, WPARAM wparam, LPARAM lparam);
        virtual void    PreCreate(CREATESTRUCT& cs);

        LRESULT WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam);

    private:
        void UpdateBars();
        CPoint m_currentPos;
        CSize m_totalSize;
        CSize m_pageSize;
        CSize m_lineSize;
        CBrush m_bkgndBrush;
    };

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


namespace Win32xx
{

    ////////////////////////////////////////
    // Definitions for the CScrollView class
    //

    inline CScrollView::CScrollView()
    {
        m_bkgndBrush.CreateSolidBrush(RGB(255, 255, 255));
    }

    inline CScrollView::~CScrollView()
    {
    }

    // Fills the area of the view that appears outside of the scrolling area.
    // Can be used in OnEraseBkgnd to draw the background efficiently.
    inline void CScrollView::FillOutsideRect(CDC& dc, HBRUSH brush)
    {
        // Get the window size in client area co-ordinates
        CRect windowRect = GetWindowRect();
        ScreenToClient(windowRect);

        // Fill the right side with the specified brush
        CRect rcRight(m_totalSize.cx, 0, windowRect.right, windowRect.bottom);
        dc.FillRect(rcRight, brush);

        // Fill the bottom side with the specified brush
        CRect rcBottom(0, m_totalSize.cy, m_totalSize.cx, windowRect.bottom);
        dc.FillRect(rcBottom, brush);
    }

    // Called when the background for the window is erased.
    inline BOOL CScrollView::OnEraseBkgnd(CDC& dc)
    {
        UNREFERENCED_PARAMETER(dc);

        if (m_totalSize == CSize(0, 0))
            return FALSE;   // Allow background erasure when the scroll bars are disabled
        else
            return TRUE;    // Prevent background erasure to reduce flicker
    }

    // Called when an event occurs in the horizontal scroll bar.
    inline LRESULT CScrollView::OnHScroll(UINT msg, WPARAM wparam, LPARAM lparam)
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(lparam);

        int xNewPos;

        switch (LOWORD(wparam))
        {
        case SB_PAGELEFT: // User clicked the scroll bar shaft left of the scroll box.
            xNewPos = m_currentPos.x - m_pageSize.cx;
            break;

        case SB_PAGERIGHT: // User clicked the scroll bar shaft right of the scroll box.
            xNewPos = m_currentPos.x + m_pageSize.cx;
            break;

        case SB_LINELEFT: // User clicked the left arrow.
            xNewPos = m_currentPos.x - m_lineSize.cx;
            break;

        case SB_LINERIGHT: // User clicked the right arrow.
            xNewPos = m_currentPos.x + m_lineSize.cx;
            break;

        case SB_THUMBTRACK: // User dragging the scroll box.
            xNewPos = HIWORD(wparam);
            break;

        default:
            xNewPos = m_currentPos.x;
        }

        // Scroll the window.
        xNewPos = MAX(0, xNewPos);
        xNewPos = MIN(xNewPos, m_totalSize.cx - GetClientRect().Width());
        int xDelta = xNewPos - m_currentPos.x;
        m_currentPos.x = xNewPos;
        ScrollWindowEx(-xDelta, 0, NULL, NULL, NULL, NULL, SW_INVALIDATE);

        UpdateBars();

        return 0;
    }

    inline LRESULT CScrollView::OnPaint(UINT msg, WPARAM wparam, LPARAM lparam)
    // Override OnPaint so we can wrap code around OnDraw
    {

        if (m_totalSize != CSize(0, 0))
        {
            CPaintDC dc(*this);
            CMemDC dcMem(dc);

            // negative sizes are not allowed.
            assert(m_totalSize.cx > 0);
            assert(m_totalSize.cy > 0);

            // Create the compatible bitmap for the memory DC
            dcMem.CreateCompatibleBitmap(GetDC(), m_totalSize.cx, m_totalSize.cy);

            // Set the background color
            CRect rcTotal(CPoint(0, 0), m_totalSize);
            dcMem.FillRect(rcTotal, m_bkgndBrush);

            // Call the overridden OnDraw function
            OnDraw(dcMem);

            // Copy the modified memory DC to the window's DC with scrolling offsets
            dc.BitBlt(0, 0, m_totalSize.cx, m_totalSize.cy, dcMem, m_currentPos.x, m_currentPos.y, SRCCOPY);

            // Set the area outside the scrolling area
            FillOutsideRect(dc, m_bkgndBrush);

            // No more drawing required
            return 0;
        }

        // Do default OnPaint if m_sizeTotal is zero
        return CWnd::OnPaint(msg, wparam, lparam);
    }

    inline LRESULT CScrollView::OnMouseWheel(UINT msg, WPARAM wparam, LPARAM lparam)
    // Called when the mouse wheel is rotated.
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(lparam);

        int WheelDelta = GET_WHEEL_DELTA_WPARAM(wparam);
        int cyPos = ::MulDiv(WheelDelta, m_lineSize.cy, WHEEL_DELTA);

        // Scroll the window.
        int yNewPos = m_currentPos.y - cyPos;
        yNewPos = MIN(yNewPos, m_totalSize.cy - GetClientRect().Height());
        yNewPos = MAX(yNewPos, 0);
        int yDelta = yNewPos - m_currentPos.y;
        m_currentPos.y = yNewPos;
        ScrollWindowEx(0, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);

        UpdateBars();

        return 0;
    }

    inline LRESULT CScrollView::OnVScroll(UINT msg, WPARAM wparam, LPARAM lparam)
    // Called when an event occurs in the vertical scroll bar.
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(lparam);

        int yNewPos;

        switch (LOWORD(wparam))
        {
            case SB_PAGEUP: // User clicked the scroll bar shaft above the scroll box.
                yNewPos = m_currentPos.y - m_pageSize.cy;
                break;

            case SB_PAGEDOWN: // User clicked the scroll bar shaft below the scroll box.
                yNewPos = m_currentPos.y + m_pageSize.cy;
                break;

            case SB_LINEUP: // User clicked the top arrow.
                yNewPos = m_currentPos.y - m_lineSize.cy;
                break;

            case SB_LINEDOWN: // User clicked the bottom arrow.
                yNewPos = m_currentPos.y + m_lineSize.cy;
                break;

            case SB_THUMBTRACK: // User dragging the scroll box.
                yNewPos = HIWORD(wparam);
                break;

            default:
                yNewPos = m_currentPos.y;
        }

        // Scroll the window.
        yNewPos = MAX(0, yNewPos);
        yNewPos = MIN( yNewPos, m_totalSize.cy - GetClientRect().Height() );
        int yDelta = yNewPos - m_currentPos.y;
        m_currentPos.y = yNewPos;
        ScrollWindowEx(0, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);

        UpdateBars();

        return 0;
    }

    inline LRESULT CScrollView::OnWindowPosChanged(UINT msg, WPARAM wparam, LPARAM lparam)
    // Called after a window's size has changed.
    {
        UNREFERENCED_PARAMETER(msg);
        UNREFERENCED_PARAMETER(wparam);
        UNREFERENCED_PARAMETER(lparam);

        UpdateBars();
        Invalidate();

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline LRESULT CScrollView::OnWindowPosChanging(UINT msg, WPARAM wparam, LPARAM lparam)
    // Called before the window's size is changed.
    {
        // We hide the scrollbars early in response to WM_WINDOWPOSCHANGING.
        // If we did this in response to WM_WINDOWPOSCHANGED we could get
        //  unexpected results due to recursion.

        // Retrieve the future size of the window
        LPWINDOWPOS pWinPos = (LPWINDOWPOS)lparam;
        assert(pWinPos);
        CRect rc(0, 0, pWinPos->cx, pWinPos->cy);

        // Possibly hide the horizontal scroll bar
        if (rc.Width() > m_totalSize.cx)
        {
            ShowScrollBar(SB_HORZ, FALSE);  // Can resize the window
        }

        // Possibly hide the vertical scroll bar
        if (rc.Height() > m_totalSize.cy)
        {
            ShowScrollBar(SB_VERT, FALSE);  // Can resize the window
        }

        return FinalWindowProc(msg, wparam, lparam);
    }

    inline void CScrollView::PreCreate(CREATESTRUCT& cs)
    {
        // Set the Window Class name
        cs.lpszClass = _T("ScrollView");

        cs.style = WS_CHILD | WS_HSCROLL | WS_VSCROLL;
    }

    inline void CScrollView::SetScrollPosition(POINT pt)
    // Sets the current scroll position.
    {
        assert(pt.x >= 0 && pt.x <= m_totalSize.cx);
        assert(pt.y >= 0 && pt.y <= m_totalSize.cy);

        m_currentPos = pt;
        UpdateBars();
    }

    inline void CScrollView::SetScrollSizes(CSize totalSize, CSize pageSize, CSize lineSize)
    // Sets the various Scroll Size parameters.
    // Note that a sizeTotal of CSize(0,0) turns scrolling off.
    {
        if (IsWindow())
        {
            ShowScrollBar(SB_BOTH, FALSE);
            Invalidate();
        }

        m_totalSize = totalSize;
        m_pageSize = pageSize;
        m_lineSize = lineSize;

        if (m_pageSize.cx == 0)
            m_pageSize.cx = m_totalSize.cx / 10;
        if (m_pageSize.cy == 0)
            m_pageSize.cy = m_totalSize.cy / 10;
        if (m_lineSize.cx == 0)
            m_lineSize.cx = m_pageSize.cx / 10;
        if (m_lineSize.cy == 0)
            m_lineSize.cy = m_pageSize.cy / 10;

        m_currentPos = CPoint(0, 0);

        UpdateBars();
    }

    inline void CScrollView::UpdateBars()
    // Updates the display state of the scrollbars and the scrollbar positions.
    // Also scrolls display view as required.
    // Note: This function can be called recursively.
    {
        if (IsWindow())
        {
            if (m_totalSize == CSize(0, 0))
            {
                ShowScrollBar(SB_BOTH, FALSE);
                m_currentPos = CPoint(0, 0);
            }
            else
            {
                DWORD exStyle = GetExStyle();
                CRect totalRect(0, 0, m_totalSize.cx, m_totalSize.cy);
                AdjustWindowRectEx(&totalRect, 0, FALSE, exStyle);

                // CRect of view, size affected by scroll bars
                CRect clientRect = GetClientRect();
                AdjustWindowRectEx(&clientRect, 0, FALSE, exStyle);

                // CRect of view, unaffected by scroll bars
                CRect viewRect = GetWindowRect();

                SCROLLINFO si;
                ZeroMemory(&si, sizeof(si));
                si.cbSize = sizeof(si);
                si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
                si.nMin = 0;
                bool IsBarNotRequired = ((viewRect.Width() >= totalRect.Width()) && (viewRect.Height() >= totalRect.Height()));

                if  ( (clientRect.Width() >= totalRect.Width()) || IsBarNotRequired )
                {
                    m_currentPos.x = 0;
                    ShowScrollBar(SB_HORZ, FALSE);
                }
                else
                {
                    si.nMax = totalRect.Width();
                    si.nPage = clientRect.Width();
                    si.nPos = m_currentPos.x;
                    SetScrollInfo(SB_HORZ, si, TRUE);
                    ShowScrollBar(SB_HORZ, TRUE);
                }

                if ( (clientRect.Height() >= totalRect.Height()) || IsBarNotRequired )
                {
                    m_currentPos.y = 0;
                    ShowScrollBar(SB_VERT, FALSE);
                }
                else
                {
                    si.nMax = totalRect.Height();
                    si.nPage = clientRect.Height();
                    si.nPos = m_currentPos.y;
                    SetScrollInfo(SB_VERT, si, TRUE);
                    ShowScrollBar(SB_VERT, TRUE);
                }

                int cxScroll = IsVScrollVisible() ? GetSystemMetrics(SM_CXVSCROLL) : 0;
                int cyScroll = IsHScrollVisible() ? GetSystemMetrics(SM_CYHSCROLL) : 0;

                int xNewPos = MIN(m_currentPos.x, totalRect.Width() - viewRect.Width() + cxScroll);
                xNewPos = MAX(xNewPos, 0);
                int xDelta = xNewPos - m_currentPos.x;

                int yNewPos = MIN(m_currentPos.y, totalRect.Height() - viewRect.Height() + cyScroll);
                yNewPos = MAX(yNewPos, 0);
                int yDelta = yNewPos - m_currentPos.y;

                ScrollWindowEx(-xDelta, -yDelta, NULL, NULL, NULL, NULL, SW_INVALIDATE);
                m_currentPos.x = xNewPos;
                m_currentPos.y = yNewPos;
            }
        }
    }

    inline LRESULT CScrollView::WndProcDefault(UINT msg, WPARAM wparam, LPARAM lparam)
    // Default message handling
    {
        switch (msg)
        {
        case WM_HSCROLL:            return OnHScroll(msg, wparam, lparam);
        case WM_MOUSEWHEEL:         return OnMouseWheel(msg, wparam, lparam);
        case WM_PAINT:              return OnPaint(msg, wparam, lparam);
        case WM_VSCROLL:            return OnVScroll(msg, wparam, lparam);
        case WM_WINDOWPOSCHANGED:   return OnWindowPosChanged(msg, wparam, lparam);
        case WM_WINDOWPOSCHANGING:  return OnWindowPosChanging(msg, wparam, lparam);
        }

        // Pass unhandled messages on for default processing
        return CWnd::WndProcDefault(msg, wparam, lparam);
    }
}

#endif // _WIN32XX_SCROLLVIEW_H_
