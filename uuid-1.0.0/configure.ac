dnl ##
dnl ##  OSSP uuid - Universally Unique Identifier
dnl ##  Copyright (c) 2004 Ralf S. Engelschall <rse@engelschall.com>
dnl ##  Copyright (c) 2004 The OSSP Project <http://www.ossp.org/>
dnl ##
dnl ##  This file is part of OSSP uuid, a library for the generation
dnl ##  of UUIDs which can found at http://www.ossp.org/pkg/lib/uuid/
dnl ##
dnl ##  Permission to use, copy, modify, and distribute this software for
dnl ##  any purpose with or without fee is hereby granted, provided that
dnl ##  the above copyright notice and this permission notice appear in all
dnl ##  copies.
dnl ##
dnl ##  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
dnl ##  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
dnl ##  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
dnl ##  IN NO EVENT SHALL THE AUTHORS AND COPYRIGHT HOLDERS AND THEIR
dnl ##  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
dnl ##  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
dnl ##  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
dnl ##  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
dnl ##  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
dnl ##  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
dnl ##  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
dnl ##  SUCH DAMAGE.
dnl ##
dnl ##  configure.ac: GNU Autoconf source script
dnl ##

AC_PREREQ(2.53)
AC_INIT
UUID_VERSION_STR=`./shtool version -l c -d long uuid_vers.h`
UUID_VERSION_HEX=`./shtool version -l c -d hex  uuid_vers.h`
./shtool echo -e "Configuring %BOSSP uuid%b (Universally Unique Identifier), version %B${UUID_VERSION_STR}%b"
AC_SUBST(UUID_VERSION_STR)
AC_SUBST(UUID_VERSION_HEX)

AC_PROG_MAKE_SET
AC_PROG_CC
AC_CHECK_DEBUGGING

m4_defun([AC_PROG_CXX])
m4_defun([AC_PROG_CXXCPP])
m4_defun([AC_PROG_F77])
with_tags=""
sinclude(libtool.m4)
AC_PROG_LIBTOOL

sinclude(uuid.ac)
UUID_CHECK_ALL

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_FILES([Makefile uuid-config uuid.h])
AC_CONFIG_COMMANDS([adjustment], [chmod a-w uuid.h; chmod a+x uuid-config])
AC_OUTPUT

