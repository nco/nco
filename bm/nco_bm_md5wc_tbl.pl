# this file is valid perl code that populates the MD5_table for validating output files.
# Perl semantics must be maintained.
# $Header$

# the md5 table needs to be populated with the md5 checksums for the output files
# generated with the no-history option.
# as in these examples - each key is the name of the test.  The md5 checksum is
# the hash of the entire output file generated with the -h flag (prevents a history
# from being written to the end of the file and therefore changing the md5.
# also have to change the benchmarks to add this flag.

##
## %MD5_tbl is a hash that contains the wordcount validation codes
##

%MD5_tbl = ( # holds md5 hashes for each test
# Benchmark entries
"ncap long algebraic operation_0" => "7037002c30744e71565bc3f07bfe718a", #MD5
"ncap long algebraic operation_1" => "fceea540e58e657e0c4f16bb8928f447", #MD5
"ncbo differencing two files_0" => "9fb75945653ce8fafb74eca0997fcc3d", #MD5
"ncea averaging 2^5 files_0" => "fe74d3a77118b7d5a66ab9fa0fea03ae", #MD5
"ncea averaging 2^5 files_1" => "f58c721ff4d2d5f13949ab3f3ecf8453", #MD5
"ncecat joining 2^5 files_0" => "e311755e9f01e427418ef8020155d8b1", #MD5
"ncecat joining 2^5 files_1" => "fbb1b621cdfe1daa1841727423a24138", #MD5
"ncflint weight-averaging 2 files_0" => "7037002c30744e71565bc3f07bfe718a", #MD5
"ncflint weight-averaging 2 files_1" => "38ee6518c570a9aa59f4aaa9d93d801d", #MD5
"ncpdq dimension-order reversal_0" => "1999df64e07a9346eb12da4219156b1d", #MD5
"ncpdq dimension-order re-ordering_0" => "1999df64e07a9346eb12da4219156b1d", #MD5
"ncpdq dimension-order re-ordering & reversing_0" => "da972b99c8cb2b059d4d54b346f59cc3", #MD5
"ncpdq packing a file_0" => "da41b3b24dfe369097c68dea9521119c", #MD5
"ncra time-averaging 2^5 (i.e. one month) ipcc files_0" => "e2853478c04242b29c74bf983b38e5e4", #MD5
"ncrcat joining 2^5 files_0" => "4f3582ac41db57a506be62efae964d4d", #MD5
"ncrcat joining 2^5 files_1" => "b1656d04ce999855f09a58b3fedef4f3", #MD5
"ncwa averaging all variables to scalars - stl_5km.nc & sqrt_0" => "b6cb85aceb699670c2eca197ebbe10b4", #MD5
"ncwa averaging all variables to scalars - stl_5km.nc & rms_0" => "250352270a64b65194000165061b1df1",#MD5
"ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt_0" => "38ee6518c570a9aa59f4aaa9d93d801d", #MD5

# Regression entries
"Testing float modulo float_0" => "8887503481d53aa13ea8bab73bca280e", #MD5
"Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)_0" => "551d25425d4c9bd69932ccbbe6b3e63c", #MD5
"Testing foo=log(e_dbl)^1_0" => "3bca0c79dec8371bf39f3059a4d2c078", #MD5
"Testing foo=4*atan(1)_0" => "2d579cd7cce9c45299abbdf9810933bf", #MD5
"Testing foo=erf(1)_0" => "e72aa31bdba32c0cd496bb2617228b5e", #MD5
"Testing foo=gamma(0.5)_0" => "e2fba9c54d9019c2e4354c71303186a3", #MD5_table
"Testing foo=sin(pi/2)_0" => "3436044dca29ccab26c25396a882d1f4", #MD5
"Testing foo=cos(pi)_0" => "44fb1ba7a59b55d825c15983dc4fbc31", #MD5
"Modify all existing units attributes to meter second-1_0" => "ee887abdddef87cefb6ed35e8ea23497", #MD5
"difference scalar missing value_0" => "df9d318631a4e2b914c0fe487dd5eda0", #MD5
"difference with missing value attribute_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference without missing value attribute_0" => "30a8a849af6540e1e92ea794d917c30c", #MD5
"missing_values differ between files_0" => "31f5a7b995a103e8cf70010f8aed1185", #MD5
"missing_values differ between files_1" => "9042d513e4aebe9b476665a13d0ac1c6", #MD5
"missing_values differ between files_2" => "88fff81701125f219866deaa4de5f704", #MD5
"ncdiff symbolically linked to ncbo_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference with missing value attribute_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference without missing value attribute_0" => "30a8a849af6540e1e92ea794d917c30c", #MD5
"ensemble mean of int across two files_0" => "d2e3e8911d679325904f4a31ed5cc807", #MD5
"ensemble mean with missing values across two files_0" => "5d0fbbd95cca6c1fd34432129e10c3ff", #MD5
"ensemble min of float across two files_0" => "f323ac17d08a3d94683986ddb92007de", #MD5
"scale factor + add_offset packing/unpacking_0" => "673cc9518a67aa8898dd8c40e1eb54c4", #MD5
"concatenate two files containing only scalar variables_2" => "5b8dd29c4f22b57737e3ac1f00ffc864", #MD5
"identity weighting_0" => "4cc63c64f5bf6147a979ce5bccf00337", #MD5
"identity interpolation_1" => "abc71f105ac3a450086b6c86731c77f5", #MD5
"identity interpolation_2" => "daf6017dc89568c34573569bf077636c", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_0" => "1318ba0f045ebb96283f1d9984b97c97", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_1" => "16b03dc221613d911e32215203207e58", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_2" => "84ea0b4116e01c03be5250d7023ec717", #MD5
"extract a dimension_0" => "f8634d6bef601cda97cff5f487114baf", #MD5
"extract a variable with limits_0" => "fe01ea80027e2988f345eb831edc7477", #MD5
"extract variable of type NC_INT_0" => "b861ca6eddfc8199cd18c0fe1b935eaa", #MD5
"Multi-slab lat and lon with srd_0" => "5740ff522eaf09023c844fcadc4a03b5", #MD5
"Multi-slab with redundant hyperslabs_0" => "e5c0cfa23955d8396d84224a8a3cc790", #MD5
"Multi-slab with coordinates_0" => "e5c0cfa23955d8396d84224a8a3cc790", #MD5
"Double-wrapped hyperslab_0" => "9cb9a60c5a357a1380716ec9e5dfcaa7", #MD5
"dimension slice using UDUnits library (fails without UDUnits library support)_0" => "9275a118e47d8d08aa7ce1986bfb0de6", #MD5
"variable wildcards A (fails without regex library)_0" => "7710233e83d78c8cc6b6447637d1e672", #MD5
"variable wildcards B (fails without regex library)_0" => "5bbf844271063c19a415e10fd3a95ad9", #MD5
"Offset past end of file_0" => "6492bf368d38ba8135ec82d71f733be7", #MD5
"reverse coordinate_0" => "80d0710f2275c530458556fd9eea6684", #MD5
"reverse three dimensional variable_0" => "468aad3c93e9137b8d360dbbf2e83475", #MD5
"re-order three dimensional variable_0" => "6e87bf2c6a8c78e2b6b0f5dbd911aed8", #MD5
"Pack and then unpack scalar (uses only add_offset)_0" => "3be2035918afec462b682a73016e3942", #MD5
"Pack and then unpack scalar (uses only add_offset)_1" => "17129104117daf91894e78bac99ed20b", #MD5
"record mean of int across two files_0" => "eb87ddae5a4f746c1370895573d80b1c", #MD5
"record mean of float with double missing values_0" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record mean of float with integer missing values_0" => "664e939df5248124fa70fd18445e18ac", #MD5
"record mean of integer with integer missing values_0" => "b982a5194ab6dc6b2d39a84ee18f34f9", #MD5
"record mean of integer with float missing values_0" => "2bea917d948229efe191e6aa5575d789", #MD5
"record mean of packed double with double missing values_0" => "d5c51a50a5a1f815a5aa181ccd7038ea", #MD5
"record mean of packed double to test precision_0" => "5681a63c98c2a26e85d658e8daed11f3", #MD5
"record mean of packed float to test precision_0" => "b91c2683c438476c3cfa347eb17b9d58", #MD5
"record mean of float with double missing values across two files_0" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record min of float with double missing values across two files_0" => "e122f17b034a0b0aec63c9b711e9ae8a", #MD5
"record max of float with double missing values across two files_0" => "d248e388229b4702029cfab576066936", #MD5
"record ttl of float with double missing values across two files_0" => "2b4a42a1bf202909da034ac2a44b755c", #MD5
"record rms of float with double missing values across two files_0" => "da5d943a97fedf07d506a36911855371", #MD5
"record sdn of float with double missing values across two files_1" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record sdn of float with double missing values across two files_2" => "24b1cb6803df6a6e00b087e1e46aac42", #MD5
"record sdn of float with double missing values across two files_3" => "18978d9c6da90b833b23338573bba3a0", #MD5
"record sdn of float with double missing values across two files_4" => "25051141b7b891e167d0b7be6eed1e82", #MD5
"normalize by denominator upper hemisphere_0" => "149b93703aaab1033e912968ed7df075", #MD5
"do not normalize by denominator_0" => "fdbf846378041855856e589cef610999", #MD5
"average with missing value attribute_0" => "b2225e092b4c572675c7075a53be775c", #MD5
"average without missing value attribute_0" => "13c674e187bc6dd62c3064cda4e77fd9", #MD5
"average masked coordinate_0" => "cb384ac035d17d47fa24c276c165a686", #MD5
"average masked variable_0" => "f00e2747fa2be0852454fef5d6cfd301", #MD5
"average masked, weighted coordinate_0" => "3484a9001e715142132440fac325d1b9", #MD5
"average masked, weighted variable_0" => "8fbd06664f1bc309b95ad3bf2269fdc0", #MD5
"weight conforms to var first time_0" => "0f39db04b2c10623c77b79830e14fef4", #MD5
"average all missing values with weights_0" => "fdf6393eba06d9d233425c7e90a37371", #MD5
"average some missing values with unity weights_0" => "8f8b34bec9190555a726b721f9d0c8c1", #MD5
"average masked variable with some missing values_0" => "552eded30274728bbb84044f510be3f9", #MD5
"min switch on type double, some missing values_0" => "81a242bf078958264585b5909f7b046f", #MD5
"Dimension reduction with min switch and missing values_0" => "e11acafc1bc5b1bcc2156010be01452d", #MD5
"Dimension reduction with min switch and missing values_1" => "e11acafc1bc5b1bcc2156010be01452d", #MD5
"Dimension reduction on type int with min switch and missing values_0" => "514ed9168c45eabf4011d2a2e8c6a8ef", #MD5
"Dimension reduction on type int with min switch and missing values_1" => "514ed9168c45eabf4011d2a2e8c6a8ef", #MD5
"Dimension reduction on type short variable with min switch and missing values_0" => "d68a97fec1c4c774116458c74c53b31f", #MD5
"Dimension reduction on type short variable with min switch and missing values_1" => "d68a97fec1c4c774116458c74c53b31f", #MD5
"Dimension reduction with min flag on type float variable_0" => "4cf3f52ee7529ce8276484262ce3f2d7", #MD5
"Max flag on type float variable_0" => "f714106cc99bb9b1ef2430c9b164b849", #MD5
"Dimension reduction on type double variable with max switch and missing values_0" => "c9fa752d585240d8881cde3fab5a36f6", #MD5
"Dimension reduction on type double variable with max switch and missing values_1" => "c9fa752d585240d8881cde3fab5a36f6", #MD5
"Dimension reduction on type int variable with min switch and missing values_0" => "53ac188efcf21756274e0299adbcb96f", #MD5
"Dimension reduction on type int variable with min switch and missing values_1" => "53ac188efcf21756274e0299adbcb96f", #MD5
"Dimension reduction on type short variable with max switch and missing values_0" => "2b5a5972ffce0cef0a9a6ae6f31a97c0", #MD5
"Dimension reduction on type short variable with max switch and missing values_1" => "2b5a5972ffce0cef0a9a6ae6f31a97c0", #MD5
"rms with weights_0" => "4efa5f65ddfa4dd571c1073d8e877bf9", #MD5
"weights would cause SIGFPE without dbl_prc patch_0" => "6a3e8ecb9768cb2a5132637e6a28a17b", #MD5
"avg would overflow without dbl_prc patch_0" => "e4d70a9fd9b8b823cf43301555434e2c", #MD5
"ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)_0" => "d12212aa41e3fef4ea7c2cdb718ee647", #MD5
"min with weights_0" => "0fcc1eb5239a4658cf8af2a0692fb5bf", #MD5
"max with weights_0" => "7855d7111ad8d937fa9ab4df171f2902", #MD5
"running ncap.in script into nco_tst.pl_0" => "", #MD5
);

