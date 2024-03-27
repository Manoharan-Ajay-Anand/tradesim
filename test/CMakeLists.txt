add_executable(tradesim_test
    main.cpp
    queue_test.cpp
)

target_link_libraries(tradesim_test PRIVATE cppevent)

add_test(NAME TradesimTest COMMAND tradesim_test)
