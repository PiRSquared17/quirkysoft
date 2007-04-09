#ifndef DocumentTest_h_seen
#define DocumentTest_h_seen

#include <cppunit/extensions/HelperMacros.h>

class Document;
class DocumentTest : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( DocumentTest );
  CPPUNIT_TEST( test0 );
  CPPUNIT_TEST( test1 );
  CPPUNIT_TEST( testHtmlAttributes );
  CPPUNIT_TEST( testHead );
  CPPUNIT_TEST( testEmpty );
  CPPUNIT_TEST( testHead2 );
  CPPUNIT_TEST( testTitle );
  CPPUNIT_TEST( testHead3 );
  CPPUNIT_TEST( testGoogle );
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp();
  void tearDown();
  void test0();
  void test1();
  void testHtmlAttributes();
  void testHead();
  void testEmpty();
  void testHead2();
  void testTitle();
  void testHead3();
  void testGoogle();
private:
  Document * m_document;
  char * m_data;
  unsigned int m_length;
  void readFile(const char * fileName);
};

#endif