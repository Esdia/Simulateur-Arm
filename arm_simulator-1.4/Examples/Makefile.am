CC=$(GCC_CROSS)
CCAS=$(GCC_CROSS)
AM_CFLAGS=-mbig-endian
AM_CCASFLAGS=-mbig-endian
AM_LDFLAGS=-nostdlib -T $(srcdir)/linker_script -n --entry main
AM_LDFLAGS+=-Wl,-EB

if HAVE_ARM_COMPILER
noinst_PROGRAMS=example1 example2 example3 example4 insertion_sort test_data_processing test_branch test_PSR_transfer test_single_swap test_halfword_transfer test_multiply

all-am: $(PROGS)
endif

example1_SOURCES=example1.s
example2_SOURCES=example2.s
example3_SOURCES=example3.s
example4_SOURCES=example4.s
insertion_sort_SOURCES=insertion_sort.c
test_data_processing_SOURCES=test_data_processing.s
test_branch_SOURCES=test_branch.s
test_PSR_transfer_SOURCES=test_PSR_transfer.s
test_single_swap_SOURCES=test_single_swap.s
test_halfword_transfer_SOURCES=test_halfword_transfer.s
test_multiply_SOURCES=test_multiply.s

EXTRA_DIST=linker_script

clean-local:
	$(RM) $(PROGS)
