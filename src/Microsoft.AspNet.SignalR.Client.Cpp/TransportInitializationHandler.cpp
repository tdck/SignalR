#include "TransportInitializationHandler.h"

TransportInitializationHandler::TransportInitializationHandler(pplx::cancellation_token disconnectToken)
{
    mInitializationTask = pplx::task_completion_event<void>();
    pInitializationInvoker = unique_ptr<ThreadSafeInvoker>(new ThreadSafeInvoker());

    // Default event
    OnFailure = [](){};

    //// We want to fail if the disconnect token is tripped while we're waiting on initialization
    //mTokenCleanup = disconnectToken.register_callback([this]()
    //{
    //    Fail();
    //});

    TaskAsyncHelper::Delay(utility::seconds(30)).then([this]()
    {
        Fail(exception("TimeoutException: Transport timed out trying to connect"));
    });
}

TransportInitializationHandler::~TransportInitializationHandler()
{
}

void TransportInitializationHandler::Fail()
{
    Fail(exception("InvalidOperationException: Transport failed trying to connect."));
}

void TransportInitializationHandler::Fail(exception& ex)
{
    pInitializationInvoker->Invoke([this, ex]()
    {
        OnFailure();
        mInitializationTask.set_exception(ex);
    });
}

void TransportInitializationHandler::Success()
{
    pInitializationInvoker->Invoke([this]()
    {
        mInitializationTask.set();
    });
}

pplx::task<void> TransportInitializationHandler::GetTask()
{
    return pplx::task<void>(mInitializationTask);
}