#if 0
int EncryptedString::read(FILE *fp, unsigned char *cbcbuffer)
{
  if(m_data != NULL) {
    trashAndDelete();
  }

  int numRead = 0;

  unsigned char lengthblock[8];
  unsigned char lcpy[8];

  int buffer_len = 0;
  numRead = fread(lengthblock, 1, sizeof lengthblock, fp);
  if (numRead != 8)
    return 0;
  memcpy(lcpy, lengthblock, 8);

  //BlowFish *Algorithm = MakeBlowFish(pass, passlen, salt, saltlen);

  m_algor->Decrypt(lengthblock, lengthblock);
  xormem(lengthblock, cbcbuffer, 8);
  memcpy(cbcbuffer, lcpy, 8);
  memcpy(m_cbc, lcpy, 8);

  int length = getInt32( lengthblock );
  //m_type = (Type)lengthblock[sizeof(int)];

  trashMemory(lengthblock, 8);
  trashMemory(lcpy, 8);

  if (length < 0) {
    //delete Algorithm;
    m_data = NULL;
    m_length = 0;
    return 0;
  }
  m_length = length;
  int BlockLength = ((m_length+7)/8)*8;
  // Following is meant for lengths < 8,
  // but results in a block being read even
  // if length is zero. This is wasteful,
  // but fixing it would break all existing databases.
  if (BlockLength == 0)
    BlockLength = 8;

  buffer_len = m_length;
  m_data = new unsigned char[BlockLength]; // so we lie a little...

  unsigned char tempcbc[8];
  numRead += fread(m_data, 1, BlockLength, fp);
  for (int x=0;x<BlockLength;x+=8)
    {
      memcpy(tempcbc, m_data+x, 8);
      //m_algor->Decrypt(buffer+x, buffer+x);
      //xormem(buffer+x, cbcbuffer, 8);
      memcpy(cbcbuffer, tempcbc, 8);
    }
	
  trashMemory(tempcbc, 8);
  //delete Algorithm;
  if (m_length == 0) {
    // delete[] buffer here since caller will see zero length
    trashAndDelete();
  }

  hash();
  return numRead;
}

int EncryptedString::write(FILE *fp, unsigned char *cbcbuffer)
{
   int numWritten = 0;

   int BlockLength = ((m_length+7)/8)*8;
   if (BlockLength == 0)
      BlockLength = 8;

   // First encrypt and write the length of the buffer
   unsigned char lengthblock[8];
   memset(lengthblock, 0, 8);
   putInt32( lengthblock, m_length );

   //lengthblock[sizeof(int)] = (unsigned char)m_type;

   xormem(lengthblock, cbcbuffer, 8); // do the CBC thing
   m_algor->Encrypt(lengthblock, lengthblock);
   memcpy(cbcbuffer, lengthblock, 8); // update CBC for next round

   numWritten = fwrite(lengthblock, 1, 8, fp);

   trashMemory(lengthblock, 8);

   // Now, encrypt and write the buffer
   unsigned char curblock[8];
   // NOTE: the string gets decrypted here
   // It could be possible to do the decryption and encryption
   // along side each other
   SecuredString data(get());
   const unsigned char *buffer = (const unsigned char *)data.get();

   for (int x=0;x<BlockLength;x+=8)
   {
      if ((m_length == 0) || ((m_length%8 != 0) && (m_length-x<8)))
      {
         //This is for an uneven last block
         memset(curblock, 0, 8);
         memcpy(curblock, buffer+x, m_length % 8);
      }
      else
         memcpy(curblock, buffer+x, 8);
      xormem(curblock, cbcbuffer, 8);
      m_algor->Encrypt(curblock, curblock);
      memcpy(cbcbuffer, curblock, 8);
      numWritten += fwrite(curblock, 1, 8, fp);
   }
   trashMemory(curblock, 8);
   return numWritten;
}
#endif

struct SafeCleaner
{
  void operator () (Safe::ItemList::reference r)
  {
    if(r != NULL) {
      delete r;
    }
  }
};

void trimRight(string &str)
{
  string::iterator i = str.end();
  for(; i != str.begin(); i--) {
    if(!isspace(*i) && *i != '\0')
      break;
  }
  str.erase(i+1, str.end());
}

void trimLeft(string &str)
{
  string::iterator i = str.begin();
  for(; i != str.end(); i++) {
    if(!isspace(*i))
      break;
  }
  str.erase(str.begin(), i);
}

vector<string> split(string &s, int c)
{
  vector<string> ret;
  string::iterator start = s.begin();
  string::iterator i = s.begin();
  for(;
      i != s.end();
      i++) {
    if(*i == c) {
      if(*start == c)
	start++;
      string item(start, i);
      ret.push_back(item);
      start = i;
    }
  }

  if(start != s.end() && start != i) {
    if(*start == c)
      start++;
    string item(start, i);
    ret.push_back(item);
  }

  return ret;
}

string getExtension(const string &path)
{
  string type(path);
  string::iterator i = type.end();
  for(; i != type.begin(); i--) {
    if(*i == '.')
      break;
  }

  if(i != type.begin()) {
    type.erase(type.begin(), (i+1));
    return type;
  }
  else
    return string("");
}
