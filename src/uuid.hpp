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
#ifndef UUID_HPP
#define UUID_HPP

#include <uuid.h>

using std::string;

/** Represents a Universal Unique ID.
 */
class UUID
{
public:
  /** Constructs a new UUID object.
   * The constructed UUID is the Nil UUID unless the parameter is true.
   * @param make_uuid Controls whether or not to generate a new UUID. If it is false or left blank,
   * the UUID is set to the Nil UUID.
   */
  UUID(bool make_uuid = false);
  UUID(UUID &uuid); //! @note this should be const
  ~UUID();

  /** Used to check if the UUID is the Nil UUID.
   * @return true if the UUID is the Nil UUID, otherwise false.
   */
  bool isNil() const;
  /** Used to check if two UUID objects are the same.
   * @param uuid The UUID that is being checked.
   * @return true if the parameter equals this UUID.
   */
  bool isEqual(const UUID &uuid) const;
  /** Checks the objects error condition.
   * @return true if the object is ok, false if there has been an error.
   */
  bool isOk() const;

  /** Generates a new UUID.
   */
  void make();
  /** Exports the UUID as a string.
   * @return a std::string representing the UUID
   */
  string toString();
  /** Exports the UUID as an array.
   * @param array Array of bytes to store the UUID in.
   */
  void toArray(unsigned char array[16]);

  /** Loads a UUID from a string.
   * @param str std::string that is storing a string representation of a UUID.
   */
  void fromString(const string &str);
  /** Loads a UUID from an array of bytes.
   * @param array Array that is storing the binary reperesentation of a UUID.
   */
  void fromArray(unsigned char array[16]);

  inline bool operator == (const UUID &uuid) const { return isEqual(uuid); }
  inline bool operator != (const UUID &uuid) const { return !isEqual(uuid); }

private:
  uuid_t *m_uuid;
  uuid_rc_t m_error;
};

#endif // UUID_HPP
