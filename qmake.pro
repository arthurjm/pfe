CONFIG   += release

TEMPLATE  = subdirs

SUBDIRS   = src

src.file = src/app.pro

debug {
	SUBDIRS     += tests
	tests.file = tests/gtest.pro
}
 