##
##  %wc_tbl is a hash that contains the wordcount validation codes
##

%wc_tbl = ( # word count table for $dta_dir/wc_out
# Benchmark entries
"ncap long algebraic operation_0" => "111111 338754 5872990", #wc
"ncap long algebraic operation_1" => "9480 30734 410027", #wc
"ncbo differencing two files_0" => "111111 338754 5542049", #wc
"ncea averaging 2^5 files_0" => "111111 333695 5528455", #wc
"ncea averaging 2^5 files_1" => "47 323 1862", #wc
"ncecat joining 2^5 files_0" => "111111 333665 4667532", #wc
"ncecat joining 2^5 files_1" => "42 302 2000", #wc
"ncflint weight-averaging 2 files_0" => "111111 338754 5872990", #wc
"ncflint weight-averaging 2 files_1" => "9479 30721 409885", #wc
"ncpdq dimension-order reversal_0" => "111111 338754 5872990", #wc
"ncpdq dimension-order re-ordering_0" => "111111 338754 5872990", #wc
"ncpdq dimension-order re-ordering & reversing_0" => "111111 338754 5873056", #wc
"ncpdq packing a file_0" => "111111 339842 4964039", #wc
"ncrcat joining 2^5 files_0" => "111111 222534 3556822", #wc
"ncrcat joining 2^5 files_1" => "42 302 2007", #wc
"ncra time-averaging 2^5 (i.e. one month) ipcc files_0" => "111111 338810 5874572", #wc
"ncwa averaging all variables to scalars - stl_5km.nc & sqrt_0" => "47 323 1862", #wc
"ncwa averaging all variables to scalars - stl_5km.nc & rms_0" => "47 323 1869", #wc
"ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt_0" => "9479 30721 409885", #wc

# Regression entries
"Testing float modulo float_0" => "24 131 947", #wc
"Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)_0" => "11 95 569", #wc
"Testing foo=log(e_dbl)^1_0" => "11 95 571", #wc
"Testing foo=4*atan(1)_0" => "11 95 575", #wc
"Testing foo=erf(1)_0" => "11 95 576", #wc
"Testing foo=gamma(0.5)_0" => "11 95 583", #wc
"Testing foo=sin(pi/2)_0" => "15 121 687", #wc
"Testing foo=cos(pi)_0" => "15 121 688", #wc
"Modify all existing units attributes to meter second-1_0" => "2188 13222 118042", #wc
"difference scalar missing value_0" => "13 119 754", #wc
"difference with missing value attribute_0" => "21 169 995", #wc
"difference without missing value attribute_0" => "20 157 922", #wc
"missing_values differ between files_0" => "27 179 1150", #wc
"missing_values differ between files_1" => "27 179 1114", #wc
"missing_values differ between files_2" => "27 179 1118", #wc
"ncdiff symbolically linked to ncbo_0" => "21 169 995", #wc
"difference with missing value attribute_0" => "21 169 995", #wc
"difference without missing value attribute_0" => "20 157 922", #wc
"ensemble mean of int across two files_0" => "23 178 1095", #wc
"ensemble mean with missing values across two files_0" => "23 180 1171", #wc
"ensemble min of float across two files_0" => "23 180 1169", #wc
"scale factor + add_offset packing/unpacking_0" => "13 154 910", #wc
"concatenate two files containing only scalar variables_2" => "17 126 754", #wc
"identity weighting_0" => "12 106 627", #wc
"identity interpolation_1" => "2188 12862 114282", #wc
"identity interpolation_2" => "12 106 627", #wc
"Create T42 variable named one, uniformly 1.0 over globe_0" => "280 586 8245", #wc
"Create T42 variable named one, uniformly 1.0 over globe_1" => "280 586 6909", #wc
"Create T42 variable named one, uniformly 1.0 over globe_2" => "16676 49836 750137", #wc
"extract a dimension_0" => "23 202 1224", #wc
"extract a variable with limits_0" => "72 492 3666", #wc
"extract variable of type NC_INT_0" => "12 106 640", #wc
"Multi-slab lat and lon with srd_0" => "24 195 1379", #wc
"Multi-slab with redundant hyperslabs_0" => "20 175 1179", #wc
"Multi-slab with coordinates_0" => "20 175 1179", #wc
"Double-wrapped hyperslab_0" => "20 175 1179", #wc
"dimension slice using UDUnits library (fails without UDUnits library support)_0" => "2188 12862 114254", #wc
"variable wildcards A (fails without regex library)_0" => "439 2813 25704", #wc
"variable wildcards B (fails without regex library)_0" => "105 643 4327", #wc
"Offset past end of file_0" => "16 115 696", #wc
"reverse coordinate_0" => "14 107 643", #wc
"reverse three dimensional variable_0" => "72 492 3666", #wc
"re-order three dimensional variable_0" => "72 492 3666", #wc
"Pack and then unpack scalar (uses only add_offset)_0" => "14 163 992", #wc
"Pack and then unpack scalar (uses only add_offset)_1" => "13 152 935", #wc
"record mean of int across two files_0" => "23 178 1099", #wc
"record mean of float with double missing values_0" => "23 180 1173", #wc
"record mean of float with integer missing values_0" => "23 180 1170", #wc
"record mean of integer with integer missing values_0" => "23 180 1168", #wc
"record mean of integer with float missing values_0" => "23 180 1168", #wc
"record mean of packed double with double missing values_0" => "24 195 1330", #wc
"record mean of packed double to test precision_0" => "23 187 1172", #wc
"record mean of packed float to test precision_0" => "23 187 1169", #wc
"record mean of float with double missing values across two files_0" => "23 180 1173", #wc
"record min of float with double missing values across two files_0" => "23 180 1169", #wc
"record max of float with double missing values across two files_0" => "23 180 1171", #wc
"record ttl of float with double missing values across two files_0" => "23 180 1174", #wc
"record rms of float with double missing values across two files_0" => "23 180 1195", #wc
"record sdn of float with double missing values across two files_1" => "23 180 1173", #wc
"record sdn of float with double missing values across two files_2" => "17 149 963", #wc
"record sdn of float with double missing values across two files_3" => "61 237 2212", #wc
"record sdn of float with double missing values across two files_4" => "23 180 1189", #wc
"normalize by denominator upper hemisphere_0" => "29 225 1195", #wc
"do not normalize by denominator_0" => "1342 7637 58740", #wc
"average with missing value attribute_0" => "17 146 857", #wc
"average without missing value attribute_0" => "16 134 790", #wc
"average masked coordinate_0" => "12 106 641", #wc
"average masked variable_0" => "17 143 843", #wc
"average masked, weighted coordinate_0" => "20 198 1166", #wc
"average masked, weighted variable_0" => "26 246 1444", #wc
"weight conforms to var first time_0" => "11 95 571", #wc
"average all missing values with weights_0" => "17 146 877", #wc
"average some missing values with unity weights_0" => "17 148 881", #wc
"average masked variable with some missing values_0" => "18 159 981", #wc
"min switch on type double, some missing values_0" => "17 149 961", #wc
"Dimension reduction with min switch and missing values_0" => "64 376 2825", #wc
"Dimension reduction with min switch and missing values_1" => "64 376 2825", #wc
"Dimension reduction on type int with min switch and missing values_0" => "64 376 2812", #wc
"Dimension reduction on type int with min switch and missing values_1" => "64 376 2812", #wc
"Dimension reduction on type short variable with min switch and missing values_0" => "64 373 2807", #wc
"Dimension reduction on type short variable with min switch and missing values_1" => "64 373 2807", #wc
"Dimension reduction with min flag on type float variable_0" => "25 201 1156", #wc
"Max flag on type float variable_0" => "37 319 1796", #wc
"Dimension reduction on type double variable with max switch and missing values_0" => "50 291 1988", #wc
"Dimension reduction on type double variable with max switch and missing values_1" => "50 291 1988", #wc
"Dimension reduction on type int variable with min switch and missing values_0" => "86 478 4001", #wc
"Dimension reduction on type int variable with min switch and missing values_1" => "86 478 4001", #wc
"Dimension reduction on type short variable with max switch and missing values_0" => "86 475 3993", #wc
"Dimension reduction on type short variable with max switch and missing values_1" => "86 475 3993", #wc
"rms with weights_0" => "11 95 570", #wc
"weights would cause SIGFPE without dbl_prc patch_0" => "17 144 860", #wc
"avg would overflow without dbl_prc patch_0" => "17 144 879", #wc
"ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)_0" => "17 144 880", #wc
"min with weights_0" => "11 95 572", #wc
"max with weights_0" => "11 95 571", #wc
);
