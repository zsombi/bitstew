/*
 * Copyright (C) 2023 bitWelder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>
 */

#ifndef TRACE_PRINTER_MOC_HPP
#define TRACE_PRINTER_MOC_HPP

#include <gmock/gmock.h>
#include <meta/log/trace.hpp>
#include <meta/log/trace_printer.hpp>

class MockPrinter : public meta::TracePrinter
{
public:
    std::string format(const meta::TraceRecord& trace) const
    {
        return trace.message;
    }
    void write(std::string text) final
    {
        log(text);
    }
    MOCK_METHOD(void, log, (const std::string& text));
};

#endif
