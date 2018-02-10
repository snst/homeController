// Copyright 2017 Stefan Schmidt
#ifndef _IDATASINK_H_
#define _IDATASINK_H_

class IDataSink
{
    public:
    virtual void setTemperature(float val) {}
    virtual void setPressure(float val) {}
    virtual void setHumidity(float val) {}
    virtual bool publish() = 0;
    virtual void publishChannelData(const char *channel, float value) = 0;
};

#endif