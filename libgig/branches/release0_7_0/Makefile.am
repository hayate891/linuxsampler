# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = src

# to prevent compile errors on some systems
AM_CXXFLAGS = -pedantic

# all man files that should be installed
man1_MANS = man/dlsdump.1 man/gigdump.1 man/gigextract.1 man/rifftree.1
# include man files into distribution (because they're not by default)
dist_MANS = man/dlsdump.1 man/gigdump.1 man/gigextract.1 man/rifftree.1
