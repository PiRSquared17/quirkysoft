#ifndef CookieWriter_h_seen
#define CookieWriter_h_seen

class Cookie;

class CookieWriter
{
  public:
    void operator()(Cookie *cookie);

    void remove(const char *domain);
};
#endif