add_executable(tradesim 
    create_endpoint.cpp
    main.cpp
    exchange.cpp
    market.cpp
    stream_endpoint.cpp
    subscription.cpp
    types.cpp
    welcome_endpoint.cpp
)

target_link_libraries(tradesim PRIVATE cppevent uring)