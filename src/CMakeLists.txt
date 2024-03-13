add_executable(tradesim 
    create_endpoint.cpp
    exchange.cpp
    main.cpp
    market_types.cpp
    market.cpp
    stream_endpoint.cpp
    subscription.cpp
    types.cpp
    welcome_endpoint.cpp
)

target_link_libraries(tradesim PRIVATE cppevent uring)