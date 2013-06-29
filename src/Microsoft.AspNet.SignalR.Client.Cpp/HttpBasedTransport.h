#pragma once

#include "Connection.h"
#include "TransportHelper.h"
#include <agents.h>
#include <typeinfo>
#include "TaskAsyncHelper.h"
#include "TransportInitializationHandler.h"

using namespace utility;
using namespace web::json;

class HttpBasedTransport :
    public IClientTransport
{
public:
    HttpBasedTransport(shared_ptr<IHttpClient> httpClient, string_t transport);
    ~HttpBasedTransport(void);

    pplx::task<shared_ptr<NegotiationResponse>> Negotiate(shared_ptr<Connection> connection);
    pplx::task<void> Start(shared_ptr<Connection> connection, string_t data, pplx::cancellation_token disconnectToken);
    pplx::task<void> Send(shared_ptr<Connection> connection, string_t data);
    void Abort(shared_ptr<Connection> connection);

protected:
    shared_ptr<IHttpClient> GetHttpClient();
    string_t GetReceiveQueryString(shared_ptr<Connection> connection, string_t data);
    string_t GetSendQueryString(string_t transport, string_t connectionToken, string_t customQuery);
    void CompleteAbort();
    bool TryCompleteAbort();
    //virtual void OnStart(shared_ptr<Connection> connection, string_t data, pplx::cancellation_token disconnectToken, function<void()> initializeCallback, function<void()> errorCallback) = 0;
    virtual void OnStart(shared_ptr<Connection> connection, string_t data, pplx::cancellation_token disconnectToken, shared_ptr<TransportInitializationHandler> initializeHandler) = 0;
    virtual void OnAbort() = 0;

private:
    bool mStartedAbort;
    bool mDisposed;
    mutex mAbortLock;
    mutex mDisposeLock;
    shared_ptr<IHttpClient> pHttpClient;
    unique_ptr<Concurrency::event> pAbortResetEvent;
};

