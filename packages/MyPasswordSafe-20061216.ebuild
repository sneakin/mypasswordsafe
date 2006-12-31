# Copyright 2004 Semantic Gap (TM) Solutions
# Distributed under the terms of the GNU General Public License v2
# $Header: /home/cvsroot/MyPasswordSafe/packages/MyPasswordSafe-20061216.ebuild,v 1.1 2006/12/31 20:03:43 nolan Exp $

inherit qt3

DESCRIPTION="MyPasswordSafe is a password manager compatible with Password Safe."
HOMEPAGE="http://www.semanticgap.com/myps/"

SRC_URI="http://www.semanticgap.com/myps/release/${P}.src.tgz"
#SRC_URI="http://localhost/~nolan/${P}.src.tgz"

LICENSE="BSD"

SLOT="0"
KEYWORDS="~x86 ~ppc"

IUSE="X"

DEPEND="$(qt_min_version 3.3.2)"

src_compile() {
	addwrite "$HOME/.qt"
	addwrite "$QTDIR/etc/settings"
	emake PREFIX=/usr all || die "emake failed"
}

src_install() {
	make PREFIX=${D}/usr install || die
}
