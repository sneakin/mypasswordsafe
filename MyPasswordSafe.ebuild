# Copyright 2004 Semantic Gap (TM) Solutions
# Distributed under the terms of the GNU General Public License v2
# $Header: /home/cvsroot/MyPasswordSafe/Attic/MyPasswordSafe.ebuild,v 1.1 2004/08/13 03:56:34 nolan Exp $

DESCRIPTION="MyPasswordSafe is a password manager compatible with Password Safe."
HOMEPAGE="http://www.semanticgap.com/myps/"

#SRC_URI="http://www.semanticgap.com/myps/release/${P}.src.tgz"
SRC_URI="http://localhost/~nolan/${P}.src.tgz"

LICENSE="BSD"

SLOT="0"
KEYWORDS="~x86 ~ppc"

IUSE="X qt"

DEPEND=">=x11-libs/qt-3.3.2"

src_compile() {
	addwrite "$HOME/.qt"
	addwrite "$QTDIR/etc/settings"
	emake PREFIX=/usr all || die "emake failed"
}

src_install() {
	make PREFIX=${D}/usr install || die
}
