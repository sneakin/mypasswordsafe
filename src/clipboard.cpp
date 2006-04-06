/*
 * clipboard.cpp - Handles the clipboard securely (ie: clears it)
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
#include <qapplication.h>
#include <qtimer.h>
#include <qclipboard.h>
#include "clipboard.hpp"

unsigned int Clipboard::m_time_out = 60 * 1000; // default to 1 minute

class Clipboard::Private
{
public:
	Private(Clipboard *parent)
	{
		cb = QApplication::clipboard();

		clear_timer = new QTimer(parent, "clear_timer");
		connect(clear_timer, SIGNAL(timeout()), parent, SLOT(timerFired()));
	}

	~Private()
	{
		delete clear_timer;
	}

	QClipboard *cb;
	QTimer *clear_timer;
	QString last_copy;
};

Clipboard::Clipboard()
{
	d = new Private(this);
}

Clipboard::~Clipboard()
{
	delete d;
}

Clipboard *Clipboard::instance()
{
	static Clipboard *inst = NULL;
	if(inst == NULL) {
		inst = new Clipboard;
	}

	return inst;
}

unsigned int Clipboard::timeOut()
{
	return m_time_out;
}

void Clipboard::setTimeOut(unsigned int t)
{
	m_time_out = t;

	if(m_time_out == 0) {
		instance()->stop();
	}
}

void Clipboard::start()
{
	d->clear_timer->start(timeOut(), true);
}

void Clipboard::stop()
{
	d->clear_timer->stop();
}

void Clipboard::timerFired()
{
	if(d->last_copy == d->cb->text(QClipboard::Clipboard)
	   || d->last_copy == d->cb->text(QClipboard::Selection)) {
		clear();
	}
}

void Clipboard::clear() 
{
	d->cb->clear();
	stop();
	emit cleared();
}

void Clipboard::copy(const QString &data, bool start_timer)
{
	d->last_copy = data;

	d->cb->setText(data, QClipboard::Clipboard);

	if(d->cb->supportsSelection()) {
		d->cb->setText(data, QClipboard::Selection);
	}

	if(start_timer) {
		start();
	}
	else {
		stop();
	}
}
