/* $HEADER$
 * Copyright (c) 2004, Semantic Gap (TM)
 * http://www.semanticgap.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef UUID_HPP
#define UUID_HPP

#include <uuid.h>

class QString;

/** Represents a Universal Unique ID.
 */
class UUID
{
public:
  enum Exception {
    Ok = 0,
    InvalidArgument,
    OutOfMemory,
    SystemError,
    InternalError,
    NotImplemented
  };

  /** Constructs a new UUID object.
   * The constructed UUID is the Nil UUID unless the parameter is true.
   * @param make_uuid Controls whether or not to generate a new UUID. If it is false or left blank,
   * the UUID is set to the Nil UUID.
   */
  UUID(bool make_uuid = true);
  UUID(const unsigned char uuid[16]);
  UUID(const UUID &uuid);
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

  /** Destroys the UUID and makes the UUID Nil.
   */
  void create();

  /** Generates a new UUID.
   */
  void make();

  /** Copies a UUID into this.
   * @param uuid The UUID to copy.
   */
  void copy(const UUID &uuid);

  /** Exports the UUID as a string.
   * @return a QString representing the UUID
   */
  QString toString() const;
  /** Exports the UUID as an array.
   * @param array Array of bytes to store the UUID in.
   */
  void toArray(unsigned char array[16]) const;

  /** Loads a UUID from a string.
   * @param str QString that is storing a string representation of a UUID.
   */
  void fromString(const QString &str);
  /** Loads a UUID from an array of bytes.
   * @param array Array that is storing the binary reperesentation of a UUID.
   */
  void fromArray(const unsigned char array[16]);

  /** Equivalent to isEqual.
   */
  inline bool operator == (const UUID &uuid) const { return isEqual(uuid); }
  /** Equivalent to !isEqual.
   */
  inline bool operator != (const UUID &uuid) const { return !isEqual(uuid); }

  /** Returns a string that describes the exception.
   * @param e Exception to describe.
   * @return Descriptive string of the exception.
   */
  static const char *exceptionToString(Exception e);

protected:
  /** Destroys the uuid, setting it back to NULL.
   */
  void destroy();

private:
  /** Translates an internal error into an exception.
   * OSSP uuid uses its own type for error codes. This method
   * translates that error code into an Exception to thrown
   * when an error occurs.
   */
  static Exception errorToException(uuid_rc_t error);

  uuid_t *m_uuid;
};

#endif // UUID_HPP
