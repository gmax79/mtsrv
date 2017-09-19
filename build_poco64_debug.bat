@echo off
call unzip_poco.bat
pushd poco
call buildwin.cmd 140 build static_mt debug x64 nosamples notests
popd
