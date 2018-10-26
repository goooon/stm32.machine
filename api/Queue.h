#ifndef __STM32F10x_QUEUE_H
#define __STM32F10x_QUEUE_H

#include "../base/type.h"
#include "../api/Tracer.h"
template <typename T,int N = 512>
class StaticQueue
{
public:
    StaticQueue():_in(0),_out(0) {}
    b32 isEmpty()
    {
        b32 ret = _in == _out;
        return ret;
    }
    b32 isFull()
    {
        b32 ret = (_in + 1) % N == _out;
        //LOG_I("%d,%d",_in,_out);
        return ret;
    }
    T& outlet()
    {
        LOG_A(!isEmpty());
        return _data[_out];
    }
    T& inlet()
    {
        LOG_A(!isEmpty());
        return _data[_in];
    }
    T& getAt(s32 index)
    {
        s32 len = (_in - _out + N) % N;
        LOG_A(index < len);
        T& d = _data[(_out + index) % N];
        return d;
    }
    u32 getLength()const
    {
        u32 len = (_in - _out + N) % N;
        return len;
    }
    b32 inQueue(const T& t)
    {
        if ((_in + 1) % N == _out)
        {
            return vFalse;
        }
        else
        {
            _data[_in] = t;
            _in += (N + 1);
            _in %= N;
        }
        return vTrue;
    }
    b32 outQueue(T& t)
    {
        if (isEmpty())
            return vFalse;
        t = _data[_out];
        _out += 1;
        _out %= N;
        return vTrue;
    }
private:
    T _data[N];
    s32 _in;
    s32 _out;
};
#endif