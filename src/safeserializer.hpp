/* $HEADER$
 * Copyright (c) 2004, Semantic Gap <http://www.semanticgap.com/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted provided
 * that the following conditions are met:
 *  -  Redistributions of source code must retain the
 *     above copyright notice, this list of conditions
 *     and the following disclaimer.
 *  -  Redistributions in binary form must reproduce the
 *     above copyright notice, this list of conditions and
 *     the following disclaimer in the documentation and/or
 *     other materials provided with the distribution.
 *  -  Neither the name of Semantic Gap Solutions nor the
 *     names of its contributors may be used to endorse or
 *     promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SAFESERIALIZER_HPP
#define SAFESERIALIZER_HPP

using std::string;
class Safe;

/** ABC that handles reading and writing Safes to various formats.
 */
class SafeSerializer
{
public:
  virtual ~SafeSerializer();

  inline const char *extension() { return m_extension.c_str(); }
  inline const char *description() { return m_description.c_str(); }
  // Returns the string to be used in file open/save dialogs

  virtual Safe::Error checkPassword(const string &path, const SecuredString &password) = 0;
  // returns true if password can open path, false if it can not

  virtual Safe::Error load(Safe &safe, const string &path, const EncryptedString &passphrase, const string &def_user) = 0;
  // loads the file specified by path into safe return true on success,
  // false on failure
  virtual Safe::Error save(Safe &safe, const string &path, const string &def_user) = 0;
  // saves safe to the file specified by path. returns true if success,
  // false on failure

protected:
  SafeSerializer(const char *ext, const char *description);

private:
  string m_extension, m_description;

  // Begin Factory methods
public:
  static SafeSerializer *createByExt(const char *ext);
  static SafeSerializer *createByName(const char *name);
  // public interface to the SafeSerializer factory. Returns a pointer
  // to a SafeSerializer that handles files with the specified
  // file extension
    
  static SafeSerializer *getNextExt(SafeSerializer *serializer);

  typedef vector<SafeSerializer *> SerializerVec;
  static const SerializerVec &serializers() { return m_serializers; }
  // returns the std::map of serializers. The map is a pair made of
  // extension and the pointer to the serializer.
  static string getTypes();
  static string getExtensions();
  static const char *getExtForType(const char *type);

protected:
  static bool add(SafeSerializer *serializer);
  // register a SafeSerializer to handle the ext extension with the factory
    
private:
  static SerializerVec m_serializers;
};

#endif // SAFESERIALIZER_HPP
