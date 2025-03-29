#pragma once
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <functional>
#include <unordered_map>
#include <json/json.h>
#include <json/reader.h>
#include <json/value.h>
#include "../../common/include/ConfigManager.h"


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;