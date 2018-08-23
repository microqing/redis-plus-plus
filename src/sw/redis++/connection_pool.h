/**************************************************************************
   Copyright (c) 2017 sewenew

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *************************************************************************/

#ifndef SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H
#define SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H

#include <chrono>
#include <mutex>
#include <condition_variable>
#include <deque>
#include "connection.h"

namespace sw {

namespace redis {

struct ConnectionPoolOptions {
    // Max number of connections, including both in-use and idle ones.
    std::size_t size = 1;

    // Max time to wait for a connection. 0ms means client waits forever.
    std::chrono::milliseconds wait_timeout{0};

    // Max lifetime of a connection. 0ms means we never expire the connection.
    std::chrono::milliseconds connection_lifetime{0};
};

class ConnectionPool {
public:
    explicit ConnectionPool(const ConnectionPoolOptions &pool_opts,
            const ConnectionOptions &connection_opts);

    ConnectionPool(ConnectionPool &&that);

    ConnectionPool& operator=(ConnectionPool &&that);

    // Fetch a connection from pool.
    Connection fetch();

    // Create a new connection.
    Connection create() const;

    void release(Connection connection);

    friend void swap(ConnectionPool &lhs, ConnectionPool &rhs);

private:
    // NOT thread-safe
    Connection _fetch();

    void _wait_for_connection(std::unique_lock<std::mutex> &lock);

    bool _need_reconnect(const Connection &connection);

    ConnectionOptions _opts;

    ConnectionPoolOptions _pool_opts;

    std::deque<Connection> _pool;

    std::mutex _mutex;

    std::condition_variable _cv;
};

}

}

#endif // end SEWENEW_REDISPLUSPLUS_CONNECTION_POOL_H
