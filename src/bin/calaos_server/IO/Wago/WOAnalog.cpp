/******************************************************************************
 **  Copyright (c) 2006-2014, Calaos. All Rights Reserved.
 **
 **  This file is part of Calaos.
 **
 **  Calaos is free software; you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation; either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  Calaos is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with Foobar; if not, write to the Free Software
 **  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 **
 ******************************************************************************/
#include <WOAnalog.h>
#include <WagoMap.h>
#include <IOFactory.h>

using namespace Calaos;
using namespace Utils;

REGISTER_OUTPUT(WOAnalog)
REGISTER_OUTPUT_USERTYPE(WagoOutputAnalog, WOAnalog)

WOAnalog::WOAnalog(Params &p):
    OutputAnalog(p),
    port(502)
{
    host = get_param("host");
    Utils::from_string(get_param("var"), address);
    if (get_params().Exists("port"))
        Utils::from_string(get_param("port"), port);

    WagoMap::Instance(host, port);

    WagoMap::Instance(host, port).read_words((UWord)address + 0x200, 1, sigc::mem_fun(*this, &WOAnalog::WagoReadCallback));

    Calaos::StartReadRules::Instance().addIO();

    cDebugDom("output") << get_param("id");
}

WOAnalog::~WOAnalog()
{
    cDebugDom("output");
}

void WOAnalog::WagoReadCallback(bool status, UWord addr, int count, vector<UWord> &values)
{
    if (!status)
    {
        cErrorDom("output") << get_param("id") << ": Failed to read value";
        Calaos::StartReadRules::Instance().ioRead();

        return;
    }

    if (!values.empty()) value = values[0];

    emitChange();

    Calaos::StartReadRules::Instance().ioRead();
}

void WOAnalog::WagoWriteCallback(bool status, UWord addr, UWord _value)
{
    if (!status)
    {
        cErrorDom("output") << get_param("id") << ": Failed to write value";
        return;
    }

    value = _value;

    emitChange();

    cInfoDom("output") << get_param("id") << ", executed action " << value << " (" << get_value_double() << ")";
}

void WOAnalog::set_value_real(double val)
{
    host = get_param("host");
    Utils::from_string(get_param("var"), address);
    if (get_params().Exists("port"))
        Utils::from_string(get_param("port"), port);

    WagoMap::Instance(host, port).write_single_word((UWord)address, val, sigc::mem_fun(*this, &WOAnalog::WagoWriteCallback));
}
