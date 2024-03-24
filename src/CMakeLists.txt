add_executable(tradesim 
    create_endpoint.cpp
    exchange.cpp
    join_endpoint.cpp
    main.cpp
    market_types.cpp
    market.cpp
    order_endpoint.cpp
    stream_endpoint.cpp
    broadcast.cpp
    types.cpp
    welcome_endpoint.cpp
)

target_link_libraries(tradesim PRIVATE cppevent uring)