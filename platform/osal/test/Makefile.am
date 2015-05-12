include $(top_srcdir)/osal.am

LIB_OSAL=../posix_src/libosalposix.a ../src/libosal.a
testdir=test
#bin_PROGRAMS =  

test_PROGRAMS = testoe

testoe_SOURCES = testtest.c testoe.c testcase.c opentest.c threadtest.c mutextest.c testprint.c
testoe_LDADD = $(LIB_OSAL)	

