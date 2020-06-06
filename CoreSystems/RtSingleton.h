/*
===============================================================================

    ReflecTech
    ==========
    File        :    RtSingleton.h
    Author      :    Jamie Taylor
    Last Edit   :    08/07/13
    Desc        :    Singleton implementation, inspired by that seen in OGRE. Effectively 'wraps/holds' the class you want to
                     make a singleton out of. This way you don't have to alter the code of the class in question, we simply use a coupling.

                     INTENDED USAGE PATTERN: 1) Construct
                                             2) Call GetSingletonPointer/Reference( )

                     SETUP/USAGE:
                     ------------
                     class testClass : public Singleton<testClass>
                     {
                         // ...
                     };
                     template<> testClass *Singleton<testClass>::singletonInstance = NULL;

===============================================================================
*/


#ifndef RT_SINGLETON_H
#define RT_SINGLETON_H


#include "RtAssert.h"


/*
===============================================================================

Singleton class

Where class T is the class been wrapped

===============================================================================
*/
template<class T>
class Singleton {
public:
               // ctor and dtor
               Singleton( void );
               ~Singleton( void );

               // access the singleton instance
    static T * GetSingletonPointer( void );
    static T & GetSingletonReference( void );

protected:
               // the single instance
    static T * singletonInstance;

private:
               Singleton( const Singleton<T> &ref ) { /*do nothing - forbidden op*/ }
    void       operator=( const Singleton<T> &ref ) { /*do nothing - forbidden op*/ }
};


/*
================
Singleton<T>::Singleton
================
*/
template<class T>
Singleton<T>::Singleton( void ) {
    RT_ASSERT( singletonInstance == NULL );
    // setup the singleton instance
    singletonInstance = static_cast<T*>( this );
}

/*
================
Singleton<T>::~Singleton
================
*/
template<class T>
Singleton<T>::~Singleton( void ) {
    singletonInstance = NULL;
}

/*
================
Singleton<T>::GetSingletonPointer
================
*/
template<class T>
T* Singleton<T>::GetSingletonPointer( void ) {
    RT_ASSERT( singletonInstance != NULL );
    return singletonInstance;
}

/*
================
Singleton<T>::Singleton<T>::GetSingletonReference
================
*/
template<class T>
T& Singleton<T>::GetSingletonReference( void ) {
    RT_ASSERT( singletonInstance != NULL );
    return *singletonInstance;
}


#endif // RT_SINGLETON_H
