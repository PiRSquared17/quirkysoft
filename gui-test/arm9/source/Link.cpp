/*
  Copyright 2007 Richard Quirk
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
 
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#include "Link.h"
#include "Rectangle.h"
#include "Canvas.h"
#include "Palette.h"

using nds::Rectangle;

Link::Link(const std::string & a)
  : m_anchor(a)
{ }

static void removeRectangle(Rectangle * r)
{
  delete r;
}

Link::~Link()
{
  for_each(m_clickZones.begin(), m_clickZones.end(), removeRectangle);
  m_clickZones.clear();
}

void Link::appendClickZone(int x, int y, int w, int h)
{
  Rectangle * rect = new Rectangle;
  rect->x = x;
  rect->y = y;
  rect->w = w;
  rect->h = h;
  m_clickZones.push_front(rect);
}

bool Link::hitTest(int x, int y) const
{
  RectangleList::const_iterator it(m_clickZones.begin());
  for (; it != m_clickZones.end() ; ++it)
  {
    Rectangle * rect = *it;
    if ( rect->hit(x,y))
      return true;
  }
  return false;
}


void Link::highlight() const
{
  RectangleList::const_iterator it(m_clickZones.begin());
  for (; it != m_clickZones.end() ; ++it)
  {
    Rectangle * rect = *it;
    nds::Canvas::instance().drawRectangle(rect->x,rect->y,rect->w, rect->h, nds::Color(31,0,30));
  }
}

std::string Link::href() const
{
  // FIXME - what about #?
  /*
  UnicodeString href = m_anchor->attribute("href");
  // strip off the # part?
  // replace ' ' with %20?
  return unicode2string(href);
  */
  return m_anchor;
}