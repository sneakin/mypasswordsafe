// BlowFish.h
//-----------------------------------------------------------------------------
#ifndef PW_BLOWFISH_H
#define PW_BLOWFISH_H

#include <stdint.h>
#include "crypto/cryptointerface.hpp"

#define MAXKEYBYTES 56 // unused

typedef unsigned char block[8];


union aword
{
   uint32_t word;
   unsigned char byte [4];
   struct
   {
      unsigned int byte3:8;
      unsigned int byte2:8;
      unsigned int byte1:8;
      unsigned int byte0:8;
   } w;
};

class BlowFish: public CryptoInterface
{
public:
   BlowFish(unsigned char* key, int keylen);
  virtual ~BlowFish();
   void encrypt(const block in, block out);
   void decrypt(const block in, block out);
  enum {bf_N = 16};
private:
  // Following are global supposedly for performance reasons. TBV...
  /*static*/ uint32_t bf_S[4][256];
  /*static*/ uint32_t bf_P[bf_N + 2];
  static const uint32_t tempbf_S[4][256];
  static const uint32_t tempbf_P[bf_N + 2];
  void Blowfish_encipher(uint32_t* xl, uint32_t* xr);
  void Blowfish_decipher(uint32_t* xl, uint32_t* xr) const;
  void InitializeBlowfish(unsigned char key[], short keybytes);
};

#endif
//-----------------------------------------------------------------------------
// Local variables:
// mode: c++
// End:
