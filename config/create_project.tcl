source {./config/config.tcl}

delete_project $project
open_project $project
set_top $top
add_files src/algos.cpp
add_files -tb tb/algos_test.cpp
add_files -tb tb/byte_vector.h
add_files -tb tb/bytelify.h
add_files -tb tb/ostream_utils.cpp
add_files -tb tb/ostream_utils.h
add_files -tb tb/test_vector.cpp
add_files -tb tb/test_vector.h
add_files -tb tb/transaction_writer.h
add_files -tb tb/unittest.h

open_solution $solution
set_part $part
create_clock -period $clock_ns -name default
