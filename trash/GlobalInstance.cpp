#include "GlobalInstance.hpp"

GlobalInstance::GlobalInstance()
{
    pool_ = new QThreadPool(this);
    pool_->setMaxThreadCount(5);
}

GlobalInstance::~GlobalInstance()
{
}

GlobalInstance &GlobalInstance::Instance()
{
    static GlobalInstance instance;
    return instance;
}

QThreadPool* GlobalInstance::pool() const
{
    return pool_;
}
