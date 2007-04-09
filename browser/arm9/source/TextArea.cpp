#include "ndspp.h"
#include "TextArea.h"
#include "Palette.h"
#include "Canvas.h"
#include "Font.h"
#include "UTF8.h"
#include "File.h"

using namespace nds;
using namespace std;

TextArea::TextArea() : 
  m_font(0),
  m_palette(0),
  m_basePalette(0),
  m_paletteLength(0),
  m_encoding("utf-8"),
  m_startLine(0)
{
  init("data/bunjalloo/fonts/vera");
}

void TextArea::setStartLine(int line)
{
  if (line < 0)
    line = 0;
  m_startLine = line;
}

int TextArea::startLine() const
{
  return m_startLine;
}

void TextArea::init(const string & fontBase)
{
  m_font=new Font(fontBase);
  setPalette(fontBase+".pal");
}

void TextArea::printAt(Font::Glyph & g, int xPosition, int yPosition)
{
  const unsigned char * data = g.data;
  int dataInc = (m_font->totalWidth() - g.width)/2;
  for (int y = 0; y < g.height; ++y)
  {
    for (int x = 0; x < g.width/2; ++x)
    {
      unsigned char pixelPair = *data++;
      int pix1 = ((pixelPair)&0xf);
      if (pix1)
        Canvas::instance().drawPixel(xPosition+(x*2), yPosition+y, m_palette[pix1]);
      int pix2 = ((pixelPair>>4)&0xf);
      if (pix2)
        Canvas::instance().drawPixel(xPosition+(x*2)+1, yPosition+y, m_palette[pix2]);
    }
    data += dataInc;
  }
}

void TextArea::setCursor(int x, int y)
{
  m_cursorx = x;
  m_cursory = y;
}

void TextArea::incrLine()
{
  m_cursorx = 0; 
  m_cursory += m_font->height();
}

void TextArea::checkLetter(Font::Glyph & g)
{
  if (m_cursorx + g.width > Canvas::instance().width())
  {
    incrLine();
  }
}

void TextArea::printu(const UnicodeString & unicodeString)
{
  // skip until we reach startLine
#if 0
  int tmpx = m_cursorx;
  int tmpy = m_cursorx;
  m_cursorx = 0;
  m_cursory = 0;
  UnicodeString::const_iterator it(unicodeString.begin());
  int position(0);
  int finalLine(m_font->height()*m_startLine);
  for (; it != unicodeString.end() and m_cursory < finalLine; ++it, ++position)
  {
    unsigned int value(*it);
    if (value == UTF8::MALFORMED) {
      value = '?';
    }
    Font::Glyph g;
    m_font->glyph(value, g);
    if (value == '\n')
    {
      incrLine();
    } 
    else {
      checkLetter(g);
      m_cursorx += g.width;
    }
    if (m_cursorx > Canvas::instance().width())
    {
      incrLine();
    }
  }
  m_cursorx = tmpx;
  m_cursory = tmpy;
  printuImpl(unicodeString.substr(position , unicodeString.length()-position));
#endif
  printuImpl(unicodeString);
}

int TextArea::textSize(const UnicodeString & unicodeString) const
{
  UnicodeString::const_iterator it(unicodeString.begin());
  int size(0);
  for (; it != unicodeString.end(); ++it)
  {
    unsigned int value(*it);
    if (value == UTF8::MALFORMED)
      value = '?';
    Font::Glyph g;
    m_font->glyph(value, g);
    size += g.width;
  }
  return size;
}

void TextArea::printuWord(const UnicodeString & word)
{
  UnicodeString::const_iterator it(word.begin());
  for (; it != word.end(); ++it)
  {
    unsigned int value(*it);
    if ( doSingleChar(value) )
    {
      break;
    }
  }
}

