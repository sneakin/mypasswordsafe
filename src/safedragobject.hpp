/* $Header: /home/cvsroot/MyPasswordSafe/src/safedragobject.hpp,v 1.3 2004/11/01 21:34:58 nolan Exp $
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
#ifndef SAFEDRAGOBJECT_HPP
#define SAFEDRAGOBJECT_HPP

#include <qdragobject.h>
#include <qptrlist.h>
#include <qdom.h>
#include <qstring.h>

class SafeItem;

class SafeDragObject: public QDragObject
{
  Q_OBJECT;

public:
  SafeDragObject(QWidget *src = NULL);
  virtual ~SafeDragObject();

  void addItem(SafeItem *);

  virtual const char *format(int i = 0) const;
  virtual bool provides(const char *mime_type) const;
  virtual QByteArray encodedData(const char *mime_type) const;
  static bool canDecode(const QMimeSource *src);
  static bool decode(const QMimeSource *src, QDomDocument &xml);

private:
  QDomDocument m_xml;
  QString m_text;

  enum State { Nothing, Single, Multiple };
  State m_state;
};

#endif
