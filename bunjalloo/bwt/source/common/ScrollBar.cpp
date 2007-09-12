/*
  Copyright (C) 2007 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Canvas.h"
#include "Palette.h"
#include "ScrollBar.h"
#include "ScrollPane.h"
#include "WidgetColors.h"
using nds::Canvas;

const static int ARROW_HEIGHT(10);
const static int MIN_HANDLE_SIZE(5);
const static int HANDLE_NOT_HELD(-1);

ScrollBar::ScrollBar():Component(),
  m_total(0),
  m_value(0),
  m_handleSize(0),
  m_handlePosition(0),
  m_handleHeld(HANDLE_NOT_HELD),
  m_scrollPane(0)
{
}

void ScrollBar::setTotal(int total)
{
  m_total = total;
}

void ScrollBar::setValue(int value)
{
  m_value = value;
}

int ScrollBar::total() const
{
  return m_total;
}

int ScrollBar::value() const
{
  return m_value;
}

void ScrollBar::setScrollable(ScrollPane * scrollPane)
{
  m_scrollPane = scrollPane;
}

bool ScrollBar::touch(int x, int y)
{
  if (m_scrollPane and m_bounds.hit(x, y))
  {

    // Check if clicking in the arrows:
    if (y < (m_bounds.y + ARROW_HEIGHT))
    {
      m_scrollPane->up();
      m_handleHeld = HANDLE_NOT_HELD;
      return true;
    }
    if (y > (m_bounds.y + m_bounds.h - ARROW_HEIGHT))
    {
      m_scrollPane->down();
      m_handleHeld = HANDLE_NOT_HELD;
      return true;
    }
    // check if clicking on the handle:
    nds::Rectangle handle = {m_bounds.x, m_handlePosition, m_bounds.w, m_handleSize};
    if (m_handleHeld == HANDLE_NOT_HELD)
    {
      if (handle.hit(x, y))
      {
        m_handleHeld = y;
        return true;
      }
    }
    else if (m_handleHeld != HANDLE_NOT_HELD)
    {
      // scroll to y...
      m_scrollPane->scrollToPercent( ((y - ARROW_HEIGHT - m_bounds.top()) * 256 ) / visibleRange());
      m_handleHeld = y;
      return true;
    }
    // check if above or below the bounds
    if (y < m_handlePosition)
    {
      m_scrollPane->upBlock();
      m_handleHeld = HANDLE_NOT_HELD;
      return true;
    }
    if (y > m_handlePosition)
    {
      m_scrollPane->downBlock();
      m_handleHeld = HANDLE_NOT_HELD;
      return true;
    }

  }
  else {
    m_handleHeld = HANDLE_NOT_HELD;
  }
  return false;
}

void ScrollBar::paint(const nds::Rectangle & clip)
{
  // background
  Canvas::instance().fillRectangle(m_bounds.x+1,
                                        m_bounds.y+1,
                                        m_bounds.w-1,
                                        m_bounds.h-1,
                                        WidgetColors::SCROLLBAR_BACKGROUND);

  // up arrow
  Canvas::instance().fillRectangle(m_bounds.x,
                                        m_bounds.y,
                                        m_bounds.w,
                                        ARROW_HEIGHT,
                                        WidgetColors::SCROLLBAR_ARROW);
  // head.
  int headX = m_bounds.x + (m_bounds.w/2);
  int headY = m_bounds.y + (ARROW_HEIGHT/2) - 1;
  drawUpArrow(headX-1, headY);

  // down arrow
  Canvas::instance().fillRectangle(m_bounds.x,
                                        m_bounds.bottom()-ARROW_HEIGHT,
                                        m_bounds.w,
                                        ARROW_HEIGHT,
                                        WidgetColors::SCROLLBAR_ARROW);
  headY = m_bounds.bottom() - (ARROW_HEIGHT/2);
  drawDownArrow(headX-1, headY);

  calculateHandle();
  unsigned short c(WidgetColors::SCROLLBAR_HANDLE_NOT_HELD);
  if (m_handleHeld != HANDLE_NOT_HELD)
  {
    c = WidgetColors::SCROLLBAR_HANDLE_HELD;
  }

  Canvas::instance().fillRectangle(m_bounds.x+1,
                                   m_handlePosition,
                                   m_bounds.w-2,
                                   m_handleSize,
                                   c);

  Canvas::instance().drawRectangle(m_bounds.x,
                                        m_bounds.y+ARROW_HEIGHT,
                                        m_bounds.w-1,
                                        m_bounds.h-1-2*ARROW_HEIGHT,
                                        WidgetColors::SCROLLBAR_BORDER);
}

void ScrollBar::calculateHandle()
{
  if (m_total <= 0) {
    return;
  }

  m_handleSize = (visibleRange() * visibleRange())/total();
  if (m_handleSize < MIN_HANDLE_SIZE)
    m_handleSize = MIN_HANDLE_SIZE;

  int scrollerHeight = visibleRange() - m_handleSize;

  if (m_total > m_bounds.h)
    m_handlePosition = ((m_value * scrollerHeight) / (m_total-m_bounds.h) );
  else
    m_handlePosition = 0;
  if (m_handlePosition < 0)
    m_handlePosition = 0;
  m_handlePosition += m_bounds.top() + ARROW_HEIGHT;
  if (m_handlePosition > (m_bounds.bottom() - ARROW_HEIGHT - m_handleSize))
  {
    m_handlePosition = m_bounds.bottom() - ARROW_HEIGHT - m_handleSize;
  }
}

int ScrollBar::visibleRange() const
{
  return m_bounds.h - ARROW_HEIGHT*2;
}

void ScrollBar::drawDownArrow(int x, int y)
{
  Canvas::instance().drawPixel(x, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x-1, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x+1, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
}
void ScrollBar::drawUpArrow(int x, int y)
{
  Canvas::instance().drawPixel(x, y, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x-1, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
  Canvas::instance().drawPixel(x+1, y+1, WidgetColors::SCROLLBAR_ARROW_HEAD);
}
