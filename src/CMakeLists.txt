add_executable(tradesim 
    broadcast.cpp
    cancel_endpoint.cpp
    create_endpoint.cpp
    exchange.cpp
    join_endpoint.cpp
    main.cpp
    market_types.cpp
    market.cpp
    order_endpoint.cpp
    stream_endpoint.cpp
    types.cpp
    welcome_endpoint.cpp
)

target_link_libraries(tradesim PRIVATE cppevent uring)