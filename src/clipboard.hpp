/*
 * clipboard.hpp - Handles the clipboard securely (ie: clears it)
 * Copyright (c) 2004 Nolan Eakins <sneakin@semanticgap.com>
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
 *
 */

#ifndef CLIPBOARD_HPP
#define CLIPBOARD_HPP

#include <qobject.h>

class Clipboard: public QObject
{
	Q_OBJECT;
protected:
	Clipboard();

public:
	static unsigned int m_time_out;

	~Clipboard();

	static Clipboard *instance();

	/** Returns the number of milliseconds used for the
	 * clear timer.
	 */
	static unsigned int timeOut();

	/** Sets the number of milliseconds used for the
	 * clear timer.
	 */
	static void setTimeOut(unsigned int);

public slots:
	/** Clears the clipboard and stops the timer.
	 */
	void clear();

	/** Copies data to the clipboard and possibly starts
	 * the timer.
	 */
	void copy(const QString &, bool start_timer = false);

	void start();
	void stop();

protected slots:
	void timerFired();

signals:
	void cleared();

private:
	class Private;
	Private *d;
};

#endif /* CLIPBOARD_HPP */
