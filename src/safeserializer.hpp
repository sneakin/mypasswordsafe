/* $HEADER$
 * Copyright (c) 2004, Semantic Gap (TM) <http://www.semanticgap.com/>
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
 *  -  Neither the name of Semantic Gap (TM) nor the
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

#include <qstring.h>

class Safe;

/** ABC that handles reading and writing Safes to various formats.
 * To create your own SafeSerializer, you need to derive a class
 * from SafeSerializer. SafeSerializer's constructor takes two
 * arguments, an extension and a name. You then need to define
 * "load", "save", and "checkPassword" for your serializer.
 * Your serializer will automatically be registered with the
 * factory and Safe::load and Safe::save will recognize it.
 */
class SafeSerializer
{
public:
  virtual ~SafeSerializer();

  /** Returns the extension that the serializer uses for files.
   */
  inline const QString &extension() { return m_extension; }
  /** Returns the name of the serializer.
   * This is used in file open/save dialogs.
   */
  inline const QString &name() { return m_name; }

  /** Checks the password of a safe.
   * @param path Filename to check
   * @param password Password that is being checked
   * @return Safe::Success if the password is correct
   */
  virtual Safe::Error checkPassword(const QString &path, const SecuredString &password) = 0;

  /** Loads a safe.
   * @param safe The safe that the data will be loaded into.
   * @param path Filename to open
   * @param passphrase Pass-phrase of the safe
   * @param def_user Default user name to use for certain items
   * @return Safe::Success if the safe was loaded, otherwise a Safe::Error condition
   *         corresponding to what happened.
   */
  virtual Safe::Error load(Safe &safe, const QString &path, const EncryptedString &passphrase, const QString &def_user) = 0;
  /** Saves a safe to a file.
   * @param safe The safe to save.
   * @param path The filename that the safe will be saved as.
   * @param def_user Default user name that is specified in the preferences.
   * @return Safe::Success if the safe was saved, otherwise a Safe::Error that
   *         that corresponds to what happened.
   */
  virtual Safe::Error save(Safe &safe, const QString &path, const QString &def_user) = 0;


protected:
  /** Called by subclasses to register with the factory.
   * @param ext File extension that the serializer uses
   * @param name Descriptive name of the serializer
   */
  SafeSerializer(const QString &ext, const QString &name);

private:
  QString m_extension, m_name;

  // Begin Factory methods
public:
  /** Creates a serializer given an extension.
   * @param ext The extension to find a serializer with. One extension
   *            can match many serializers.
   * @return SafeSerializer instance whose extension() == ext
   */
  static SafeSerializer *createByExt(const QString &ext);
  /** Returns the next serializer that has the same extension as
   * the parameter.
   * @param serializer The serializer to start searching from.
   * @return Next SafeSerializer that has the same extension, or NULL
   *         if there aren't anymore.
   */
  static SafeSerializer *getNextExt(SafeSerializer *serializer);

  /** Creates a serializer given a name.
   * @param name The name of the serializer. Only one serializer will
   *             match.
   * @return SafeSerializer instance whose name() == name
   */
  static SafeSerializer *createByName(const QString &name);

  typedef vector<SafeSerializer *> SerializerVec;
  /** Returns the vector of all the serializers.
   */
  static const SerializerVec &serializers() { return m_serializers; }

  /** Returns a multilined string with all the serializer names.
   */
  static QString getTypes();
  /** Returns a multilined string with all the extensions that are
   * registered.
   */
  static QString getExtensions();
  /** Returns the extension for the given name.
   */
  static const QString getExtForName(const QString &type);

protected:
  /** Registers a SafeSerializer with the factory.
   */
  static bool add(SafeSerializer *serializer);
    
private:
  static SerializerVec m_serializers;
};

#endif // SAFESERIALIZER_HPP
