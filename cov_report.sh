
# lcov --directory . --zerocounters
# ./CUTest
lcov --directory . --capture --output-file report.info
genhtml report.info -o report --branch
