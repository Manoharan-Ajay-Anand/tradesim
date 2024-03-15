add_executable(tradesim_test
    main.cpp
    queue_test.cpp
)

add_test(NAME TradesimTest COMMAND tradesim_test)