void TextArea::printuImpl(const UnicodeString & unicodeString)
{
  UnicodeString::const_iterator it(unicodeString.begin());
  const unsigned int intDelimiters[] = {0x0020, 0x0009, 0x000a, 0x000b, 0x000c, 0x000d};
  const static UnicodeString delimiters(intDelimiters,6);
  int currPosition(0);
  for (; it != unicodeString.end(); ++it, ++currPosition)
  {
    // find the next space character
    unsigned int position = unicodeString.find_first_of(delimiters,currPosition);
    position = position==string::npos?unicodeString.length():position;
    const UnicodeString word(unicodeString.substr(currPosition,position-currPosition+1));
    int size = textSize(word);
    if (m_cursorx + size > Canvas::instance().width())
    {
      incrLine();
    }
    printuWord(word);
    it += word.length()-1;
    currPosition += word.length()-1;
    if (m_cursory > Canvas::instance().height()) {
      break;
    }
  }
}

bool TextArea::doSingleChar(unsigned int value)
{
  if (m_palette) {
    if (value == UTF8::MALFORMED) {
      value = '?';
    }
    Font::Glyph g;
    m_font->glyph(value, g);
    if (value == '\n')
    {
      incrLine();
    } 
    else {
      checkLetter(g);
      if (g.data) {
        printAt(g, m_cursorx, m_cursory);
      }
      m_cursorx += g.width;
    }
  } else {
    // could not load the font?
    Font::Glyph g;
    m_font->glyph(' ', g);
    Canvas::instance().fillRectangle(m_cursorx, m_cursory, 
        g.width, g.height, Color(31,0,0));
    m_cursorx += g.width;
  }
  if (m_cursorx > Canvas::instance().width())
  {
    incrLine();
  }
  return (m_cursory > Canvas::instance().height());
}

void TextArea::print(const char * text, int amount)
{
  int total = 0;
  while (total < amount)
  {
    unsigned int value;
    unsigned int read(1);
    if (m_encoding == "utf-8") {
      read = UTF8::decode(text, value);
    } else {
      value = (int)(text[0]&0xff);
    }
    if ( doSingleChar(value) )
    {
      break;
    }
    text += read;
    total += read;
  }
}

void TextArea::setColor(unsigned short color)
{
  Color newColor(color);
  // assuming m_basePalette is black and white...
  int mred = 31 - newColor.red();
  int mgreen = 31 - newColor.green();
  int mblue = 31 - newColor.blue();

  for (int i = 0; i < m_paletteLength; ++i)
  {
    // convert from grey scale to colour scale.
    Color c(m_basePalette[i]);

    // y |       ____   y = mx+c
    //  c|___----
    //   |___________ 
    //                x
    //  c = initial colour      (intercept)
    //  m = 31 - initial  / 32  (gradient)
    //  x = grey value

    int yred   = (mred * c.red())/32 + newColor.red();
    int ygreen = (mgreen * c.green())/32 + newColor.green();
    int yblue  = (mblue * c.blue())/32 + newColor.blue();
    c.red(yred);
    c.green(ygreen);
    c.blue(yblue);

    m_palette[i] = c;

  }
}

void TextArea::setPalette(const std::string & fileName)
{
  File palFile;
  palFile.open(fileName.c_str());
  // read the lot
  if (palFile.is_open())
  {
    int size = palFile.size();
    char * data = new char[size+2];
    palFile.read(data);
    data[size] = 0;
    m_palette = (unsigned short*) data;
    char * baseData = new char[size+2];
    copy(data, data+size, baseData);
    m_basePalette = (unsigned short*)baseData;
    m_paletteLength = size/2;
    Canvas & canvas( Canvas::instance());
    canvas.fillRectangle(0,0,canvas.width(), canvas.height(), m_palette[0]);
  } else {
    Canvas::instance().fillRectangle(130,0,10,128,Color(31,0,0));
  }
}

TextArea::~TextArea()
{
  delete m_font;
  delete [] m_palette;
}

void TextArea::setEncoding(const std::string & encoding)
{
  m_encoding = encoding;
}
