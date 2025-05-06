#include <boost/asio/signal_set.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/version.hpp>

#include <boost/config.hpp>

#include <iostream>
#include <string_view>

namespace beast     = boost::beast;
namespace http      = beast::http;
namespace websocket = beast::websocket;
namespace asio      = boost::asio;

std::string_view mime_type(std::string_view path)
{
    auto const ext = [&path]
    {
        auto const pos = path.rfind(".");
        if(pos == std::string_view::npos) return std::string_view{};
        return path.substr(pos);
    }();

    if(beast::iequals(ext, ".htm"))  return "text/html";
    if(beast::iequals(ext, ".html")) return "text/html";
    if(beast::iequals(ext, ".php"))  return "text/html";
    if(beast::iequals(ext, ".css"))  return "text/css";
    if(beast::iequals(ext, ".txt"))  return "text/plain";
    if(beast::iequals(ext, ".js"))   return "application/javascript";
    if(beast::iequals(ext, ".json")) return "application/json";
    if(beast::iequals(ext, ".xml"))  return "application/xml";
    if(beast::iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(beast::iequals(ext, ".flv"))  return "video/x-flv";
    if(beast::iequals(ext, ".png"))  return "image/png";
    if(beast::iequals(ext, ".jpe"))  return "image/jpeg";
    if(beast::iequals(ext, ".jpeg")) return "image/jpeg";
    if(beast::iequals(ext, ".jpg"))  return "image/jpeg";
    if(beast::iequals(ext, ".gif"))  return "image/gif";
    if(beast::iequals(ext, ".bmp"))  return "image/bmp";
    if(beast::iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(beast::iequals(ext, ".tiff")) return "image/tiff";
    if(beast::iequals(ext, ".tif"))  return "image/tiff";
    if(beast::iequals(ext, ".svg"))  return "image/svg+xml";
    if(beast::iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

std::string path_cat(std::string_view base, std::string_view path)
{
    if(base.empty()) return std::string(path);
    std::string result(base);

    char constexpr path_separator = '/';
    if(result.back() == path_separator) result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    return result;
}

template <class Body, class Allocator>
http::message_generator handle_request
(
    std::string_view doc_root, 
    http::request<Body, http::basic_fields<Allocator>>&& req
)
{
    auto const bad_request = [&req](std::string_view why)
    {
        http::response<http::string_body> res{http::status::bad_request, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = why;
        res.prepare_payload();
        return res;
    };

    auto const not_found = [&req](std::string_view target)
    {
        http::response<http::string_body> res{http::status::not_found, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string{target} + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&req](std::string_view what)
    {
        http::response<http::string_body> res{http::status::internal_server_error, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string{what} + "'";
        res.prepare_payload();
        return res;
    };

    if(req.method() != http::verb::get && req.method() != http::verb::head)
    {
        return bad_request("Unknown HTTP-method");
    }

    if(req.target().empty() || req.target()[0] != '/' || req.target().find("..") != std::string_view::npos)
    {
        return bad_request("Illegal request-target");
    }

    std::string path = path_cat(doc_root, req.target());
    if(req.target().back() == '/')
    {
        path.append("index.html");
    }

    beast::error_code ec;
    http::file_body::value_type body;
    body.open(path.c_str(), beast::file_mode::scan, ec);

    if(ec == beast::errc::no_such_file_or_directory)
    {
        return not_found(req.target());
    }

    if(ec)
    {
        return server_error(ec.message());
    }

    auto const size = body.size();

    if(req.method() == http::verb::head)
    {
        http::response<http::empty_body> res{http::status::ok, req.version()};
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(path));
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    http::response<http::file_body> res
    {
        std::piecewise_construct,
        std::make_tuple(std::move(body)),
        std::make_tuple(http::status::ok, req.version())
    };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(path));
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

asio::awaitable<void> ws_handle(asio::ip::tcp::socket socket, http::request<http::string_body> req)
{
    websocket::stream<asio::ip::tcp::socket> ws(std::move(socket));
    co_await ws.async_accept(req, asio::use_awaitable);

    try
    {
        for(;;)
        {
            beast::flat_buffer buffer;
            std::size_t length = co_await ws.async_read(buffer, asio::use_awaitable);

            std::string message = beast::buffers_to_string(buffer.data());
            std::cout << "Received: [" << message << "]" << std::endl;
            
            message = std::to_string(length);

            ws.text(ws.got_text());
            co_await ws.async_write(asio::buffer("Server received: " + message), asio::use_awaitable);
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

asio::awaitable<void> session(asio::io_context& io_ctx, asio::ip::tcp::socket socket, std::string_view doc_root, int s)
{
    try
    {
        beast::flat_buffer buffer;
        for(int i = 0;;i++)
        {
            http::request<http::string_body> req;
            co_await http::async_read(socket, buffer, req, asio::use_awaitable);
            std::cout << "Request[" << s << ", " << i << "]: " << req.method() << " " << req.target() << std::endl;

            if(websocket::is_upgrade(req))
            {
                co_await ws_handle(std::move(socket), std::move(req));
                break;
            }

            http::message_generator msg = handle_request(doc_root, std::move(req));
            bool keep_alive = msg.keep_alive();
            co_await beast::async_write(socket, std::move(msg), asio::use_awaitable);
            if(!keep_alive) break;
        }
        std::cout << "Stoped session: " << s << std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

asio::awaitable<void> server(asio::io_context& io_ctx, uint16_t port)
{
    try
    {
        asio::ip::tcp::acceptor acceptor(io_ctx, asio::ip::tcp::endpoint{asio::ip::tcp::v6(), port});
        for(int i = 0;;i++)
        {
            asio::ip::tcp::socket socket = co_await acceptor.async_accept(asio::use_awaitable);
            std::cout << "Address server: " << socket.local_endpoint() << std::endl;
            std::cout << "New connection: " << socket.remote_endpoint() << std::endl;
            asio::co_spawn(io_ctx, session(io_ctx, std::move(socket), "res", i), asio::detached);
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port> <doc_root>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 21521 res" << std::endl;
        return 1;
    }

    const auto port = static_cast<unsigned short>(std::atoi(argv[1]));
    const auto doc_root = std::string_view(argv[2]);

    try
    {
        asio::io_context io_ctx;
        asio::co_spawn(io_ctx, server(io_ctx, port), asio::detached);

        asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
        signals.async_wait([&](auto, auto){ io_ctx.stop(); });

        io_ctx.run();
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}