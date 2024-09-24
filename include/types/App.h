#pragma once
#include <crow.h>

#include "crow/middlewares/session.h"
#include "middleware/ReqLoggerMiddleware.h"

using Session = crow::SessionMiddleware<crow::InMemoryStore>;
using CrowApp = crow::App<crow::CookieParser, Session, ReqLoggerMiddleware>;