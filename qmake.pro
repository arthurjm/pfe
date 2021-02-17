TEMPLATE = subdirs

SUBDIRS =  \
        project \
        tests
project.file = src/fast_hierarchy.pro
tests.file = tests/gtest.pro

CONFIG += ordered
