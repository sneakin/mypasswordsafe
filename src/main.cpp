/* $Header: /home/cvsroot/MyPasswordSafe/src/main.cpp,v 1.3 2004/11/01 17:35:51 nolan Exp $
 * Copyright (c) 2004, Semantic Gap Solutions
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
#include <iostream>
#include <qapplication.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <qmime.h>
#include <qimage.h>
#include "config.h"
#include "safe.hpp"
#include "mypasswordsafe.h"
#include "startupdlg.hpp"

using namespace std;

const char *locale_dir = PREFIX "/share/MyPasswordSafe/locale";

bool doStartupDialog(MyPasswordSafe *myps, int argc, char *argv[])
{
  StartupDlg dlg(myps);

  if(argc > 1) {
    // use the last argument
    dlg.setFilename(argv[argc - 1]);
    dlg.setAction(StartupDlg::Browse);
  }
  else if(myps->getDefaultSafe().length() > 0) {
    dlg.setAction(StartupDlg::OpenDefault);
  }
  else {
    dlg.setAction(StartupDlg::CreateNew);
  }

  if(dlg.exec() == StartupDlg::Rejected)
    return false;
  else
    return true;
}

int main( int argc, char ** argv )
{
  QApplication a( argc, argv );
  MyPasswordSafe myps;

  QTranslator qt(0);
#ifdef DEBUG
  cout << "Using locale: " << QTextCodec::locale() << endl;
#endif
  qt.load(QString("qt_") + QTextCodec::locale(), locale_dir);
  a.installTranslator(&qt);
  QTranslator myapp(0);
  if(!myapp.load(QString("mypasswordsafe_") + QTextCodec::locale(),
		 locale_dir)) {
#ifdef DEBUG
    cout << "No locale file for " << QTextCodec::locale()
	 << " found in " << locale_dir << endl;
#endif
  }
  else {
    a.installTranslator(&myapp);
  }

  if(!doStartupDialog(&myps, argc, argv)) {
    return 0;
  }

  myps.show();
  a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
  return a.exec();
}
