# NCO-specific definitions
all: test_data
.PHONY: test_data
test_data = in.nc big.nc
.cdl.nc:
	ncgen -o $@ $<
