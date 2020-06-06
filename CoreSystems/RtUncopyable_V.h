/*
    Reflec-Tech
    -----------
    File        :    RtUncopyable.h
    Author      :    Jamie Taylor
    Last Edit   :    18/08/12
    Desc        :    Basic utility class, link with your class (via inheritance) to prevent it being copied via copy ctor or assignment operator

                     SETUP/USAGE:
                     ------------
                     class testClass : public Uncopyable
                     {
                         // ...
                     };
*/
#ifndef RT_UNCOPYABLE_H
#define RT_UNCOPYABLE_H


class Uncopyable
{
public:
    Uncopyable() { }
    ~Uncopyable() { }

private:
    Uncopyable(const Uncopyable &ref) { /*do nothing - forbidden op*/ }
    Uncopyable& operator=(const Uncopyable &rhs) { /*do nothing - forbidden op*/ }
};


#endif // RT_UNCOPYABLE_H
