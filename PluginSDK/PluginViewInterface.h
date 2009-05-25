#ifndef PLUGINVIEWINTERFACE_H
#define PLUGINVIEWINTERFACE_H

#include <boost/smart_ptr.hpp>
#include <boost/signal.hpp>
#include <ItemAssistant/AOMessageParsers.h> // Todo, move parsers into the SDK
using namespace Parsers;

struct PluginViewInterface
{
    typedef boost::signal<void ()>  status_signal_t;
    typedef boost::signals::connection connection_t;

    virtual connection_t connectStatusChanged(status_signal_t::slot_function_type slot) = 0;
    virtual void disconnect(connection_t slot) = 0;

    virtual void OnAOServerMessage(AOMessageBase &msg) = 0;
    virtual void OnAOClientMessage(AOClientMessageBase &msg) = 0;
    virtual bool PreTranslateMsg(MSG* pMsg) = 0;
    virtual HWND GetWindow() const = 0;
    virtual void OnActive(bool doActivation) = 0;
    virtual HWND GetToolbar() const = 0;
    virtual std::tstring GetStatusText() const = 0;
};

/// Signature of plugin DLL factory function.
typedef boost::shared_ptr<PluginViewInterface>(*AOIA_CreatePlugin)(std::string const&);

#endif // PLUGINVIEWINTERFACE_H
