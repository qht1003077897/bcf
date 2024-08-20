#pragma once

#include <config.h>
#include <base/globaldefine.h>
#include <protocolparser/byheadprotocolparser.h>
#include <ichannel.h>
#include <requesthandler.h>
#include <abstractprotocolmodel.h>
#include <iprotocolparser.h>
#include <iprotocolbuilder.h>
#ifdef BCF_USE_QT_SERIALPORT
    #include <serialchannel_qt.h>
#endif
