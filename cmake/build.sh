#! /bin/sh
cmake .. \
-DANTLR_INCLUDE:PATH=/home/pvicente/install/antlr-2.7.7/include \
-DANTLR_LIBRARY:FILE=/home/pvicente/install/antlr-2.7.7/lib/libantlr.a \
-DGSL_INCLUDE:PATH=/home/pvicente/install/gsl-1.16/include \
-DGSL_LIBRARY:FILE=/home/pvicente/install/gsl-1.16/lib/libgsl.a
