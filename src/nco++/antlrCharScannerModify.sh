#!/bin/sh
#
# Crude patch for the include file antlr/CharScanner.hpp
#
# it takes the folllowing C++ code line
#
# class ANTLR_API CharScannerLiteralsLess : public ANTLR_USE_NAMESPACE(std)binary_function<ANTLR_USE_NAMESPACE(std)string,ANTLR_USE_NAMESPACE(std)string,bool> {
#
#   and removes the   std::binary_function<std::string,std::string, bool>
#
# class ANTLR_API CharScannerLiteralsLess {
#

if [ $# -lt 2 ];then
  echo "usage: $0 <header-in> <header-out>"
  exit 1;
fi

sed -E  's/:[ \t]+public ANTLR_USE_NAMESPACE\(std\)binary_function<[^\>]+>//g' <$1 >$2
